#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "OctreeInterface.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

#include <tracy/Tracy.hpp>

void OctreeInterface::PreTickCallback(btDynamicsWorld* world, btScalar timeStep) {
	static_cast<OctreeInterface*>(world->getWorldUserInfo())->preStep(timeStep);
}

void OctreeInterface::PostTickCallback(btDynamicsWorld* world, btScalar timeStep) {
	static_cast<OctreeInterface*>(world->getWorldUserInfo())->postStep(timeStep);
}

OctreeInterface::OctreeInterface(StateMachine& machine) : State(machine, CurrentState::OCTREEINTERFACE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 5.0f, -60.0f) + Vector3f(0.0f, 0.0f, m_offsetDistance), Vector3f(0.0f, 5.0f, -60.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setOffsetDistance(m_offsetDistance);

	m_camera.setRotationSpeed(m_rotationSpeed);
	m_camera.setMovingSpeed(2.0f);

	ShapeDrawer::Get().init(32768);
	ShapeDrawer::Get().setCamera(m_camera);

	createPhysics();

	glClearColor(0.7f, 0.7f, 0.7f, 0.0f);

	bool useThreads = true;

	// Create subsystems that don't depend on the application window / OpenGL context
	workQueue = new WorkQueue(useThreads ? 0 : 1);
	log = new Log();
	cache = new ResourceCache();
	cache->AddResourceDir("Data");

	// Create the Graphics subsystem to open the application window and initialize OpenGL
	graphics = new Graphics("Turso3D renderer test", IntVector2(Application::Width, Application::Height));
	graphics->Initialize();
	hasInstancing = graphics->HasInstancing();
	if (hasInstancing){
		instanceVertexBuffer = new VertexBuffer();
		instanceVertexElements.push_back(VertexElement(ELEM_VECTOR4, SEM_TEXCOORD, 3));
		instanceVertexElements.push_back(VertexElement(ELEM_VECTOR4, SEM_TEXCOORD, 4));
		instanceVertexElements.push_back(VertexElement(ELEM_VECTOR4, SEM_TEXCOORD, 5));
	}
	// Create subsystems that depend on the application window / OpenGL
	RegisterRendererLibrary();

	debugRenderer = new DebugRenderer();


	scene = ObjectTu::Create<Scene>();
	camera = ObjectTu::Create<CameraTu>();

	CreateScene(scene, camera, 0);

	camera->SetPosition(Vector3(0.0f, 20.0f, 75.0f));
	camera->SetAspectRatio((float)Application::Width / (float)Application::Height);

	//m_camera.setPosition(Vector3f(0.0f, 20.0f, 75.0f));
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);


	perViewDataBuffer = new UniformBuffer();
	perViewDataBuffer->Define(USAGE_DYNAMIC, sizeof(PerViewUniforms));

	octantResults = new OctreeInterface::ThreadOctantResult[NUM_OCTANT_TASKS];
	batchResults = new OctreeInterface::ThreadBatchResult[workQueue->NumThreads()];
	for (size_t i = 0; i < NUM_OCTANTS + 1; ++i) {
		collectOctantsTasks[i] = new OctreeInterface::CollectOctantsTask(this, &OctreeInterface::CollectOctantsWork);
		collectOctantsTasks[i]->resultIdx = i;
	}
}

OctreeInterface::~OctreeInterface() {
	Globals::physics->removeAllCollisionObjects();
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}


void OctreeInterface::preStep(btScalar timeStep) {
	m_character->FixedUpdate(timeStep);
}

void OctreeInterface::fixedUpdate() {
	m_character->FixedUpdate(m_fdt);
	Globals::physics->stepSimulation(m_fdt);
	m_character->FixedPostUpdate(m_fdt);
}

void OctreeInterface::postStep(btScalar timeStep) {
	m_character->FixedPostUpdate(timeStep);
}

void OctreeInterface::update() {

	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyPressed(Keyboard::KEY_F1))
		CreateScene(scene, camera, 0);
	if (keyboard.keyPressed(Keyboard::KEY_F2))
		CreateScene(scene, camera, 1);
	if (keyboard.keyPressed(Keyboard::KEY_F3))
		CreateScene(scene, camera, 2);
	if (keyboard.keyPressed(Keyboard::KEY_4))
		m_drawDebug = !m_drawDebug;
	if (keyboard.keyPressed(Keyboard::KEY_5))
		m_debugPhysic = !m_debugPhysic;
	
	Vector3f directrion = Vector3f();
	bool move = false;
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}
	Vector3 pos = beta->Position();
	m_camera.Camera::setTarget(Vector3f(pos.x, pos.y, pos.z));

	Mouse &mouse = Mouse::instance();
	float dx = 0.0f;
	float dy = 0.0f;

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy, Vector3f(pos.x, pos.y, pos.z));
			beta->Rotate(QuaternionTu(-dx  * m_rotationSpeed, Vector3::UP));
		}

		/*if (move) {
			float moveSpeed = (keyboard.keyDown(Keyboard::KEY_LSHIFT) || keyboard.keyDown(Keyboard::KEY_RSHIFT)) ? 50.0f : 5.0f;
			m_camera.move(directrion  * m_dt * moveSpeed);
		}*/
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
		
		//if (beta) {
		//	AnimationState* state = beta->AnimationStates()[0];
		//	state->AddTime(m_dt);
		//}
	}

	++frameNumber;
	if (!frameNumber)
		++frameNumber;

	updateOctree();

	animController->Update(m_dt);
}

void OctreeInterface::renderDirect() {

	Graphics::BindDefaultVao(true);

	debugRenderer->SetView(camera);
	debugRenderer->SetViewProjection(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());

	glClearColor(DEFAULT_FOG_COLOR.r, DEFAULT_FOG_COLOR.g, DEFAULT_FOG_COLOR.b, DEFAULT_FOG_COLOR.a);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	UpdateInstanceTransforms(instanceTransforms);
	RenderBatches(camera, opaqueBatches);

	if (m_drawDebug) {
		DebugRenderer* debug = Subsystem<DebugRenderer>();
		for (size_t i = 0; i < rootLevelOctants.size(); ++i) {
			const ThreadOctantResult& result = octantResults[i];

			for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
				Octant* octant = oIt->first;
				octant->OnRenderDebug(debug);
				const std::vector<Drawable*>& drawables = octant->Drawables();

				for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt) {
					Drawable* drawable = *dIt;
					if (drawable->TestFlag(DF_GEOMETRY) && drawable->LastFrameNumber() == frameNumber)
						drawable->OnRenderDebug(debug);
				}
			}
		}
	}

	debugRenderer->Render();

	Graphics::UnbindDefaultVao();

	if (m_debugPhysic) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ShapeDrawer::Get().drawDynmicsWorld(Physics::GetDynamicsWorld());
		glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
	}

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getPerspectiveMatrix()[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getViewMatrix()[0][0]);

	kinematicCharacter->DebugDrawContacts();*/
}

void OctreeInterface::render() {
	
	renderDirect();

	if (m_drawUi)
		renderUi();
}

void OctreeInterface::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void OctreeInterface::OnMouseWheel(Event::MouseWheelEvent& event) {
	if (event.direction == 1u) {
		m_offsetDistance += 2.0f;
		m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
		m_camera.setOffsetDistance(m_offsetDistance);
	}

	if (event.direction == 0u) {
		m_offsetDistance -= 2.0f;
		m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
		m_camera.setOffsetDistance(m_offsetDistance);
	}
}

void OctreeInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void OctreeInterface::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void OctreeInterface::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void OctreeInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	camera->SetAspectRatio((float)Application::Width / (float)Application::Height);
}


void OctreeInterface::renderUi() {
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
	ImGui::Checkbox("Draw Debug", &m_drawDebug);
	ImGui::Checkbox("Debug Physic", &m_debugPhysic);
	ImGui::Checkbox("Use Culling", &m_useCulling);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OctreeInterface::CreateScene(Scene* scene, CameraTu* camera, int preset) {
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
			/*StaticModel* object = new StaticModel();
			m_octree->QueueUpdate(object->GetDrawable());
			object->SetStatic(true);
			object->SetPosition(Vector3(0, -0.05f, 0));
			object->SetScale(Vector3(100.0f, 0.1f, 100.0f));
			object->SetModel(cache->LoadResource<Model>("Box.mdl"));
			object->SetMaterial(cache->LoadResource<MaterialTu>("Stone.json"));*/
		}

		{
			/*AnimatedModel* object = new AnimatedModel();
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
			animatingObjects.push_back(object);*/
		}

		{
			beta = new AnimatedModel();
			m_octree->QueueUpdate(beta->GetDrawable());
			static_cast<AnimatedModelDrawable*>(beta->GetDrawable())->SetOctree(m_octree);

 			beta->SetStatic(true);
			beta->SetPosition(Vector3(28.0f, 8.0f, -4.0f));
			//beta->SetRotation(QuaternionTu(225.0f, Vector3::UP));
			beta->SetScale(1.0f);
			beta->SetModel(cache->LoadResource<Model>("Beta/Beta.mdl"));
			beta->SetMaterial(cache->LoadResource<MaterialTu>("Beta/Beta.json"));
			beta->SetCastShadows(true);
			beta->SetMaxDistance(600.0f);
			animController = new AnimationController(beta);

			StaticModel* base = new StaticModel();
			m_octree->QueueUpdate(base->GetDrawable());
			base->SetStatic(true);
			base->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
			base->SetScale(Vector3(0.01f, 0.01f, 0.01f));
			base->SetModel(cache->LoadResource<Model>("Models/base.mdl"));
			base->SetMaterial(cache->LoadResource<MaterialTu>("Models/Models.json"));

			StaticModel* upperFloor = new StaticModel();
			m_octree->QueueUpdate(upperFloor->GetDrawable());
			upperFloor->SetStatic(true);
			upperFloor->SetPosition(Vector3(30.16f, 6.98797f, 10.0099f));
			upperFloor->SetScale(Vector3(0.01f, 0.01f, 0.01f));
			upperFloor->SetModel(cache->LoadResource<Model>("Models/upperFloor.mdl"));
			upperFloor->SetMaterial(cache->LoadResource<MaterialTu>("Models/Models.json"));

			StaticModel* ramp1 = new StaticModel();
			m_octree->QueueUpdate(ramp1->GetDrawable());
			ramp1->SetStatic(true);
			ramp1->SetPosition(Vector3(13.5771f, 6.23965f, 10.9272f));
			ramp1->SetScale(Vector3(0.01f, 0.01f, 0.01f));
			ramp1->SetModel(cache->LoadResource<Model>("Models/ramp.mdl"));
			ramp1->SetMaterial(cache->LoadResource<MaterialTu>("Models/Models.json"));

			StaticModel* ramp2 = new StaticModel();
			m_octree->QueueUpdate(ramp2->GetDrawable());
			ramp2->SetStatic(true);
			ramp2->SetPosition(Vector3(-22.8933f, 2.63165f, -23.6786f));
			ramp2->SetScale(Vector3(0.01f, 0.01f, 0.01f));
			ramp2->SetModel(cache->LoadResource<Model>("Models/ramp2.mdl"));
			ramp2->SetMaterial(cache->LoadResource<MaterialTu>("Models/Models.json"));

			StaticModel* ramp3 = new StaticModel();
			m_octree->QueueUpdate(ramp3->GetDrawable());
			ramp3->SetStatic(true);
			ramp3->SetPosition(Vector3(-15.2665f, 1.9782f, -43.135f));
			ramp3->SetScale(Vector3(0.01f, 0.01f, 0.01f));
			ramp3->SetModel(cache->LoadResource<Model>("Models/ramp3.mdl"));
			ramp3->SetMaterial(cache->LoadResource<MaterialTu>("Models/Models.json"));

			StaticModel* lift = new StaticModel();
			m_octree->QueueUpdate(lift->GetDrawable());
			lift->SetStatic(true);
			lift->SetPosition(Vector3(35.5938f, 0.350185f, 10.4836f));
			lift->SetScale(Vector3(0.01f, 0.01f, 0.01f));
			lift->SetModel(cache->LoadResource<Model>("Models/Lift.mdl"));
			lift->SetMaterial(cache->LoadResource<MaterialTu>("Models/Models.json"));

			StaticModel* liftExterior = new StaticModel();
			m_octree->QueueUpdate(liftExterior->GetDrawable());
			liftExterior->SetStatic(true);
			liftExterior->SetPosition(Vector3(35.6211f, 7.66765f, 10.4388f));
			liftExterior->SetScale(Vector3(0.01f, 0.01f, 0.01f));
			liftExterior->SetModel(cache->LoadResource<Model>("Models/liftExterior.mdl"));
			liftExterior->SetMaterial(cache->LoadResource<MaterialTu>("Models/Models.json"));

			StaticModel* liftButton = new StaticModel();
			m_octree->QueueUpdate(liftButton->GetDrawable());
			liftButton->SetStatic(true);
			liftButton->SetPosition(Vector3(35.5938f, 0.412104f, 10.4836f));
			liftButton->SetScale(Vector3(0.01f, 0.01f, 0.01f));
			liftButton->SetModel(cache->LoadResource<Model>("Models/LiftButton.mdl"));
			liftButton->SetMaterial(cache->LoadResource<MaterialTu>("Models/Models.json"));

			StaticModel* disk1 = new StaticModel();
			m_octree->QueueUpdate(disk1->GetDrawable());
			disk1->SetStatic(true);
			disk1->SetPosition(Vector3(26.1357f, 7.00645f, -34.7563f));
			disk1->SetScale(Vector3(0.01f, 0.01f, 0.01f));
			disk1->SetModel(cache->LoadResource<Model>("Models/disk.mdl"));
			disk1->SetMaterial(cache->LoadResource<MaterialTu>("Models/Models.json"));

			StaticModel* disk2 = new StaticModel();
			m_octree->QueueUpdate(disk2->GetDrawable());
			disk2->SetStatic(true);
			disk2->SetPosition(Vector3(4.14317f, 7.00645f, 35.1134f));
			disk2->SetScale(Vector3(0.01f, 0.01f, 0.01f));
			disk2->SetModel(cache->LoadResource<Model>("Models/disk.mdl"));
			disk2->SetMaterial(cache->LoadResource<MaterialTu>("Models/Models.json"));

			StaticModel* cylinder = new StaticModel();
			m_octree->QueueUpdate(cylinder->GetDrawable());
			cylinder->SetStatic(true);
			cylinder->SetPosition(Vector3(-0.294956f, 2.48167f, 28.3161f));
			cylinder->SetScale(Vector3(6.0f, 6.0f, 6.0f));
			cylinder->SetModel(cache->LoadResource<Model>("Models/Cylinder.mdl"));
			cylinder->SetMaterial(cache->LoadResource<MaterialTu>("Models/Models.json"));

			kinematicCharacter = new KinematicCharacterController();

			m_character = new Character(beta, animController, kinematicCharacter);
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
				object->SetStatic(true);
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

void OctreeInterface::updateOctree() {

	frustum = camera->WorldFrustum();
	frustum.Define(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	viewMask = camera->ViewMask();
	rootLevelOctants.clear();
	opaqueBatches.Clear();
	instanceTransforms.clear();
	geometryBounds.Undefine();

	for (size_t i = 0; i < NUM_OCTANT_TASKS; ++i)
		octantResults[i].Clear();

	for (size_t i = 0; i < workQueue->NumThreads(); ++i)
		batchResults[i].Clear();

	// Process moved / animated objects' octree reinsertions
	m_octree->Update(frameNumber);
	m_octree->FinishUpdate();

	// Enable threaded update during geometry / light gathering in case nodes' OnPrepareRender() causes further reinsertion queuing
	m_octree->SetThreadedUpdate(workQueue->NumThreads() > 1);

	// Find the starting points for octree traversal. Include the root if it contains drawables that didn't fit elsewhere
	Octant* rootOctant = m_octree->Root();
	if (rootOctant->Drawables().size()) {
		rootLevelOctants.push_back(rootOctant);
	}

	for (size_t i = 0; i < NUM_OCTANTS; ++i) {
		if (rootOctant->Child(i)) {
			rootLevelOctants.push_back(rootOctant->Child(i));
		}
	}

	numPendingBatchTasks.store((int)rootLevelOctants.size());

	for (size_t i = 0; i < rootLevelOctants.size(); ++i) {
		collectOctantsTasks[i]->startOctant = rootLevelOctants[i];
	}

	workQueue->QueueTasks(rootLevelOctants.size(), reinterpret_cast<Task**>(&collectOctantsTasks[0]));

	while (numPendingBatchTasks.load() > 0)
		workQueue->TryComplete();

	SortMainBatches();
	workQueue->Complete();
	m_octree->SetThreadedUpdate(false);
}

void OctreeInterface::ThreadOctantResult::Clear() {
	drawableAcc = 0;
	taskOctantIdx = 0;
	batchTaskIdx = 0;
	octants.clear();
	occlusionQueries.clear();
}

void OctreeInterface::ThreadBatchResult::Clear(){
	minZ = M_MAX_FLOAT;
	maxZ = 0.0f;
	geometryBounds.Undefine();
	opaqueBatches.clear();
}

void OctreeInterface::RegisterRendererLibrary() {
	static bool registered = false;
	if (registered)
		return;

	// Scene node base attributes are needed
	RegisterSceneLibrary();
	Octree::RegisterObject();
	Bone::RegisterObject();
	CameraTu::RegisterObject();
	OctreeNode::RegisterObject();
	GeometryNode::RegisterObject();
	StaticModel::RegisterObject();
	AnimatedModel::RegisterObject();
	Light::RegisterObject();
	LightEnvironment::RegisterObject();
	MaterialTu::RegisterObject();
	Model::RegisterObject();
	Animation::RegisterObject();

	registered = true;
}

void OctreeInterface::RenderBatches(CameraTu* camera_, const BatchQueue& queue) {
	//fill in uniforms
	float nearClip = camera->NearClip();
	float farClip = camera->FarClip();
	perViewData.projectionMatrix = camera->ProjectionMatrix();
	perViewData.viewMatrix = camera->ViewMatrix();
	perViewData.viewProjMatrix = camera->ViewMatrix() * camera->ProjectionMatrix();
	//perViewData.viewProjMatrix = camera->ProjectionMatrix() * camera->ViewMatrix() ;

	perViewData.depthParameters = Vector4(nearClip, farClip, camera->IsOrthographic() ? 0.5f : 0.0f, camera->IsOrthographic() ? 0.5f : 1.0f / farClip);
	perViewData.cameraPosition = Vector4(camera->WorldPosition(), 1.0f);
	perViewData.ambientColor = DEFAULT_AMBIENT_COLOR;
	perViewData.view = m_camera.getViewMatrix();
	perViewData.projection = m_camera.getPerspectiveMatrix();
	perViewData.viewProjection = m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix();

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

	short count = 0;

	for (auto it = queue.batches.begin(); it != queue.batches.end(); ++it){
		const Batch& batch = *it;
		unsigned char geometryBits = batch.programBits & SP_GEOMETRYBITS;

		ShaderProgram* program = batch.pass->GetShaderProgram(batch.programBits);
		if (!program->Bind(count == 0))
			continue;

		MaterialTu* material = batch.pass->Parent();
		
		for (size_t i = 0; i < MAX_MATERIAL_TEXTURE_UNITS; ++i){
			TextureTu* texture = material->GetTexture(i);
			if (texture)
				texture->Bind(i);
		}

		UniformBuffer* materialUniforms = material->GetUniformBuffer();
		if (materialUniforms)
			materialUniforms->Bind(UB_MATERIALDATA);

		CullMode cullMode = material->GetCullMode();		
		graphics->SetRenderState(batch.pass->GetBlendMode(), cullMode, batch.pass->GetDepthTest(), batch.pass->GetColorWrite(), batch.pass->GetDepthWrite());

		Geometry* geometry = batch.geometry;
		VertexBuffer* vb = geometry->vertexBuffer;
		IndexBuffer* ib = geometry->indexBuffer;

		vb->Bind(program->Attributes(), count == 0);
		if (ib)
			ib->Bind(count == 0);

		if (geometryBits == GEOM_INSTANCED){
			if (ib)
				graphics->DrawIndexedInstanced(PT_TRIANGLE_LIST, geometry->drawStart, geometry->drawCount, instanceVertexBuffer, batch.instanceStart, batch.instanceCount);
			else
				graphics->DrawInstanced(PT_TRIANGLE_LIST, geometry->drawStart, geometry->drawCount, instanceVertexBuffer, batch.instanceStart, batch.instanceCount);

			it += batch.instanceCount - 1;
		}else{

			if (!geometryBits) {
				graphics->SetUniform(program, U_WORLDMATRIX, *batch.worldTransform);
				//graphics->SetUniform(program, "worldMatrix", *batch.worldTransform);
				//graphics->SetUniform(program, "worldMatrix4", Matrix4(*batch.worldTransform));
			}else
				batch.drawable->OnRender(program, batch.geomIndex);

			if (ib)
				graphics->DrawIndexed(PT_TRIANGLE_LIST, geometry->drawStart, geometry->drawCount);
			else
				graphics->Draw(PT_TRIANGLE_LIST, geometry->drawStart, geometry->drawCount);
		}

		count++;
	}

}

void OctreeInterface::CollectOctants(Octant* octant, ThreadOctantResult& result, unsigned char planeMask) {
	const BoundingBox& octantBox = octant->CullingBox();

	if (planeMask) {
		// If not already inside all frustum planes, do frustum test and terminate if completely outside
		planeMask = m_useCulling ? frustum.IsInsideMasked(octantBox, planeMask) : 0x00;
		if (planeMask == 0xff) {
			// If octant becomes frustum culled, reset its visibility for when it comes back to view, including its children
			if (octant->Visibility() != VIS_OUTSIDE_FRUSTUM)
				octant->SetVisibility(VIS_OUTSIDE_FRUSTUM, true);
			return;
		}
	}

	octant->SetVisibility(VIS_VISIBLE, false);

	const std::vector<Drawable*>& drawables = octant->Drawables();

	for (auto it = drawables.begin(); it != drawables.end(); ++it) {
		Drawable* drawable = *it;

		if (drawable->TestFlag(DF_STATIC)) {
			result.octants.push_back(std::make_pair(octant, planeMask));
			result.drawableAcc += drawables.end() - it;
			break;
		}
	}

	if (result.drawableAcc >= DRAWABLES_PER_BATCH_TASK) {
		if (result.collectBatchesTasks.size() <= result.batchTaskIdx)
			result.collectBatchesTasks.push_back(new CollectBatchesTask(this, &OctreeInterface::CollectBatchesWork));

		CollectBatchesTask* batchTask = result.collectBatchesTasks[result.batchTaskIdx];
		batchTask->octants.clear();
		batchTask->octants.insert(batchTask->octants.end(), result.octants.begin() + result.taskOctantIdx, result.octants.end());
		numPendingBatchTasks.fetch_add(1);
		workQueue->QueueTask(batchTask);

		result.drawableAcc = 0;
		result.taskOctantIdx = result.octants.size();
		++result.batchTaskIdx;
	}

	if (octant != m_octree->Root() && octant->HasChildren()) {
		for (size_t i = 0; i < NUM_OCTANTS; ++i){
			if (octant->Child(i))
				CollectOctants(octant->Child(i), result, planeMask);
		}
	}
}

void OctreeInterface::SortMainBatches() {

	for (size_t i = 0; i < workQueue->NumThreads(); ++i){
		ThreadBatchResult& res = batchResults[i];
		if (res.geometryBounds.IsDefined())
			geometryBounds.Merge(res.geometryBounds);
	}

	// Join per-thread collected batches and sort
	for (size_t i = 0; i < workQueue->NumThreads(); ++i) {
		ThreadBatchResult& res = batchResults[i];
		if (res.opaqueBatches.size())
			opaqueBatches.batches.insert(opaqueBatches.batches.end(), res.opaqueBatches.begin(), res.opaqueBatches.end());

	}

	opaqueBatches.Sort(instanceTransforms, SORT_STATE_AND_DISTANCE, hasInstancing);
}


void OctreeInterface::CollectOctantsWork(Task* task_, unsigned threadIndex) {

	CollectOctantsTask* task = static_cast<CollectOctantsTask*>(task_);
	Octant* octant = task->startOctant;
	ThreadOctantResult& result = octantResults[task->resultIdx];

	CollectOctants(octant, result);

	if (result.drawableAcc) {
		if (result.collectBatchesTasks.size() <= result.batchTaskIdx)
			result.collectBatchesTasks.push_back(new CollectBatchesTask(this, &OctreeInterface::CollectBatchesWork));

		CollectBatchesTask* batchTask = result.collectBatchesTasks[result.batchTaskIdx];
		batchTask->octants.clear();
		batchTask->octants.insert(batchTask->octants.end(), result.octants.begin() + result.taskOctantIdx, result.octants.end());
		numPendingBatchTasks.fetch_add(1);

		workQueue->QueueTask(batchTask);
	}

	numPendingBatchTasks.fetch_add(-1);
}

void OctreeInterface::CollectBatchesWork(Task* task_, unsigned threadIndex) {

	CollectBatchesTask* task = static_cast<CollectBatchesTask*>(task_);
	ThreadBatchResult& result = batchResults[threadIndex];
	bool threaded = workQueue->NumThreads() > 1;

	std::vector<std::pair<Octant*, unsigned char> >& octants = task->octants;
	std::vector<Batch>& opaqueQueue = threaded ? result.opaqueBatches : opaqueBatches.batches;


	const Matrix3x4& viewMatrix = camera->ViewMatrix();
	Vector3 viewZ = Vector3(viewMatrix.m20, viewMatrix.m21, viewMatrix.m22);
	Vector3 absViewZ = viewZ.Abs();
	float farClipMul = 32767.0f / camera->FarClip();

	// Scan octants for geometries
	for (auto it = octants.begin(); it != octants.end(); ++it){
		Octant* octant = it->first;
		unsigned char planeMask = it->second;
		const std::vector<Drawable*>& drawables = octant->Drawables();

		for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt){
			Drawable* drawable = *dIt;

			if (drawable->TestFlag(DF_GEOMETRY) && (drawable->LayerMask() & viewMask)){
				const BoundingBox& geometryBox = drawable->WorldBoundingBox();

				// Note: to strike a balance between performance and occlusion accuracy, per-geometry occlusion tests are skipped for now,
				// as octants are already tested with combined actual drawable bounds
				if ((!planeMask || frustum.IsInsideMaskedFast(geometryBox, planeMask)) && drawable->OnPrepareRender(frameNumber, camera)){

					result.geometryBounds.Merge(geometryBox);

					Vector3 center = geometryBox.Center();
					Vector3 edge = geometryBox.Size() * 0.5f;

					float viewCenterZ = viewZ.DotProduct(center) + viewMatrix.m23;
					float viewEdgeZ = absViewZ.DotProduct(edge);
					result.minZ = Min(result.minZ, viewCenterZ - viewEdgeZ);
					result.maxZ = Max(result.maxZ, viewCenterZ + viewEdgeZ);

					Batch newBatch;

					unsigned short distance = (unsigned short)(drawable->Distance() * farClipMul);
					const SourceBatches& batches = static_cast<GeometryDrawable*>(drawable)->Batches();
					size_t numGeometries = batches.NumGeometries();

					for (size_t j = 0; j < numGeometries; ++j){
						MaterialTu* material = batches.GetMaterial(j);

						// Assume opaque first
						newBatch.pass = material->GetPass(PASS_OPAQUE);
						newBatch.geometry = batches.GetGeometry(j);
						newBatch.programBits = (unsigned char)(drawable->Flags() & DF_GEOMETRY_TYPE_BITS);
						newBatch.geomIndex = (unsigned char)j;

						if (!newBatch.programBits)
							newBatch.worldTransform = &drawable->WorldTransform();
						else
							newBatch.drawable = static_cast<GeometryDrawable*>(drawable);

						if (newBatch.pass){
							// Perform distance sort in addition to state sort
							if (newBatch.pass->lastSortKey.first != frameNumber || newBatch.pass->lastSortKey.second > distance){
								newBatch.pass->lastSortKey.first = frameNumber;
								newBatch.pass->lastSortKey.second = distance;
							}

							if (newBatch.geometry->lastSortKey.first != frameNumber || newBatch.geometry->lastSortKey.second > distance + (unsigned short)j){
								newBatch.geometry->lastSortKey.first = frameNumber;
								newBatch.geometry->lastSortKey.second = distance + (unsigned short)j;
							}

							opaqueQueue.push_back(newBatch);
						}

					}
				}
			}
		}
	}

	numPendingBatchTasks.fetch_add(-1);
}


void OctreeInterface::UpdateInstanceTransforms(const std::vector<Matrix3x4>& transforms){
	if (hasInstancing && transforms.size()){
		if (instanceVertexBuffer->NumVertices() < transforms.size())
			instanceVertexBuffer->Define(USAGE_DYNAMIC, transforms.size(), instanceVertexElements, &transforms[0]);
		else
			instanceVertexBuffer->SetData(0, transforms.size(), &transforms[0]);
	}
}

void OctreeInterface::createPhysics() {
	

	mdlConverter.mdlToBuffer("res/Models/base.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_base.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/upperFloor.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_upperFloor.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/ramp.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_ramp.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/ramp2.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_ramp2.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/ramp3.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_ramp3.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/Lift.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_lift.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/liftExterior.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_liftExterior.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/LiftButton.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_liftButton.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/disk.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_disk.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/Cylinder.mdl", 6.0f, vertexBuffer, indexBuffer);
	m_cylinder.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();


	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(0.0f, -0.05f, 0.0f)), new btConvexHullShape((btScalar*)(&m_base.getPositions()[0]), m_base.getPositions().size(), 3 * sizeof(btScalar)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY, btCollisionObject::CF_STATIC_OBJECT);
	Globals::physics->addStaticModel(Physics::CreateStaticCollisionShapes(&m_upperFloor, 1.0f), Physics::BtTransform(btVector3(30.16f, 6.98797f, 10.0099f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY);

	Globals::physics->addStaticModel(Physics::CreateStaticCollisionShapes(&m_ramp, 1.0f), Physics::BtTransform(btVector3(13.5771f, 6.23965f, 10.9272f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY);
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(-22.8933f, 2.63165f, -23.6786f)), new btConvexHullShape((btScalar*)(&m_ramp2.getPositions()[0]), m_ramp2.getPositions().size(), 3 * sizeof(btScalar)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY, btCollisionObject::CF_STATIC_OBJECT);
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(-15.2665f, 1.9782f, -43.135f)), new btConvexHullShape((btScalar*)(&m_ramp3.getPositions()[0]), m_ramp3.getPositions().size(), 3 * sizeof(btScalar)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY, btCollisionObject::CF_STATIC_OBJECT);

	Globals::physics->addStaticModel(Physics::CreateStaticCollisionShapes(&m_lift, 1.0f), Physics::BtTransform(btVector3(35.5938f, 0.350185f, 10.4836f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY);
	Globals::physics->addStaticModel(Physics::CreateStaticCollisionShapes(&m_liftExterior, 1.0f), Physics::BtTransform(btVector3(35.6211f, 7.66765f, 10.4388f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY);
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(35.5938f, 0.412104f + 0.15f, 10.4836f)), new btCylinderShape(btVector3(80.0f, 15.0f, 1.0f) * 0.01f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY, btCollisionObject::CF_STATIC_OBJECT);

	Globals::physics->addStaticModel(Physics::CreateStaticCollisionShapes(&m_disk, 1.0f), Physics::BtTransform(btVector3(26.1357f, 7.00645f, -34.7563f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY);
	Globals::physics->addStaticModel(Physics::CreateStaticCollisionShapes(&m_disk, 1.0f), Physics::BtTransform(btVector3(4.14317f, 7.00645f, 35.1134f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY);


	
	Globals::physics->addStaticModel(Physics::CreateStaticCollisionShapes(&m_cylinder, 1.0f), Physics::BtTransform(btVector3(-0.294956f, 2.48167f, 28.3161f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY);

	//Physics::GetDynamicsWorld()->setInternalTickCallback(OctreeInterface::PreTickCallback, static_cast<void*>(this), true);
	//Physics::GetDynamicsWorld()->setInternalTickCallback(OctreeInterface::PostTickCallback, static_cast<void*>(this), false);
}
