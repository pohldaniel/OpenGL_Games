#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/DebugRenderer.h>
#include <States/Menu.h>

#include "OctreeInterface.h"
#include "Application.h"
#include "Globals.h"


//const float OctreeInterface::OCCLUSION_MARGIN = 0.1f;

OctreeInterface::OctreeInterface(StateMachine& machine) : State(machine, States::OCTREEINTERFACE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	float aspect = static_cast<float>(Application::Width) / static_cast<float>(Application::Height);
	m_view.lookAt(Vector3f(0.0f, 50.0f, 0.0f), Vector3f(0.0f, 50.0f - 1.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f));
	m_camera.orthographic(-50.0f * aspect, 50.0f * aspect, -50.0f, 50.0f, -1000.0f, 1000.0f);

	m_camera.lookAt(Vector3f(0.0f, 0.0f, 54.0f), Vector3f(0.0f, 0.0f, 54.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(15.0f);

	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	
	WorkQueue::Init(0);
	m_octree = new Octree(m_camera, m_frustum);

	DebugRenderer::Get().setEnable(true);
	m_root = new SceneNodeLC();
	ShapeNode* child;
	for (int x = -10; x < 10; x++) {
		for (int y = -10; y < 10; y++) {
			for (int z = -10; z < 10; z++) {
				child = m_root->addChild<ShapeNode, Shape>(Globals::shapeManager.get("cube"));
				child->setPosition(2.2f * x, 2.2f * y, 2.2f * z);
				child->OnOctreeSet(m_octree);
				m_entities.push_back(child);
			}
		}
	}
	child = m_root->addChild<ShapeNode, Shape>(Globals::shapeManager.get("quad_xy"));
	child->setPosition(0.0f, 0.0f, 30.0f);
	child->OnOctreeSet(m_octree);
	m_entities.push_back(child);

	m_frustum.init();
	m_frustum.getDebug() = true;
}

OctreeInterface::~OctreeInterface() {
	Globals::physics->removeAllCollisionObjects();
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void OctreeInterface::fixedUpdate() {
	Globals::physics->stepSimulation(m_fdt);
}

void OctreeInterface::update() {
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

	perspective.perspective(m_fovx, (float)Application::Width / (float)Application::Height, m_near, m_far);
	m_frustum.updatePlane(perspective, m_camera.getViewMatrix());
	m_frustum.updateVertices(perspective, m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);

	m_octree->updateOctree(m_dt);
}

void OctreeInterface::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", !m_overview ? m_camera.getPerspectiveMatrix() : m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", !m_overview ? m_camera.getViewMatrix() : m_view);

	Globals::textureManager.get("marble").bind(0);

	for (size_t i = 0; i < m_octree->rootLevelOctants.size(); ++i) {
		const Octree::ThreadOctantResult& result = m_octree->octantResults[i];
		for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
			Octant* octant = oIt->first;
			octant->OnRenderAABB();

			const std::vector<OctreeNode*>& drawables = octant->getOctreeNodes();
			for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt) {
				ShapeNode* drawable = static_cast<ShapeNode*>(*dIt);

				shader->loadMatrix("u_model", drawable->getWorldTransformation());
				drawable->getShape().drawRaw();
				//drawable->OnRenderAABB(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
			}
		}
	}

	shader->unuse();

	if (m_useOcclusion)
		m_octree->RenderOcclusionQueries();

	m_frustum.updateVbo(perspective, m_camera.getViewMatrix());

	!m_overview ? m_frustum.drawFrustum(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix(), m_distance) : m_frustum.drawFrustum(m_camera.getOrthographicMatrix(), m_view, m_distance);
	!m_overview ? DebugRenderer::Get().SetProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix()) : DebugRenderer::Get().SetProjectionView(m_camera.getOrthographicMatrix(), m_view);

	DebugRenderer::Get().drawBuffer();

	if (m_drawUi)
		renderUi();
}

void OctreeInterface::OnMouseMotion(Event::MouseMoveEvent& event) {
	
}

void OctreeInterface::OnMouseWheel(Event::MouseWheelEvent& event) {

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
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void OctreeInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
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
	ImGui::SliderFloat("Fovx", &m_fovx, 0.01f, 180.0f);
	ImGui::SliderFloat("Far", &m_far, 25.0f, 1100.0f);
	ImGui::SliderFloat("Near", &m_near, 0.1f, 200.0f);
	ImGui::SliderFloat("Distance", &m_distance, -100.0f, 100.0f);
	ImGui::Checkbox("Overview", &m_overview);
	if (ImGui::Checkbox("Use Culling", &m_useCulling)) {
		m_octree->setUseCulling(m_useCulling);
	}

	if (ImGui::Checkbox("Use Occlusion", &m_useOcclusion)) {
		m_octree->setUseOcclusionCulling(m_useOcclusion);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}