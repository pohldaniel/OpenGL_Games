#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <thread>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"
#include <windows.h>

#define posix_memalign(p, a, s) (((*(p)) = _aligned_malloc((s), (a))), *(p) ?0 :errno)

Game::Game(StateMachine& machine) : State(machine, States::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-5.0f);
	applyTransformation(m_trackball);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	video_reader_open(&vr_state, "sample.avi");
	constexpr int ALIGNMENT = 128;
	frame_width = vr_state.width;
	frame_height = vr_state.height;

	glGenTextures(1, &tex_handle);
	glBindTexture(GL_TEXTURE_2D, tex_handle);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, vr_state.width, vr_state.height,0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	posix_memalign((void**)&frame_data, ALIGNMENT, frame_width * frame_height * 4);

	vertex = Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/shader/gauss.vert");
	
	recompileShader();
	createBuffers(AttachmentTex::AttachmentTex::RGBA);

	m_mainRT.create(Application::Width, Application::Height);
	m_mainRT.attachTexture2D(AttachmentTex::RGBA);
	m_mainRT.attachRenderbuffer(AttachmentRB::DEPTH24);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {

}

void Game::update() {
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
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		m_background.addOffset(0.001f);
		m_background.setSpeed(0.005f);
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
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}

		if (move) {
			m_camera.move(directrion * m_dt);
		}
	}

	m_background.update(m_dt);
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());
}

void Game::render() {
	m_mainRT.bindWrite();
	glClearBufferfv(GL_COLOR, 0, std::vector<float>{0.494f, 0.686f, 0.796f, 1.0f}.data());
	glClearBufferfv(GL_DEPTH, 0, &std::vector<float>{1.0f}[0]);


	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int64_t pts;
	video_reader_read_frame(&vr_state, frame_data, &pts);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_handle);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame_width, frame_height, GL_RGBA, GL_UNSIGNED_BYTE, frame_data);
	
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	//shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * Matrix4f::Scale(static_cast<float>(frame_width), static_cast<float>(frame_height), 1.0f));
	shader->loadVector("u_color", Vector4f(m_color[0], m_color[1], m_color[2], 1.0f));
	shader->loadBool("u_flip", true);
	//shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	//shader->loadUnsignedInt("u_texture", 0u);
	Globals::shapeManager.get("quad").drawRaw();

	glBindTexture(GL_TEXTURE_2D, 0);
	m_mainRT.unbindWrite();

	blurPass(m_mainRT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	//shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	//shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	shader->loadBool("u_flip", false);


	m_useBlue ? blurBuffer[1].bindColorTexture() : m_mainRT.bindColorTexture();
	m_zoomableQuad.drawRaw();
	m_useBlue ? blurBuffer[1].unbindColorTexture() : m_mainRT.unbindColorTexture();

	shader->unuse();

	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {	
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
	
} 

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {

	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {

	if (event.delta < 0) {
		m_zoomFactor -= 0.1f;
		m_zoomableQuad.setZoom(m_zoomFactor);
	}

	if (event.delta > 0) {
		m_zoomFactor += 0.1f;
		m_zoomableQuad.setZoom(m_zoomFactor);
	}
}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {

}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_mainRT.resize(Application::Width, Application::Height);

	createBuffers(AttachmentTex::AttachmentTex::RGBA);
	recompileShader();
}

void Game::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
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
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.2f, nullptr, &dockSpaceId);
		//ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Gaussian Blur", &m_useBlue);

	if (ImGui::SliderFloat("Blur Width", &m_blurWidth, 1.0f, 10.0f)) {
		recompileShader();
	}
	ImGui::ColorEdit3("color", m_color);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::recompileShader() {
	float *weights;
	int width;
	weights = generateGaussianWeights(m_blurWidth, width);
	generate1DConvolutionFP_filter(blurH, weights, width, false, true, Application::Width / 2, Application::Height / 2);
	generate1DConvolutionFP_filter(blurV, weights, width, true, true, Application::Width / 2, Application::Height / 2);
}

void Game::createBuffers(AttachmentTex::AttachmentTex texFormat) {	
	for (int i = 0; i<BLUR_BUFFERS; i++) {
		blurBuffer[i].cleanup();
	}
	
	for (int i = 0; i < BLUR_BUFFERS; i++) {
		blurBuffer[i].create(Application::Width / 4, Application::Height / 4);
		blurBuffer[i].attachTexture2D(texFormat);
	}	
}


// 1d Gaussian distribution, s is standard deviation
float gaussian(float x, float s) {
	return expf(-x*x / (2.0f*s*s)) / (s*sqrtf(2.0f*PI));
}

// generate array of weights for Gaussian blur
float* Game::generateGaussianWeights(float s, int &width) {
	width = (int)floor(3.0f*s) - 1;
	int size = width * 2 + 1;
	float *weight = new float[size];

	float sum = 0.0;
	int x;
	for (x = 0; x<size; x++) {
		weight[x] = gaussian((float)x - width, s);
		sum += weight[x];
	}

	for (x = 0; x<size; x++) {
		weight[x] /= sum;
	}
	return weight;
}

float* generateTriangleWeights(int width) {
	float *weights = new float[width];
	float sum = 0.0f;
	for (int i = 0; i<width; i++) {
		float t = i / (float)(width - 1);
		weights[i] = 1.0f - abs(t - 0.5f)*2.0f;
		sum += weights[i];
	}

	for (int i = 0; i<width; i++) {
		weights[i] /= sum;
	}
	return weights;
}

void Game::generate1DConvolutionFP_filter(Shader*& shader, float *weights, int width, bool vertical, bool tex2D, int img_width, int img_height) {
	// calculate new set of weights and offsets
	int nsamples = 2 * width + 1;
	int nsamples2 = (int)ceilf(nsamples / 2.0f);
	float *weights2 = new float[nsamples2];
	float *offsets = new float[nsamples2];

	for (int i = 0; i<nsamples2; i++) {
		float a = weights[i * 2];
		float b;
		if (i * 2 + 1 > nsamples - 1)
			b = 0;
		else
			b = weights[i * 2 + 1];
		weights2[i] = a + b;
		offsets[i] = b / (a + b);
	}

	std::ostringstream ost;
	ost << "#version 410 core" << std::endl << std::endl
		<< "in vec2 vTexCoord;" << std::endl << std::endl
		<< "uniform sampler2D TexSampler;" << std::endl << std::endl
		<< "out vec4 color;" << std::endl << std::endl
		<< "void main(){" << std::endl << std::endl
		<< "\tvec3 sum = vec3(0.0,0.0,0.0);" << std::endl << std::endl
		<< "\tvec2 texcoord;" << std::endl << std::endl;
	for (int i = 0; i < nsamples2; i++) {
		float x_offset = 0, y_offset = 0;
		if (vertical) {
			y_offset = (i * 2) - width + offsets[i];
		}
		else {
			x_offset = (i * 2) - width + offsets[i];
		}
		if (tex2D) {
			x_offset = x_offset / img_width;
			y_offset = y_offset / img_height;
		}
		float weight = weights2[i];
		ost << "\ttexcoord = vTexCoord + vec2(" << x_offset << ", " << y_offset << ");" << std::endl;
		ost << "\tsum += texture2D(TexSampler, texcoord).rgb *" << weight << ";" << std::endl << std::endl;
	}
	ost << "\tcolor = vec4(sum, 1.0);" << std::endl;
	ost << "}";

	delete[] weights2;
	delete[] offsets;

	if (shader)
		delete shader;

	GLuint fragment = Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, ost.str());

	shader = new Shader();

	shader->attachShader(vertex);
	shader->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, ost.str()));
	shader->linkShaders();
}

void Game::blurPass(Framebuffer& src) {
	blurBuffer[0].bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(blurH->m_program);

	blurH->loadInt("TexSampler", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, src.getColorTexture(0));
	Globals::shapeManager.get("quad").drawRaw();
	glUseProgram(0);

	blurBuffer[1].bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(blurV->m_program);

	blurV->loadInt("TexSampler", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurBuffer[0].getColorTexture(0));
	Globals::shapeManager.get("quad").drawRaw();
	glUseProgram(0);

	blurBuffer[1].unbind();
}