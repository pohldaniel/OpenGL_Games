#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <magic_enum.hpp>

#include <engine/DebugRenderer.h>
#include <Utils/BinaryIO.h>
#include <States/Menu.h>

#include <Physics/ShapeDrawer.h>

#include "Utils/RandomNew.h"
#include "NavigationStreamState.h"
#include "Application.h"
#include "Globals.h"

std::vector<ShapeNode*> NavigationStreamState::Marker;
Octree* NavigationStreamState::_Octree = nullptr;
SceneNodeLC* NavigationStreamState::Root = nullptr;

auto hash = [](const std::array<int, 2>& p) {  return std::hash<int>()(p[0]) ^ std::hash<int>()(p[1]) << 1; };
auto equal = [](const std::array<int, 2>& p1, const std::array<int, 2>& p2) { return p1[0] == p2[0] && p1[1] == p2[1]; };

NavigationStreamState::NavigationStreamState(StateMachine& machine) :
	State(machine, States::NAVIGATION_STREAM),
	m_separaionWeight(3.0f),
	m_height(2.0f),
	m_streamingDistance(6),
	m_addedTiles(0, hash, equal),
	m_tileData(0, hash, equal) {

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
	Material::AddTexture("res/models/woman/Woman.png", TextureType::TEXTURE2D, false);
	Material::AddTexture("res/textures/StoneDiffuse.dds");
	Material::GetTextures().back().setWrapMode(GL_REPEAT);
	Material::AddTexture("res/textures/Mushroom.dds", TextureType::TEXTURE2D, false);
	Material::AddTexture();

	m_beta.loadModelMdl("res/models/BetaLowpoly/Beta.mdl");
	m_beta.getMeshes()[0]->getMeshBones()[0].initialRotation.rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f);
	m_jack.loadModelMdl("res/models/Jack/Jack.mdl");
	m_woman.loadModelAssimp("res/models/woman/Woman.gltf", true, false);
	m_woman.getMeshes()[0]->getMeshBones()[0].initialScale.scale(0.004f, 0.004f, 0.004f);

	Globals::animationManagerNew.loadAnimationAni("beta_idle", "res/models/BetaLowpoly/Beta_Idle.ani");
	Globals::animationManagerNew.loadAnimationAni("beta_run", "res/models/BetaLowpoly/Beta_Run.ani");
	Globals::animationManagerNew.loadAnimationAni("jack_idle", "res/models/Jack/Jack_Walk.ani");
	Globals::animationManagerNew.loadAnimationAni("jack_walk", "res/models/Jack/Jack_Walk.ani");

	Globals::animationManagerNew.loadAnimationAssimp("woman_walk", "res/models/woman/Woman.gltf", "Walking", "woman_walk");
	Globals::animationManagerNew.loadAnimationAssimp("woman_lean_left", "res/models/woman/Woman.gltf", "Lean_Left", "woman_lean_left");
	Globals::animationManagerNew.loadAnimationAssimp("woman_run", "res/models/woman/Woman.gltf", "Running", "woman_run");
	Globals::animationManagerNew.loadAnimationAssimp("woman_jump_1", "res/models/woman/Woman.gltf", "Jump", "woman_jump_1");
	Globals::animationManagerNew.loadAnimationAssimp("woman_jump_2", "res/models/woman/Woman.gltf", "Jump2", "woman_jump_2");
	Globals::animationManagerNew.loadAnimationAssimp("woman_pick_up", "res/models/woman/Woman.gltf", "PickUp", "woman_pick_up");
	Globals::animationManagerNew.loadAnimationAssimp("woman_sit_idle", "res/models/woman/Woman.gltf", "SitIdle", "woman_sit_idle");
	Globals::animationManagerNew.loadAnimationAssimp("woman_idle", "res/models/woman/Woman.gltf", "Idle", "woman_idle");
	Globals::animationManagerNew.loadAnimationAssimp("woman_punch", "res/models/woman/Woman.gltf", "Punch", "woman_punch");
	Globals::animationManagerNew.loadAnimationAssimp("woman_sit", "res/models/woman/Woman.gltf", "Sitting", "woman_sit");

	createShapes();
	createPhysics();
	createScene();

	m_navigationMesh = new DynamicNavigationMesh();


	m_navigationMesh->m_navigables = m_navigables;
	m_navigationMesh->SetPadding(Vector3f(0.0f, 10.0f, 0.0f));
	m_navigationMesh->SetTileSize(16);

	m_navigationMesh->SetCellSize(0.3);
	m_navigationMesh->SetCellHeight(0.2f);

	m_navigationMesh->SetAgentMaxSlope(45.0f);
	m_navigationMesh->SetAgentMaxClimb(0.9f);
	m_navigationMesh->SetAgentHeight(2.0f);
	m_navigationMesh->SetAgentRadius(0.6f);
	m_navigationMesh->Build();

	SceneNodeLC* boxGroup = Root->findChild<SceneNodeLC>("box_group");
	createBoxOffMeshConnections(m_navigationMesh, boxGroup);
	m_navigationMesh->Build();

	m_crowdManager = new CrowdManager();
	m_crowdManager->setNavigationMesh(m_navigationMesh);
	m_crowdManager->setOnCrowdFormation([&m_crowdManager = m_crowdManager](const Vector3f& pos, CrowdAgent* agent) {
		if (agent) {
			Vector3f _pos = m_crowdManager->getRandomPointInCircle(pos, agent->getRadius(), agent->getQueryFilterType());
			return _pos;
		}
		return Vector3f(pos);
	});

	m_navigationMesh->m_crowdManager = m_crowdManager;

	

	for (unsigned i = 0; i < 100; ++i)
		createMushroom(Vector3f(Utils::random(90.0f) - 45.0f, 0.0f, Utils::random(90.0f) - 45.0f));

	CrowdObstacleAvoidanceParams params = m_crowdManager->getObstacleAvoidanceParams(0);
	params.velBias = 0.5f;
	params.adaptiveDivs = 7;
	params.adaptiveRings = 3;
	params.adaptiveDepth = 3;
	m_crowdManager->setObstacleAvoidanceParams(0, params);

	spawnJack(Vector3f(0.0f, 0.0f, 0.0f));
	spawnBeta(Vector3f(-5.0f, 0.5f, -30.0f));
	spawnWoman(Vector3f(-10.0f, 0.5f, -30.0f));

	auto shader = Globals::shaderManager.getAssetPointer("map");
	shader->use();
	shader->loadFloat("u_tileFactor", 4.0f);
	shader->loadVector("u_blendColor", Vector4f(0.8f, 1.0f, 1.0f, 1.0f));
	shader->unuse();
}

NavigationStreamState::~NavigationStreamState() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void NavigationStreamState::fixedUpdate() {
	Globals::physics->stepSimulation(m_fdt);
}

void NavigationStreamState::update() {
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

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_useStreaming = !m_useStreaming;
		toggleStreaming(m_useStreaming);
	}

	if (keyboard.keyPressed(Keyboard::KEY_R)) {
		m_navigationMesh->Build();
		m_crowdManager->resetNavMesh(m_navigationMesh->GetDetourNavMesh());
		m_crowdManager->initNavquery(m_navigationMesh->GetDetourNavMesh());
	}

	if (keyboard.keyPressed(Keyboard::KEY_C)) {
		
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

	

	if (m_useStreaming)
		updateStreaming();

	m_crowdManager->update(m_dt);
	//m_navigationMesh->update(m_dt);

	for (auto&& entity : m_entities)
		entity->update(m_dt);

	_Octree->updateOctree();
}

void NavigationStreamState::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("animation");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());

	shader = Globals::shaderManager.getAssetPointer("map");
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
	//m_mousePicker.drawPicker(m_camera);

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

void NavigationStreamState::OnMouseMotion(Event::MouseMoveEvent& event) {
	//m_mousePicker.updatePosition(event.x, event.y, m_camera);
}

void NavigationStreamState::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	    if (Keyboard::instance().keyDown(Keyboard::KEY_TAB)) {
		    if (m_mousePicker.clickAll(event.x, event.y, m_camera, nullptr)) {
			    const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
				btVector3 pos = callbackAll.m_hitPointWorld[callbackAll.index];
			    addOrRemoveObject(Physics::VectorFrom(pos), static_cast<PhysicalObjects>(callbackAll.m_userIndex), static_cast<Obstacle*>(callbackAll.m_userPoiner), callbackAll.m_collisionObject);
		    }
	    }else if (Keyboard::instance().keyDown(Keyboard::KEY_LSHIFT)) {
			if (m_mousePicker.clickAll(event.x, event.y, m_camera, nullptr)) {
				const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
				btVector3 pos = callbackAll.m_hitPointWorld[callbackAll.index];
				spawnBeta(Physics::VectorFrom(pos));
			}
		}else if (Keyboard::instance().keyDown(Keyboard::KEY_LCTRL)) {
			if (m_mousePicker.clickAll(event.x, event.y, m_camera, nullptr)) {
				const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
				btVector3 pos = callbackAll.m_hitPointWorld[callbackAll.index];
				spawnJack(Physics::VectorFrom(pos));
			}
		}else {
			clearMarker();
			if (m_mousePicker.clickAll(event.x, event.y, m_camera, nullptr)) {
				const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
				btVector3 pos = callbackAll.m_hitPointWorld[callbackAll.index];
				Vector3f pathPos = m_navigationMesh->FindNearestPoint(Physics::VectorFrom(pos), Vector3f(1.5f, 1.5f, 1.5f));
				m_crowdManager->setCrowdTarget(pathPos);
			}
		}
	}
}

void NavigationStreamState::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void NavigationStreamState::OnMouseWheel(Event::MouseWheelEvent& event) {

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

void NavigationStreamState::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void NavigationStreamState::OnKeyUp(Event::KeyboardEvent& event) {

}

void NavigationStreamState::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void NavigationStreamState::renderUi() {
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

void NavigationStreamState::createShapes() {
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

	mdlConverter.mdlToBuffer("res/models/Plane.mdl", {100.0f, 1.0f, 100.0f}, vertexBuffer, indexBuffer);
	m_plane.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_plane.createBoundingBox();
	m_plane.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/Box.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_box.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_box.createBoundingBox();
	m_box.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/Mushroom.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_mushroom.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_mushroom.createBoundingBox();
	m_mushroom.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();
}

void NavigationStreamState::createPhysics() {
	btCollisionObject* collisionObject = Physics::AddStaticObject(Physics::BtTransform(btVector3(0.0f, 0.0f, 0.0f)), Physics::CreateCollisionShape(&m_plane, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	collisionObject->setUserIndex(PhysicalObjects::GROUND);
}

void NavigationStreamState::createScene() {
	Root = new SceneNodeLC();
	ShapeNode* shapeNode;
	shapeNode = Root->addChild<ShapeNode, Shape>(m_plane);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(6);
	shapeNode->setSortKey(2);
	shapeNode->setShader(Globals::shaderManager.getAssetPointer("map"));
	m_navigables.push_back(new Navigable(shapeNode));

	SceneNodeLC* boxGroup = Root->addChild<SceneNodeLC>();
	boxGroup->setName("box_group");

	for (unsigned i = 0; i < 20; ++i){
		float size = 1.0f + Utils::random(10.0f);		
		ShapeNode* boxNode = boxGroup->addChild<ShapeNode, Shape>(m_box);
		boxNode->OnOctreeSet(_Octree);
		boxNode->setPosition(Utils::random(80.0f) - 40.0f, 0.5f * size, Utils::random(80.0f) - 40.0f);
		boxNode->setTextureIndex(6);
		boxNode->setScale(size);
		m_navigables.push_back(new Navigable(boxNode));
		Physics::AddStaticObject(Physics::BtTransform(Physics::VectorFrom(boxNode->getPosition())), Physics::CreateCollisionShape(&m_box, btVector3(size, size, size)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	}
}

void NavigationStreamState::clearMarker() {
	for (auto shapeNode : Marker) {
		shapeNode->OnOctreeSet(nullptr);
		shapeNode->eraseSelf();
	}
	Marker.clear();
}

void NavigationStreamState::addMarker(const Vector3f& pos) {
	Marker.push_back(Root->addChild<ShapeNode, Shape>(Globals::shapeManager.get("sphere")));
	Marker.back()->setPosition(pos);
	Marker.back()->setTextureIndex(4);
	Marker.back()->OnOctreeSet(_Octree);
}

void NavigationStreamState::spawnAgent(const Vector3f& pos) {
	CrowdAgent* agent = new CrowdAgent();
	m_crowdManager->addAgent(agent, pos);

	agent->setHeight(2.0f);
	agent->setMaxSpeed(6.0f);
	agent->setMaxAccel(10.0f);
	agent->setRadius(0.5f);
	agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	agent->setSeparationWeight(m_separaionWeight);
	
	m_empty.push_back(new EmptyAgentEntity(*agent, nullptr));
}

void NavigationStreamState::spawnBeta(const Vector3f& pos) {
	CrowdAgent* agent = new CrowdAgent();
	m_crowdManager->addAgent(agent, pos);

	agent->setHeight(2.0f);
	agent->setMaxSpeed(6.0f);
	agent->setMaxAccel(10.0f);
	agent->setRadius(0.5f);
	agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	agent->setSeparationWeight(m_separaionWeight);

	AnimationNode* animationNode = Root->addChild<AnimationNode, AnimatedModel>(m_beta);
	animationNode->setPosition(pos);
	animationNode->setOrientation(0.0f, 180.0f, 0.0f);
	animationNode->OnOctreeSet(_Octree);
	animationNode->setTextureIndex(3);
	animationNode->setId(Root->countChild<AnimationNode>() - 1);
	animationNode->setSortKey(1);
	animationNode->setShader(Globals::shaderManager.getAssetPointer("animation"));

	Beta* beta = new Beta(*agent, animationNode);

	//overcome t pose
	beta->setUpdateSilent(true);
	beta->update(0.0f);
	beta->setUpdateSilent(false);

	m_entities.push_back(beta);
}

void NavigationStreamState::spawnJack(const Vector3f& pos) {
	CrowdAgent* agent = new CrowdAgent();
	m_crowdManager->addAgent(agent, pos);

	agent->setHeight(2.0f);
	agent->setMaxSpeed(6.0f);
	agent->setMaxAccel(10.0f);
	agent->setRadius(0.5f);
	agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	agent->setSeparationWeight(m_separaionWeight);

	AnimationNode* animationNode = Root->addChild<AnimationNode, AnimatedModel>(m_jack);
	animationNode->setPosition(pos);
	animationNode->setOrientation(0.0f, 180.0f, 0.0f);
	animationNode->OnOctreeSet(_Octree);
	animationNode->setTextureIndex(3);
	animationNode->setId(Root->countChild<AnimationNode>() - 1);
	animationNode->setSortKey(1);
	animationNode->setShader(Globals::shaderManager.getAssetPointer("animation"));

	m_entities.push_back(new Jack(*agent, animationNode));
}

void NavigationStreamState::spawnWoman(const Vector3f& pos) {
	CrowdAgent* agent = new CrowdAgent();
	m_crowdManager->addAgent(agent, pos);

	agent->setHeight(2.0f);
	agent->setMaxSpeed(6.0f);
	agent->setMaxAccel(10.0f);
	agent->setRadius(0.5f);
	agent->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM);
	agent->setSeparationWeight(m_separaionWeight);

	AnimationNode* animationNode = Root->addChild<AnimationNode, AnimatedModel>(m_woman);
	animationNode->setPosition(pos);
	animationNode->setOrientation(0.0f, 180.0f, 0.0f);
	animationNode->OnOctreeSet(_Octree);
	animationNode->setTextureIndex(5);
	animationNode->setId(Root->countChild<AnimationNode>() - 1);
	animationNode->setSortKey(1);
	animationNode->setShader(Globals::shaderManager.getAssetPointer("animation"));
	m_entities.push_back(new Woman(*agent, animationNode));
}

void NavigationStreamState::createMushroom(const Vector3f& pos) {

	float scale = 2.0f + Utils::random(0.5f);
	Quaternion orientation = Quaternion(0.0f, Utils::random(360.0f), 0.0f);

	ShapeNode* shapeNode;
	shapeNode = Root->addChild<ShapeNode, Shape>(m_mushroom);
	shapeNode->setPosition(pos);
	shapeNode->setOrientation(orientation);
	shapeNode->setScale(scale);
	shapeNode->OnOctreeSet(_Octree);
	shapeNode->setTextureIndex(7);

	Obstacle* obstacle = new Obstacle(shapeNode);	

	obstacle = new Obstacle(shapeNode);
	obstacle->ownerMesh_ = m_navigationMesh;
	obstacle->SetRadius(scale);
	obstacle->SetHeight(scale);
	m_navigationMesh->AddObstacle(obstacle, false);
	m_navigationMesh->m_obstacles.push_back(obstacle);

	btCollisionObject* collisionObject = Physics::AddStaticObject(Physics::BtTransform(Physics::VectorFrom(pos), Physics::QuaternionFrom(orientation)), Physics::CreateConvexHullShape(&m_mushroom, {scale, scale, scale}), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER);
	collisionObject->setUserIndex(PhysicalObjects::MUSHRROM);
	collisionObject->setUserPointer(obstacle);
	
	//m_navigationMesh->wait();
}

void NavigationStreamState::addOrRemoveObject(const Vector3f& pos, PhysicalObjects physicalObjects, Obstacle* obstacle, btCollisionObject* collisionObject){

	switch (physicalObjects){
	  case MUSHRROM:
		  obstacle->isEnabled_ = false;
		  obstacle->m_node->OnOctreeSet(nullptr);
		  obstacle->m_node->eraseSelf();
		  obstacle->m_node = nullptr;

		  delete obstacle;
		  m_navigationMesh->m_obstacles.erase(std::remove(m_navigationMesh->m_obstacles.begin(), m_navigationMesh->m_obstacles.end(), obstacle), m_navigationMesh->m_obstacles.end());

		  Physics::DeleteCollisionObject(collisionObject);
		  m_navigationMesh->wait();
		break;
	  case ENTITY:
		  //std::cout << magic_enum::enum_name(physicalObjects) << std::endl;
		break;
	  case GROUND:
		  createMushroom(pos);
		  m_navigationMesh->wait();
		break;
	  default:
		break;
	}
}

void NavigationStreamState::toggleStreaming(bool enabled) {
	if (enabled){
		int maxTiles = (2 * m_streamingDistance + 1) * (2 * m_streamingDistance + 1);
		BoundingBox boundingBox = m_navigationMesh->GetBoundingBox();		
		saveNavigationData();
		m_navigationMesh->Allocate(boundingBox, maxTiles);		
		m_crowdManager->resetNavMesh(m_navigationMesh->GetDetourNavMesh());
		m_crowdManager->initNavquery(m_navigationMesh->GetDetourNavMesh());	
		updateStreaming();
	}else {
		m_navigationMesh->Build();
		m_crowdManager->resetNavMesh(m_navigationMesh->GetDetourNavMesh());
		m_crowdManager->initNavquery(m_navigationMesh->GetDetourNavMesh());
		m_crowdManager->reCreateCrowd();
	}
}

void NavigationStreamState::updateStreaming() {
	// Center the navigation mesh at the crowd of jacks
	Vector3f averageAgentPosition;
	const float numAgents = static_cast<float>(m_crowdManager->getAgents().size());
	for (auto agent : m_crowdManager->getAgents()) {
		averageAgentPosition += agent->getPosition();
	}
	averageAgentPosition /= numAgents;

	// Compute currently loaded area
	const std::array<int, 2> jackTile = m_navigationMesh->GetTileIndex(averageAgentPosition);
	const std::array<int, 2> numTiles = m_navigationMesh->GetNumTiles();

	const std::array<int, 2> beginTile = { std::max(0, jackTile[0] - m_streamingDistance), std::max(0, jackTile[1] - m_streamingDistance) };
	const std::array<int, 2> endTile = { std::min(jackTile[0] + m_streamingDistance, numTiles[0] - 1), std::min(jackTile[1] + m_streamingDistance, numTiles[1] - 1) };

	// Remove tiles
	for (std::unordered_set<std::array<int, 2>>::iterator i = m_addedTiles.begin(); i != m_addedTiles.end();){
		const std::array<int, 2> tileIdx = *i;
		if (beginTile[0] <= tileIdx[0] && tileIdx[0] <= endTile[0] && beginTile[1] <= tileIdx[1] && tileIdx[1] <= endTile[1])
			++i;
		else{
			m_navigationMesh->RemoveTile(tileIdx);
			i = m_addedTiles.erase(i);
		}
	}

	m_navigationMesh->m_agentsToReset.clear();

	// Add tiles
	for (int z = beginTile[1]; z <= endTile[1]; ++z) {
		for (int x = beginTile[0]; x <= endTile[0]; ++x) {
			const std::array<int, 2> tileIdx = { x, z };	
			bool tmp = m_navigationMesh->HasTile(tileIdx);
			if (!m_navigationMesh->HasTile(tileIdx) && m_tileData.find(tileIdx) != m_tileData.end()){
				m_addedTiles.insert(tileIdx);
				m_navigationMesh->AddTile(m_tileData[tileIdx]);
			}
		}
	}

	for (CrowdAgent* agent : m_navigationMesh->m_agentsToReset)
		agent->resetParameter();
}

void NavigationStreamState::AddMarker(const Vector3f& pos) {
	Marker.push_back(Root->addChild<ShapeNode, Shape>(Globals::shapeManager.get("sphere")));
	Marker.back()->setPosition(pos);
	Marker.back()->setTextureIndex(4);
	Marker.back()->OnOctreeSet(_Octree);
}

void NavigationStreamState::saveNavigationData(){
	DynamicNavigationMesh* navMesh = m_navigationMesh;
	m_tileData.clear();
	m_addedTiles.clear();
	const  std::array<int, 2> numTiles = navMesh->GetNumTiles();
	for (int z = 0; z < numTiles[1]; ++z)
		for (int x = 0; x <= numTiles[0]; ++x){
			const std::array<int, 2> tileIdx = { x, z };
			navMesh->GetTileData(m_tileData[tileIdx], tileIdx);		
		}
}

void NavigationStreamState::createBoxOffMeshConnections(DynamicNavigationMesh* navMesh, SceneNodeLC* boxGroup) {
	const std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>& boxes = boxGroup->getChildren();

	for (auto it = boxes.begin(); it != boxes.end(); ++it) {
		ShapeNode* box = dynamic_cast<ShapeNode*>((*it).get());
		Vector3f boxPos = box->getWorldPosition();
		float boxHalfSize = (box->getWorldScale()[0] / 2);

		Matrix4f inv = box->getWorldTransformation().inverse();

		SceneNodeLC* connectionStart = box->addChild<SceneNodeLC>();
		connectionStart->setPosition(inv ^ navMesh->FindNearestPoint(boxPos + Vector3f(boxHalfSize + 0.05f * boxHalfSize, -boxHalfSize, 0)));

		SceneNodeLC* connectionEnd = box->addChild<SceneNodeLC>();
		connectionEnd->setPosition(inv ^ navMesh->FindNearestPoint(boxPos + Vector3f(boxHalfSize, boxHalfSize, 0)));

		OffMeshConnection* connection = new OffMeshConnection(connectionStart);
		connection->SetEndPoint(connectionEnd);

		navMesh->m_offMeshConnections.push_back(connection);
	}
}