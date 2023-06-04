#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Clouds.h"
#include "Constants.h"
#include "Application.h"

Clouds::Clouds(StateMachine& machine) : State(machine, CurrentState::SHAPEINTERFACE) , 
SceneFBO(1440, 900),
camera(glm::vec3(0.0f, 800.0f, 0.0f)), 
cloudsModel(&scene, &skybox),
volumetricClouds(1440, 900, &cloudsModel),
reflectionVolumetricClouds(1280, 720, &cloudsModel),
PostProcessing("shaders/post_processing.frag"),
fboVisualizer("shaders/visualizeFbo.frag") {

	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);

	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-2.5f);
	applyTransformation(m_trackball);

	fogColor = glm::vec3(0.5, 0.6, 0.7);
	lightColor = glm::vec3(255, 255, 230);
	lightColor /= 255.0;

	glm::vec3 lightPosition, seed;
	glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)Application::Width / (float)Application::Height, 5.f, 10000000.0f);
	glm::vec3 lightDir = glm::vec3(-.5, 0.5, 1.0);

	//Every scene object need these informations to be rendered
	scene.lightPos = lightPosition;
	scene.lightColor = lightColor;
	scene.fogColor = fogColor;
	scene.seed = seed;
	scene.projMatrix = proj;
	scene.cam = &camera;
	scene.sceneFBO = &SceneFBO;
	scene.lightDir = lightDir;

	drawableObject::scene = &scene;
}

Clouds::~Clouds() {

}

void Clouds::fixedUpdate() {

}

void Clouds::update() {
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
			m_camera.move(directrion * 25.0f * m_dt);
		}
	}
	m_trackball.idle();
	applyTransformation(m_trackball);
};

void Clouds::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene.lightDir = glm::normalize(scene.lightDir);
	scene.lightPos = scene.lightDir*1e6f + camera.Position;
	
	cloudsModel.update();

	SceneFBO.bind();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(fogColor[0], fogColor[1], fogColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = scene.cam->GetViewMatrix();
	scene.projMatrix = glm::perspective(glm::radians(camera.Zoom), (float)Application::Width / (float)Application::Height, 5.f, 10000000.0f);


	ScreenSpaceShader::disableTests();

	volumetricClouds.draw();

	unbindCurrentFrameBuffer();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Shader2& post = PostProcessing.getShader();
	post.use();
	post.setVec2("resolution", glm::vec2(Application::Width, Application::Height));
	post.setVec3("cameraPosition", scene.cam->Position);
	post.setSampler2D("screenTexture", SceneFBO.tex, 0);
	post.setSampler2D("cloudTEX", volumetricClouds.getCloudsTexture(), 1);
	post.setSampler2D("depthTex", SceneFBO.depthTex, 2);
	post.setSampler2D("cloudDistance", volumetricClouds.getCloudsTexture(VolumetricClouds::cloudDistance), 3);

	post.setBool("wireframe", scene.wireframe);

	post.setMat4("VP", scene.projMatrix * view);
	PostProcessing.draw();



	// Texture visualizer
	Shader2& fboVisualizerShader = fboVisualizer.getShader();
	fboVisualizerShader.use();
	fboVisualizerShader.setSampler2D("fboTex", volumetricClouds.getCloudsTexture(), 0);
	renderUi();
}

void Clouds::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Clouds::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Clouds::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}
	else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}


void Clouds::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

void Clouds::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void Clouds::renderUi() {
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
	
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

