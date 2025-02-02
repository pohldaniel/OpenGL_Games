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
#include "Globals.h"

std::vector<ShapeNode*> NavigationState::Marker;
Octree* NavigationState::_Octree = nullptr;
SceneNodeLC* NavigationState::Root = nullptr;

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
	_Octree = new Octree(m_camera, m_frustum, m_dt);
	_Octree->setUseOcclusionCulling(false);

	DebugRenderer::Get().setEnable(true);

	Material::AddTexture("res/textures/ProtoWhite256.jpg");
	Material::AddTexture("res/textures/ProtoGreen256.jpg");
	Material::AddTexture("res/textures/ProtoBlue256.jpg");
	Material::AddTexture("res/textures/ProtoRed256.jpg");
	Material::AddTexture("res/textures/ProtoYellow256.jpg");
	Material::AddTexture();

	m_beta.loadModelMdl("res/models/BetaLowpoly/Beta.mdl");
	m_jack.loadModelMdl("res/models/Jack/Jack.mdl");

	Globals::animationManagerNew.loadAnimationAni("beta_idle", "res/models/BetaLowpoly/Beta_Idle.ani");
	Globals::animationManagerNew.loadAnimationAni("beta_run", "res/models/BetaLowpoly/Beta_Run.ani");
	Globals::animationManagerNew.loadAnimationAni("jack_idle", "res/models/Jack/Jack_Walk.ani");
	Globals::animationManagerNew.loadAnimationAni("jack_walk", "res/models/Jack/Jack_Walk.ani");

	createShapes();
	createPhysics();
	createScene();

	
	m_animationControllerBeta = new AnimationController(Root->findChild<AnimationNode>(0, false));
	m_animationControllerBeta->playExclusive("beta_idle", 0, true, 0.0f);
	m_animationControllerJack = new AnimationController(Root->findChild<AnimationNode>(1, false));

	m_navigationMesh = new NavigationMesh();
	m_navigationMesh->m_navigables = m_navigables;
	m_navigationMesh->SetPadding(Vector3f(0.0f, 10.0f, 0.0f));
	m_navigationMesh->SetTileSize(8);

	m_navigationMesh->SetCellSize(0.3);
	m_navigationMesh->SetCellHeight(0.2f);

	m_navigationMesh->SetAgentMaxSlope(45.0f);
	m_navigationMesh->SetAgentMaxClimb(0.9f);
	m_navigationMesh->SetAgentHeight(2.0f);
	m_navigationMesh->SetAgentRadius(0.6f);
	m_navigationMesh->Build();


	m_crowdManager = new CrowdManager();
	m_crowdManager->setNavigationMesh(m_navigationMesh);
	/*CrowdObstacleAvoidanceParams params = m_crowdManager->GetObstacleAvoidanceParams(0);
	// Set the params to "High (66)" setting
	params.velBias = 0.5f;
	params.adaptiveDivs = 7;
	params.adaptiveRings = 3;
	params.adaptiveDepth = 3;
	m_crowdManager->SetObstacleAvoidanceParams(0, params);

	unsigned numqs = m_crowdManager->GetNumQueryFilterTypes();
	m_crowdManager->SetIncludeFlags(0, NAVPOLYFLAG_LEVEL1);
	m_crowdManager->SetExcludeFlags(0, NAVPOLYFLAG_LEVEL2 | NAVPOLYFLAG_LEVEL3);

	m_crowdManager->SetIncludeFlags(1, NAVPOLYFLAG_LEVEL1 | NAVPOLYFLAG_LEVEL2);
	m_crowdManager->SetExcludeFlags(1, NAVPOLYFLAG_LEVEL3);

	m_crowdManager->SetIncludeFlags(2, NAVPOLYFLAG_LEVEL1 | NAVPOLYFLAG_LEVEL2 | NAVPOLYFLAG_LEVEL3);*/
	
	m_crowdAgentBeta = new CrowdAgent();	
	m_crowdManager->addAgent(m_crowdAgentBeta, Root->findChild<AnimationNode>(0)->getWorldPosition());

	m_crowdAgentBeta->setHeight(2.0f);
	m_crowdAgentBeta->setMaxSpeed(6.0f);
	m_crowdAgentBeta->setMaxAccel(10.0f);
	m_crowdAgentBeta->setRadius(0.5f);
	m_crowdAgentBeta->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	m_crowdAgentBeta->setSeparationWeight(m_separaionWeight);

	m_crowdAgentBeta->setOnPositionVelocityUpdate([&m_crowdManager = m_crowdManager, m_node = Root->findChild<AnimationNode>(0), &m_animationControllerBeta = m_animationControllerBeta](const Vector3f& pos, const Vector3f& vel) {
		m_animationControllerBeta->playExclusive("beta_run", 0, true, 0.1f);
		m_node->getOrientation().set(-vel);
		m_node->setPosition(pos);
	});

	m_crowdAgentBeta->setOnInactive([&m_animationControllerBeta = m_animationControllerBeta]() {
		m_animationControllerBeta->playExclusive("beta_idle", 0, true, 0.5f);
	});

	m_crowdAgentBeta->setOnCrowdFormation([this](const Vector3f& pos, const unsigned int index, CrowdAgent* agent) {
		if (index) {
			Vector3f _pos = m_crowdManager->getRandomPointInCircle(pos, agent->getRadius(), agent->getQueryFilterType());
			return _pos;
		}
		return Vector3f(pos);
	});

	m_crowdAgentBeta->setOnTarget([this](const Vector3f& pos) {
		addMarker(pos);
	});

	m_crowdAgentJack = new CrowdAgent();
	m_crowdManager->addAgent(m_crowdAgentJack, Root->findChild<AnimationNode>(1)->getWorldPosition());

	m_crowdAgentJack->setHeight(2.0f);
	m_crowdAgentJack->setMaxSpeed(6.0f);
	m_crowdAgentJack->setMaxAccel(10.0f);
	m_crowdAgentJack->setRadius(0.5f);
	m_crowdAgentJack->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	m_crowdAgentJack->setSeparationWeight(m_separaionWeight);

	m_crowdAgentJack->setOnPositionVelocityUpdate([&m_crowdManager = m_crowdManager, m_node = Root->findChild<AnimationNode>(1), &m_animationControllerJack = m_animationControllerJack](const Vector3f& pos, const Vector3f& vel) {
		m_animationControllerJack->playExclusive("jack_walk", 0, true, 0.1f);
		m_node->getOrientation().set(vel);
		m_node->setPosition(pos);
	});

	m_crowdAgentJack->setOnInactive([&m_animationControllerJack = m_animationControllerJack]() {
		m_animationControllerJack->stop("jack_walk", 0.5f);
	});
		
	m_crowdAgentJack->setOnCrowdFormation([this](const Vector3f& pos, const unsigned int index, CrowdAgent* agent) {

		if (index) {
			Vector3f _pos = m_crowdManager->getRandomPointInCircle(pos, agent->getRadius(), agent->getQueryFilterType());
			return _pos;
		}
		return Vector3f(pos);
	});

	m_crowdAgentJack->setOnTarget([this](const Vector3f& pos) {
		addMarker(pos);
	});

	CrowdAgent* agent = new CrowdAgent();
	m_crowdManager->addAgent(agent, Root->findChild<AnimationNode>(2)->getWorldPosition());

	agent->setHeight(2.0f);
	agent->setMaxSpeed(6.0f);
	agent->setMaxAccel(10.0f);
	agent->setRadius(0.5f);
	agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	agent->setSeparationWeight(m_separaionWeight);

	agent->setOnCrowdFormation([this](const Vector3f& pos, const unsigned int index, CrowdAgent* agent) {
		if (index) {
			Vector3f _pos = m_crowdManager->getRandomPointInCircle(pos, agent->getRadius(), agent->getQueryFilterType());
			return _pos;
		}
		return Vector3f(pos);
	});

	/*agent->setOnTarget([this](const Vector3f& pos) {
		addMarker(pos);
	});*/
	m_betaNew = new Beta(*agent, Root->findChild<AnimationNode>(2, false));
}

NavigationState::~NavigationState() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
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

	_Octree->updateFrameNumber();

	m_frustum.updatePlane(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	m_frustum.updateVertices(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);

	m_animationControllerBeta->update(m_dt);
	Root->findChild<AnimationNode>(0)->update(m_dt);

	m_animationControllerJack->update(m_dt);
	Root->findChild<AnimationNode>(1)->update(m_dt);

	m_betaNew->update(m_dt);

	m_crowdManager->update(m_dt);
	_Octree->updateOctree();
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
	
	for (const Batch& batch : _Octree->getOpaqueBatches().m_batches) {
		OctreeNode* drawable = batch.octreeNode;
		shader->loadMatrix("u_model", drawable->getWorldTransformation());
		drawable->drawRaw();
	}
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	shader->unuse();

	if (m_debugTree) {

		for (size_t i = 0; i < _Octree->getRootLevelOctants().size(); ++i) {
			const Octree::ThreadOctantResult& result = _Octree->getOctantResults()[i];
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
		m_navigationMesh->DrawDebugGeometry(&DebugRenderer::Get(), false);
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
		if (!Keyboard::instance().keyDown(Keyboard::KEY_LSHIFT)) {
			clearMarker();
			if (m_mousePicker.clickAll(event.x, event.y, m_camera, m_groundObject)) {
				const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
				btVector3 pos = callbackAll.m_hitPointWorld[callbackAll.index];
				Vector3f pathPos = m_navigationMesh->FindNearestPoint(Physics::VectorFrom(pos), Vector3f(1.0f, 1.0f, 1.0f));
				m_crowdManager->setCrowdTarget(pathPos);
			}
		}else {
			if (m_mousePicker.clickAll(event.x, event.y, m_camera, m_groundObject)) {
				const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
				btVector3 pos = callbackAll.m_hitPointWorld[callbackAll.index];
				spawnAgent(Physics::VectorFrom(pos));
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
		clearMarker();

	if (ImGui::SliderFloat("Separation Weight", &m_separaionWeight, 0.0f, 32.0f)) {
		//m_crowdManager->setSeparationWeight(m_separaionWeight);
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
	Root = new SceneNodeLC();
	ShapeNode* shapeNode;

	shapeNode = Root->addChild<ShapeNode, Shape>(m_ground);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(0);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cylinder);
	shapeNode->setPosition(23.2655f, -0.414571f, -24.8348f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(1);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-0.0426907f, 2.31663f, -9.42164f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-14.0839f, 2.31663f, 1.92646f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-11.4615f, 2.31663f, -22.13f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-21.9248f, 2.31663f, -8.26868f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-6.17903f, 2.31663f, 6.16944f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(22.4007f, 2.30943f, -9.9086f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, -20.859f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, -9.20306f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, -32.5239f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, -44.1777f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube14);
	shapeNode->setPosition(-22.3009f, 2.33817f, -31.3599f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, 16.6828f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(22.4007f, 2.30943f, -40.1603f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube17);
	shapeNode->setPosition(-26.3733f, 4.53696f, -18.4788f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(1);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(11.2092f, 2.31663f, -30.8257f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube14);
	shapeNode->setPosition(-30.3823f, 2.33817f, -42.2312f);
	shapeNode->setOrientation(Quaternion(0.0f, 1.0f, 0.0f, 0.0f));
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube14);
	shapeNode->setPosition(18.3143f, 2.33817f, -13.2117f);
	shapeNode->setOrientation(Quaternion(0.0f, 1.0f, 0.0f, 0.0f));
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	shapeNode = Root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(22.4007f, 2.30943f, 1.55447f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(2);
	m_navigables.push_back(new Navigable(shapeNode));

	AnimationNode* animationNode = Root->addChild<AnimationNode, AnimatedModel>(m_beta);
	animationNode->setPosition(0.0f, 0.5f, -30.0f);
	animationNode->setOrientation(0.0f, 180.0f, 0.0f);
	animationNode->OnOctreeSet(_Octree);
	animationNode->setTextureIndex(3);
	animationNode->setId(0);
	animationNode->setSortKey(1);
	animationNode->setShader(Globals::shaderManager.getAssetPointer("animation"));

	animationNode = Root->addChild<AnimationNode, AnimatedModel>(m_jack);
	animationNode->setPosition(5.0f, 0.5f, -30.0f);
	animationNode->setOrientation(0.0f, 180.0f, 0.0f);
	animationNode->OnOctreeSet(_Octree);
	animationNode->setTextureIndex(3);
	animationNode->setId(1);
	animationNode->setSortKey(1);
	animationNode->setShader(Globals::shaderManager.getAssetPointer("animation"));

	animationNode = Root->addChild<AnimationNode, AnimatedModel>(m_beta);
	animationNode->setPosition(-5.0f, 0.5f, -30.0f);
	animationNode->setOrientation(0.0f, 180.0f, 0.0f);
	animationNode->OnOctreeSet(_Octree);
	animationNode->setTextureIndex(3);
	animationNode->setId(2);
	animationNode->setSortKey(1);
	animationNode->setShader(Globals::shaderManager.getAssetPointer("animation"));

}

void NavigationState::clearMarker() {
	for (auto shapeNode : Marker) {
		shapeNode->OnOctreeSet(nullptr);
		shapeNode->eraseSelf();
	}
	Marker.clear();
}

void NavigationState::addMarker(const Vector3f& pos) {
	Marker.push_back(Root->addChild<ShapeNode, Shape>(Globals::shapeManager.get("sphere")));
	Marker.back()->setPosition(pos);
	Marker.back()->setTextureIndex(4);
	Marker.back()->OnOctreeSet(_Octree);
}

void NavigationState::spawnAgent(const Vector3f& pos){
	CrowdAgent* agent = new CrowdAgent();
	m_crowdManager->addAgent(agent, pos);
	
	agent->setHeight(2.0f);
	agent->setMaxSpeed(6.0f);
	agent->setMaxAccel(10.0f);
	agent->setRadius(0.5f);
	agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	agent->setSeparationWeight(m_separaionWeight);

	agent->setOnPositionVelocityUpdate([&m_crowdManager = m_crowdManager](const Vector3f& pos, const Vector3f& vel) {

	});

	agent->setOnInactive([]() {

	});

	agent->setOnCrowdFormation([this](const Vector3f& pos, const unsigned int index, CrowdAgent* agent) {
		if (index) {
			Vector3f _pos = m_crowdManager->getRandomPointInCircle(pos, agent->getRadius(), agent->getQueryFilterType());
			return _pos;
		}
		return Vector3f(pos);
	});

	agent->setOnTarget([this](const Vector3f& pos) {
		addMarker(pos);
	});
}

void NavigationState::AddMarker(const Vector3f& pos) {
	Marker.push_back(Root->addChild<ShapeNode, Shape>(Globals::shapeManager.get("sphere")));
	Marker.back()->setPosition(pos);
	Marker.back()->setTextureIndex(4);
	Marker.back()->OnOctreeSet(_Octree);
}