#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Environment.h"
#include "Application.h"
#include "Menu.h"
#include "Globals.h"
#include "Application.h"

EnvironmentInterface::EnvironmentInterface(StateMachine& machine) : State(machine, CurrentState::ENVIRONMENTNTERFACE),
																	m_cloudsModel(Application::Width, Application::Height, m_light),
																	m_sky(Application::Width, Application::Height, m_light) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 4.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(0.0f);
	applyTransformation(m_trackball);

	m_light.color = Vector3f(255.0f, 255.0f, 230.0f) / 255.0f;
	m_light.direction = Vector3f(-.5f, 0.5f, 1.0f);
	m_light.position = m_light.direction*1e6f + m_camera.getPosition();

	Globals::shapeManager.buildSphere("_sphere", 1.0f, Vector3f(0.0f, 0.0f, 0.0f), 64, 64, true, true, false);
	Globals::shaderManager.loadShader("model", "res/cubemap/model.vert", "res/cubemap/model.frag");

	m_cubeBuffer = new CubeBuffer(GL_RGBA8, 1024);
	m_cubeBuffer->setFiltering(GL_LINEAR);
	m_cubeBuffer->setShader(Globals::shaderManager.getAssetPointer("quad_back"));

	m_cubeBuffer->setDrawFunction([&]() {
		for (GLuint face = 0; face < 6; ++face) {
			m_cubeBuffer->updateViewMatrix(face);
			
			m_sky.draw(m_cubeBuffer->getInvPerspectiveMatrix(), m_cubeBuffer->getInvViewMatrix());
			m_cloudsModel.draw(m_cubeBuffer->getPerspectiveMatrix(), m_cubeBuffer->getViewMatrix(), m_cubeBuffer->getInvPerspectiveMatrix(), m_cubeBuffer->getInvViewMatrix(), Vector3f(0.0f, 0.0f, 0.0f), m_cubeBuffer->getViewDirection(), m_sky);

			m_cubeBuffer->getFramebuffer().bind();
			m_cubeBuffer->getShader()->use();
			m_cubeBuffer->bindVao();
			m_cubeBuffer->attachTexture(face);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			m_cubeBuffer->getShader()->loadMatrix("view", m_cubeBuffer->getViewMatrix());
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_cloudsModel.getPostTexture());
			Globals::shapeManager.get("quad").drawRaw();
			m_cubeBuffer->unbindVao();
			m_cubeBuffer->getShader()->unuse();			
		}
		m_cubeBuffer->unbind();
	});

	m_cubeBuffer->getShader()->use();
	m_cubeBuffer->getShader()->loadMatrix("projection", m_cubeBuffer->getPerspectiveMatrix());
	m_cubeBuffer->getShader()->unuse();
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

	m_light.update(m_camera.getPosition());
	m_sky.update();
	m_cloudsModel.update();
};

void EnvironmentInterface::render() {

	if(m_updateBuffer || m_initUi)
		m_cubeBuffer->draw();

	glViewport(0, 0, Application::Width, Application::Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("model");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", m_transform.getTransformationMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));

	shader->loadInt("u_cubeMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeBuffer->getTexture());
	Globals::shapeManager.get("_sphere").drawRaw();

	Texture::Unbind(GL_TEXTURE_CUBE_MAP);
	shader->unuse();
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
	ImGui::Checkbox("Update Buffer", &m_updateBuffer);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}