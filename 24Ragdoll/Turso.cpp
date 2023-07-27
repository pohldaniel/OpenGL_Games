#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Turso.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

#include <tracy/Tracy.hpp>


TursoInterface::TursoInterface(StateMachine& machine) : State(machine, CurrentState::TURSOINTERFACE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	glClearColor(0.7f, 0.7f, 0.7f, 0.0f);

	bool useThreads = true;

	// Create subsystems that don't depend on the application window / OpenGL context
	workQueue = new WorkQueue(useThreads ? 0 : 1);
	profiler = new Profiler();
	log = new Log();
	cache = new ResourceCache();
	cache->AddResourceDir("Data");

	// Create the Graphics subsystem to open the application window and initialize OpenGL
	graphics = new Graphics("Turso3D renderer test", IntVector2(Application::Width, Application::Height));
	graphics->Initialize();
	graphics->BindDefaultVao(true);

	// Create subsystems that depend on the application window / OpenGL
	//input = new InputTu(graphics->Window());
	renderer = new Renderer();
	debugRenderer = new DebugRenderer();

	
	// Rendertarget textures
	viewFbo = new FrameBufferTu();
	viewMRTFbo = new FrameBufferTu();
	ssaoFbo = new FrameBufferTu();
	colorBuffer = new TextureTu();
	normalBuffer = new TextureTu();
	depthStencilBuffer = new TextureTu();
	ssaoTexture = new TextureTu();
	occlusionDebugTexture = new TextureTu();

	// Random noise texture for SSAO
	unsigned char noiseData[4 * 4 * 4];
	for (int i = 0; i < 4 * 4; ++i){
		Vector3 noiseVec(Random() * 2.0f - 1.0f, Random() * 2.0f - 1.0f, Random() * 2.0f - 1.0f);
		noiseVec.Normalize();

		noiseData[i * 4 + 0] = (unsigned char)(noiseVec.x * 127.0f + 128.0f);
		noiseData[i * 4 + 1] = (unsigned char)(noiseVec.y * 127.0f + 128.0f);
		noiseData[i * 4 + 2] = (unsigned char)(noiseVec.z * 127.0f + 128.0f);
		noiseData[i * 4 + 3] = 0;
	}

	ImageLevel noiseDataLevel(IntVector2(4, 4), FMT_RGBA8, &noiseData[0]);
	noiseTexture = new TextureTu();
	noiseTexture->Define(TEX_2D, IntVector2(4, 4), FMT_RGBA8, 1, 1, &noiseDataLevel);
	noiseTexture->DefineSampler(FILTER_POINT);

	// Create the scene and camera. Camera is created outside scene so it's not disturbed by scene clears
	scene = ObjectTu::Create<Scene>();
	camera = ObjectTu::Create<CameraTu>();

	CreateScene(scene, camera, 0);
	//CreateScene(scene, camera, 2);

	camera->SetPosition(Vector3(0.0f, 20.0f, -75.0f));
	camera->SetAspectRatio((float)Application::Width / (float)Application::Height);

	colorBuffer->Define(TEX_2D, IntVector2(Application::Width, Application::Height), FMT_RGBA8);
	colorBuffer->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
	depthStencilBuffer->Define(TEX_2D, IntVector2(Application::Width, Application::Height), FMT_D32);
	depthStencilBuffer->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
	normalBuffer->Define(TEX_2D, IntVector2(Application::Width, Application::Height), FMT_RGBA8);
	normalBuffer->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
	viewFbo->Define(colorBuffer, depthStencilBuffer);

	std::vector<TextureTu*> mrt;
	mrt.push_back(colorBuffer.Get());
	mrt.push_back(normalBuffer.Get());
	viewMRTFbo->Define(mrt, depthStencilBuffer);

	ssaoTexture->Define(TEX_2D, IntVector2(colorBuffer->Width() / 2, colorBuffer->Height() / 2), FMT_R32F, 1, 1);
	ssaoTexture->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
	ssaoFbo->Define(ssaoTexture, nullptr);

	graphics->SetViewport(IntRect(0, 0, Application::Width, Application::Height));


	SubscribeToEvent(eventTu, new EventHandlerImpl<TursoInterface, EventTu>(this, &TursoInterface::HandleUpdate));

	perViewDataBuffer = new UniformBuffer();
	perViewDataBuffer->Define(USAGE_DYNAMIC, sizeof(PerViewUniforms));

	octantResults = new ThreadOctantResult2[NUM_OCTANT_TASKS];
	for (size_t i = 0; i < NUM_OCTANTS + 1; ++i) {
		collectOctantsTasks[i] = new CollectOctantsTask2(this, &TursoInterface::CollectOctantsWork);
		collectOctantsTasks[i]->resultIdx = i;
	}
}

TursoInterface::~TursoInterface() {
	Globals::physics->removeAllCollisionObjects();
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void TursoInterface::fixedUpdate() {
	Globals::physics->stepSimulation(m_fdt);
}

void TursoInterface::update() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyPressed(Keyboard::KEY_F1))
		CreateScene(scene, camera, 0);
	if (keyboard.keyPressed(Keyboard::KEY_F2))
		CreateScene(scene, camera, 1);
	if (keyboard.keyPressed(Keyboard::KEY_F3))
		CreateScene(scene, camera, 2);

	if (keyboard.keyPressed(Keyboard::KEY_2))
		drawSSAO = !drawSSAO;
	if (keyboard.keyPressed(Keyboard::KEY_3))
		useOcclusion = !useOcclusion;
	if (keyboard.keyPressed(Keyboard::KEY_4))
		drawDebug = !drawDebug;
	if (keyboard.keyPressed(Keyboard::KEY_5))
		drawShadowDebug = !drawShadowDebug;
	if (keyboard.keyPressed(Keyboard::KEY_6))
		drawOcclusionDebug = !drawOcclusionDebug;
	if (keyboard.keyPressed(Keyboard::KEY_SPACE))
		animate = !animate;


	Vector3 directrion = Vector3();
	bool move = false;	
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3::FORWARD;
		move = true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3::BACK;
		move = true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3::LEFT;
		move = true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3::RIGHT;
		move = true;
	}

	Mouse &mouse = Mouse::instance();
	float dx = 0.0f;
	float dy = 0.0f;

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {

			yaw += dx * 0.1f;
			pitch += dy * 0.1f;

			pitch = Clamp(pitch, -90.0f, 90.0f);

			camera->SetRotation(QuaternionTu(pitch, yaw, 0.0f));
		}

		if (move) {
			float moveSpeed = (keyboard.keyDown(Keyboard::KEY_LSHIFT) || keyboard.keyDown(Keyboard::KEY_RSHIFT)) ? 50.0f : 5.0f;

			camera->Translate(directrion * m_dt * moveSpeed);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	// Scene animation
	if (animate) {
		if (rotatingObjects.size()) {
			angle += 100.0f * m_dt;
			QuaternionTu rotQuat(angle, Vector3::ONE);
			for (auto it = rotatingObjects.begin(); it != rotatingObjects.end(); ++it)
				(*it)->SetRotation(rotQuat);

		}else if (animatingObjects.size()) {
			for (auto it = animatingObjects.begin(); it != animatingObjects.end(); ++it) {
				AnimatedModel* object = *it;
				AnimationState* state = object->AnimationStates()[0];
				state->AddTime(m_dt);
				object->Translate(Vector3::FORWARD * 2.0f * m_dt);

				// Rotate to avoid going outside the plane
				Vector3 pos = object->Position();
				if (pos.x < -45.0f || pos.x > 45.0f || pos.z < -45.0f || pos.z > 45.0f)
					object->Yaw(45.0f * m_dt);
			}
		}
	}

	SendEvent(eventTu);
	updateOctree();
}

void TursoInterface::renderDirect() {

	

	debugRenderer->SetView(camera);


	glClearColor(DEFAULT_FOG_COLOR.r, DEFAULT_FOG_COLOR.g, DEFAULT_FOG_COLOR.b, DEFAULT_FOG_COLOR.a);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//fill in uniforms
	float nearClip = camera->NearClip();
	float farClip = camera->FarClip();
	perViewData.projectionMatrix = camera->ProjectionMatrix();
	perViewData.viewMatrix = camera->ViewMatrix();
	perViewData.viewProjMatrix = camera->ProjectionMatrix() * camera->ViewMatrix();
	perViewData.depthParameters = Vector4(nearClip, farClip, camera->IsOrthographic() ? 0.5f : 0.0f, camera->IsOrthographic() ? 0.5f : 1.0f / farClip);
	perViewData.cameraPosition = Vector4(camera->WorldPosition(), 1.0f);
	perViewData.ambientColor = DEFAULT_AMBIENT_COLOR;
	perViewData.fogColor = DEFAULT_FOG_COLOR;
	float fogStart = DEFAULT_FOG_START;
	float fogEnd = DEFAULT_FOG_END;
	float fogRange = Max(fogEnd - fogStart, M_EPSILON);
	perViewData.fogParameters = Vector4(fogEnd / farClip, farClip / fogRange, 0.0f, 0.0f);
	perViewData.dirLightDirection = Vector4::ZERO;
	perViewData.dirLightColor = Color::BLACK;
	perViewData.dirLightShadowParameters = Vector4::ONE;
	perViewDataBuffer->SetData(0, sizeof(PerViewUniforms), &perViewData);

	perViewDataBuffer->Bind(UB_PERVIEWDATA);

	if(drawDebug) {
		DebugRenderer* debug = Subsystem<DebugRenderer>();

		for (size_t i = 0; i < renderer->rootLevelOctants.size(); ++i) {
			const ThreadOctantResult& result = renderer->octantResults[i];

			for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
				Octant* octant = oIt->first;
				octant->OnRenderDebug(debug);
			}
		}
	}

	debugRenderer->Render();
}

void TursoInterface::renderDirect2() {
	

	
}

void TursoInterface::render() {
	renderDirect();
	
	// Collect geometries and lights in frustum. Also set debug renderer to use the correct camera view
	/*{
		PROFILE(PrepareView);
		renderer->PrepareView(scene, camera, shadowMode > 0, useOcclusion);
		debugRenderer->SetView(camera);
	}

	// Raycast into the scene using the camera forward vector. If has a hit, draw a small debug sphere at the hit location
	{
		PROFILE(Raycast);

		Ray cameraRay(camera->WorldPosition(), camera->WorldDirection());
		RaycastResult res = scene->FindChild<Octree>()->RaycastSingle(cameraRay, DF_GEOMETRY);
		if (res.drawable)
			debugRenderer->AddSphere(Sphere(res.position, 0.05f), Color::WHITE, true);
	}

	// Now render the scene, starting with shadowmaps and opaque geometries
	{
		PROFILE(RenderView);

		renderer->RenderShadowMaps();

		// The default opaque shaders can write both color (first RT) and view-space normals (second RT).
		// If going to render SSAO, bind both rendertargets, else just the color RT
		if (drawSSAO)
			graphics->SetFrameBuffer(viewMRTFbo);
		else
			graphics->SetFrameBuffer(viewFbo);

		graphics->SetViewport(IntRect(0, 0, Application::Width, Application::Height));
		renderer->RenderOpaque();

		// Optional SSAO effect. First sample the normals and depth buffer, then apply a blurred SSAO result that darkens the opaque geometry
		if (drawSSAO)
		{
			float farClip = camera->FarClip();
			float nearClip = camera->NearClip();
			Vector3 nearVec, farVec;
			camera->FrustumSize(nearVec, farVec);

			ShaderProgram* program = graphics->SetProgram("Shaders/SSAO.glsl");
			graphics->SetFrameBuffer(ssaoFbo);
			graphics->SetViewport(IntRect(0, 0, ssaoTexture->Width(), ssaoTexture->Height()));
			graphics->SetUniform(program, "noiseInvSize", Vector2(ssaoTexture->Width() / 4.0f, ssaoTexture->Height() / 4.0f));
			graphics->SetUniform(program, "screenInvSize", Vector2(1.0f / colorBuffer->Width(), 1.0f / colorBuffer->Height()));
			graphics->SetUniform(program, "frustumSize", Vector4(farVec, (float)Application::Height / (float)Application::Width));
			graphics->SetUniform(program, "aoParameters", Vector4(0.15f, 1.0f, 0.025f, 0.15f));
			graphics->SetUniform(program, "depthReconstruct", Vector2(farClip / (farClip - nearClip), -nearClip / (farClip - nearClip)));
			graphics->SetTexture(0, depthStencilBuffer);
			graphics->SetTexture(1, normalBuffer);
			graphics->SetTexture(2, noiseTexture);
			graphics->SetRenderState(BLEND_REPLACE, CULL_NONE, CMP_ALWAYS, true, false);
			graphics->DrawQuad();
			graphics->SetTexture(1, nullptr);
			graphics->SetTexture(2, nullptr);

			program = graphics->SetProgram("Shaders/SSAOBlur.glsl");
			graphics->SetFrameBuffer(viewFbo);
			graphics->SetViewport(IntRect(0, 0, Application::Width, Application::Height));
			graphics->SetUniform(program, "blurInvSize", Vector2(1.0f / ssaoTexture->Width(), 1.0f / ssaoTexture->Height()));
			graphics->SetTexture(0, ssaoTexture);
			graphics->SetRenderState(BLEND_SUBTRACT, CULL_NONE, CMP_ALWAYS, true, false);
			graphics->DrawQuad();
			graphics->SetTexture(0, nullptr);
		}

		// Render alpha geometry. Now only the color rendertarget is needed
		graphics->SetFrameBuffer(viewFbo);
		graphics->SetViewport(IntRect(0, 0, Application::Width, Application::Height));
		renderer->RenderAlpha();

		// Optional render of debug geometry
		if (drawDebug)
			renderer->RenderDebug();

		debugRenderer->Render();

		// Optional debug render of shadowmap. Draw both dir light cascades and the shadow atlas
		if (drawShadowDebug)
		{
			Matrix4 quadMatrix = Matrix4::IDENTITY;
			quadMatrix.m00 = 0.33f * 2.0f * (9.0f / 16.0f);
			quadMatrix.m11 = 0.33f;
			quadMatrix.m03 = -1.0f + quadMatrix.m00;
			quadMatrix.m13 = -1.0f + quadMatrix.m11;

			ShaderProgram* program = graphics->SetProgram("Shaders/DebugShadow.glsl");
			graphics->SetUniform(program, "worldViewProjMatrix", quadMatrix);
			graphics->SetTexture(0, renderer->ShadowMapTexture(0));
			graphics->SetRenderState(BLEND_REPLACE, CULL_NONE, CMP_ALWAYS, true, false);
			graphics->DrawQuad();

			quadMatrix.m03 += 1.5f * quadMatrix.m00;
			quadMatrix.m00 = 0.33f * (9.0f / 16.0f);

			graphics->SetUniform(program, "worldViewProjMatrix", quadMatrix);
			graphics->SetTexture(0, renderer->ShadowMapTexture(1));
			graphics->DrawQuad();

			graphics->SetTexture(0, nullptr);
		}

		// Blit rendered contents to backbuffer now before presenting
		graphics->Blit(nullptr, IntRect(0, 0, Application::Width, Application::Height), viewFbo, IntRect(0, 0, Application::Width, Application::Height), true, false, FILTER_POINT);
	}

	{
		PROFILE(Present);
		graphics->Present();
	}

	FrameMark;*/

	if (m_drawUi)
		renderUi();
}

void TursoInterface::OnMouseMotion(Event::MouseMoveEvent& event) {
	
}

void TursoInterface::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void TursoInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void TursoInterface::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void TursoInterface::OnKeyDown(Event::KeyboardEvent& event) {	
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void TursoInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	camera->SetAspectRatio((float)Application::Width / (float)Application::Height);
	
	if (colorBuffer->Width() != Application::Width || colorBuffer->Height() != Application::Height) {
		colorBuffer->Define(TEX_2D, IntVector2(Application::Width, Application::Height), FMT_RGBA8);
		colorBuffer->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
		depthStencilBuffer->Define(TEX_2D, IntVector2(Application::Width, Application::Height), FMT_D32);
		depthStencilBuffer->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
		normalBuffer->Define(TEX_2D, IntVector2(Application::Width, Application::Height), FMT_RGBA8);
		normalBuffer->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
		viewFbo->Define(colorBuffer, depthStencilBuffer);

		std::vector<TextureTu*> mrt;
		mrt.push_back(colorBuffer.Get());
		mrt.push_back(normalBuffer.Get());
		viewMRTFbo->Define(mrt, depthStencilBuffer);
	}

	// Similarly recreate SSAO texture if needed
	if (drawSSAO && (ssaoTexture->Width() != colorBuffer->Width() / 2 || ssaoTexture->Height() != colorBuffer->Height() / 2)) {
		ssaoTexture->Define(TEX_2D, IntVector2(colorBuffer->Width() / 2, colorBuffer->Height() / 2), FMT_R32F, 1, 1);
		ssaoTexture->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
		ssaoFbo->Define(ssaoTexture, nullptr);
	}
}


void TursoInterface::renderUi() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();

	if (m_initUi) {
		m_initUi = false;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::Checkbox("Use Occlusion", &useOcclusion);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void TursoInterface::CreateScene(Scene* scene, CameraTu* camera, int preset)
{
	rotatingObjects.clear();
	animatingObjects.clear();

	ResourceCache* cache = ObjectTu::Subsystem<ResourceCache>();

	scene->Clear();
	scene->CreateChild<Octree>("octree");
	m_octree = scene->FindChild<Octree>();
	//m_octree = new Octree();
	LightEnvironment* lightEnvironment = scene->CreateChild<LightEnvironment>();
	SetRandomSeed(1);

	// Preset 0: occluders, static meshes and many local shadowcasting lights in addition to ambient light
	if (preset == 0)
	{
		lightEnvironment->SetAmbientColor(Color(0.3f, 0.3f, 0.3f));
		camera->SetFarClip(1000.0f);


		{
			StaticModel* model = new StaticModel();
			m_octree->QueueUpdate(model->GetDrawable());
			model->SetStatic(true);
			model->SetPosition(Vector3(0, -0.05f, 0));
			model->SetScale(Vector3(100.0f, 0.1f, 100.0f));
			model->SetModel(cache->LoadResource<Model>("Box.mdl"));
			model->SetMaterial(cache->LoadResource<MaterialTu>("Stone.json"));
		}

		{
			AnimatedModel* object = new AnimatedModel();
			m_octree->QueueUpdate(object->GetDrawable());
			static_cast<AnimatedModelDrawable*>(object->GetDrawable())->SetOctree(m_octree);

			object->SetStatic(true);
			object->SetPosition(Vector3(Random() * 90.0f - 45.0f, 0.0f, Random() * 90.0f - 45.0f));
			object->SetRotation(QuaternionTu(Random(360.0f), Vector3::UP));
			object->SetModel(cache->LoadResource<Model>("Jack.mdl"));
			object->SetCastShadows(true);
			object->SetMaxDistance(600.0f);
			AnimationState* state = object->AddAnimationState(cache->LoadResource<Animation>("Jack_Walk.ani"));
			state->SetWeight(1.0f);
			state->SetLooped(true);
			animatingObjects.push_back(object);
		}
	}
	// Preset 1: high number of animating cubes
	else if (preset == 1)
	{
		lightEnvironment->SetFogColor(Color(0.3f, 0.3f, 0.3f));
		lightEnvironment->SetFogStart(300.0f);
		lightEnvironment->SetFogEnd(500.0f);
		camera->SetFarClip(500.0f);

		SharedPtr<MaterialTu> customMat = MaterialTu::DefaultMaterial()->Clone();
		customMat->SetUniform("matDiffColor", Vector4(0.75f, 0.35f, 0.0f, 1.0f));
		customMat->SetUniform("matSpecColor", Vector4(0.75f / 3.0f, 0.35f / 3.0f, 0.0f, 1.0f));

		for (int y = -125; y <= 125; ++y)
		{
			for (int x = -125; x <= 125; ++x)
			{
				StaticModel* object = scene->CreateChild<StaticModel>();
				//object->SetStatic(true);
				object->SetPosition(Vector3(x * 0.3f, 0.0f, y * 0.3f));
				object->SetScale(0.25f);
				object->SetModel(cache->LoadResource<Model>("Box.mdl"));
				object->SetMaterial(customMat);
				rotatingObjects.push_back(object);
			}
		}

		Light* light = scene->CreateChild<Light>();
		light->SetLightType(LIGHT_DIRECTIONAL);
		light->SetColor(Color(1.0f, 1.0f, 1.0f, 0.5f));
		light->SetRotation(QuaternionTu(45.0f, 45.0f, 0.0f));
	}
	// Preset 2: skinned characters with directional light shadows
	else if (preset == 2)
	{
		lightEnvironment->SetFogColor(Color(0.5f, 0.5f, 0.75f));
		lightEnvironment->SetFogStart(300.0f);
		lightEnvironment->SetFogEnd(500.0f);
		camera->SetFarClip(500.0f);

		{
			StaticModel* object = scene->CreateChild<StaticModel>();
			object->SetStatic(true);
			object->SetPosition(Vector3(0, -0.05f, 0));
			object->SetScale(Vector3(100.0f, 0.1f, 100.0f));
			object->SetModel(cache->LoadResource<Model>("Box.mdl"));
			object->SetMaterial(cache->LoadResource<MaterialTu>("Stone.json"));
		}

		for (int i = 0; i < 500; ++i)
		{
			AnimatedModel* object = scene->CreateChild<AnimatedModel>();
			object->SetStatic(true);
			object->SetPosition(Vector3(Random() * 90.0f - 45.0f, 0.0f, Random() * 90.0f - 45.0f));
			object->SetRotation(QuaternionTu(Random(360.0f), Vector3::UP));
			object->SetModel(cache->LoadResource<Model>("Jack.mdl"));
			object->SetCastShadows(true);
			object->SetMaxDistance(600.0f);
			AnimationState* state = object->AddAnimationState(cache->LoadResource<Animation>("Jack_Walk.ani"));
			state->SetWeight(1.0f);
			state->SetLooped(true);
			animatingObjects.push_back(object);
		}

		Light* light = scene->CreateChild<Light>();
		light->SetLightType(LIGHT_DIRECTIONAL);
		light->SetCastShadows(true);
		light->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		light->SetRotation(QuaternionTu(45.0f, 45.0f, 0.0f));
		light->SetShadowMapSize(2048);
		light->SetShadowMaxDistance(100.0f);
	}
}

void TursoInterface::HandleUpdate(EventTu& eventType) {
	//std::cout << "Hello from Event: " << std::endl;
}

void TursoInterface::CollectOctantsWork(Task* task_, unsigned int idx) {

	CollectOctantsTask* task = static_cast<CollectOctantsTask*>(task_);

	Octant* octant = task->startOctant;
	ThreadOctantResult2& result = octantResults[task->resultIdx];

	CollectOctantsAndLights(octant, result);
	numPendingBatchTasks.fetch_add(-1);
}



void TursoInterface::updateOctree() {
	renderer->rootLevelOctants.clear();
	renderer->opaqueBatches.Clear();

	for (size_t i = 0; i < NUM_OCTANT_TASKS; ++i)
		renderer->octantResults[i].Clear();

	// Process moved / animated objects' octree reinsertions
	m_octree->Update(0);
	m_octree->FinishUpdate();

	// Enable threaded update during geometry / light gathering in case nodes' OnPrepareRender() causes further reinsertion queuing
	m_octree->SetThreadedUpdate(workQueue->NumThreads() > 1);

	// Find the starting points for octree traversal. Include the root if it contains drawables that didn't fit elsewhere
	Octant* rootOctant = m_octree->Root();
	if (rootOctant->Drawables().size())
		renderer->rootLevelOctants.push_back(rootOctant);

	for (size_t i = 0; i < NUM_OCTANTS; ++i) {
		if (rootOctant->Child(i)) {
			renderer->rootLevelOctants.push_back(rootOctant->Child(i));
		}
	}

	renderer->numPendingBatchTasks.store((int)renderer->rootLevelOctants.size());

	for (size_t i = 0; i < renderer->rootLevelOctants.size(); ++i) {
		renderer->collectOctantsTasks[i]->startOctant = renderer->rootLevelOctants[i];
	}

	workQueue->QueueTasks(renderer->rootLevelOctants.size(), reinterpret_cast<Task**>(&renderer->collectOctantsTasks[0]));

	while (renderer->numPendingBatchTasks.load() > 0)
		workQueue->TryComplete();

	workQueue->Complete();
	m_octree->SetThreadedUpdate(false);

}

void TursoInterface::CollectOctantsAndLights(Octant* octant, ThreadOctantResult2& result, unsigned char planeMask) {
	const BoundingBox& octantBox = octant->CullingBox();

	if (planeMask) {
		// If not already inside all frustum planes, do frustum test and terminate if completely outside
		//planeMask = frustum.IsInsideMasked(octantBox, planeMask);
		if (planeMask == 0xff) {
			// If octant becomes frustum culled, reset its visibility for when it comes back to view, including its children
			if (false && octant->Visibility() != VIS_OUTSIDE_FRUSTUM)
				octant->SetVisibility(VIS_OUTSIDE_FRUSTUM, true);
			return;
		}
	}

	// When occlusion not in use, reset all traversed octants to visible-unknown
	octant->SetVisibility(VIS_VISIBLE_UNKNOWN, false);


	const std::vector<Drawable*>& drawables = octant->Drawables();

	for (auto it = drawables.begin(); it != drawables.end(); ++it) {
		Drawable* drawable = *it;

		if (!drawable->TestFlag(DF_LIGHT)) {
			result.octants.push_back(std::make_pair(octant, planeMask));
			result.drawableAcc += drawables.end() - it;
		}

	}

	// Root octant is handled separately. Otherwise recurse into child octants
	if (octant != m_octree->Root() && octant->HasChildren()) {
		for (size_t i = 0; i < NUM_OCTANTS; ++i) {
			if (octant->Child(i))
				CollectOctantsAndLights(octant->Child(i), result, planeMask);
		}
	}
}

void ThreadOctantResult2::Clear(){
	drawableAcc = 0;
	taskOctantIdx = 0;
	batchTaskIdx = 0;
	octants.clear();
}