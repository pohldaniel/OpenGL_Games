#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <engine/DebugRenderer.h>
#include <Physics/ShapeDrawer.h>
#include <States/Menu.h>
#include <Utils/BinaryIO.h>
#include "Adrian.h"
#include "Application.h"
#include "Globals.h"

auto hash2 = [](const std::array<int, 2>& p) {  return std::hash<int>()(p[0]) ^ std::hash<int>()(p[1]) << 1; };
auto equal2 = [](const std::array<int, 2>& p1, const std::array<int, 2>& p2) { return p1[0] == p2[0] && p1[1] == p2[1]; };

Adrian::Adrian(StateMachine& machine) : State(machine, States::MAP),
	m_camera(Application::Width, Application::Height),
	m_addedTiles(0, hash2, equal2),
	m_streamingDistance(6) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	//m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -static_cast<float>(Application::Width) / m_zoom, static_cast<float>(Application::Width) / m_zoom);
	m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -5000.0f, 5000.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	Material::AddTexture("data/models/dynamic/hero/hero.tga");
	Material::AddTexture("res/textures/los.tga");
	Material::AddTexture();
	Material::AddTexture("res/textures/crate.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/barrel.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/aircraftgun.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/generalsbuilding.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/wall.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::GetTextures().back().setWrapMode(GL_REPEAT);
	Material::AddTexture("res/textures/200x200building.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/bunker.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/metal.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();
	Material::AddTexture("res/textures/ground.tga", TextureType::TEXTURE2D, false);
	Material::GetTextures().back().setLinear();

	m_hero.load("data/models/dynamic/hero/hero.md2");

	WorkQueue::Init(0);
	m_octree = new Octree(m_camera, m_frustum, m_dt);
	m_octree->setUseOcclusionCulling(false);
	m_octree->setUseCulling(m_useCulling);

	DebugRenderer::Get().setEnable(true);
	m_root = new SceneNodeLC();
	m_heroEnity = m_root->addChild<Md2Entity, Md2Model>(m_hero);
	m_heroEnity->setPosition(-780.0f, MAP_MODEL_HEIGHT_Y, 780.0f);
	m_heroEnity->setOrientation(0.0f, 0.0f, 0.0f);
	m_heroEnity->setTextureIndex(0);
	m_heroEnity->OnOctreeSet(m_octree);
	m_heroEnity->setSortKey(1);
	m_heroEnity->Md2Node::setShader(Globals::shaderManager.getAssetPointer("shape_color"));

	ShapeDrawer::Get().init(32768);
	ShapeDrawer::Get().setCamera(m_camera);
	m_heroEnity->setRigidBody(Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(m_heroEnity->getWorldPosition())), new btBoxShape(btVector3(0.5f, 0.5f, 0.5f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER, nullptr, false));
	//m_heroEnity->setRigidBody(Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(m_heroEnity->getWorldPosition())), new btBoxShape(btVector3(12.5f, 12.5f, 12.5f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER, nullptr, false));

	m_segment.buildSegmentXZ(150.0f, -30.0f, 30.0f, Vector3f(0.0f, 0.0f, 0.0f), 20, 20, true, false, false);
	m_segment.createBoundingBox();
	m_segment.markForDelete();
	m_segmentNode = m_heroEnity->addChild<ShapeNode, Shape>(m_segment);
	m_segmentNode->setPosition(0.0f, -MAP_MODEL_HEIGHT_Y + 0.01f, 0.0f);
	m_segmentNode->setTextureIndex(1);
	m_segmentNode->OnOctreeSet(m_octree);

	m_disk.buildDiskXZ(20.0f, Vector3f(0.0f, 0.0f, 0.0f), 20, 20, true, false, false);
	m_disk.createBoundingBox();
	m_disk.markForDelete();

	m_sphere.buildSphere(10.0f, Vector3f(0.0f, 0.0f, 0.0f), 10, 10, true, false, false);
	m_sphere.createBoundingBox();
	m_sphere.markForDelete();

	m_ground = Physics::AddStaticObject(Physics::BtTransform(btVector3(0.0f, 0.0f, 0.0f)), new  btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), -0.1f), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER, nullptr);

	loadBuilding("res/building_0.bld");
	loadBuilding("res/building_1.bld");
	loadBuilding("res/building_2.bld");
	loadBuilding("res/building_3.bld");
	loadBuilding("res/building_5.bld");
	loadBuilding("res/building_6.bld");
	loadBuilding("res/building_8.bld");
	loadBuilding("res/building_9.bld");
	loadBuilding("res/building_7.bld");	
	loadBuilding("res/building_4.bld", true);

	loadBuilding("res/building_10.bld", true);
	loadBuilding("res/building_11.bld", true);
	loadBuilding("res/building_12.bld");
	loadBuilding("res/building_13.bld", true);
	loadBuilding("res/building_14.bld");
	loadBuilding("res/building_15.bld", true);

	loadBuilding("res/building_16.bld", true);
	loadBuilding("res/building_17.bld");
	loadBuilding("res/building_18.bld");
	loadBuilding("res/building_19.bld");
	loadBuilding("res/building_20.bld");
	
	loadBuilding("res/building_21.bld", true);
	loadBuilding("res/building_22.bld");
	loadBuilding("res/building_23.bld", true);
	loadBuilding("res/building_24.bld", true);
	loadBuilding("res/building_25.bld");
	createScene();

	m_navigationMesh = new NavigationMesh();

	m_navigationMesh->m_navigables = m_navigables;
	m_navigationMesh->SetPadding(Vector3f(0.0f, 10.0f, 0.0f));
	m_navigationMesh->SetTileSize(128);

	m_navigationMesh->SetCellSize(0.3);
	m_navigationMesh->SetCellHeight(0.2f);

	m_navigationMesh->SetAgentMaxSlope(45.0f);
	m_navigationMesh->SetAgentMaxClimb(0.9f);
	m_navigationMesh->SetAgentHeight(2.0f);
	m_navigationMesh->SetAgentRadius(0.6f);
	//m_navigationMesh->Build();

	Utils::NavIO navIO;
	navIO.readNavigationMap("res/data.nav", m_navigationMesh->numTilesX_, m_navigationMesh->numTilesZ_, m_navigationMesh->boundingBox_, m_navigationMesh->m_tileData);
	m_navigationMesh->Allocate();
	m_navigationMesh->AddTiles();
}

Adrian::~Adrian() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	Material::CleanupTextures();
	ShapeDrawer::Get().shutdown();

	delete m_octree;
	m_octree = nullptr;

	delete m_root;
	m_root = nullptr;

	Physics::DeleteAllCollisionObjects();
}

void Adrian::fixedUpdate() {
	m_heroEnity->fixedUpdate(m_fdt);
	Globals::physics->stepSimulation(m_fdt);
}

void Adrian::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
		m_camera.moveDown();
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
		m_camera.moveUp();
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		direction += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
		m_camera.moveRight();
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
		m_camera.moveLeft();
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		if (Keyboard::instance().keyDown(Keyboard::KEY_LSHIFT)) {
			m_height += 0.05f;
			m_camera.setHeight(m_height);
		}else {
			m_angle += 0.005f;
			m_camera.rotate(m_angle);
		}
		
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		if (Keyboard::instance().keyDown(Keyboard::KEY_LSHIFT)) {
			m_height -= 0.05f;
			m_camera.setHeight(m_height);
		}else {
			m_angle -= 0.005f;
			m_camera.rotate(m_angle);
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_useStreaming = !m_useStreaming;
		toggleStreaming(m_useStreaming);
	}

	Vector3f moveDir = Vector3f::ZERO;
	if (keyboard.keyDown(Keyboard::KEY_UP))
		moveDir += Vector3f::RIGHT;
	if (keyboard.keyDown(Keyboard::KEY_DOWN))
		moveDir += Vector3f::LEFT;
	if (keyboard.keyDown(Keyboard::KEY_RIGHT))
		moveDir += Vector3f::FORWARD;
	if (keyboard.keyDown(Keyboard::KEY_LEFT))
		moveDir += Vector3f::BACKWARD;

	if (moveDir.lengthSq() > 0.0f)
		Vector3f::Normalize(moveDir);

	m_heroEnity->translateRelative(moveDir);

	m_octree->updateFrameNumber();
	m_heroEnity->update(m_dt);

	m_frustum.updatePlane(m_camera.getOrthographicMatrix(), m_camera.getViewMatrix());
	m_frustum.updateVertices(m_camera.getOrthographicMatrix(), m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);

	if (m_useStreaming)
		updateStreaming();

	m_octree->updateOctree();
}

void Adrian::render() {

	int tilex, tilez;

	const int TILE_SIZE = 3000;
	const float TILE_LOWFACTOR = 4.0;
	const float TILE_HIGHFACTOR = 4.0;
	int tileSzFactor = TILE_HIGHFACTOR + TILE_LOWFACTOR;

	tilex = ((int)m_camera.m_initx / TILE_SIZE) * TILE_SIZE;
	tilez = ((int)m_camera.m_initz / TILE_SIZE) * TILE_SIZE;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Globals::textureManager.get("ground").bind(0);
	auto shader = Globals::shaderManager.getAssetPointer("map");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::Translate(tilex, 0.0f, tilez) * Matrix4f::Scale(TILE_SIZE * TILE_LOWFACTOR, 0.0f, TILE_SIZE * TILE_HIGHFACTOR));
	shader->loadFloat("u_tileFactor", m_tileFactor);
	Globals::shapeManager.get("quad_xz").drawRaw();
	shader->unuse();


	shader = Globals::shaderManager.getAssetPointer("shape_color");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadVector("u_color", m_heroEnity->getColor());

	shader = Globals::shaderManager.getAssetPointer("shape");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	for (const Batch& batch : m_octree->getOpaqueBatches().m_batches) {
		OctreeNode* drawable = batch.octreeNode;
		shader->loadMatrix("u_model", drawable->getWorldTransformation());
		drawable->drawRaw();
	}
	
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
	}

	shader->unuse();

	if (m_debugPhysic) {
		ShapeDrawer::Get().setProjectionView(m_camera.getOrthographicMatrix(), m_camera.getViewMatrix());

		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ShapeDrawer::Get().drawDynmicsWorld(Physics::GetDynamicsWorld());
		glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
		//glEnable(GL_CULL_FACE);
	}

	if (m_debugNavmesh) {
		m_navigationMesh->OnRenderDebug();
	}

	if (m_debugTree || m_debugNavmesh) {
		DebugRenderer::Get().SetProjectionView(m_camera.getOrthographicMatrix(), m_camera.getViewMatrix());
		DebugRenderer::Get().drawBuffer();
	}

	if (m_drawUi)
		renderUi();
}

void Adrian::OnMouseMotion(Event::MouseMoveEvent& event) {
	//m_mousePicker.updatePositionOrthographic(event.x, event.y, m_camera);
}

void Adrian::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);

		if (m_mousePicker.clickOrthographicAll(event.x, event.y, m_camera, m_ground)) {
			const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
			m_marker.push_back(m_root->addChild<ShapeNode, Shape>(m_sphere));
			m_marker.back()->setPosition(Physics::VectorFrom(callbackAll.m_hitPointWorld[callbackAll.index]));
			m_marker.back()->setTextureIndex(2);
			m_marker.back()->OnOctreeSet(m_octree);
			const Vector3f& pos = m_marker.back()->getPosition();
			m_heroEnity->move(pos[0], pos[2]);
		}
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
		m_mousePicker.updatePosition(event.x, event.y, m_camera);
		if (m_mousePicker.clickOrthographicAll(event.x, event.y, m_camera, m_heroEnity->getRigidBody())) {
			if (!m_heroEnity->isActive()) {
				m_diskNode = m_heroEnity->addChild<ShapeNode, Shape>(m_disk);
				m_diskNode->setPosition(0.0f, -MAP_MODEL_HEIGHT_Y + 0.01f, 0.0f);
				m_diskNode->setTextureIndex(2);
				m_diskNode->setName("disk");
				m_diskNode->OnOctreeSet(m_octree);
			}else {
				ShapeNode* marker = m_heroEnity->findChild<ShapeNode>("disk");
				marker->OnOctreeSet(nullptr);
				marker->eraseSelf();
			}
			m_heroEnity->setIsActive(!m_heroEnity->isActive());
		}else {
			if (m_heroEnity->isActive()) {
				ShapeNode* marker = m_heroEnity->findChild<ShapeNode>("disk");
				marker->OnOctreeSet(nullptr);
				marker->eraseSelf();
			}
			m_heroEnity->setIsActive(false);
		}
	}
}

void Adrian::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void Adrian::OnMouseWheel(Event::MouseWheelEvent& event) {
	if (event.direction == 1u) {
		m_zoom = m_zoom - 0.05f;
		m_zoom = Math::Clamp(m_zoom, 0.2f, 5.0f);
	}

	if (event.direction == 0u) {
		m_zoom = m_zoom + 0.05f;
		m_zoom = Math::Clamp(m_zoom, 0.2f, 5.0f);
	}
	//m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -static_cast<float>(Application::Width) / m_zoom, static_cast<float>(Application::Width) / m_zoom);
	m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -5000.0f, 5000.0f);
}

void Adrian::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Adrian::OnKeyUp(Event::KeyboardEvent& event) {

}

void Adrian::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	//m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -static_cast<float>(Application::Width) / m_zoom, static_cast<float>(Application::Width) / m_zoom);
	m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -5000.0f, 5000.0f);
	m_camera.resize(Application::Width, Application::Height);
}

void Adrian::renderUi() {
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
	ImGui::SliderFloat("Tile", &m_tileFactor, 1.0f, 100.0f);

	if (ImGui::SliderFloat("Rotate", &m_angle, -2 * PI, 2 * PI)) {
		m_camera.rotate(m_angle);
	}

	if (ImGui::SliderFloat("Height", &m_height, 1.0f, 150.0f)) {
		m_camera.setHeight(m_height);
	}

	if (ImGui::Checkbox("Use Culling", &m_useCulling)) {
		m_octree->setUseCulling(m_useCulling);
	}
	ImGui::Checkbox("Debug Tree", &m_debugTree);
	ImGui::Checkbox("Debug Physic", &m_debugPhysic);
	ImGui::Checkbox("Debug Navmesh", &m_debugNavmesh);
	if (ImGui::Button("Clear Marker"))
		clearMarker();

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Adrian::clearMarker() {
	for (auto shapeNode : m_marker) {
		shapeNode->OnOctreeSet(nullptr);
		shapeNode->eraseSelf();
	}
	m_marker.clear();
}

void Adrian::loadBuilding(const char* fn, bool changeWinding) {
	FILE *f;
	char buf[1024];
	char filepath[256];
	strncpy(filepath, fn, 256);

	if ((f = fopen(filepath, "r")) == NULL) {
		fprintf(stderr, "Cannot open building file: %s\n", filepath);
		exit(-1);
	}

	std::vector<Vector3f> positions;
	std::vector<Vector2f> texels;
	
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<Vector4f> shapeColor;

	Vector4f currentColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	std::string currentTexture;
	unsigned int currentPolygon;

	while (fgets(buf, 1024, f) != NULL) {
		char *str = buf + 5;
		float fl[5];
		int ret;

		if (!strncmp(buf, "#", 1))
			continue;

		for (int i = 0; str[i]; i++)
			if (str[i] == '\n' || str[i] == '\r')
				str[i] = '\0';

		ret = sscanf(str, "%f,%f,%f,%f,%f", &fl[0], &fl[1], &fl[2], &fl[3], &fl[4]);
		if (!strncmp(buf, "txco:", 5)) {
			texels.push_back({ fl[0], fl[1] });
		}else if (!strncmp(buf, "colr:", 5)) {
			currentColor.set(fl[0], fl[1], fl[2], fl[3]);
		}else if (!strncmp(buf, "vrtx:", 5)) {
			positions.push_back({ fl[0], fl[1], fl[2] });
		}else if (!strncmp(buf, "begn:", 5)) {
			sscanf(str, "%d", &currentPolygon);
		}else if (!strncmp(buf, "txtr:", 5)) {
			if (currentTexture != str && !currentTexture.empty()) {
				m_buildings.push_back(Shape(vertices, indices, 8u));
				m_buildings.back().createBoundingBox();
				m_buildings.back().setVec4Attribute(shapeColor, 0u, 3u);

				vertices.shrink_to_fit();
				vertices.clear();
				indices.shrink_to_fit();
				indices.clear();
				shapeColor.shrink_to_fit();
				shapeColor.clear();
			}
			currentTexture = str;
		}else if (!strncmp(buf, "ends", 4)) {
		
			if (currentPolygon == 6u) {
				unsigned int baseIndex = vertices.size() / 8;
				for (int i = 0; i < positions.size(); i++) {
					vertices.push_back(positions[i][0]); vertices.push_back(positions[i][1]); vertices.push_back(positions[i][2]);
					if (texels.size() > i) {
						vertices.push_back(texels[i][0]); vertices.push_back(texels[i][1]);
					}
					else {
						vertices.push_back(0.0f); vertices.push_back(0.0f);
					}
					vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
					shapeColor.push_back(currentColor);
				}

				for (unsigned int i = 1u; i < positions.size() - 1; i++) {
					indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? i + 1u + baseIndex : i + baseIndex); indices.push_back(changeWinding ? i+ baseIndex : i + 1u + baseIndex);
				}

				positions.shrink_to_fit();
				positions.clear();
				texels.shrink_to_fit();
				texels.clear();
			}else if (currentPolygon == 7u) {
				unsigned int baseIndex = vertices.size() / 8;
				for (int i = 0; i < positions.size(); i++) {
					vertices.push_back(positions[i][0]); vertices.push_back(positions[i][1]); vertices.push_back(positions[i][2]);
					if (texels.size() > i) {						
						vertices.push_back(texels[i][0]); vertices.push_back(texels[i][1]);
					}else {
						vertices.push_back(0.0f); vertices.push_back(0.0f);
					}
					vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
					shapeColor.push_back(currentColor);
				}
			
				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 2u + baseIndex : 1u + baseIndex); indices.push_back(changeWinding ? 1u + baseIndex : 2u + baseIndex);
				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 3u + baseIndex : 2u + baseIndex); indices.push_back(changeWinding ? 2u + baseIndex : 3u + baseIndex);

				
				positions.shrink_to_fit();
				positions.clear();
				texels.shrink_to_fit();
				texels.clear();
			}else if (currentPolygon == 8u) {

				unsigned int baseIndex = vertices.size() / 8;
				for (int i = 0; i < positions.size(); i++) {
					vertices.push_back(positions[i][0]); vertices.push_back(positions[i][1]); vertices.push_back(positions[i][2]);
					if (texels.size() > i) {
						vertices.push_back(texels[i][0]); vertices.push_back(texels[i][1]);
					}else {
						vertices.push_back(0.0f); vertices.push_back(0.0f);
					}
					vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
					shapeColor.push_back(currentColor);
				}
				
				for (unsigned int i = 0; i < positions.size() - 2; i = i + 2) {
					indices.push_back(0u + i + baseIndex); indices.push_back(changeWinding ? 1u + i + baseIndex : 2u + i + baseIndex); indices.push_back(changeWinding ? 2u + i + baseIndex : 1u + i + baseIndex);
					indices.push_back(1u + i + baseIndex); indices.push_back(changeWinding ? 3u + i + baseIndex : 2u + i + baseIndex); indices.push_back(changeWinding ? 2u + i + baseIndex : 3u + i + baseIndex);
				}

				positions.shrink_to_fit();
				positions.clear();
				texels.shrink_to_fit();
				texels.clear();
			}else if (currentPolygon == 9u) {
				
				unsigned int baseIndex = vertices.size() / 8;
				for (int i = 0; i < 8; i++) {
					vertices.push_back(positions[i][0]); vertices.push_back(positions[i][1]); vertices.push_back(positions[i][2]);
					if (texels.size() > i) {
						vertices.push_back(texels[i][0]); vertices.push_back(texels[i][1]);
					}else {
						vertices.push_back(0.0f); vertices.push_back(0.0f);
					}
					vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
					shapeColor.push_back(currentColor);
				}
				

				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 2u + baseIndex : 1u + baseIndex); indices.push_back(changeWinding ? 1u + baseIndex : 2u + baseIndex);
				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 3u + baseIndex : 2u + baseIndex); indices.push_back(changeWinding ? 2u + baseIndex : 3u + baseIndex);

				indices.push_back(0u + baseIndex);  indices.push_back(changeWinding ? 7u + baseIndex : 6u + baseIndex); indices.push_back(changeWinding ? 6u + baseIndex : 7u + baseIndex);
				indices.push_back(0u + baseIndex);  indices.push_back(changeWinding ? 6u + baseIndex : 5u + baseIndex); indices.push_back(changeWinding ? 5u + baseIndex : 6u + baseIndex);

				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 5u + baseIndex : 4u + baseIndex); indices.push_back(changeWinding ? 4u + baseIndex : 5u + baseIndex);
				indices.push_back(0u + baseIndex); indices.push_back(changeWinding ? 4u + baseIndex : 3u + baseIndex); indices.push_back(changeWinding ? 3u + baseIndex : 4u + baseIndex);

							
				positions.shrink_to_fit();
				positions.clear();
				texels.shrink_to_fit();
				texels.clear();
			}
			
		}		
	}
	m_buildings.push_back(Shape(vertices, indices, 8u));
	m_buildings.back().createBoundingBox();
	m_buildings.back().setVec4Attribute(shapeColor, 0u, 3u);
}

void Adrian::createScene(bool recreate) {

	for (int i = 0; i < 4; i++) {
		m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[i]);
		m_buildingNode->setTextureIndex(7);
		m_buildingNode->OnOctreeSet(m_octree);
		m_navigables.push_back(new Navigable(m_buildingNode));
	}

	for (int i = 4; i < 8; i++) {
		m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[i]);
		m_buildingNode->setTextureIndex(i < 6 ? 3 : 4);
		m_buildingNode->OnOctreeSet(m_octree);
		m_navigables.push_back(new Navigable(m_buildingNode));
	}
	

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[8]);
	m_buildingNode->setTextureIndex(5);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[9]);
	m_buildingNode->setTextureIndex(6);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[10]);
	m_buildingNode->setTextureIndex(3);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[11]);
	m_buildingNode->setTextureIndex(3);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[12]);
	m_buildingNode->setTextureIndex(8);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[13]);
	m_buildingNode->setTextureIndex(3);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[14]);
	m_buildingNode->setTextureIndex(7);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[15]);
	m_buildingNode->setTextureIndex(4);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[16]);
	m_buildingNode->setTextureIndex(4);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[17]);
	m_buildingNode->setTextureIndex(8);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[18]);
	m_buildingNode->setTextureIndex(7);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[19]);
	m_buildingNode->setTextureIndex(8);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[20]);
	m_buildingNode->setTextureIndex(9);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[21]);
	m_buildingNode->setTextureIndex(10);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[22]);
	m_buildingNode->setTextureIndex(10);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[23]);
	m_buildingNode->setTextureIndex(5);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[24]);
	m_buildingNode->setTextureIndex(4);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[25]);
	m_buildingNode->setTextureIndex(9);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[26]);
	m_buildingNode->setTextureIndex(9);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(m_buildings[27]);
	m_buildingNode->setTextureIndex(10);
	m_buildingNode->OnOctreeSet(m_octree);
	m_navigables.push_back(new Navigable(m_buildingNode));

	m_buildingNode = m_root->addChild<ShapeNode, Shape>(Globals::shapeManager.get("quad_xz"));
	m_buildingNode->translate(0.0f, 0.1f, 0.0f);
	m_buildingNode->setScale(1000.0f, 0.0f, 1000.0f);
	m_navigables.push_back(new Navigable(m_buildingNode));
}

void Adrian::toggleStreaming(bool enabled) {
	if (enabled) {
		int maxTiles = (2 * m_streamingDistance + 1) * (2 * m_streamingDistance + 1);
		BoundingBox boundingBox = m_navigationMesh->GetBoundingBox();
		saveNavigationData();
		m_navigationMesh->Allocate(boundingBox, maxTiles);
		updateStreaming();
	}else {
		rebuild();
	}
}

void Adrian::saveNavigationData() {
	m_addedTiles.clear();
	m_navigationMesh->saveToTileData();
}

void Adrian::updateStreaming() {
	Vector3f averageAgentPosition = m_heroEnity->getWorldPosition();
	const std::array<int, 2> heroTile = m_navigationMesh->GetTileIndex(averageAgentPosition);
	const std::array<int, 2> numTiles = m_navigationMesh->GetNumTiles();
	const std::array<int, 2> beginTile = { std::max(0, heroTile[0] - m_streamingDistance), std::max(0, heroTile[1] - m_streamingDistance) };
	const std::array<int, 2> endTile = { std::min(heroTile[0] + m_streamingDistance, numTiles[0] - 1), std::min(heroTile[1] + m_streamingDistance, numTiles[1] - 1) };

	// Remove tiles
	for (std::unordered_set<std::array<int, 2>>::iterator i = m_addedTiles.begin(); i != m_addedTiles.end();) {
		const std::array<int, 2> tileIdx = *i;
		if (beginTile[0] <= tileIdx[0] && tileIdx[0] <= endTile[0] && beginTile[1] <= tileIdx[1] && tileIdx[1] <= endTile[1])
			++i;
		else {
			m_navigationMesh->RemoveTile(tileIdx, 3u);
			i = m_addedTiles.erase(i);
		}
	}
	
	// Add tiles
	for (int z = beginTile[1]; z <= endTile[1]; ++z) {
		for (int x = beginTile[0]; x <= endTile[0]; ++x) {
			const std::array<int, 2> tileIdx = { x, z };
			bool tmp = m_navigationMesh->HasTile(tileIdx);
			if (!m_navigationMesh->HasTile(tileIdx) && m_navigationMesh->HasTileData(x, z)) {
				m_addedTiles.insert(tileIdx);
				m_navigationMesh->AddTile(x, z);
			}
		}
	}
}

void Adrian::rebuild() {
	const  std::array<int, 2> numTiles = m_navigationMesh->GetNumTiles();
	BoundingBox boundingBox = m_navigationMesh->GetBoundingBox();
	m_navigationMesh->Allocate(boundingBox, numTiles[0], numTiles[1]);
	m_navigationMesh->AddTiles();
}