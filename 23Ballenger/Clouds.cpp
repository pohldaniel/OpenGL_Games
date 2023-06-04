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

camera(glm::vec3(0.0f, 800.0f, 0.0f)), 
cloudsModel(&scene, &skybox),
volumetricClouds(1440, 900, &cloudsModel),
reflectionVolumetricClouds(1280, 720, &cloudsModel) {

	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 800.0f, 0.0f), Vector3f(0.0f, 800.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-2.5f);
	applyTransformation(m_trackball);

	fogColor = glm::vec3(0.5, 0.6, 0.7);
	lightColor = glm::vec3(255, 255, 230);
	lightColor /= 255.0;

	glm::vec3 lightPosition, seed;
	glm::mat4 proj = glm::perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
	glm::vec3 lightDir = glm::vec3(-.5, 0.5, 1.0);

	//Every scene object need these informations to be rendered
	scene.lightPos = lightPosition;
	scene.lightColor = lightColor;
	scene.fogColor = fogColor;
	scene.seed = seed;
	scene.projMatrix = proj;
	scene.cam = &camera;
	scene.lightDir = glm::normalize(lightDir);
	scene.lightPos = scene.lightDir*1e6f + camera.Position;

	drawableObject::scene = &scene;

	sceneBuffer.create(Application::Width, Application::Height);
	sceneBuffer.attachTexture(AttachmentTex::RGBA32F);
	sceneBuffer.attachTexture(AttachmentTex::DEPTH32F);

	Texture::SetFilter(sceneBuffer.getColorTexture(0), GL_LINEAR_MIPMAP_LINEAR);
	Texture::SetFilter(sceneBuffer.getDepthTexture(), GL_LINEAR);

	m_textureSet[0].createEmptyTexture(Application::Width, Application::Height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	m_textureSet[1].createEmptyTexture(Application::Width, Application::Height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	m_textureSet[2].createEmptyTexture(Application::Width, Application::Height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	m_textureSet[3].createEmptyTexture(Application::Width, Application::Height, GL_RGBA32F, GL_RGBA, GL_FLOAT);

	m_textureSet[0].setWrapMode(GL_REPEAT);
	m_textureSet[1].setWrapMode(GL_REPEAT);
	m_textureSet[2].setWrapMode(GL_REPEAT);
	m_textureSet[3].setWrapMode(GL_REPEAT);

	cloudsBuffer.create(Application::Width, Application::Height);
	cloudsBuffer.attachTexture(AttachmentTex::RGBA32F);
	cloudsBuffer.attachTexture(AttachmentTex::DEPTH32F);

	Texture::SetFilter(cloudsBuffer.getColorTexture(0), GL_LINEAR_MIPMAP_LINEAR);
	Texture::SetFilter(cloudsBuffer.getColorTexture(1), GL_LINEAR_MIPMAP_LINEAR);
	Texture::SetFilter(cloudsBuffer.getDepthTexture(), GL_LINEAR);

	m_clock.restart();
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

	cloudsModel.update();

	glBindImageTexture(0, m_textureSet[0].getTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(1, m_textureSet[1].getTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(2, m_textureSet[2].getTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(3, m_textureSet[3].getTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	auto shader = Globals::shaderManager.getAssetPointer("clouds");
	shader->use();
	shader->loadVector("iResolution", Vector2f(Application::Width, Application::Height));
	shader->loadFloat("iTime", m_clock.getElapsedTimeSec());
	shader->loadMatrix("inv_proj", m_camera.getInvPerspectiveMatrix());
	shader->loadMatrix("inv_view", m_camera.getInvViewMatrix());
	shader->loadVector("cameraPosition", m_camera.getPosition());
	shader->loadFloat("FOV", scene.cam->Zoom);

	Vector3f lightDirection = Vector3f::Normalize(Vector3f(scene.lightPos[0], scene.lightPos[1], scene.lightPos[2]) - Vector3f(scene.cam->Position[0], scene.cam->Position[1], scene.cam->Position[2]));
	shader->loadVector("lightDirection", lightDirection);
	shader->loadVector("lightColor", Vector3f(scene.lightColor[0], scene.lightColor[1], scene.lightColor[2]));

	shader->loadFloat("coverage_multiplier", cloudsModel.coverage);
	shader->loadFloat("cloudSpeed", cloudsModel.cloudSpeed);
	shader->loadFloat("crispiness", cloudsModel.crispiness);
	shader->loadFloat("curliness", cloudsModel.curliness);
	shader->loadFloat("absorption", cloudsModel.absorption * 0.01f);
	shader->loadFloat("densityFactor", cloudsModel.density);

	shader->loadFloat("earthRadius", cloudsModel.earthRadius);
	shader->loadFloat("sphereInnerRadius", cloudsModel.sphereInnerRadius);
	shader->loadFloat("sphereOuterRadius", cloudsModel.sphereOuterRadius);

	shader->loadVector("cloudColorTop", Vector3f(cloudsModel.cloudColorTop[0], cloudsModel.cloudColorTop[1], cloudsModel.cloudColorTop[2]));
	shader->loadVector("cloudColorBottom", Vector3f(cloudsModel.cloudColorBottom[0], cloudsModel.cloudColorBottom[1], cloudsModel.cloudColorBottom[2]));

	shader->loadVector("skyColorTop", Vector3f(cloudsModel.sky->skyColorTop[0], cloudsModel.sky->skyColorTop[1], cloudsModel.sky->skyColorTop[2]));
	shader->loadVector("skyColorBottom", Vector3f(cloudsModel.sky->skyColorBottom[0], cloudsModel.sky->skyColorBottom[1], cloudsModel.sky->skyColorBottom[2]));

	shader->loadMatrix("invViewProj", m_camera.getInvViewMatrix() * m_camera.getInvPerspectiveMatrix());
	shader->loadMatrix("gVP", m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix());

	shader->loadInt("cloud", 0);
	shader->loadInt("worley32", 1);
	shader->loadInt("weatherTex", 2);
	shader->loadInt("depthMap", 3);
	shader->loadInt("sky", 4);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, cloudsModel.perlinTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, cloudsModel.worley32);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, cloudsModel.weatherTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sceneBuffer.getDepthTexture());

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, cloudsModel.sky->getSkyTexture());

	glDispatchCompute(INT_CEIL(Application::Width, 16), INT_CEIL(Application::Height, 16), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	sceneBuffer.bind();
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	sceneBuffer.unbind();

	if (cloudsModel.postProcess) {

		cloudsBuffer.bind();

		auto post = Globals::shaderManager.getAssetPointer("cloud_post");
		post->use();

		post->loadInt("clouds", 0);
		post->loadInt("emissions", 1);
		post->loadInt("depthMap", 2);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureSet[0].getTexture());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_textureSet[1].getTexture());

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, sceneBuffer.getDepthTexture());

		post->loadVector("cloudRenderResolution", Vector2f(Application::Width, Application::Height));
		post->loadVector("resolution", Vector2f(Application::Width, Application::Height));

		glm::mat4 vp = scene.projMatrix * scene.cam->GetViewMatrix();
		glm::mat4 lightModel;
		lightModel = glm::translate(lightModel, scene.lightPos);
		glm::vec4 pos = vp * lightModel * glm::vec4(0.0, 60.0, 0.0, 1.0);
		pos = pos / pos.w;
		pos = pos * 0.5f + 0.5f;

		post->loadVector("lightPos", Vector4f(pos[0], pos[1], pos[2], pos[3]));


		bool isLightInFront = false;
		float lightDotCameraFront = glm::dot(glm::normalize(scene.lightPos - scene.cam->Position), glm::normalize(scene.cam->Front));
		if (lightDotCameraFront > 0.2) {
			isLightInFront = true;
		}
		post->loadBool("isLightInFront", isLightInFront);
		post->loadBool("enableGodRays", cloudsModel.enableGodRays);
		post->loadBool("lightDotCameraFront", lightDotCameraFront);
		post->loadFloat("time", m_clock.getElapsedTimeSec());
		
		if (!scene.wireframe)
			Globals::shapeManager.get("quad").drawRaw();

		cloudsBuffer.unbind();
	}

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto post = Globals::shaderManager.getAssetPointer("post");
	post->use();
	post->loadVector("cameraPosition", Vector3f(scene.cam->Position[0], scene.cam->Position[1], scene.cam->Position[2]));
	post->loadBool("wireframe", scene.wireframe);
	post->loadInt("screenTexture", 0);
	post->loadInt("cloudTEX", 1);
	post->loadInt("depthTex", 2);
	post->loadInt("cloudDistance", 3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, cloudsBuffer.getColorTexture(0));

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, sceneBuffer.getDepthTexture());

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_textureSet[3].getTexture());

	Globals::shapeManager.get("quad").drawRaw();
	post->unuse();
	
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

