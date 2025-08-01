#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <engine/DebugRenderer.h>
#include <States/Menu.h>

#include "Md2State.h"
#include "Application.h"
#include "Globals.h"

Md2State::Md2State(StateMachine& machine) : State(machine, States::MD2){

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 20.0f, 50.0f), Vector3f(0.0f, 19.5f, 50.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(20.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	
	Material::AddTexture("data/models/dynamic/hero/hero.tga");
	Material::AddTexture("data/models/dynamic/ripper/ripper.tga");
	Material::AddTexture("data/models/dynamic/corpse/corpse.tga");
	m_hero.load("data/models/dynamic/hero/hero.md2");
	m_ripper.load("data/models/dynamic/ripper/ripper.md2");
	m_corpse.load("data/models/dynamic/corpse/corpse.md2");

	WorkQueue::Init(0);
	m_octree = new Octree(m_camera, m_frustum, m_dt);
	m_octree->setUseOcclusionCulling(false);

	DebugRenderer::Get().setEnable(true);
	m_root = new SceneNodeLC();

	m_heroNode = m_root->addChild<Md2Node, Md2Model>(m_hero);
	m_heroNode->setTextureIndex(0);
	m_heroNode->setPosition(-20.0f, 0.0f, 0.0f);
	m_heroNode->setScale(0.5f, 0.5f, 0.5f);
	m_heroNode->setOrientation(0.0f, -90.0f, 0.0f);
	m_heroNode->setAnimationType(AnimationType::RUN);
	m_heroNode->OnOctreeSet(m_octree);

	m_heroNode2 = m_root->addChild<Md2Node, Md2Model>(m_hero);
	m_heroNode2->setTextureIndex(0);
	m_heroNode2->setPosition(0.0f, 0.0f, 0.0f);
	m_heroNode2->setScale(0.5f, 0.5f, 0.5f);
	m_heroNode2->setOrientation(0.0f, -90.0f, 0.0f);
	m_heroNode2->setAnimationType(AnimationType::STAND);
	m_heroNode2->OnOctreeSet(m_octree);

	m_ripperNode = m_root->addChild<Md2Node, Md2Model>(m_ripper);
	m_ripperNode->setTextureIndex(1);
	m_ripperNode->setPosition(20.0f, 0.0f, 0.0f);
	m_ripperNode->setScale(0.5f, 0.5f, 0.5f);
	m_ripperNode->setOrientation(0.0f, -90.0f, 0.0f);
	m_ripperNode->OnOctreeSet(m_octree);

	m_corpseNode = m_root->addChild<Md2Node, Md2Model>(m_corpse);
	m_corpseNode->setTextureIndex(2);
	m_corpseNode->setPosition(20.0f, 0.0f, -15.0f);
	m_corpseNode->setScale(0.5f, 0.5f, 0.5f);
	m_corpseNode->setOrientation(0.0f, -90.0f, 0.0f);
	m_corpseNode->setAnimationType(AnimationType::DEATH_BACK);
	m_corpseNode->OnOctreeSet(m_octree);

	//m_frustum.init();
	//m_frustum.getDebug() = true;
}

Md2State::~Md2State() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	Material::CleanupTextures();

	delete m_octree;
	m_octree = nullptr;

	delete m_root;
	m_root = nullptr;

	WorkQueue::Shutdown();
}

void Md2State::fixedUpdate() {

}

void Md2State::update() {
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
	}
	m_octree->updateFrameNumber();

	m_ripperNode->update(m_dt);
	m_heroNode->update(m_dt);
	m_heroNode2->update(m_dt);
	m_corpseNode->update(m_dt);

	m_frustum.updatePlane(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	m_frustum.updateVertices(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);
	m_octree->updateOctree();
}

void Md2State::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	auto shader = Globals::shaderManager.getAssetPointer("shape");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
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
		DebugRenderer::Get().SetProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
		DebugRenderer::Get().drawBuffer();
	}

	shader->unuse();

	if (m_drawUi)
		renderUi();
}

void Md2State::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Md2State::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void Md2State::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void Md2State::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Md2State::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Md2State::OnKeyUp(Event::KeyboardEvent& event) {

}

void Md2State::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Md2State::renderUi() {
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
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}