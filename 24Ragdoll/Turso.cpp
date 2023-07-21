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
	graphics = new Graphics("Turso3D renderer test", IntVector2(1920, 1080));
	graphics->Initialize();
//		return 1;

	// Create subsystems that depend on the application window / OpenGL
	//input = new InputTu(graphics->Window());
	renderer = new Renderer();
	debugRenderer = new DebugRenderer();

	renderer->SetupShadowMaps(1024, 2048, FMT_D16);

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

	camera->SetPosition(Vector3(0.0f, 20.0f, -75.0f));
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
			float moveSpeed = (keyboard.keyDown(Keyboard::KEY_LSHIFT) || keyboard.keyDown(Keyboard::KEY_RSHIFT)) ? 500.0f : 50.0f;

			camera->Translate(directrion * m_dt * moveSpeed);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());
}

void TursoInterface::render() {

	ZoneScoped;

	// Scene animation
	if (animate)
	{
		ZoneScopedN("MoveObjects");

		PROFILE(MoveObjects);

		if (rotatingObjects.size())
		{
			angle += 100.0f * m_dt;
			QuaternionTu rotQuat(angle, Vector3::ONE);
			for (auto it = rotatingObjects.begin(); it != rotatingObjects.end(); ++it)
				(*it)->SetRotation(rotQuat);
		}
		else if (animatingObjects.size())
		{
			for (auto it = animatingObjects.begin(); it != animatingObjects.end(); ++it)
			{
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

	// Recreate rendertarget textures if window resolution changed
	int width = Application::Width;
	int height = Application::Height;

	if (colorBuffer->Width() != width || colorBuffer->Height() != height)
	{
		colorBuffer->Define(TEX_2D, IntVector2(width, height), FMT_RGBA8);
		colorBuffer->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
		depthStencilBuffer->Define(TEX_2D, IntVector2(width, height), FMT_D32);
		depthStencilBuffer->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
		normalBuffer->Define(TEX_2D, IntVector2(width, height), FMT_RGBA8);
		normalBuffer->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
		viewFbo->Define(colorBuffer, depthStencilBuffer);

		std::vector<TextureTu*> mrt;
		mrt.push_back(colorBuffer.Get());
		mrt.push_back(normalBuffer.Get());
		viewMRTFbo->Define(mrt, depthStencilBuffer);
	}

	// Similarly recreate SSAO texture if needed
	if (drawSSAO && (ssaoTexture->Width() != colorBuffer->Width() / 2 || ssaoTexture->Height() != colorBuffer->Height() / 2))
	{
		ssaoTexture->Define(TEX_2D, IntVector2(colorBuffer->Width() / 2, colorBuffer->Height() / 2), FMT_R32F, 1, 1);
		ssaoTexture->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
		ssaoFbo->Define(ssaoTexture, nullptr);
	}

	camera->SetAspectRatio((float)width / (float)height);

	// Collect geometries and lights in frustum. Also set debug renderer to use the correct camera view
	{
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

		graphics->SetViewport(IntRect(0, 0, width, height));
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
			graphics->SetUniform(program, "frustumSize", Vector4(farVec, (float)height / (float)width));
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
			graphics->SetViewport(IntRect(0, 0, width, height));
			graphics->SetUniform(program, "blurInvSize", Vector2(1.0f / ssaoTexture->Width(), 1.0f / ssaoTexture->Height()));
			graphics->SetTexture(0, ssaoTexture);
			graphics->SetRenderState(BLEND_SUBTRACT, CULL_NONE, CMP_ALWAYS, true, false);
			graphics->DrawQuad();
			graphics->SetTexture(0, nullptr);
		}

		// Render alpha geometry. Now only the color rendertarget is needed
		graphics->SetFrameBuffer(viewFbo);
		graphics->SetViewport(IntRect(0, 0, width, height));
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
		graphics->Blit(nullptr, IntRect(0, 0, width, height), viewFbo, IntRect(0, 0, width, height), true, false, FILTER_POINT);
	}

	{
		PROFILE(Present);
		graphics->Present();
	}

	FrameMark;

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
	scene->CreateChild<Octree>();
	LightEnvironment* lightEnvironment = scene->CreateChild<LightEnvironment>();

	SetRandomSeed(1);

	// Preset 0: occluders, static meshes and many local shadowcasting lights in addition to ambient light
	if (preset == 0)
	{
		lightEnvironment->SetAmbientColor(Color(0.3f, 0.3f, 0.3f));
		camera->SetFarClip(1000.0f);

		for (int y = -55; y <= 55; ++y)
		{
			for (int x = -55; x <= 55; ++x)
			{
				StaticModel* object = scene->CreateChild<StaticModel>();
				object->SetStatic(true);
				object->SetPosition(Vector3(10.5f * x, -0.05f, 10.5f * y));
				object->SetScale(Vector3(10.0f, 0.1f, 10.0f));
				object->SetModel(cache->LoadResource<Model>("Box.mdl"));
				object->SetMaterial(cache->LoadResource<MaterialTu>("Stone.json"));
			}
		}

		for (unsigned i = 0; i < 10000; ++i)
		{
			StaticModel* object = scene->CreateChild<StaticModel>();
			object->SetStatic(true);
			object->SetPosition(Vector3(Random() * 1000.0f - 500.0f, 0.0f, Random() * 1000.0f - 500.0f));
			object->SetScale(1.5f);
			object->SetModel(cache->LoadResource<Model>("Mushroom.mdl"));
			object->SetMaterial(cache->LoadResource<MaterialTu>("Mushroom.json"));
			object->SetCastShadows(true);
			object->SetLodBias(2.0f);
			object->SetMaxDistance(600.0f);
		}

		Vector3 quadrantCenters[] =
		{
			Vector3(-290.0f, 0.0f, -290.0f),
			Vector3(290.0f, 0.0f, -290.0f),
			Vector3(-290.0f, 0.0f, 290.0f),
			Vector3(290.0f, 0.0f, 290.0f),
		};

		std::vector<Light*> lights;

		for (unsigned i = 0; i < 100; ++i)
		{
			Light* light = scene->CreateChild<Light>();
			light->SetStatic(true);
			light->SetLightType(LIGHT_POINT);
			light->SetCastShadows(true);
			Vector3 colorVec = 2.0f * Vector3(Random(), Random(), Random()).Normalized();
			light->SetColor(Color(colorVec.x, colorVec.y, colorVec.z, 0.5f));
			light->SetRange(40.0f);
			light->SetShadowMapSize(256);
			light->SetShadowMaxDistance(200.0f);
			light->SetMaxDistance(900.0f);

			for (;;)
			{
				Vector3 newPos = quadrantCenters[i % 4] + Vector3(Random() * 500.0f - 250.0f, 10.0f, Random() * 500.0f - 250.0f);
				bool posOk = true;

				for (unsigned j = 0; j < lights.size(); ++j)
				{
					if ((newPos - lights[j]->Position()).Length() < 80.0f)
					{
						posOk = false;
						break;
					}
				}

				if (posOk)
				{
					light->SetPosition(newPos);
					break;
				}
			}

			lights.push_back(light);
		}

		{
			StaticModel* object = scene->CreateChild<StaticModel>();
			object->SetStatic(true);
			object->SetPosition(Vector3(0.0f, 25.0f, 0.0f));
			object->SetScale(Vector3(1165.0f, 50.0f, 1.0f));
			object->SetModel(cache->LoadResource<Model>("Box.mdl"));
			object->SetMaterial(cache->LoadResource<MaterialTu>("Stone.json"));
			object->SetCastShadows(true);
		}

		{
			StaticModel* object = scene->CreateChild<StaticModel>();
			object->SetStatic(true);
			object->SetPosition(Vector3(0.0f, 25.0f, 0.0f));
			object->SetScale(Vector3(1.0f, 50.0f, 1165.0f));
			object->SetModel(cache->LoadResource<Model>("Box.mdl"));
			object->SetMaterial(cache->LoadResource<MaterialTu>("Stone.json"));
			object->SetCastShadows(true);
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
