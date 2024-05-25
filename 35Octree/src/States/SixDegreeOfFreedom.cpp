#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/DebugRenderer.h>
#include <States/Menu.h>
#include <Utils/BinaryIO.h>

#include "SixDegreeOfFreedom.h"
#include "Application.h"
#include "Globals.h"

SixDegreeOfFreedom::SixDegreeOfFreedom(StateMachine& machine) : State(machine, States::DEFAULT) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 5.0f, -60.0f) + Vector3f(0.0f, 0.0f, m_offsetDistance), Vector3f(0.0f, 5.0f, -60.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(m_rotationSpeed);
	m_camera.setMovingSpeed(15.0f);
	m_camera.setOffsetDistance(m_offsetDistance);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);
	m_background.resize(Application::Width, Application::Height);
	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	DebugRenderer::Get().setEnable(true);
	WorkQueue::Init(0);
	m_octree = new Octree(m_camera, m_frustum, m_dt);
	m_octree->setUseCulling(true);
	m_octree->setUseOcclusionCulling(true);

	createShapes();
	createScene();
}

SixDegreeOfFreedom::~SixDegreeOfFreedom() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void SixDegreeOfFreedom::fixedUpdate() {

}

void SixDegreeOfFreedom::update() {
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
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(1.0f, 0.0f, 0.0f);
		m_background.addOffset(0.001f);
		m_background.setSpeed(0.005f);
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

	m_background.update(m_dt);
	m_octree->updateFrameNumber();

	m_frustum.updatePlane(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	m_frustum.updateVertices(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);
	m_octree->updateOctree();
}

void SixDegreeOfFreedom::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	Globals::textureManager.get("proto").bind();

	for (size_t i = 0; i < m_octree->getRootLevelOctants().size(); ++i) {
		const Octree::ThreadOctantResult& result = m_octree->getOctantResults()[i];
		for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
			Octant* octant = oIt->first;
			if (m_debugTree)
				octant->OnRenderAABB(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

			const std::vector<OctreeNode*>& drawables = octant->getOctreeNodes();
			for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt) {
				OctreeNode* drawable = *dIt;
				shader->loadMatrix("u_model", drawable->getWorldTransformation());
				drawable->drawRaw();
				if (m_debugTree)
					drawable->OnRenderAABB(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
			}
		}
	}
	shader->unuse();

	if (m_drawUi)
		renderUi();
}

void SixDegreeOfFreedom::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void SixDegreeOfFreedom::OnMouseWheel(Event::MouseWheelEvent& event) {
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

void SixDegreeOfFreedom::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void SixDegreeOfFreedom::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void SixDegreeOfFreedom::OnKeyDown(Event::KeyboardEvent& event) {
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

void SixDegreeOfFreedom::OnKeyUp(Event::KeyboardEvent& event) {

}

void SixDegreeOfFreedom::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void SixDegreeOfFreedom::renderUi() {
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

void SixDegreeOfFreedom::createShapes() {
	Utils::MdlIO mdlConverter;
	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

	mdlConverter.mdlToBuffer("res/models/box.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_boxShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_boxShape.createBoundingBox();
	m_boxShape.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/Cylinder.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_cylinderShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_cylinderShape.createBoundingBox();
	m_cylinderShape.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/ship.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_shipShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_shipShape.createBoundingBox();
	m_shipShape.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/hoverbike.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_hoverbikeShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_hoverbikeShape.createBoundingBox();
	m_hoverbikeShape.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();
}

void SixDegreeOfFreedom::createScene() {
	m_root = new SceneNodeLC();
	ShapeNode* shapeNode;

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_boxShape);
	shapeNode->setPosition(0.0f, 0.0f, 0.0f);
	shapeNode->setScale(500.0f, 1.0f, 500.0f);
	shapeNode->OnOctreeSet(m_octree);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_boxShape);
	shapeNode->setPosition(0.0f, 4.99f, 250.0f);
	shapeNode->setScale(500.0f, 50.0f, 1.0f);
	shapeNode->OnOctreeSet(m_octree);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_boxShape);
	shapeNode->setPosition(0.0f, 4.99f, -250.0f);
	shapeNode->setScale(500.0f, 50.0f, 1.0f);
	shapeNode->OnOctreeSet(m_octree);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_boxShape);
	shapeNode->setPosition(250.0f, 4.99f, 0.0f);
	shapeNode->setScale(1.0f, 50.0f, 500.0f);
	shapeNode->OnOctreeSet(m_octree);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_boxShape);
	shapeNode->setPosition(-250.0f, 4.99f, 0.0f);
	shapeNode->setScale(1.0f, 50.0f, 500.0f);
	shapeNode->OnOctreeSet(m_octree);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cylinderShape);
	shapeNode->setPosition(-52.0564f, -20.7696f, -80.7397f);
	shapeNode->setScale(50.0f, 30.0f, 50.0f);
	shapeNode->setOrientation(90.0f, 0.0f, 0.0f);
	shapeNode->OnOctreeSet(m_octree);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_boxShape);
	shapeNode->setPosition(-20.1763f, -7.45501f, 3.0711f);
	shapeNode->setScale(20.0f, 30.0f, 20.0f);
	shapeNode->setOrientation(0.0f, 0.0f, 0.642788f, 0.766044f);
	shapeNode->OnOctreeSet(m_octree);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cylinderShape);
	shapeNode->setPosition(131.649f, 8.14253f, 22.008f);
	shapeNode->setScale(20.0f, 30.0f, 20.0f);
	shapeNode->OnOctreeSet(m_octree);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cylinderShape);
	shapeNode->setPosition(-166.368f, 8.14253f, 22.008f);
	shapeNode->setScale(20.0f, 30.0f, 20.0f);
	shapeNode->OnOctreeSet(m_octree);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_shipShape);
	shapeNode->setPosition(-15.8898f, 1.49614f, -22.0555f);
	shapeNode->OnOctreeSet(m_octree);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_hoverbikeShape);
	shapeNode->setPosition(80.0f, 2.0f, 40.0f);
	shapeNode->OnOctreeSet(m_octree);
}