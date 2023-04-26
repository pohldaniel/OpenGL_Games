#include <time.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "ShapeInterface.h"
#include "Constants.h"
#include "Application.h"

ShapeInterface::ShapeInterface(StateMachine& machine) : State(machine, CurrentState::SHAPEINTERFACE) {
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 1.5f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-2.5f);
	applyTransformation(m_trackball);

	Globals::shaderManager.loadShader("texture", "res/program.vert", "res/texture.frag");
	Globals::shaderManager.loadShader("normal", "res/program.vert", "res/normal.frag");
	Globals::shaderManager.loadShader("tangent", "res/program.vert", "res/tangent.frag");
	Globals::shaderManager.loadShader("bitangent", "res/program.vert", "res/bitangent.frag");
	Globals::shaderManager.loadShader("geometry", "res/normalGS.vs", "res/normalGS.fs", "res/normalGS.gs");

	Globals::shapeManager.buildCapsule("capsule", 0.5f, 1.0f, Vector3f(0.0f, 0.0f, 0.0f), 20, 20, true, true, true);
	Globals::shapeManager.buildTorus("torus", 0.5f, 0.25f, Vector3f(0.0f, 0.0f, 0.0f), 49, 49, true, true, true);
	Globals::shapeManager.buildSphere("sphere_", 1.0f, Vector3f(0.0f, 0.0f, 0.0f), 49, 49, true, true, true);
	Globals::shapeManager.buildSpiral("spiral", 0.5f, 0.25f, 1.5f, 2, true, Vector3f(0.0f, -0.75f, 0.0f), 49, 49, true, true, true);
	Globals::shapeManager.buildCylinder("cylinder", 1.0f, 0.2f, 1.0f, Vector3f(0.0f, 0.0f, 0.0f), 20, 20, true, true, true);
	Globals::shapeManager.buildQuadXY("quad_", Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(2.0f, 2.0f), 10, 10, true, true, true);
	Globals::shapeManager.buildCube("cube_", Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(2.0f, 2.0f, 2.0f), 10, 10, true, true, true);

	m_currentShape = Globals::shapeManager.get("cylinder");
	m_currentShader = Globals::shaderManager.getAssetPointer("texture");
}

ShapeInterface::~ShapeInterface() {

}

void ShapeInterface::fixedUpdate() {

}

void ShapeInterface::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f directrion = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotateSmoothly(dx, dy, 0.0f);

		}

		if (move) {
			m_camera.move(directrion * 5.0f * m_dt);
		}
	}
	m_trackball.idle();
	applyTransformation(m_trackball);
};

void ShapeInterface::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_currentShader->m_program);

	m_currentShader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	m_currentShader->loadMatrix("u_view", m_camera.getViewMatrix());
	m_currentShader->loadMatrix("u_model", m_transform.getTransformationMatrix());

	m_currentShader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
	Globals::textureManager.get("grid").bind(0);
	m_currentShape.drawRaw();
	glUseProgram(0);
	renderUi();
}

void ShapeInterface::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void ShapeInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {

	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
	applyTransformation(m_trackball);
}

void ShapeInterface::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
	applyTransformation(m_trackball);
}


void ShapeInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

void ShapeInterface::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void ShapeInterface::renderUi() {
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
	int currentModel = model;
	if (ImGui::Combo("Model", &currentModel, "Torus\0Capsule\0Sphere\0Spiral\0Cylinder\0Quad\0Cube\0\0")) {
		model = static_cast<Model>(currentModel);
		switch (model) {
		case Model::TORUS:
			m_currentShape = Globals::shapeManager.get("torus");
			break;
		case Model::CAPSULE:
			m_currentShape = Globals::shapeManager.get("capsule");
			break;
		case Model::SPHERE:
			m_currentShape = Globals::shapeManager.get("sphere_");
			break;
		case Model::SPIRAL:
			m_currentShape = Globals::shapeManager.get("spiral");
			break;
		case Model::CYLINDER:
			m_currentShape = Globals::shapeManager.get("cylinder");
			break;
		case Model::QUAD:
			m_currentShape = Globals::shapeManager.get("quad_");
			break;
		case Model::CUBE:
			m_currentShape = Globals::shapeManager.get("cube_");
			break;
		}
	}

	int currentRenderMode = renderMode;
	if (ImGui::Combo("Render", &currentRenderMode, "Texture\0Normal\0Tangent\0Bitangent\0Geometry\0\0")) {
		renderMode = static_cast<RenderMode>(currentRenderMode);
		switch (renderMode) {
		case RenderMode::TEXTURE:
			m_currentShader = Globals::shaderManager.getAssetPointer("texture");
			break;
		case RenderMode::NORMAL:
			m_currentShader = Globals::shaderManager.getAssetPointer("normal");
			break;
		case RenderMode::TANGENT:
			m_currentShader = Globals::shaderManager.getAssetPointer("tangent");
			break;
		case RenderMode::BITANGENT:
			m_currentShader = Globals::shaderManager.getAssetPointer("bitangent");
			break;
		case RenderMode::GEOMETRY:
			m_currentShader = Globals::shaderManager.getAssetPointer("geometry");
			break;
		}
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}