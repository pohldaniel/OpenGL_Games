#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "engine/input/EventDispatcher.h"

#include "Game.h"
#include "Application.h"

#define MSAA_4x 4, 0, "MSAA_4x"
#define CSAA_8x 4, 8, "CSAA_8x"
#define CSAA_8xQ 8, 8, "CSAA_8xQ"
#define CSAA_16x 4, 16, "CSAA_16x"
#define CSAA_16xQ 8, 16, "CSAA_16xQ"

struct SAAMode {
	int ds;
	int cs;
	const char * name;
};

int g_numAAModes = 5;
SAAMode g_aaModes[] = {
	{ MSAA_4x },
	{ CSAA_8x },
	{ CSAA_8xQ },
	{ CSAA_16x },
	{ CSAA_16xQ }
};

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);
	
	m_camera = Camera();
	m_camera.perspective(60.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 10.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 3.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_trackball.reshape(Application::Width, Application::Height);
	applyTransformation(m_trackball);

	lineProg = new Shader("res/DrawLine.vert", "res/DrawLine.frag");
	instanceProg = new Shader("res/instance.vert", "res/instance.frag");
	glEnable(GL_DEPTH_TEST);

	float* buf = genLineSphere(40, &m_sphereBufferSize, 1);
	glGenBuffers(1, &m_sphereVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, m_sphereBufferSize, buf, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] buf;

	m_torus = new MeshTorus(Vector3f(0.0f, 0.0f, 0.0f), 0.6f, 0.1f, false, true, false, false);
	m_torus->addInstance(Matrix4f::IDENTITY);
	m_torus->addInstance(Matrix4f::Rotate(Vector3f(1.0f, 0.0f, 0.0f), 90.0f));

	m_cone = new MeshCylinder(Vector3f(0.0f, 0.0f, 0.0f), 0.1f, 0.0f, 1.0f, false, true, false, false);
	m_cone->addInstance(Matrix4f::Translate(0.0f, 0.0f, 0.1f) * Matrix4f::Rotate(Vector3f(1.0f, 0.0f, 0.0f), -90.0f));

	m_coneTop = new MeshCylinder(Vector3f(0.0f, 0.0f, 0.0f), 0.01f, 0.0f, 0.5f, false, true, false, false);

	m_coneTop->addInstance(Matrix4f::Translate(0.25f, 0.0f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(0.25f, 0.05f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(0.25f, 0.1f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(0.25f, 0.15f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(0.25f, 0.2f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(0.25f, 0.25f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(0.25f, 0.3f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(0.25f, 0.35f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(0.25f, 0.4f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(0.25f, 0.45f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));

	m_coneTop->addInstance(Matrix4f::Translate(-0.25f, 0.0f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(-0.25f, 0.05f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(-0.25f, 0.1f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(-0.25f, 0.15f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(-0.25f, 0.2f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(-0.25f, 0.25f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(-0.25f, 0.3f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(-0.25f, 0.35f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(-0.25f, 0.4f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneTop->addInstance(Matrix4f::Translate(-0.25f, 0.45f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));

	m_coneBottom = new MeshCylinder(Vector3f(0.0f, 0.0f, 0.0f), 0.01f, 0.0f, 0.5f, false, true, false, false);

	m_coneBottom->addInstance(Matrix4f::Translate(0.25f, -0.05f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(0.25f, -0.1f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(0.25f, -0.15f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(0.25f, -0.2f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(0.25f, -0.25f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(0.25f, -0.3f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(0.25f, -0.35f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(0.25f, -0.4f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(0.25f, -0.45f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), -90.0f));

	m_coneBottom->addInstance(Matrix4f::Translate(-0.25f, -0.05f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(-0.25f, -0.1f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(-0.25f, -0.15f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(-0.25f, -0.2f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(-0.25f, -0.25f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(-0.25f, -0.3f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(-0.25f, -0.35f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(-0.25f, -0.4f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));
	m_coneBottom->addInstance(Matrix4f::Translate(-0.25f, -0.45f, 0.4f) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f));

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (g_pAABox) delete g_pAABox;
	g_pAABox = new AABox();
	g_pAABox->Initialize(Application::Width, Application::Height, g_curSSSize, g_aaModes[aaMode].ds, g_aaModes[aaMode].cs);
}

Game::~Game() {}

void Game::fixedUpdate() {

}

void Game::update() {
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

	if (keyboard.keyDown(Keyboard::KEY_1)) {
		aaMode = static_cast<AAMode>(0);
	}

	if (keyboard.keyDown(Keyboard::KEY_2)) {
		aaMode = static_cast<AAMode>(1);
	}

	if (keyboard.keyDown(Keyboard::KEY_3)) {
		aaMode = static_cast<AAMode>(2);
	}

	if (keyboard.keyDown(Keyboard::KEY_4)) {
		aaMode = static_cast<AAMode>(3);
	}

	if (keyboard.keyDown(Keyboard::KEY_5)) {
		aaMode = static_cast<AAMode>(4);
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
			m_camera.move(direction * 5.0f * m_dt);
		}
	}

	m_trackball.idle();
	applyTransformation(m_trackball);
};

void Game::render(unsigned int &frameBuffer) {

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_doAA){
		g_pAABox->Activate();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	glUseProgram(lineProg->m_program);
	lineProg->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	lineProg->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
	lineProg->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
	lineProg->loadVector("color", Vector4f(0.2f, 0.2f, 0.2f, 0.0f));
	
	glLineWidth(m_doAA ? g_curSSSize : 1.0);

	glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, 41 * 40 * 2);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);

	glUseProgram(instanceProg->m_program);
	instanceProg->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	instanceProg->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
	instanceProg->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
	instanceProg->loadVector("u_viewPos", m_camera.getPosition());
	
	glLineWidth(1.0);
	instanceProg->loadVector("color", Vector4f(0.6f, 0.8f, 1.0f, 1.0f));
	m_torus->drawRawInstanced();

	glLineWidth(m_doAA ? g_curSSSize : 1.0);
	instanceProg->loadVector("color", Vector4f(0.7f, 0.5f, 0.7f, 1.0f));
	m_cone->drawRawInstanced();

	instanceProg->loadVector("color", Vector4f(0.4f, 0.9f, 0.7f, 0.0f));
	m_coneTop->drawRawInstanced();

	instanceProg->loadVector("color", Vector4f(0.7f, 0.9f, 0.4f, 1.0f));
	m_coneBottom->drawRawInstanced();

	glUseProgram(0);

	if (m_doAA) {
		g_pAABox->Deactivate();
		g_pAABox->Draw(renderMode);
	}
	///////////////////////////////////////////////////////////
	glLineWidth(1.0);

	renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {	
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {	
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {	
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::applyTransformation(TrackBall& arc) {

	m_transform.fromMatrix(arc.getTransform());
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(60.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 10.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

GLfloat* Game::genLineSphere(int sphslices, int* bufferSize, float scale) {
	GLfloat rho, drho, theta, dtheta;
	GLfloat x, y, z;
	GLfloat s, t, ds, dt;
	GLint i, j;
	GLfloat* buffer;

	int count = 0;
	int slices = (int)sphslices;
	int stacks = slices;
	buffer = new GLfloat[(slices + 1)*stacks * 3 * 2];
	if (bufferSize) *bufferSize = (slices + 1)*stacks * 3 * 2 * 4;
	ds = GLfloat(1.0 / sphslices);//slices;
	dt = GLfloat(1.0 / sphslices);//stacks;
	t = 1.0;
	drho = GLfloat(PI / (GLfloat)stacks);
	dtheta = GLfloat(2.0 * PI / (GLfloat)slices);

	for (i = 0; i < stacks; i++) {
		rho = i * drho;

		s = 0.0;
		for (j = 0; j <= slices; j++) {
			theta = (j == slices) ? GLfloat(0.0) : GLfloat(j * dtheta);
			x = -sin(theta) * sin(rho)*scale;
			z = cos(theta) * sin(rho)*scale;
			y = -cos(rho)*scale;
			buffer[count + 0] = x;
			buffer[count + 1] = y;
			buffer[count + 2] = z;
			count += 3;

			x = -sin(theta) * sin(rho + drho)*scale;
			z = cos(theta) * sin(rho + drho)*scale;
			y = -cos(rho + drho)*scale;
			buffer[count + 0] = x;
			buffer[count + 1] = y;
			buffer[count + 2] = z;
			count += 3;

			s += ds;
		}
		t -= dt;
	}
	return buffer;
}

void Game::renderUi() {
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
	ImGui::Checkbox("Draw Wirframe", &Globals::enableWireframe);
	ImGui::Checkbox("Do AA", &m_doAA);

	int currentAA = aaMode;
	if (ImGui::Combo("AA Mode", &currentAA, "FOUR\0EIGHT\0EIGHTQ\0SIXTEEN\0SIXTEENQ\0\0")) {
		aaMode = static_cast<AAMode>(currentAA);
		g_pAABox->Initialize(Application::Width, Application::Height, g_curSSSize, g_aaModes[aaMode].ds, g_aaModes[aaMode].cs);
	}

	int currentRenderMode = renderMode;
	if (ImGui::Combo("Program", &currentRenderMode, "simple bilinear downsampling\0downsampling 5 taps\0kernels 2 on alpha + downsampling 5 taps\0\0")) {
		renderMode = static_cast<RenderMode>(currentRenderMode);
		
	}
	if (ImGui::SliderFloat("SS factor", &g_curSSSize, 1.0f, 2.0f)) {
		g_pAABox->Initialize(Application::Width, Application::Height, g_curSSSize, g_aaModes[aaMode].ds, g_aaModes[aaMode].cs);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}