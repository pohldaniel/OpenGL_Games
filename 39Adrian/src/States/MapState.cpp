#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <engine/DebugRenderer.h>
#include <Physics/ShapeDrawer.h>
#include <States/Menu.h>

#include "MapState.h"
#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

MapState::MapState(StateMachine& machine) : State(machine, States::MAP), m_camera(Application::Width, Application::Height) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -static_cast<float>(Application::Width) / m_zoom, static_cast<float>(Application::Width) / m_zoom);
	
	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	Material::AddTexture("data/models/dynamic/hero/hero.tga");
	Material::AddTexture("res/textures/los.tga");
	Material::AddTexture();

	m_heroModel.load("data/models/dynamic/hero/hero.md2");

	WorkQueue::Init(0);
	Renderer::Get().init(new Octree(m_camera, m_frustum, m_dt), new SceneNodeLC());
	m_octree = Renderer::Get().getOctree();
	m_octree->setUseOcclusionCulling(false);
	m_octree->setUseCulling(m_useCulling);
	m_root = Renderer::Get().getScene();

	DebugRenderer::Get().setEnable(true);
	m_root = new SceneNodeLC();
	m_heroEnity = m_root->addChild<Md2Entity, Md2Model>(m_heroModel);
	m_heroEnity->Md2Node::setPosition(-780.0f, MAP_MODEL_HEIGHT_Y, 780.0f);
	m_heroEnity->Md2Node::setOrientation(0.0f, 0.0f, 0.0f);
	m_heroEnity->Md2Node::setTextureIndex(0);
	m_heroEnity->OnOctreeSet(m_octree);

	ShapeDrawer::Get().init(32768);
	ShapeDrawer::Get().setCamera(m_camera);
	m_rigidBody = Physics::AddKinematicRigidBody(Physics::BtTransform(Physics::VectorFrom(m_heroEnity->getWorldPosition())), new btBoxShape(btVector3(0.5f, 0.5f, 0.5f)), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER, nullptr, false);

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

	m_ground = Physics::AddStaticObject(Physics::BtTransform(btVector3(0.0f, 0.0f, 0.0f)), new  btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), -0.1f), Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::MOUSEPICKER, nullptr);
}

MapState::~MapState() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	Material::CleanupTextures();
	Renderer::Get().shutdown();
	ShapeDrawer::Get().shutdown();

	Physics::DeleteAllCollisionObjects();
}

void MapState::fixedUpdate() {
	const BoundingBox& aabb = m_heroEnity->getLocalBoundingBox();
	const Vector3f& pos = m_heroEnity->getWorldPosition();
	const Quaternion& rot = m_heroEnity->getWorldOrientation();
	const Vector3f size = aabb.getSize();
	Vector3f pivot(0.0f, aabb.min[1] + size[1] * 0.5f, 0.0f);
	btTransform trans = Physics::BtTransform(pos + pivot, rot);
	m_rigidBody->setWorldTransform(trans);
	m_rigidBody->getCollisionShape()->setLocalScaling(Physics::VectorFrom(size * 0.75));

	Globals::physics->stepSimulation(m_fdt);
}

void MapState::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
		m_camera.moveDown(m_dt);	
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
		m_camera.moveUp(m_dt);
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		direction += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
		m_camera.moveRight(m_dt);
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
		m_camera.moveLeft(m_dt);
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
		Renderer::Get().clearMarker();
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

	//m_heroEnity->translateRelative(moveDir);

	m_octree->updateFrameNumber();
	m_heroEnity->update(m_dt);

	m_frustum.updatePlane(m_camera.getOrthographicMatrix(), m_camera.getViewMatrix());
	m_frustum.updateVertices(m_camera.getOrthographicMatrix(), m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);
	m_octree->updateOctree();
}

void MapState::render() {

	int tilex, tilez;

	const int TILE_SIZE = 300;
	const int TILE_LOWFACTOR = 4.0;
	const int TILE_HIGHFACTOR = 4.0;
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
	shader->loadVector("u_blendColor", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Globals::shapeManager.get("quad_xz").drawRaw();
	shader->unuse();


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
		DebugRenderer::Get().SetProjectionView(m_camera.getOrthographicMatrix(), m_camera.getViewMatrix());
		DebugRenderer::Get().drawBuffer();
	}

	shader->unuse();

	if (m_debugPhysic) {
		ShapeDrawer::Get().setProjectionView(m_camera.getOrthographicMatrix(), m_camera.getViewMatrix());

		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ShapeDrawer::Get().drawDynmicsWorld(Physics::GetDynamicsWorld());
		glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	if (m_drawUi)
		renderUi();
}

void MapState::OnMouseMotion(Event::MouseMoveEvent& event) {
	//m_mousePicker.updatePositionOrthographic(event.x, event.y, m_camera);
}

void MapState::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
		
		if (m_mousePicker.clickOrthographicAll(event.x, event.y, m_camera, m_ground, {})) {
			const MousePickCallbackAll& callbackAll = m_mousePicker.getCallbackAll();
			Vector3f pos = Physics::VectorFrom(callbackAll.m_hitPointWorld[callbackAll.index]);
			Renderer::Get().addMarker(pos, 20.0f, 2);
		}
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
		
		m_mousePicker.updatePosition(event.x, event.y, m_camera);
		if (m_mousePicker.clickOrthographic(event.x, event.y, m_camera, m_rigidBody)) {
			if (!m_heroEnity->isActive()) {
				m_diskNode = m_heroEnity->addChild<ShapeNode, Shape>(m_disk);
				m_diskNode->setPosition(0.0f, -MAP_MODEL_HEIGHT_Y + 0.01f, 0.0f);
				m_diskNode->setTextureIndex(2);
				m_diskNode->setName("disk");
				m_diskNode->OnOctreeSet(m_octree);	
				m_heroEnity->setIsActive(true);
			}else {
				ShapeNode* marker = m_heroEnity->findChild<ShapeNode>("disk");
				marker->OnOctreeSet(nullptr);
				marker->eraseSelf();
				m_heroEnity->setIsActive(false);
			}
			
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

void MapState::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void MapState::OnMouseWheel(Event::MouseWheelEvent& event) {
	if (event.direction == 1u) {
		m_zoom = m_zoom - 0.05f;
		m_zoom = Math::Clamp(m_zoom, 0.2f, 5.0f);
	}

	if (event.direction == 0u) {
		m_zoom = m_zoom + 0.05f;
		m_zoom = Math::Clamp(m_zoom, 0.2f, 5.0f);
	}
	m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -static_cast<float>(Application::Width) / m_zoom, static_cast<float>(Application::Width) / m_zoom);
}

void MapState::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void MapState::OnKeyUp(Event::KeyboardEvent& event) {

}

void MapState::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	//m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.orthographic(-static_cast<float>(Application::Width / 2) / m_zoom, static_cast<float>(Application::Width / 2) / m_zoom, -static_cast<float>(Application::Height / 2) / m_zoom, static_cast<float>(Application::Height / 2) / m_zoom, -static_cast<float>(Application::Width) / m_zoom, static_cast<float>(Application::Width) / m_zoom);


	m_camera.resize(Application::Width, Application::Height);
}

void MapState::renderUi() {
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

	if (ImGui::SliderFloat("Rotate", &m_angle, -2*PI, 2 * PI)) {
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
	if (ImGui::Button("Clear Marker"))
		Renderer::Get().clearMarker();
	
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}