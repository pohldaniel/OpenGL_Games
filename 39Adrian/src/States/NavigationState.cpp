#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <engine/DebugRenderer.h>
#include <Utils/BinaryIO.h>
#include <States/Menu.h>

#include "NavigationState.h"
#include "Application.h"
#include "Globals.h"

NavigationState::NavigationState(StateMachine& machine) : State(machine, States::NAVIGATION) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

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

	WorkQueue::Init(0);
	m_octree = new Octree(m_camera, m_frustum, m_dt);
	m_octree->setUseOcclusionCulling(false);

	DebugRenderer::Get().setEnable(true);

	Material::AddTexture("res/textures/ProtoWhite256.jpg");
	Material::AddTexture("res/textures/ProtoGreen256.jpg");
	Material::AddTexture("res/textures/ProtoBlue256.jpg");
	Material::AddTexture("res/textures/ProtoRed256.jpg");
	Material::AddTexture("res/textures/ProtoYellow256.jpg");
	Material::AddTexture();

	m_beta.loadModelMdl("res/models/BetaLowpoly/Beta.mdl");
	createShapes();
	createScene();
}

NavigationState::~NavigationState() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void NavigationState::fixedUpdate() {

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

	m_octree->updateFrameNumber();

	
	m_frustum.updatePlane(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	m_frustum.updateVertices(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);
	m_octree->updateOctree();
}

void NavigationState::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("shape");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
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

	if (m_debugTree) {
		DebugRenderer::Get().SetProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
		DebugRenderer::Get().drawBuffer();
	}

	shader->unuse();

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
	}
}

void NavigationState::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void NavigationState::OnMouseWheel(Event::MouseWheelEvent& event) {

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

void NavigationState::createScene() {
	m_root = new SceneNodeLC();
	ShapeNode* shapeNode;

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_ground);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(0);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cylinder);
	shapeNode->setPosition(23.2655f, -0.414571f, -24.8348f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(1);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-0.0426907f, 2.31663f, -9.42164f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-14.0839f, 2.31663f, 1.92646f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-11.4615f, 2.31663f, -22.13f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-21.9248f, 2.31663f, -8.26868f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-6.17903f, 2.31663f, 6.16944f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(22.4007f, 2.30943f, -9.9086f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, -20.859f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, -9.20306f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, -32.5239f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, -44.1777f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube14);
	shapeNode->setPosition(-22.3009f, 2.33817f, -31.3599f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(-26.3652f, 2.36106f, 16.6828f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(22.4007f, 2.30943f, -40.1603f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube17);
	shapeNode->setPosition(-26.3733f, 4.53696f, -18.4788f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(1);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(11.2092f, 2.31663f, -30.8257);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube14);
	shapeNode->setPosition(-30.3823f, 2.33817f, -42.2312f);
	shapeNode->setOrientation(Quaternion(0.0f, 1.0f, 0.0f, 0.0f));
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube14);
	shapeNode->setPosition(18.3143f, 2.33817f, -13.2117f);
	shapeNode->setOrientation(Quaternion(0.0f, 1.0f, 0.0f, 0.0f));
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	shapeNode = m_root->addChild<ShapeNode, Shape>(m_cube);
	shapeNode->setPosition(22.4007f, 2.30943f, 1.55447f);
	shapeNode->setScale(1.71352f, 1.0f, 3.86812f);
	shapeNode->OnOctreeSet(m_octree);
	shapeNode->setTextureIndex(2);

	AnimationNode* animationNode = m_root->addChild<AnimationNode, AnimatedModel>(m_beta);
	animationNode->setPosition(0.0f, 0.5f, -30.0f);
	animationNode->OnOctreeSet(m_octree);
	animationNode->setTextureIndex(3);
}