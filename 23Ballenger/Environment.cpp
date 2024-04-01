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
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 4.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(0.0f);
	applyTransformation(m_trackball);

	m_light.color = Vector3f(255.0f, 255.0f, 230.0f) / 255.0f;
	m_light.direction = Vector3f(-.5f, 0.5f, 1.0f);
	m_light.position = m_light.direction*1e6f + m_camera.getPosition();

	m_terrain.init("res/terrain01.raw");
	m_quadTree.init(m_terrain.getPositions().data(), m_terrain.getIndexBuffer().data(), static_cast<unsigned int>(m_terrain.getIndexBuffer().size()), m_terrain.getMin(), m_terrain.getMax(), 64.0f);
	const Vector3f& pos = Vector3f(512.0f, m_terrain.heightAt(512.0f, 512.0f) + 1.0f, 512.0f);

	Globals::shapeManager.buildSphere("sphere64", 1.0f, Vector3f(0.0f, 0.0f, 0.0f), 64, 64, true, true, false);
	Globals::shaderManager.loadShader("simple", "res/cubemap/simple.vert", "res/cubemap/simple.frag", "res/cubemap/simple.gem");
	Globals::shaderManager.loadShader("terrain_gs", "res/cubemap/terrain.vert", "res/cubemap/terrain.frag", "res/cubemap/terrain.gem");
	Globals::shaderManager.loadShader("terrain_fc", "res/cubemap/terrain_fc.vert", "res/cubemap/terrain_fc.frag", "res/cubemap/terrain_fc.gem");
	Globals::shaderManager.loadShader("model", "res/cubemap/model.vert", "res/cubemap/model.frag");

	m_cubeBufferCloud = new CubeBuffer(GL_RGBA8, 1024);
	m_cubeBufferCloud->setFiltering(GL_LINEAR);
	m_cubeBufferCloud->setShader(Globals::shaderManager.getAssetPointer("quad_back"));

	m_cubeBufferCloud->getShader()->use();
	m_cubeBufferCloud->getShader()->loadMatrix("projection", m_cubeBufferCloud->getPerspectiveMatrix());
	m_cubeBufferCloud->getShader()->unuse();

	m_cubeBufferCloud->setDrawFunction([&]() {
		for (GLuint face = 0; face < 6; ++face) {
			m_cubeBufferCloud->updateViewMatrix(face);
			
			m_sky.draw(m_cubeBufferCloud->getInvPerspectiveMatrix(), m_cubeBufferCloud->getInvViewMatrix());
			m_cloudsModel.draw(m_cubeBufferCloud->getPerspectiveMatrix(), m_cubeBufferCloud->getViewMatrix(), m_cubeBufferCloud->getInvPerspectiveMatrix(), m_cubeBufferCloud->getInvViewMatrix(), Vector3f(0.0f, 0.0f, 0.0f), m_cubeBufferCloud->getViewDirection(), m_sky);

			m_cubeBufferCloud->getFramebuffer().bind();
			m_cubeBufferCloud->getShader()->use();
			m_cubeBufferCloud->attachTexture(face);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			m_cubeBufferCloud->getShader()->loadMatrix("view", m_cubeBufferCloud->getViewMatrix());
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_cloudsModel.getPostTexture());
			Globals::shapeManager.get("quad").drawRaw();
			m_cubeBufferCloud->getShader()->unuse();
		}
		m_cubeBufferCloud->unbind();
	});

	
	std::vector<std::array<float, 4>> colors = { { 1.0f, 0.0f, 0.0f, 1.0f},{ 0.0f, 1.0f, 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f, 1.0f } ,{ 1.0f, 1.0f, 0.0f, 1.0f } ,{ 1.0f, 0.0f, 1.0f, 1.0f } ,{ 0.0f, 1.0f, 1.0f, 1.0f } };

	m_cubeBufferLayerd = new CubeBuffer(GL_RGBA8, 1024);
	m_cubeBufferLayerd->setFiltering(GL_LINEAR);
	m_cubeBufferLayerd->setShader(Globals::shaderManager.getAssetPointer("simple"));

	m_cubeBufferLayerd->getShader()->use();
	m_cubeBufferLayerd->getShader()->loadMatrix("u_projection", m_cubeBufferLayerd->getPerspectiveMatrix());
	m_cubeBufferLayerd->getShader()->loadVectorArray("u_colors", colors, 6);
	m_cubeBufferLayerd->getShader()->unuse();
	m_cubeBufferLayerd->attachLayerd();

	m_cubeBufferLayerd->setDrawFunction([&]() {
		glFrontFace(GL_CW);
		m_cubeBufferLayerd->updateAllViewMatrices();
		m_cubeBufferLayerd->getFramebuffer().bind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_cubeBufferLayerd->getShader()->use();
		m_cubeBufferLayerd->getShader()->loadMatrixArray("u_cameraMatrices", m_cubeBufferLayerd->getViewMatrices(), 6);
		m_cubeBufferLayerd->getShader()->loadMatrix("u_model", Matrix4f::Scale(2.0f, 2.0f, 2.0f));
		Globals::shapeManager.get("cube").drawRaw();
		m_cubeBufferLayerd->getShader()->unuse();

		m_cubeBufferLayerd->unbind();
		glFrontFace(GL_CCW);

	});
	m_cubeBufferLayerd->draw();

	m_cubeBufferTerrain = new CubeBuffer(GL_RGBA8, 1024);
	m_cubeBufferTerrain->setFiltering(GL_LINEAR);
	m_cubeBufferTerrain->setShader(Globals::shaderManager.getAssetPointer("terrain_gs"));

	m_cubeBufferTerrain->getShader()->use();
	m_cubeBufferTerrain->getShader()->loadMatrix("u_projection", m_cubeBufferTerrain->getPerspectiveMatrix());
	m_cubeBufferTerrain->getShader()->loadVectorArray("u_colors", colors, 6);
	m_cubeBufferTerrain->getShader()->unuse();
	m_cubeBufferTerrain->setPosition(pos);
	m_cubeBufferTerrain->attachLayerd();
	m_cubeBufferTerrain->setDrawFunction([&]() {

		m_cubeBufferTerrain->updateAllViewMatrices();
		m_cubeBufferTerrain->getFramebuffer().bind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_cubeBufferTerrain->getShader()->use();
		m_cubeBufferTerrain->getShader()->loadMatrix("u_projection", m_cubeBufferTerrain->getPerspectiveMatrix());
		m_cubeBufferTerrain->getShader()->loadMatrixArray("u_cameraMatrices", m_cubeBufferTerrain->getViewMatrices(), 6);
		m_cubeBufferTerrain->getShader()->loadMatrix("u_model", Matrix4f::IDENTITY);

		m_cubeBufferTerrain->getShader()->loadVector("lightPos", Vector3f(50.0f, 50.0f, 50.0f));
		m_cubeBufferTerrain->getShader()->loadVector("lightAmbient", Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
		m_cubeBufferTerrain->getShader()->loadVector("lightDiffuse", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		m_cubeBufferTerrain->getShader()->loadVector("lightSpecular", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

		m_cubeBufferTerrain->getShader()->loadVector("matAmbient", Vector4f(0.7f, 0.7f, 0.7f, 1.0f));
		m_cubeBufferTerrain->getShader()->loadVector("matDiffuse", Vector4f(0.8f, 0.8f, 0.8f, 1.0f));
		m_cubeBufferTerrain->getShader()->loadVector("matSpecular", Vector4f(0.3f, 0.3f, 0.3f, 1.0f));
		m_cubeBufferTerrain->getShader()->loadVector("matEmission", Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
		m_cubeBufferTerrain->getShader()->loadFloat("matShininess", 100.0f);

		m_cubeBufferTerrain->getShader()->loadInt("tex_top", 0);
		m_cubeBufferTerrain->getShader()->loadInt("tex_side", 1);
		m_cubeBufferTerrain->getShader()->loadFloat("height", 0.0f);
		m_cubeBufferTerrain->getShader()->loadFloat("hmax", 4.0f);

		Globals::textureManager.get("grass").bind(0);
		Globals::textureManager.get("rock").bind(1);

		m_terrain.drawRaw();
		m_cubeBufferTerrain->getShader()->unuse();
		m_cubeBufferTerrain->unbind();

	});
	m_cubeBufferTerrain->draw();

	m_cubeBufferTerrainFC = new CubeBuffer(GL_RGBA8, 1024);
	m_cubeBufferTerrainFC->setFiltering(GL_LINEAR);
	m_cubeBufferTerrainFC->setShader(Globals::shaderManager.getAssetPointer("terrain_fc"));
	m_cubeBufferTerrainFC->setPosition(pos);
	m_cubeBufferTerrainFC->attachLayerd();
	m_cubeBufferTerrainFC->setDrawFunction([&]() {
	
		m_cubeBufferTerrainFC->updateAllViewMatrices();
		m_cubeBufferTerrainFC->getFramebuffer().bind();
		
		glClearDepth(0.0f);
		glDepthFunc(GL_GREATER);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_cubeBufferTerrainFC->getShader()->use();

		m_cubeBufferTerrainFC->getShader()->loadMatrix("g_modelMatrix", Matrix4f::InvTranslate(m_cubeBufferTerrainFC->getPosition()));
		m_cubeBufferTerrainFC->getShader()->loadMatrix("g_viewMatrix", Matrix4f::IDENTITY);
		m_cubeBufferTerrainFC->getShader()->loadMatrixArray("u_cameraMatrices", m_cubeBufferTerrainFC->getViewMatrices(), 6);
		
		m_cubeBufferTerrainFC->getShader()->loadVector("lightPos", Vector3f(50.0f, 50.0f, 50.0f));
		m_cubeBufferTerrainFC->getShader()->loadVector("lightAmbient", Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
		m_cubeBufferTerrainFC->getShader()->loadVector("lightDiffuse", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		m_cubeBufferTerrainFC->getShader()->loadVector("lightSpecular", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

		m_cubeBufferTerrainFC->getShader()->loadVector("matAmbient", Vector4f(0.7f, 0.7f, 0.7f, 1.0f));
		m_cubeBufferTerrainFC->getShader()->loadVector("matDiffuse", Vector4f(0.8f, 0.8f, 0.8f, 1.0f));
		m_cubeBufferTerrainFC->getShader()->loadVector("matSpecular", Vector4f(0.3f, 0.3f, 0.3f, 1.0f));
		m_cubeBufferTerrainFC->getShader()->loadVector("matEmission", Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
		m_cubeBufferTerrainFC->getShader()->loadFloat("matShininess", 100.0f);

		m_cubeBufferTerrainFC->getShader()->loadInt("tex_top", 0);
		m_cubeBufferTerrainFC->getShader()->loadInt("tex_side", 1);
		m_cubeBufferTerrainFC->getShader()->loadFloat("height", 0.0f);
		m_cubeBufferTerrainFC->getShader()->loadFloat("hmax", 4.0f);

		Globals::textureManager.get("grass").bind(0);
		Globals::textureManager.get("rock").bind(1);

		m_terrain.drawRaw();
		m_cubeBufferTerrainFC->getShader()->unuse();
		m_cubeBufferTerrainFC->unbind();

		glClearDepth(1.0f);
		glDepthFunc(GL_LESS);

	});
	m_cubeBufferTerrainFC->draw();
}

EnvironmentInterface::~EnvironmentInterface() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);

	delete m_cubeBufferCloud;
	delete m_cubeBufferLayerd;
	delete m_cubeBufferTerrain;
	delete m_cubeBufferTerrainFC;
}

void EnvironmentInterface::fixedUpdate() {

}

void EnvironmentInterface::update() {
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
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);

		}

		if (move) {
			m_camera.move(direction * 5.0f * m_dt);
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
		m_cubeBufferCloud->draw();

	glViewport(0, 0, Application::Width, Application::Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (m_drawSphere) {

		auto shader = Globals::shaderManager.getAssetPointer("model");
		shader->use();
		shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", m_camera.getViewMatrix());
		shader->loadMatrix("u_model", m_transform.getTransformationMatrix());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));

		shader->loadInt("u_cubeMap", 0);
		glActiveTexture(GL_TEXTURE0);

		switch (m_mode) {
			case Mode::CLOUD:
				glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeBufferCloud->getTexture());
				break;
			case Mode::WITHCULLING:
				glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeBufferTerrainFC->getTexture());
				break;
			case Mode::WITHOUTCULLING:
			
				glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeBufferTerrain->getTexture());
				break;
			default:
				glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeBufferCloud->getTexture());
				break;
		}

		Globals::shapeManager.get("sphere64").drawRaw();
		Texture::Unbind(GL_TEXTURE_CUBE_MAP);
		shader->unuse();

	}else {

		auto shader = Globals::shaderManager.getAssetPointer("skybox");
		shader->use();
		shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
		shader->loadInt("u_texture", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeBufferLayerd->getTexture());

		Globals::shapeManager.get("cube").drawRaw();

		Texture::Unbind(GL_TEXTURE_CUBE_MAP);

		shader->unuse();
	}

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
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1500.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_trackball.reshape(Application::Width, Application::Height);
	m_cloudsModel.resize(Application::Width, Application::Height);
	m_sky.resize(Application::Width, Application::Height);
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
	if (ImGui::Button("Toggle VSYNC")) {
		Application::ToggleVerticalSync();
	}
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::Checkbox("Update Cloud Buffer", &m_updateBuffer);
	ImGui::Checkbox("Draw Sphere", &m_drawSphere);

	int currentMode = m_mode;
	if (ImGui::Combo("Render", &currentMode, "Clouds\0With Culling\0Without Culling\0\0")) {
		m_mode = static_cast<Mode>(currentMode);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}