#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/BuiltInShader.h>
#include <engine/DebugRenderer.h>
#include <States/Menu.h>

#include "AnimationInterface.h"
#include "Application.h"
#include "Globals.h"

AnimationInterface::AnimationInterface(StateMachine& machine) : State(machine, States::ANIMATIONINTERFACE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	float aspect = static_cast<float>(Application::Width) / static_cast<float>(Application::Height);
	m_view.lookAt(Vector3f(0.0f, 50.0f, 0.0f), Vector3f(0.0f, 50.0f - 1.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f));
	m_camera.orthographic(-50.0f * aspect, 50.0f * aspect, -50.0f, 50.0f, -1000.0f, 1000.0f);

	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(15.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_beta.loadModelMdl("res/models/BetaLowpoly/Beta.mdl");
	m_beta.m_meshes[0]->getMeshBones()[0].initialPosition.translate(-1.0f, 0.0f, 0.0f);
	m_beta.m_meshes[0]->getMeshBones()[0].initialRotation.rotate(0.0f, 180.0f, 0.0f);
	
	glGenBuffers(1, &BuiltInShader::matrixUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4f) * 96, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 3, BuiltInShader::matrixUbo, 0, sizeof(Matrix4f) * 96);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	WorkQueue::Init(0);
	m_octree = new Octree(m_camera, m_frustum, m_dt);

	m_root = new SceneNodeLC();
	AnimationNode* child;
	for (int x = -5; x < 5; x++) {
		for (int z = -5; z < 5; z++) {
			child = m_root->addChild<AnimationNode, AnimatedModel>(m_beta);
			child->setPosition(2.0f * x, 0.0f , 2.0f * z);
			child->addAnimationState(Globals::animationManagerNew.getAssetPointer("beta_run"));
			child->getAnimationState(0)->setLooped(true);
			child->OnOctreeSet(m_octree);
			m_entities.push_back(child);
		}
	}

	DebugRenderer::Get().setEnable(true);
	m_frustum.init();
	m_frustum.getDebug() = true;
}

AnimationInterface::~AnimationInterface() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void AnimationInterface::fixedUpdate() {

}

void AnimationInterface::update() {
	
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
	for (auto&& entitie : m_entities) {
		entitie->update(m_dt);
	}
	perspective.perspective(m_fovx, (float)Application::Width / (float)Application::Height, m_near, m_far);
	m_frustum.updatePlane(perspective, m_camera.getViewMatrix());
	m_frustum.updateVertices(perspective, m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);

	m_octree->updateOctree();
}

void AnimationInterface::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadMatrix("u_projection", !m_overview ? m_camera.getPerspectiveMatrix() : m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", !m_overview ? m_camera.getViewMatrix() : m_view);
	shader->loadVector("u_light", Vector3f(1.0f, 1.0f, 1.0f));
	shader->loadVector("u_color", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));

	Globals::textureManager.get("null").bind();

	for (size_t i = 0; i < m_octree->getRootLevelOctants().size(); ++i) {
		const Octree::ThreadOctantResult& result = m_octree->getOctantResults()[i];
		for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
			Octant* octant = oIt->first;
			if(m_debugTree)
				octant->OnRenderAABB(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

			const std::vector<OctreeNode*>& drawables = octant->getOctreeNodes();
			for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt) {
				OctreeNode* drawable = *dIt;
				drawable->drawRaw();				
				if (m_debugTree)
					drawable->OnRenderAABB(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
			}
		}
	}
	
	shader->unuse();

	if (m_useOcclusion)
		m_octree->renderOcclusionQueries();

	if (m_overview) {
		m_frustum.updateVbo(perspective, m_camera.getViewMatrix());
		m_frustum.drawFrustum(m_camera.getOrthographicMatrix(), m_view, m_distance);
	}

	!m_overview ? DebugRenderer::Get().SetProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix()) : DebugRenderer::Get().SetProjectionView(m_camera.getOrthographicMatrix(), m_view);
	
	if (m_debugTree)
		DebugRenderer::Get().drawBuffer();

	if (m_drawUi)
		renderUi();
}

void AnimationInterface::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void AnimationInterface::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void AnimationInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void AnimationInterface::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void AnimationInterface::OnKeyDown(Event::KeyboardEvent& event) {
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

void AnimationInterface::OnKeyUp(Event::KeyboardEvent& event) {

}

void AnimationInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void AnimationInterface::renderUi() {
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
	ImGui::Checkbox("Debug Tree", &m_debugTree);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}