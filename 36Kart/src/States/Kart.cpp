#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <States/Menu.h>

#include "Kart.h"
#include "Application.h"
#include "Globals.h"

Kart::Kart(StateMachine& machine) : State(machine, States::KART) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, -10.0f), Vector3f(0.0f, 2.0f, -10.0f) + Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(m_rotationSpeed);
	m_camera.setOffsetDistance(m_offsetDistance);
	m_camera.setMovingSpeed(50.0f);

	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClearDepth(1.0f);
	
	m_bulletDebugDrawer = new BulletDebugDrawer(Globals::shaderManager.getAssetPointer("main")->getProgram());
	Physics::GetDynamicsWorld()->setDebugDrawer(m_bulletDebugDrawer);
	ChunkNew::LoadChunks(Globals::shapeManager.get("map"));

	physicsChunkManager = new PhysicsChunkManager(ChunkNew::Chunks);

	m_root = new SceneNodeLC();
	ShapeEntity* shapeEntity;
	shapeEntity = m_root->addChild<ShapeEntity, Shape>(Globals::shapeManager.get("map"));
	shapeEntity->setScale(40.0f, 40.0f, 40.0f);

	m_entities.push_back(shapeEntity);
	m_meshSequence.loadSequence("ressources/volga/");
	m_meshSequence.loadSequenceGpu();
	m_vehicle = new Vehicle(m_meshSequence);

	for (int i = 0; i < 10; i++) {
		Globals::physics->stepSimulation(PHYSICS_STEP);
	}
}

Kart::~Kart() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Kart::fixedUpdate() {
	Globals::physics->stepSimulation(m_fdt);
}

void Kart::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f direction = Vector3f();

	processInput();
	updateVehicleControls(currentAcceleration, currentTurn);

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
		m_vehicle->roate(0.0f, 0.0f, 180.0f);
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

	float pX = m_vehicle->vehicle.getX();
	float pY = m_vehicle->vehicle.getY();
	float pZ = m_vehicle->vehicle.getZ();

	physicsChunkManager->update(pX, pZ);

	//Vector3f pos = Vector3f(pX, pY, pZ);
	//pos[1] += 1.5f;
	//m_camera.Camera::setTarget(pos);

	m_camera.follow(Physics::MatrixFrom(m_vehicle->getWorldTransform()), Physics::VectorFrom(m_vehicle->getLinearVelocity()), m_dt);
}

void Kart::render() {

	lightCtr += 0.01f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("main");
	shader->use();
	shader->loadFloat("useTexture", false);
	shader->loadVector("lightDir", Vector3f(cos(lightCtr), sin(lightCtr), 0.0f));
	shader->loadVector("lightColor", Vector3f(1.0f, 1.0f, 1.0f));
	shader->loadMatrix("modelMatrix", Matrix4f::IDENTITY);
	shader->loadMatrix("camMatrix", m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix());
	shader->loadInt("tex0", 0);

	if (m_drawBulletDebug){
		Physics::GetDynamicsWorld()->debugDrawWorld();
		m_bulletDebugDrawer->flushLines();
	}

	shader->loadFloat("useTexture", true);
	Globals::textureManager.get("map_albedo").bind(0u);
	for (auto&& entity : m_entities) {
		shader->loadMatrix("modelMatrix", entity->getWorldTransformation());
		entity->draw();
	}
	m_vehicle->draw();

	if (m_drawUi)
		renderUi();
}

void Kart::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Kart::OnMouseWheel(Event::MouseWheelEvent& event) {
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

void Kart::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void Kart::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void Kart::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Kart::OnKeyUp(Event::KeyboardEvent& event) {

}

void Kart::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Kart::renderUi() {
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
	ImGui::Checkbox("Draw Chunk", &m_drawBulletDebug);
	if (ImGui::Button("Reset Car")) {
		m_vehicle->roate(0.0f, 0.0f, 180.0f);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Kart::updateVehicleControls(Control accelerationControl, Control turnControl) {
	// Handle acceleration or braking based on the accelerationControl parameter
	switch (accelerationControl) {
	case VehicleAccelerate:
		m_vehicle->vehicle.ApplyEngineForce(2000);
		break;
	case VehicleBrake:
		m_vehicle->vehicle.ApplyEngineForce(-2500);
		break;
	default: // Covers GameInputState::Null and any other unspecified cases
		m_vehicle->vehicle.ApplyEngineForce(0);
		break;
	}

	// Handle turning based on the turnControl parameter
	switch (turnControl) {
	case VehicleTurnLeft:
		m_vehicle->vehicle.ApplySteer(0.13);
		break;
	case VehicleTurnRight:
		m_vehicle->vehicle.ApplySteer(-0.13);
		break;
	default: // Covers GameInputState::Null and any other unspecified cases
		m_vehicle->vehicle.ApplySteer(0);
		break;
	}
}

void Kart::processInput() {
	Keyboard &keyboard = Keyboard::instance();

	// Reset control states
	currentAcceleration = Control::Null;
	currentTurn = Control::Null;

	// Check for acceleration/brake
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		currentAcceleration = Control::VehicleAccelerate;
	}else if (keyboard.keyDown(Keyboard::KEY_S)) {
		currentAcceleration = Control::VehicleBrake;
	}

	// Check for turning
	if (keyboard.keyDown(Keyboard::KEY_A)) {
		currentTurn = Control::VehicleTurnLeft;
	}else if (keyboard.keyDown(Keyboard::KEY_D)) {
		currentTurn = Control::VehicleTurnRight;
	}
}