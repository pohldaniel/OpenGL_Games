#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <engine/DebugRenderer.h>
#include <Utils/BinaryIO.h>
#include <States/Menu.h>

#include <Physics/ShapeDrawer.h>

#include "NavigationState.h"
#include "Application.h"
#include "Renderer.h"
#include "Globals.h"

NavigationState::NavigationState(StateMachine& machine) : 
	State(machine, States::NAVIGATION), 
	m_separaionWeight(3.0f), 
	m_height(2.0f){

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 20.0f, -75.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(50.0f);
	m_offsetDistance = m_camera.getOffsetDistance();

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	glGenBuffers(1, &BuiltInShader::matrixUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4f) * 96, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 3, BuiltInShader::matrixUbo, 0, sizeof(Matrix4f) * 96);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	ShapeDrawer::Get().init(32768);
	ShapeDrawer::Get().setCamera(m_camera);

	WorkQueue::Init(0);
	Renderer::Get().init(new Octree(m_camera, m_frustum, m_dt), new SceneNodeLC());
	m_octree = Renderer::Get().getOctree();
	m_root = Renderer::Get().getScene();

	DebugRenderer::Get().setEnable(true);

	Material::AddTexture("res/textures/ProtoWhite256.jpg");
	Material::AddTexture("res/textures/ProtoGreen256.jpg");
	Material::AddTexture("res/textures/ProtoBlue256.jpg");
	Material::AddTexture("res/textures/ProtoRed256.jpg");
	Material::AddTexture("res/textures/ProtoYellow256.jpg");
	Material::AddTexture("res/models/woman/Woman.png", TextureType::TEXTURE2D, false);
	Material::AddTexture();

	m_beta.loadModelMdl("res/models/BetaLowpoly/Beta.mdl");
	m_beta.getMeshes()[0]->getMeshBones()[0].initialRotation.rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f);
	m_jack.loadModelMdl("res/models/Jack/Jack.mdl");
	m_woman.loadModelAssimp("res/models/woman/Woman.gltf", true, false);
	m_woman.getMeshes()[0]->getMeshBones()[0].initialScale.scale(0.004f, 0.004f, 0.004f);
	
	createShapes();
	createPhysics();

	m_navigationMesh = new NavigationMesh();
	createScene();

	m_navigationMesh->setPadding(Vector3f(0.0f, 10.0f, 0.0f));
	m_navigationMesh->setTileSize(8);

	m_navigationMesh->setCellSize(0.3);
	m_navigationMesh->setCellHeight(0.2f);

	m_navigationMesh->setAgentMaxSlope(45.0f);
	m_navigationMesh->setAgentMaxClimb(0.9f);
	m_navigationMesh->setAgentHeight(2.0f);
	m_navigationMesh->setAgentRadius(0.6f);
	m_navigationMesh->build();

	m_crowdManager = new CrowdManager();
	m_crowdManager->setNavigationMesh(m_navigationMesh);

	spawnJack(Vector3f( 5.0f, 0.5f, -30.0f));
	spawnBeta(Vector3f(-5.0f, 0.5f, -30.0f));
	spawnWoman(Vector3f(-10.0f, 0.5f, -30.0f));
}

NavigationState::~NavigationState() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	Material::CleanupTextures();
	Renderer::Get().shutdown();
	ShapeDrawer::Get().shutdown();

	delete m_navigationMesh;
	m_navigationMesh = nullptr;

	delete m_crowdManager;
	m_crowdManager = nullptr;

	for (CrowdAgentEntity* entity : m_entities)
		delete entity;

	WorkQueue::Shutdown();
	Physics::DeleteAllCollisionObjects();
}

void NavigationState::fixedUpdate() {
	Globals::physics->stepSimulation(m_fdt);
}

void NavigationState::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		direction += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		direction += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		direction += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}

		if (move) {
			m_camera.move(direction * m_dt);
		}
		m_camera.updateTarget();
	}

	m_octree->updateFrameNumber();

	m_frustum.updatePlane(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	m_frustum.updateVertices(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);

	m_crowdManager->update(m_dt);

	for (auto&& entity : m_entities)
		entity->update(m_dt);

	m_octree->updateOctree();
}

void NavigationState::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("animation");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());

	shader = Globals::shaderManager.getAssetPointer("shape");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	
	for (const Batch& batch : m_octree->getOpaqueBatches().m_batches) {
		OctreeNode* drawable = batch.octreeNode;
		shader->loadMatrix("u_model", drawable->getWorldTransformation());
		drawable->drawRaw();
	}
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	shader->unuse();

	if (m_debugTree) {

		for (size_t i = 0; i < m_octree->getRootLevelOctants().size(); ++i) {
			const Octree::ThreadOctantResult& result = m_octree->getOctantResults()[i];
			for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
				Octant* octant = oIt->first;
				if (m_debugTree)
					octant->OnRenderAABB(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

				const std::vector<OctreeNode*>& drawables = octant->getOctreeNodes();
				for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt) {
					(*dIt)->OnRenderAABB(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
				}
			}
		}

		DebugRenderer::Get().SetProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
		DebugRenderer::Get().drawBuffer();
	}

	if (m_debugNavmesh) {
		m_navigationMesh->OnRenderDebug();
		m_crowdManager->OnRenderDebug();
		DebugRenderer::Get().SetProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
		DebugRenderer::Get().drawBuffer();
	}
	
	if (m_debugPhysic) {
		ShapeDrawer::Get().setProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());

		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ShapeDrawer::Get().drawDynmicsWorld(Physics::GetDynamicsWorld());
		glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	if (m_drawUi)
		renderUi();
}

void NavigationState::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void NavigationState::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());	
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
		if (Keyboard::instance().keyDown(Keyboard::KEY_LSHIFT)) {
			if (m_mousePicker.clickAll(event.x, event.y, m_camera, m_groundObject)) {
				const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
				btVector3 pos = callbackAll.m_hitPointWorld[callbackAll.index];
				spawnBeta(Physics::VectorFrom(pos));
			}
		}else if (Keyboard::instance().keyDown(Keyboard::KEY_LCTRL)) {
			if (m_mousePicker.clickAll(event.x, event.y, m_camera, m_groundObject)) {
				const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
				btVector3 pos = callbackAll.m_hitPointWorld[callbackAll.index];
				spawnAgent(Physics::VectorFrom(pos));
			}
		}else{			
			Renderer::Get().clearMarker();
			if (m_mousePicker.clickAll(event.x, event.y, m_camera, m_groundObject)) {
				const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
				btVector3 pos = callbackAll.m_hitPointWorld[callbackAll.index];
				Vector3f pathPos = m_navigationMesh->findNearestPoint(Physics::VectorFrom(pos), Vector3f(1.0f, 1.0f, 1.0f));			
				m_crowdManager->setCrowdTarget(pathPos);
			}
		}
	}
}

void NavigationState::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void NavigationState::OnMouseWheel(Event::MouseWheelEvent& event) {

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

void NavigationState::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void NavigationState::OnKeyUp(Event::KeyboardEvent& event) {

}

void NavigationState::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void NavigationState::renderUi() {
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
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::Checkbox("Debug Tree", &m_debugTree);
	ImGui::Checkbox("Debug Physic", &m_debugPhysic);
	ImGui::Checkbox("Debug Navmesh", &m_debugNavmesh);
	if (ImGui::Button("Clear Marker"))
		Renderer::Get().clearMarker();

	if (ImGui::SliderFloat("Separation Weight", &m_separaionWeight, 0.0f, 32.0f)) {
		for (CrowdAgent* agent : m_crowdManager->getAgents()) {
			agent->setSeparationWeight(m_separaionWeight);
		}
	}
		

	if (ImGui::SliderFloat("Height", &m_height, 0.0f, 8.0f)) {
		for (CrowdAgent* agent : m_crowdManager->getAgents()) {
			agent->setHeight(m_height);
		}
	}

	int currentMode = m_mode;
	if (ImGui::Combo("Mode", &currentMode, "Low\0Medium\0Heigh\0None\0\0")) {
		m_mode = static_cast<NavigationPushiness>(currentMode);
		for (CrowdAgent* agent : m_crowdManager->getAgents()) {
			agent->setNavigationPushiness(m_mode);
		}
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void NavigationState::createShapes() {
	Utils::MdlIO mdlConverter;
	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

	mdlConverter.mdlToBuffer("res/models/pCube2.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_ground.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_ground.createBoundingBox();
	m_ground.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/pCylinder2.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_cylinder.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_cylinder.createBoundingBox();
	m_cylinder.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/pCube3.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_cube.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_cube.createBoundingBox();
	m_cube.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/pCube14.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_cube14.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_cube14.createBoundingBox();
	m_cube14.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/pCube17.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_cube17.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_cube17.createBoundingBox();
	m_cube17.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();
}

void NavigationState::createPhysics() {
	m_groundObject = Physics::AddStaticObject(Physics::BtTransform(btVector3(0.0f, 0.0f, 0.0f)), Physics::CreateCollisionShape(&m_ground, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(23.2655f, -0.414571f, -24.8348f)), Physics::CreateCollisionShape(&m_cylinder, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(-0.0426907f, 2.31663f, -9.42164f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(-14.0839f, 2.31663f, 1.92646f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(-11.4615f, 2.31663f, -22.13f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(-21.9248f, 2.31663f, -8.26868f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(-6.17903f, 2.31663f, 6.16944f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	
	Physics::AddStaticObject(Physics::BtTransform(btVector3(22.4007f, 2.30943f, -9.9086f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.71352f, 1.0f, 3.86812f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(-26.3652f, 2.36106f, -20.859f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.71352f, 1.0f, 3.86812f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(-26.3652f, 2.36106f, -9.20306f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.71352f, 1.0f, 3.86812f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(-26.3652f, 2.36106f, -32.5239f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.71352f, 1.0f, 3.86812f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(-26.3652f, 2.36106f, -44.1777f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.71352f, 1.0f, 3.86812f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);


	Physics::AddStaticObject(Physics::BtTransform(btVector3(-22.3009f, 2.33817f, -31.3599f)), Physics::CreateCollisionShape(&m_cube14, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(-26.3652f, 2.36106f, 16.6828f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.71352f, 1.0f, 3.86812f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(22.4007f, 2.30943f, -40.1603f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.71352f, 1.0f, 3.86812f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);

	Physics::AddStaticObject(Physics::BtTransform(btVector3(-26.3733f, 4.53696f, -18.4788f)), Physics::CreateCollisionShape(&m_cube17, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(11.2092f, 2.31663f, -30.8257f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(-30.3823f, 2.33817f, -42.2312f), btQuaternion(0.0f, 1.0f, 0.0f, 0.0f)), Physics::CreateCollisionShape(&m_cube14, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	Physics::AddStaticObject(Physics::BtTransform(btVector3(18.3143f, 2.33817f, -13.2117f), btQuaternion(0.0f, 1.0f, 0.0f, 0.0f)), Physics::CreateCollisionShape(&m_cube14, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);

	Physics::AddStaticObject(Physics::BtTransform(btVector3(22.4007f, 2.30943f, 1.55447f)), Physics::CreateCollisionShape(&m_cube, btVector3(1.71352f, 1.0f, 3.86812f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
}

void NavigationState::createScene() {
	ShapeNode* shapeNode;

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_ground);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(0);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cylinder);
	shapeNode->setPosition(23.2655f, -0.414571f, -24.8348f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(1);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-0.0426907f, 2.31663f, -9.42164f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-14.0839f, 2.31663f, 1.92646f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-11.4615f, 2.31663f, -22.13f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-21.9248f, 2.31663f, -8.26868f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-6.17903f, 2.31663f, 6.16944f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(22.4007f, 2.30943f, -9.9086f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, -20.859f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, -9.20306f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, -32.5239f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, -44.1777f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube14);
	shapeNode->setPosition(-22.3009f, 2.33817f, -31.3599f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, 16.6828f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(22.4007f, 2.30943f, -40.1603f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube17);
	shapeNode->setPosition(-26.3733f, 4.53696f, -18.4788f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(1);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(11.2092f, 2.31663f, -30.8257f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube14);
	shapeNode->setPosition(-30.3823f, 2.33817f, -42.2312f);
	shapeNode->setOrientation(Quaternion(0.0f, 1.0f, 0.0f, 0.0f));
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube14);
	shapeNode->setPosition(18.3143f, 2.33817f, -13.2117f);
	shapeNode->setOrientation(Quaternion(0.0f, 1.0f, 0.0f, 0.0f));
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(22.4007f, 2.30943f, 1.55447f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);
	m_navigationMesh->addNavigable(Navigable(shapeNode));
}

void NavigationState::spawnAgent(const Vector3f& pos){
	CrowdAgent* agent = m_crowdManager->addAgent(pos);
	
	agent->setHeight(2.0f);
	agent->setMaxSpeed(6.0f);
	agent->setMaxAccel(10.0f);
	agent->setRadius(0.5f);
	agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	agent->setSeparationWeight(m_separaionWeight);
	agent->initCallbacks();
}

void NavigationState::spawnBeta(const Vector3f& pos) {
	CrowdAgent* agent = m_crowdManager->addAgent(pos);

	agent->setHeight(2.0f);
	agent->setMaxSpeed(6.0f);
	agent->setMaxAccel(10.0f);
	agent->setRadius(0.5f);
	agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	agent->setSeparationWeight(m_separaionWeight);

	AnimationNode* animationNode = m_root->addChild<AnimationNode, AnimatedModel>(m_beta);
	animationNode->setPosition(pos);
	animationNode->setOrientation(0.0f, 180.0f, 0.0f);
	animationNode->OnOctreeSet(m_octree);
	animationNode->setTextureIndex(3);
	animationNode->setId(m_root->countChild<AnimationNode>() - 1);
	animationNode->setSortKey(1);
	animationNode->setShader(Globals::shaderManager.getAssetPointer("animation"));

	Beta* beta = new Beta(*agent, animationNode);

	//overcome t pose
	beta->setUpdateSilent(true);
	beta->update(0.0f);
	beta->setUpdateSilent(false);

	m_entities.push_back(beta);
}

void NavigationState::spawnJack(const Vector3f& pos) {
	CrowdAgent* agent = m_crowdManager->addAgent(pos);

	agent->setHeight(2.0f);
	agent->setMaxSpeed(6.0f);
	agent->setMaxAccel(10.0f);
	agent->setRadius(0.5f);
	agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	agent->setSeparationWeight(m_separaionWeight);

	AnimationNode* animationNode = m_root->addChild<AnimationNode, AnimatedModel>(m_jack);
	animationNode->setPosition(pos);
	animationNode->setOrientation(0.0f, 180.0f, 0.0f);
	animationNode->OnOctreeSet(m_octree);
	animationNode->setTextureIndex(3);
	animationNode->setId(m_root->countChild<AnimationNode>() - 1);
	animationNode->setSortKey(1);
	animationNode->setShader(Globals::shaderManager.getAssetPointer("animation"));

	m_entities.push_back(new Jack(*agent, animationNode));
}

void NavigationState::spawnWoman(const Vector3f& pos) {
	CrowdAgent* agent = m_crowdManager->addAgent(pos);

	agent->setHeight(2.0f);
	agent->setMaxSpeed(6.0f);
	agent->setMaxAccel(10.0f);
	agent->setRadius(0.5f);
	agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	agent->setSeparationWeight(m_separaionWeight);

	AnimationNode* animationNode = m_root->addChild<AnimationNode, AnimatedModel>(m_woman);
	animationNode->setPosition(pos);
	animationNode->setOrientation(0.0f, 180.0f, 0.0f);
	animationNode->OnOctreeSet(m_octree);
	animationNode->setTextureIndex(5);
	animationNode->setId(m_root->countChild<AnimationNode>() - 1);
	animationNode->setSortKey(1);
	animationNode->setShader(Globals::shaderManager.getAssetPointer("animation"));

	m_entities.push_back(new Woman(*agent, animationNode));
}