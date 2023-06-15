#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "CloudInterface.h"
#include "Application.h"
#include "Menu.h"
#include "Globals.h"

CloudInterface::CloudInterface(StateMachine& machine) : State(machine, CurrentState::CLOUDINTERFACE),
										m_cloudsModel(Application::Width, Application::Height, m_light), 
										m_sky(Application::Width, Application::Height, m_light){

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-5.0f);
	applyTransformation(m_trackball);



	m_light.color = Vector3f(255.0f, 255.0f, 230.0f) / 255.0f;
	m_light.direction = Vector3f(-.5f, 0.5f, 1.0f);
	m_light.position = m_light.direction*1e6f + m_camera.getPosition();

	m_fogColor = m_sky.getFogColor();
	

	sceneBuffer.create(Application::Width, Application::Height);
	sceneBuffer.attachTexture2D(AttachmentTex::RGBA32F);
	sceneBuffer.attachTexture2D(AttachmentTex::DEPTH32F);

	Texture::SetFilter(sceneBuffer.getColorTexture(0), GL_LINEAR_MIPMAP_LINEAR);
	Texture::SetFilter(sceneBuffer.getDepthTexture(), GL_LINEAR);

	m_clock.restart();


	m_slicedCube.create(128, 128, 128);

	m_orthographic.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), 1.0f, -1.0f);

	m_noiseGen.getGloudShape(texture1);
	m_noiseGen.getGloudDetail(texture2);

	if (!VolumeBuffer::LoadVolumeFromRaw("res/noise/worley.raw", worley, 32, 32, 32)){
		m_volumeBuffer = new VolumeBuffer(GL_RGBA8, 32, 32, 32);
		m_volumeBuffer->setFiltering(GL_LINEAR_MIPMAP_LINEAR);
		m_volumeBuffer->setWrapMode(GL_REPEAT);
		m_volumeBuffer->setShader(Globals::shaderManager.getAssetPointer("worley"));
		m_volumeBuffer->draw();
		m_volumeBuffer->writeVolumeToRaw("res/noise/worley.raw");	
		m_volumeBuffer->getVolume(worley);
	}else {
		Texture::SetWrapMode(worley, GL_REPEAT, GL_TEXTURE_3D);
		Texture::SetFilter(worley, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TEXTURE_3D);
	}
	
	if (!VolumeBuffer::LoadVolumeFromRaw("res/noise/perlinworley.raw", perlinworley, 128, 128, 128)) {

		if (m_volumeBuffer) {
			m_volumeBuffer->resize(128, 128, 128);
		}else {			
			m_volumeBuffer = new VolumeBuffer(GL_RGBA8, 128, 128, 128);
			m_volumeBuffer->setFiltering(GL_LINEAR_MIPMAP_LINEAR);
			m_volumeBuffer->setWrapMode(GL_REPEAT);
		}
		m_volumeBuffer->setShader(Globals::shaderManager.getAssetPointer("perlinworley"));
		m_volumeBuffer->draw();
		m_volumeBuffer->writeVolumeToRaw("res/noise/perlinworley.raw");
		m_volumeBuffer->getVolume(perlinworley);
	}else {
		Texture::SetWrapMode(perlinworley, GL_REPEAT, GL_TEXTURE_3D);
		Texture::SetFilter(perlinworley, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TEXTURE_3D);
	}


	m_cloudGenerator = new Shader();
	m_cloudGenerator->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/clouds/cloudArray.vert"));
	m_cloudGenerator->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/clouds/common.frag"));
	m_cloudGenerator->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/clouds/noise.frag"));
	m_cloudGenerator->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/clouds/cloudArray.frag"));
	m_cloudGenerator->linkShaders();

	if (!ArrayBuffer::LoadArrayFromRaw("res/clouds/noise.raw", cloudsTex, 32, 32, 64)) {
		std::cout << "Build Noise" << std::endl;
		m_arrayBuffer = new ArrayBuffer(GL_RGBA8, 32, 32, 64, 8);
		m_arrayBuffer->setFiltering(GL_LINEAR_MIPMAP_LINEAR);
		m_arrayBuffer->setWrapMode(GL_REPEAT);
		m_arrayBuffer->setShader(m_cloudGenerator);

		m_arrayBuffer->setInnerDrawFunction([&]() {
			float res[8];		
			for (int j = 0; j < 8; j++) {
				if(j != 0)
					m_arrayBuffer->rebind(8, j * 8);

				res[0] = (j * 8 + 0) / 4.0f;
				res[1] = (j * 8 + 1) / 4.0f;
				res[2] = (j * 8 + 2) / 4.0f;
				res[3] = (j * 8 + 3) / 4.0f;
				res[4] = (j * 8 + 4) / 4.0f;
				res[5] = (j * 8 + 5) / 4.0f;
				res[6] = (j * 8 + 6) / 4.0f;
				res[7] = (j * 8 + 7) / 4.0f;

				m_arrayBuffer->getShader()->loadFloat("u_numCells", 2.0f);
				m_arrayBuffer->getShader()->loadFloatArray("u_zLevel", res, 8);

				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
			}
		});
		m_arrayBuffer->draw();
		m_arrayBuffer->writeArrayToRaw("res/clouds/noise.raw");
		m_arrayBuffer->getArray(cloudsTex);
		m_arrayBuffer->getVolume(cloudsTo3D);
	} else {
		Texture::ArrayTo3D(cloudsTex, cloudsTo3D);
		Texture::SetWrapMode(cloudsTo3D, GL_REPEAT, GL_TEXTURE_3D);
		Texture::SetFilter(cloudsTo3D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TEXTURE_3D);
	}
	
	m_sdfGenerator = new Shader();
	m_sdfGenerator->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/clouds/sdfArray.vert"));
	m_sdfGenerator->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/clouds/common.frag"));
	m_sdfGenerator->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/clouds/noise.frag"));
	m_sdfGenerator->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/clouds/sdfArray.frag"));
	m_sdfGenerator->linkShaders();

	if (!ArrayBuffer::LoadArrayFromRaw("res/clouds/sdf.raw", sdfTex, 128, 128, 32)) {
		std::cout << "Build SDF" << std::endl;
		m_buffer = new ArrayBuffer(GL_RGBA8, 128, 128, 32, 8);
		m_buffer->setFiltering(GL_LINEAR_MIPMAP_LINEAR);
		m_buffer->setWrapMode(GL_REPEAT);
		m_buffer->setShader(m_sdfGenerator);

		m_buffer->setInnerDrawFunction([&]() {
			float res[8];
			for (int j = 0; j < 4; j++) {
				if (j != 0)
					m_buffer->rebind(8, j * 8);

				res[0] = (j * 8 + 0);
				res[1] = (j * 8 + 1);
				res[2] = (j * 8 + 2);
				res[3] = (j * 8 + 3);
				res[4] = (j * 8 + 4);
				res[5] = (j * 8 + 5);
				res[6] = (j * 8 + 6);
				res[7] = (j * 8 + 7);
				
				m_buffer->getShader()->loadFloat("u_numCells", 2.0f);
				m_buffer->getShader()->loadFloatArray("u_zLevel", res, 8);

				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
			}
		});
		m_buffer->draw();
		m_buffer->writeArrayToRaw("res/clouds/sdf.raw");
		m_buffer->getArray(sdfTex);
		m_buffer->getVolume(sdfTo3D);

	}else {
		Texture::ArrayTo3D(sdfTex, sdfTo3D);
		Texture::SetWrapMode(sdfTo3D, GL_REPEAT, GL_TEXTURE_3D);
		Texture::SetFilter(sdfTo3D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TEXTURE_3D);
	}

	Shader::SetIncludeFromFile("common.glsl", "res/clouds/common.frag");
	Shader::SetIncludeFromFile("header.glsl", "res/clouds/header.frag");
	m_raymarch = new Shader();

	m_raymarch->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/clouds/raymarch.vert"));
	m_raymarch->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/clouds/oklab.frag"));
	m_raymarch->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/clouds/raymarch.frag"));
	m_raymarch->linkShaders();

	m_post = new Shader();
	m_post->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/clouds/post.vert"));
	m_post->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/clouds/common.frag"));
	m_post->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/clouds/oklab.frag"));
	m_post->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/clouds/post.frag"));
	m_post->linkShaders();

	rmTarget.create(Application::Width, Application::Height);
	rmTarget.attachTexture2D(AttachmentTex::RGBA32F);
	m_blueNoise.loadFromFile("res/clouds/HDR_L_0.png", true, GL_R8, GL_R, 0, 0, 0, 0);
	m_blueNoise.setWrapMode(GL_REPEAT);


	m_terrain.init("Levels/terrain01.raw");
	Globals::shaderManager.loadShader("culling", "res/terrain.vert", "res/terrain.frag");

	float height = m_terrain.heightAt(512.0f, 512.0f);
	m_camera.lookAt(Vector3f(512.0f, height + 1.75f, 512.0f), Vector3f(512.0f, height + 1.75f, 512.0f - 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
}

CloudInterface::~CloudInterface() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void CloudInterface::fixedUpdate() {

}

void CloudInterface::update() {
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

void CloudInterface::render() {

	if (m_showCloud) {	
		rmTarget.bind();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		frame++;
		if (frame == 32)
			frame = 0;
		m_raymarch->use();
		m_raymarch->loadVector("resolution", Vector2f(Application::Width, Application::Height));
		m_raymarch->loadFloat("time", m_clock.getElapsedTimeSec());
		m_raymarch->loadInt("frame", frame);
		m_raymarch->loadInt("blueNoise", 0);
		m_raymarch->loadInt("perlinWorley", 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_blueNoise.getTexture());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, cloudsTo3D);

		Globals::shapeManager.get("quad").drawRaw();
		m_raymarch->unuse();
		rmTarget.unbind();

		m_post->use();
		m_post->loadVector("resolution", Vector2f(Application::Width, Application::Height));
		m_post->loadVector("frameResolution", Vector2f(Application::Width, Application::Height));
		m_post->loadFloat("time", m_clock.getElapsedTimeSec());
		m_post->loadInt("frameTexture", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rmTarget.getColorTexture());

		Globals::shapeManager.get("quad").drawRaw();
		m_post->unuse();

		renderUi();
		return;
	}

	m_light.update(m_camera.getPosition());

	m_sky.update();
	m_cloudsModel.update();

	glEnable(GL_DEPTH_TEST);
	sceneBuffer.bind();
	glClearDepth(1.0f);


	glClearColor(m_fogColor[0], m_fogColor[1], m_fogColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (m_drawScene) {
		auto scene = Globals::shaderManager.getAssetPointer("culling");
		scene->use();
		scene->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
		scene->loadMatrix("u_view", m_camera.getViewMatrix());
		scene->loadMatrix("u_model", Matrix4f::IDENTITY);
		scene->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix()));
		scene->loadVector("campos", m_camera.getPosition());
		scene->loadVector("color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		m_terrain.drawRaw();

		scene->unuse();
	}
	sceneBuffer.unbind();
	glDisable(GL_DEPTH_TEST);

	m_sky.draw(m_camera);
	m_cloudsModel.draw(m_camera, m_sky);

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
	m_cloudsModel.postProcess ? Texture::Bind(m_cloudsModel.getPostTexture(), 1)  : m_cloudsModel.getColorTexture(cloudsTextureNames::fragColor).bind(1);
	sceneBuffer.bindDepthTexture(2);
	m_cloudsModel.getColorTexture(cloudsTextureNames::cloudDistance).bind(3);

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
			case Noise::PERLINCOMP:
				glBindTexture(GL_TEXTURE_3D, m_cloudsModel.perlinTex);
				break;
			case Noise::WORLEYCOMP:
				glBindTexture(GL_TEXTURE_3D, m_cloudsModel.worley32);
				break;	
			case Noise::PERLINVERT:
				glBindTexture(GL_TEXTURE_3D, perlinworley);
				break;
			case Noise::WORLEYVERT:
				glBindTexture(GL_TEXTURE_3D, worley);
				break;
			case Noise::PERLINCPU:
				glBindTexture(GL_TEXTURE_3D, texture1);
				break;
			case Noise::WORLEYCPU:
				glBindTexture(GL_TEXTURE_3D, texture2);
				break;
			case Noise::CLOUDS:
				glBindTexture(GL_TEXTURE_3D, cloudsTo3D);
				break;
			case Noise::SDF:
				glBindTexture(GL_TEXTURE_3D, sdfTo3D);
				break;
			default:
				glBindTexture(GL_TEXTURE_3D, m_cloudsModel.perlinTex);
				break;
		}
		m_slicedCube.drawRaw();
		shader->unuse();
	}

	if (m_showWeatherMap) {

		auto shader = Globals::shaderManager.getAssetPointer("texture");
		shader->use();
		glUseProgram(shader->m_program);
		shader->loadMatrix("u_projection", m_orthographic);
		shader->loadMatrix("u_view", Matrix4f::IDENTITY);
		shader->loadMatrix("u_model", Matrix4f::Translate(Application::Width - 150.0f, 150.0f, 0.0f) * Matrix4f::Scale(150.0f, 150.0f, 0.0f));
		shader->loadInt("u_texture", 0);
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_cloudsModel.weatherTex);
		glBindTexture(GL_TEXTURE_2D, m_sky.getSkyTexture());
		Globals::shapeManager.get("quad").drawRaw();
		shader->unuse();
	}

	if (m_showQuad) {
		auto shader = Globals::shaderManager.getAssetPointer("ray_march");
		
		shader->use();

		shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", Matrix4f::IDENTITY);
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

	if (m_showNoiseArray || m_showSDFArray) {

		auto shader = Globals::shaderManager.getAssetPointer("debug");

		shader->use();
		shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", Matrix4f::IDENTITY);
		shader->loadMatrix("u_model", m_transform.getTransformationMatrix());
		shader->loadInt("u_texture", 0);
		shader->loadUnsignedInt("u_layer", m_currentArrayIndex);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_showSDFArray ? sdfTex : cloudsTex);
		
		Globals::shapeManager.get("quad").drawRaw();
		shader->unuse();
	}

	renderUi();
}

void CloudInterface::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void CloudInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);

	}else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void CloudInterface::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);

	}else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void CloudInterface::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void CloudInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

void CloudInterface::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void CloudInterface::renderUi() {
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
	ImGui::Checkbox("Draw Scene", &m_drawScene);

	ImGui::Checkbox("Show Weather Map", &m_showWeatherMap);
	if (ImGui::Checkbox("Show Noise", &m_showNoise)) {
		m_showQuad = false;
		m_showNoiseArray = false;
		m_showSDFArray = false;
	}

	if (m_showNoise) {
		int currentNoise = m_noise;
		if (ImGui::Combo("Render", &currentNoise, "Perlin comp\0Worley comp\0Perlin vert/frag\0Worley vert/frag \0Perlin cpu\0Worley cpu\0Clouds\0Sdf\0\0")) {
			m_noise = static_cast<Noise>(currentNoise);
		}
	}

	if (ImGui::Checkbox("Show Cloud Quad", &m_showQuad)) {
		m_showNoise = false;
		m_showNoiseArray = false;
		m_showSDFArray = false;
	}

	if (ImGui::Checkbox("Show Noise Array", &m_showNoiseArray)) {
		m_showNoise = false;
		m_showQuad = false;
		m_showSDFArray = false;
		m_currentArrayIndex = 0;
	}

	if (ImGui::Checkbox("Show SDF Array", &m_showSDFArray)) {
		m_showNoise = false;
		m_showQuad = false;
		m_showNoiseArray = false;
		m_currentArrayIndex = 0;
	}

	if (m_showNoiseArray) {
		ImGui::SliderInt("Num Array", &m_currentArrayIndex, 0, 63);
	}

	if (m_showSDFArray) {
		ImGui::SliderInt("Num Array", &m_currentArrayIndex, 0, 31);
	}

	ImGui::Checkbox("Show Cloud", &m_showCloud);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

