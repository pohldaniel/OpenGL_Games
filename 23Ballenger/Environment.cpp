#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Environment.h"
#include "Application.h"
#include "Menu.h"
#include "Globals.h"

EnvironmentInterface::EnvironmentInterface(StateMachine& machine) : State(machine, CurrentState::ENVIRONMENTNTERFACE) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 1.5f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-2.5f);
	applyTransformation(m_trackball);

	/*
	Create dynamic cubemap
	*/
	// Create empty cubemap
	glGenTextures(1, &dynamicCubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, dynamicCubeTex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Allocate space for each side of the cube map
	for (GLuint i = 0; i < 6; ++i){
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, cubeMapSize,cubeMapSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	// Create framebuffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glGenRenderbuffers(1, &depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubeMapSize, cubeMapSize);
	// Attach one of the faces of the cubemap texture to current framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, dynamicCubeTex, 0);
	// Attach depth buffer to framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
	// Attach only the +X cubemap texture (for completeness)
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, dynamicCubeTex, 0);

	// Check if current configuration of framebuffer is correct
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	// Set default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	modelOrigin = Vector3f(0.0f, 0.0f, 0.0f);
	bluePos = Vector3f(4.0f, 5.0f, 1.0f);
	yellowPos = Vector3f(-5.0f, -1.0f, 1.0f);
	greenPos = Vector3f(4.5f, -5.0f, 1.5f);
	pinkPos = Vector3f(-3.5f, 7.0f, 5.0f);
	redPos = Vector3f(15.0f, -3.5f, 0.0f);

	m_skybox = RenderableObject("cube", "skybox", "skybox");
	m_skybox.setDrawFunction([&](const Camera& camera) {
		if (m_skybox.isDisabled()) return;
		glDepthFunc(GL_LEQUAL);
		glFrontFace(GL_CW);

		auto shader = Globals::shaderManager.getAssetPointer(m_skybox.getShader());
		Matrix4f view = camera.getViewMatrix();
		view[3][0] = 0.0f; view[3][1] = 0.0f; view[3][2] = 0.0f;
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", view);
		shader->loadInt("u_texture", 0);

		Globals::textureManager.get(m_skybox.getTexture()).bind(0);
		Globals::shapeManager.get(m_skybox.getShape()).drawRaw();

		Texture::Unbind(GL_TEXTURE_CUBE_MAP);

		shader->unuse();

		glFrontFace(GL_CCW);
		glDepthFunc(GL_LESS);
	});

	Globals::shaderManager.loadShader("cubemap", "res/cubemap/vert.vert", "res/cubemap/frag.frag");
	m_cubeBuffer = new CubeBuffer(GL_RGBA8, 1024);
	m_cubeBuffer->setFiltering(GL_LINEAR);
	m_cubeBuffer->setShader(Globals::shaderManager.getAssetPointer("cubemap"));

	m_cubeBuffer->getShader()->use();
	m_cubeBuffer->getShader()->loadMatrix("projection", m_cubeBuffer->getProjectionCube());
	m_cubeBuffer->getShader()->loadMatrix("u_texture", m_cubeBuffer->getProjectionCube());
	m_cubeBuffer->getShader()->unuse();

	Globals::textureManager.get("player").bind(0);

	m_cubeBuffer->setInnerDrawFunction([&]() {
		m_cubeBuffer->getShader()->loadMatrix("view", m_cubeBuffer->getViewCube());
		m_cubeBuffer->getShader()->loadMatrix("model", Matrix4f::Translate(m_cubeBuffer->getVieDirection() * 5.0f));
		Globals::shapeManager.get("sphere").drawRaw();
		
	});

	m_cubeBuffer->draw();
}

EnvironmentInterface::~EnvironmentInterface() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void EnvironmentInterface::fixedUpdate() {

}

void EnvironmentInterface::update() {
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
			m_camera.rotate(dx, dy);

		}

		if (move) {
			m_camera.move(directrion * 5.0f * m_dt);
		}
	}
	m_trackball.idle();
	applyTransformation(m_trackball);
};

void EnvironmentInterface::render() {

	glViewport(0, 0, Application::Width, Application::Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CW);

	auto shader = Globals::shaderManager.getAssetPointer("skybox");
	Matrix4f view = m_camera.getViewMatrix();
	view[3][0] = 0.0f; view[3][1] = 0.0f; view[3][2] = 0.0f;
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", view);
	shader->loadInt("u_texture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeBuffer->getTexture());

	Globals::shapeManager.get("cube").drawRaw();

	Texture::Unbind(GL_TEXTURE_CUBE_MAP);

	shader->unuse();

	glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS);

	renderUi();
}

void EnvironmentInterface::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void EnvironmentInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void EnvironmentInterface::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void EnvironmentInterface::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void EnvironmentInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

void EnvironmentInterface::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void EnvironmentInterface::renderUi() {
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
	

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}