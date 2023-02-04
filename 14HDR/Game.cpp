#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>

#include "engine/input/EventDispatcher.h"

#include "Game.h"
#include "Application.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);

	m_quad = new Quad();
	m_cube = new Cube();

	m_camera = Camera();
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 1.5f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-2.5f);
	applyTransformation(m_trackball.getTransform());

	image.loadHDRIFromFile("res/grace_new_cross.hdr");
	image.convertCrossToCubemap();

	m_texture = createCubemapTexture(image, GL_RGBA16F);

	m_cubemap.setTexture(m_texture);

	float *weights;
	int width;
	weights = generateGaussianWeights(blur_width, width);
	vertex = Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/gauss.vert");

	generate1DConvolutionFP_filter(blurH, weights, width, false, true, Application::Width / 2, Application::Height / 2);
	generate1DConvolutionFP_filter(blurV, weights, width, true, true, Application::Width / 2, Application::Height / 2);

	object = new Shader("res/object.vert", "res/object.frag");
	tone = new Shader("res/tonemap.vert", "res/tonemap.frag");	
	down2 = new Shader("res/down2.vert", "res/down2.frag");
	down4 = new Shader("res/down4.vert", "res/down4.frag");

	venus.loadModel("res/models/venusm.obj");

	sceneBuffer.create(Application::Width, Application::Height);
	sceneBuffer.attachTexture(AttachmentTex::RGBA16F);
	sceneBuffer.attachRenderbuffer(AttachmentRB::DEPTH24);

	for (int i = 0; i < BLUR_BUFFERS; i++) {
		blurBuffer[i].create(Application::Width / 4, Application::Height / 4);
		blurBuffer[i].attachTexture(AttachmentTex::RGBA16F);
	}

	int w = Application::Width;
	int h = Application::Height;
	for (int i = 0; i < DOWNSAMPLE_BUFFERS; i++) {
		w /= 2;
		h /= 2;
		downsampleBuffer[i].create(w, h);
		downsampleBuffer[i].attachTexture(AttachmentTex::RGBA16F);
	}	
}

Game::~Game() {}

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

	if (keyboard.keyPressed(Keyboard::KEY_M)) {
		m_drawSkaybox = !m_drawSkaybox;
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
	applyTransformation(m_trackball.getTransform());

};

void Game::render(unsigned int &frameBuffer) {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderScene();

	downsample4(sceneBuffer, downsampleBuffer[0]);
	downsample4(downsampleBuffer[0], downsampleBuffer[1]);
	blurPass(downsampleBuffer[1]);

	glUseProgram(tone->m_program);

	tone->loadFloat("blurAmount", blur_amount);
	tone->loadFloat("effectAmount", effect_amount);
	tone->loadVector("windowSize", Vector2f(2.0f / (float)Application::Width, 2.0f / (float)Application::Height));
	tone->loadFloat("exposure", exposure);

	tone->loadInt("sceneTex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneBuffer.getColorTexture(0));

	tone->loadInt("blurTex", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, blurBuffer[1].getColorTexture(0));

	m_quad->drawRaw();
	glUseProgram(0);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// renderer and other code before this point
	{std::array<float, 3> tmp = { 0.0f, 0.0f, 0.0f };
		ImGui::SliderFloat3("Model Matrix Translation", tmp.data(), 0.0f, 960.0f);
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball.getTransform());
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
	applyTransformation(m_trackball.getTransform());
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
	applyTransformation(m_trackball.getTransform());
}

void Game::applyTransformation(Matrix4f& mtx) {
	m_transform.fromMatrix(mtx);
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	
	sceneBuffer.resize(Application::Width, Application::Height);

	for (int i = 0; i<BLUR_BUFFERS; i++) {
		blurBuffer[i].resize(Application::Width / 4, Application::Height / 4);
	}
	
	int w = Application::Width;
	int h = Application::Height;
	for (int i = 0; i < DOWNSAMPLE_BUFFERS; i++) {
		w /= 2;
		h /= 2;
		downsampleBuffer[i].resize(w, h);
	}

	float *weights;
	int width;
	weights = generateGaussianWeights(blur_width, width);
	generate1DConvolutionFP_filter(blurH, weights, width, false, true, Application::Width / 2, Application::Height / 2);
	generate1DConvolutionFP_filter(blurV, weights, width, true, true, Application::Width / 2, Application::Height / 2);
}

GLuint Game::createCubemapTexture(HDRImage &img, GLint internalformat) {
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2.0f);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

	// load face data
	for (int i = 0; i<6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, img.getWidth(), img.getHeight(), 0, GL_RGB, GL_FLOAT, img.getLevel(0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i));
	}

	return tex;
}

void Game::renderScene() {
	sceneBuffer.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_drawSkaybox) {
		glDisable(GL_DEPTH_TEST);
		auto shader = Globals::shaderManager.getAssetPointer("sky");

		Matrix4f view = m_camera.getViewMatrix();
		view[3][0] = 0.0f; view[3][1] = 0.0f; view[3][2] = 0.0f;

		glUseProgram(shader->m_program);
		shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
		shader->loadMatrix("u_view", view);
		shader->loadMatrix("u_model", Matrix4f::IDENTITY);
		m_cubemap.bind(0);
		m_cube->drawRaw();

		Cubemap::Unbind();

		glUseProgram(0);	
	}

	glEnable(GL_DEPTH_TEST);
	glUseProgram(object->m_program);
	object->loadMatrix("u_model", m_transform.getTransformationMatrix());
	object->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	object->loadMatrix("u_view", m_camera.getViewMatrix());

	//Vector4f eyePos_eye(0.0, 0.0, 0.0, 1.0);
	//Matrix4f invView = m_camera.getInvViewMatrix();
	//Vector4f eyePos_model = eyePos_eye ^ invView;
	//Vector3f eyePos_model1 = eyePos_eye * invView;	
	//object->loadVector("u_eyePos", eyePos_model);

	object->loadVector("u_eyePos", m_camera.getPosition());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

	venus.drawRaw();

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glUseProgram(0);

	sceneBuffer.unbind();
}

void Game::blurPass(Framebuffer& src) {
	blurBuffer[0].bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(blurH->m_program);

	blurH->loadInt("TexSampler", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, src.getColorTexture(0));
	m_quad->drawRaw();
	glUseProgram(0);

	blurBuffer[1].bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(blurV->m_program);

	blurV->loadInt("TexSampler", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurBuffer[0].getColorTexture(0));
	m_quad->drawRaw();
	glUseProgram(0);

	blurBuffer[1].unbind();
}

void Game::downsample2(Framebuffer& src, Framebuffer& dest) {
	dest.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(down2->m_program);
	down2->loadInt("u_texture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, src.getColorTexture(0));

	m_quad->drawRaw();

	glUseProgram(0);
	dest.unbind();	
}

void Game::downsample4(Framebuffer& src, Framebuffer& dest) {
	dest.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(down4->m_program);
	down4->loadVector("twoTexelSize", Vector2f(2.0f / (float)Application::Width, 2.0f / (float)Application::Height));

	down4->loadInt("u_texture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, src.getColorTexture(0));

	m_quad->drawRaw();

	glUseProgram(0);
	dest.unbind();
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