#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Clouds.h"
#include "Globals.h"
#include "Application.h"

Clouds::Clouds(StateMachine& machine) : State(machine, CurrentState::SHAPEINTERFACE) {

	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-5.0f);
	applyTransformation(m_trackball);

	fogColor = Vector3f(0.5, 0.6, 0.7);
	lightColor = Vector3f(255, 255, 230);
	lightColor /= 255.0;

	lightDirection = Vector3f(-.5, 0.5, 1.0);
	lightPosition = lightDirection*1e6f + m_camera.getPosition();

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
	Texture::SetFilter(cloudsBuffer.getDepthTexture(), GL_LINEAR);

	skyBuffer.create(Application::Width, Application::Height);
	skyBuffer.attachTexture(AttachmentTex::RGBA32F);
	skyBuffer.attachTexture(AttachmentTex::DEPTH32F);

	Texture::SetFilter(skyBuffer.getColorTexture(0), GL_LINEAR_MIPMAP_LINEAR);
	Texture::SetFilter(skyBuffer.getDepthTexture(), GL_LINEAR);

	m_clock.restart();

	SunsetPreset1();
	DefaultPreset();

	m_slicedCube.create(128, 128, 128);
	m_orthographic.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), 1.0f, -1.0f);

	m_noiseGen.getGloudShape(texture1);
	m_noiseGen.getGloudDetail(texture2);
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
	//idle();
};

void Clouds::render() {

	cloudsModel.update();
	updateSky();

	Vector3f lightDirection = Vector3f::Normalize(lightPosition - m_camera.getPosition());

	skyBuffer.bind();
	auto sky = Globals::shaderManager.getAssetPointer("sky");
	sky->use();
	sky->loadVector("resolution",Vector2f(Application::Width, Application::Height));
	sky->loadMatrix("inv_proj", m_camera.getInvPerspectiveMatrix());
	sky->loadMatrix("inv_view", m_camera.getInvViewMatrix());
	sky->loadVector("lightDirection", lightDirection);
	sky->loadVector("skyColorTop", skyColorTop);
	sky->loadVector("skyColorBottom", skyColorBottom);
	Globals::shapeManager.get("quad").drawRaw();
	sky->unuse();
	skyBuffer.unbind();

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
	shader->loadFloat("FOV", 60.0f);
	shader->loadVector("lightDirection", lightDirection);
	shader->loadVector("lightColor", Vector3f(lightColor[0], lightColor[1], lightColor[2]));

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

	shader->loadVector("skyColorTop", skyColorTop);
	shader->loadVector("skyColorBottom", skyColorBottom);

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

	sceneBuffer.bindDepthTexture(3);
	skyBuffer.bindColorTexture(4, 0);

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

		m_textureSet[cloudsTextureNames::fragColor].bind(0);
		m_textureSet[cloudsTextureNames::bloom].bind(1);
		sceneBuffer.bindDepthTexture(2);

		post->loadVector("cloudRenderResolution", Vector2f(Application::Width, Application::Height));
		post->loadVector("resolution", Vector2f(Application::Width, Application::Height));

		Matrix4f vp = m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix();
		Matrix4f lightModel;
		lightModel.translate(lightPosition);
		Vector4f pos = Vector4f(0.0, 60.0, 0.0, 1.0) ^ (vp * lightModel);
		pos = pos / pos[3];
		pos = pos * 0.5f + 0.5f;

		post->loadVector("lightPos", pos);

		bool isLightInFront = false;
		float lightDotCameraFront = Vector3f::Dot(Vector3f::Normalize(lightPosition - m_camera.getPosition()), m_camera.getViewDirection());
		if (lightDotCameraFront > 0.2) {
			isLightInFront = true;
		}
		post->loadBool("isLightInFront", isLightInFront);
		post->loadBool("enableGodRays", cloudsModel.enableGodRays);
		post->loadBool("lightDotCameraFront", lightDotCameraFront);
		post->loadFloat("time", m_clock.getElapsedTimeSec());
		
		if (!StateMachine::GetEnableWireframe())
			Globals::shapeManager.get("quad").drawRaw();

		cloudsBuffer.unbind();
	}

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto post = Globals::shaderManager.getAssetPointer("post");
	post->use();
	post->loadVector("cameraPosition", m_camera.getPosition());
	post->loadBool("wireframe", StateMachine::GetEnableWireframe());
	post->loadInt("screenTexture", 0);
	post->loadInt("cloudTEX", 1);
	post->loadInt("depthTex", 2);
	post->loadInt("cloudDistance", 3);

	sceneBuffer.bindColorTexture(0, 0);
	cloudsModel.postProcess ? cloudsBuffer.bindColorTexture(1, 0) : m_textureSet[cloudsTextureNames::fragColor].bind(1);
	sceneBuffer.bindDepthTexture(2);
	m_textureSet[cloudsTextureNames::cloudDistance].bind(3);

	Globals::shapeManager.get("quad").drawRaw();
	post->unuse();

	if (m_showNoise) {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		auto shader = Globals::shaderManager.getAssetPointer("texture3d");
		shader->use();
		glUseProgram(shader->m_program);
		shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", Matrix4f::IDENTITY);
		shader->loadMatrix("u_model", m_transform.getTransformationMatrix());
		shader->loadInt("u_texture", 0);
		glActiveTexture(GL_TEXTURE0);
		
		switch (m_noise) {
			case Noise::PERLIN:
				glBindTexture(GL_TEXTURE_3D, cloudsModel.perlinTex);
				break;
			case Noise::WORLEY:
				glBindTexture(GL_TEXTURE_3D, cloudsModel.worley32);
				break;
			case Noise::PERLIN2:
				glBindTexture(GL_TEXTURE_3D, texture1);
				break;
			case Noise::WORLEY2:
				glBindTexture(GL_TEXTURE_3D, texture2);
				break;
			default:
				glBindTexture(GL_TEXTURE_3D, cloudsModel.perlinTex);
				break;
		}

		m_slicedCube.drawRaw();
		shader->unuse();
	}

	if (m_showWeatherMap) {
		glDisable(GL_DEPTH_TEST);
		auto shader = Globals::shaderManager.getAssetPointer("texture");
		shader->use();
		glUseProgram(shader->m_program);
		shader->loadMatrix("u_projection", m_orthographic);
		shader->loadMatrix("u_view", Matrix4f::IDENTITY);
		shader->loadMatrix("u_model", Matrix4f::Translate(Application::Width - 150.0f, 150.0f, 0.0f) * Matrix4f::Scale(150.0f, 150.0f, 0.0f));
		shader->loadInt("u_texture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cloudsModel.weatherTex);

		Globals::shapeManager.get("quad").drawRaw();
		shader->unuse();
	}

	if (m_showQuad) {
		auto shader = Globals::shaderManager.getAssetPointer("ray_march");
		
		shader->use();

		shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", m_camera.getViewMatrix());
		shader->loadMatrix("u_model", m_transform.getTransformationMatrix());

		shader->loadFloat("time", m_clock.getElapsedTimeSec());
		shader->loadVector("windowSize", Vector2f(Application::Width, Application::Height));
		shader->loadMatrix("_CameraToWorld", Matrix4f::LookAt(Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0, -2.0f, 0), Vector3f(0.0, 1.0, 0.0)));
		shader->loadMatrix("_CameraInverseProjection", m_camera.getInvPerspectiveMatrix());

		shader->loadVector("_boxScale", Vector3f(2.0f, 2.0f, 2.0f));
		shader->loadVector("_boxPosition", Vector3f(0.0f, -0.5f, 0.4f));
		shader->loadVector("_shape", Vector4f(0.159f, 0.5f, 0.25f, 0.174f));
		shader->loadVector("_detail", Vector3f(0.615f, 0.369f, 0.379f));
		shader->loadVector("_offset", Vector3f(1.0f, 1.385f, 0.0f));
		shader->loadVector("_lightColor", Vector3f(0.804f, 0.771f, 0.662f));
		shader->loadVector("_cloudColor", Vector3f(0.809f, 0.734f, 0.819f));

		shader->loadInt("_CloudShape", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, texture1);

		shader->loadInt("_CloudDetail", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, texture2);

		Globals::shapeManager.get("quad").drawRaw();
		shader->unuse();
	}
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

	}else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Clouds::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);

	}else if (event.button == 2u) {
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

colorPreset Clouds::SunsetPreset() {
	colorPreset preset;

	preset.cloudColorBottom = Vector3f(89, 96, 109) / 255.f;
	preset.skyColorTop = Vector3f(177, 174, 119) / 255.f;
	preset.skyColorBottom = Vector3f(234, 125, 125) / 255.f;

	preset.lightColor = Vector3f(255, 171, 125) / 255.f;
	preset.fogColor = Vector3f(85, 97, 120) / 255.f;

	presetSunset = preset;

	return preset;
}

colorPreset Clouds::SunsetPreset1() {
	colorPreset preset;

	preset.cloudColorBottom = Vector3f(97, 98, 120) / 255.f;
	preset.skyColorTop = Vector3f(133, 158, 214) / 255.f;
	preset.skyColorBottom = Vector3f(241, 161, 161) / 255.f;

	preset.lightColor = Vector3f(255, 201, 201) / 255.f;
	preset.fogColor = Vector3f(128, 153, 179) / 255.f;

	presetSunset = preset;

	return preset;
}

colorPreset Clouds::DefaultPreset() {
	colorPreset preset;

	preset.cloudColorBottom = (Vector3f(65., 70., 80.)*(1.5f / 255.f));

	preset.skyColorTop = Vector3f(0.5, 0.7, 0.8)*1.05f;
	preset.skyColorBottom = Vector3f(0.9, 0.9, 0.95);

	preset.lightColor = Vector3f(255, 255, 230) / 255.f;
	preset.fogColor = Vector3f(0.5, 0.6, 0.7);

	highSunPreset = preset;

	return preset;
}

void Clouds::mixSkyColorPreset(float v, colorPreset p1, colorPreset p2) {
	float a = std::min(std::max(v, 0.0f), 1.0f);
	float b = 1.0 - a;

	//cloudColorBottom = p1.cloudColorBottom*a + p2.cloudColorBottom*b;
	skyColorTop = p1.skyColorTop*a + p2.skyColorTop*b;
	skyColorBottom = p1.skyColorBottom*a + p2.skyColorBottom*b;
	lightColor = p1.lightColor*a + p2.lightColor*b;
	fogColor = p1.fogColor*a + p2.fogColor*b;
}

void Clouds::updateSky() {
	auto sigmoid = [](float v) { return 1 / (1.0 + exp(8.0 - v * 40.0)); };
	mixSkyColorPreset(sigmoid(lightDirection[1]), highSunPreset, presetSunset);
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
	ImGui::Checkbox("Show Weather Map", &m_showWeatherMap);
	if (ImGui::Checkbox("Show Noise", &m_showNoise)) {
		m_showQuad = false;
	}

	if (ImGui::Checkbox("Show Cloud Quad", &m_showQuad)) {
		m_showNoise = false;
	}

	if (m_showNoise) {
		int currentNoise = m_noise;
		if (ImGui::Combo("Render", &currentNoise, "Perlin\0Worley\0Perlin 2\0Worley 2\0\0")) {
			m_noise = static_cast<Noise>(currentNoise);
		}
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}