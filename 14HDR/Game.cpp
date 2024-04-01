#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "engine/input/EventDispatcher.h"

#include "Game.h"
#include "Application.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);

	m_quad = new Quad();
	m_cube = new Cube();
	m_tetraedron = new Tetraedron();

	m_mquad = new MeshQuad(Vector3f(-1.0f, -1.0f, -1.0f), Vector2f(2.0f, 2.0f), true, true, false, false);
	m_mcube = new MeshCube(Vector3f(-2.25f, -0.25f, -0.25f), Vector3f(0.5f, 0.5f, 0.5f), true, true, false, false);
	m_sphere = new MeshSphere (true, true, false, false);
	m_torus = new MeshTorus(true, true, false, false);
	m_spiral = new MeshSpiral(true, true, false, false);
	m_cylinder = new MeshCylinder(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, 2.0f, true, true, false, false);
	m_cone = new MeshCylinder(Vector3f(0.0f, 0.0f, 0.0f), 1.0f, 0.0f, 1.0f, true, true, false, false);
	m_midpoint = new MeshSphere(Vector3f(0.0f, 0.0f, 0.0f), 0.2f, true, true, false, false);
	m_capsule = new Capsule (Vector3f(0.0f, 0.0f, 0.0f), 0.5f, 1.0f, true, true, false, false);

	m_camera = Camera();
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 1.5f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-2.5f);

	applyTransformation(m_trackball);

	vertex = Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/gauss.vert");
	recompileShader();

	object = new Shader("res/object.vert", "res/object.frag");
	tone = new Shader("res/tonemap.vert", "res/tonemap.frag");	
	down2 = new Shader("res/down2.vert", "res/down2.frag");
	down4 = new Shader("res/down4.vert", "res/down4.frag");
	normal = new Shader("res/normal.vert", "res/normal.frag");
	texture = new Shader("res/texture.vert", "res/texture.frag");
	panToCube = new Shader("res/panToCube.vert", "res/panToCube.frag");

	venus.loadModel("res/models/venusm.obj");

	createBuffers(bufferTokens[currentBuffer], rbTokens[currentBuffer], aaModes[currentMode]);

	m_hdriCross.loadCrossHDRIFromFile("res/grace_new_cross.hdr");
	m_hdriEnv.loadHDRIFromFile("res/uffizi-large.hdr", false);
	Cubemap::CreateEmptyCubemap(m_cubemapEnv, m_hdriEnv.getHeight() / 2, m_hdriEnv.getHeight() / 2, GL_RGB32F, GL_RGB, GL_FLOAT);

	cubeBuffer.create(m_hdriEnv.getHeight() / 2, m_hdriEnv.getHeight() / 2);
	panoramaToCubemap();
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_hdriCross.getTexture());
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, m_aniso);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapEnv);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, m_aniso);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	cubemap = m_hdriCross.getTexture();
	m_cubemap.setTexture(cubemap);
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
			m_camera.move(direction * 5.0f * m_dt);
		}
	}

	m_trackball.idle();
	applyTransformation(m_trackball);

};

void Game::render(unsigned int &frameBuffer) {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	if (renderMode == RenderMode::HDR1 || renderMode == RenderMode::HDR2) {
		renderScene();

		if (aaModes[currentMode].samples > 0) {
			// blit from multisampled fbo to scene buffer
			glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaBuffer.getFramebuffer());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, sceneBuffer.getFramebuffer());
			glBlitFramebuffer(0, 0, Application::Width - 1, Application::Height - 1, 0, 0, Application::Width - 1, Application::Height - 1, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			Framebuffer::UnbindWrite();
			Framebuffer::UnbindRead();
		}

		if (m_glow || m_rays) {
			downsample4(sceneBuffer, downsampleBuffer[0]);
			downsample4(downsampleBuffer[0], downsampleBuffer[1]);
			blurPass(downsampleBuffer[1]);
		}

		glUseProgram(tone->m_program);

		tone->loadFloat("blurAmount", m_glow ? m_blurAmount : 0.0f);
		tone->loadFloat("effectAmount", m_glow ? m_effectAmount : 0.0f);
		tone->loadFloat("exposure", m_exposure);

		tone->loadInt("sceneTex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sceneBuffer.getColorTexture(0));

		tone->loadInt("blurTex", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, blurBuffer[1].getColorTexture(0));

		m_quad->drawRaw();
		glUseProgram(0);
	}

	glPolygonMode(GL_FRONT_AND_BACK, m_wireframe ? GL_LINE : GL_FILL);
	if (renderMode == RenderMode::NORMAL) {

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glUseProgram(normal->m_program);
		normal->loadMatrix("u_transform", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
		normal->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
		switch (model) {
		case Model::QUAD:
			m_mquad->drawRaw();
			break;
		case Model::SPHERE:
			m_sphere->drawRaw();
			break;
		case Model::TETRA:
			m_tetraedron->drawRaw();
			break;
		case Model::CUBE:
			m_midpoint->drawRaw();
			m_mcube->drawRaw();
			break;
		case Model::TORUS:
			m_torus->drawRaw();
			break;
		case Model::SPIRAL:
			m_spiral->drawRaw();
			break;
		case Model::CYLINDER:
			m_cylinder->drawRaw();
			break;
		case Model::CONE:
			m_cone->drawRaw();
			break;
		case Model::CAPSULE:
			m_capsule->drawRaw();
			break;
		case Model::VENUS:
			venus.drawRaw();
			break;
		}
	}

	if (renderMode == RenderMode::TEXTURE) {
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glUseProgram(texture->m_program);
		texture->loadMatrix("u_transform", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
		texture->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));

		Globals::textureManager.get("grid").bind(0);

		switch (model) {
			case Model::QUAD:
				m_mquad->drawRaw();
				break;
			case Model::SPHERE:
				m_sphere->drawRaw();
				break;
			case Model::TETRA:
				m_tetraedron->drawRaw();
				break;
			case Model::CUBE:
				m_midpoint->drawRaw();
				m_mcube->drawRaw();
				break;
			case Model::TORUS:
				m_torus->drawRaw();
				break;
			case Model::SPIRAL:
				m_spiral->drawRaw();
				break;
			case Model::CYLINDER:
				m_cylinder->drawRaw();
				break;
			case Model::CONE:
				m_cone->drawRaw();
				break;
			case Model::CAPSULE:
				m_capsule->drawRaw();
				break;
			case Model::VENUS:
				venus.drawRaw();
				break;
		}
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

	if (model == Model::CUBE) {		
		m_transform.scale(m_scale, m_scale, m_scale, m_mcube->getCenter());
	}else if (model == Model::VENUS) {
		m_transform.scale(m_scale, m_scale, m_scale, venus.getCenter());
	}else {
		m_transform.scale(m_scale, m_scale, m_scale);
	}

	m_transform.translate(m_translate[0], m_translate[1], m_translate[2]);
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);	
	createBuffers(bufferTokens[currentBuffer], rbTokens[currentBuffer], aaModes[currentMode]);
	recompileShader();
}

void Game::renderScene() {

	if (aaModes[currentMode].samples > 0) {
		msaaBuffer.bind();
	} else {
		sceneBuffer.bind();
	}

	
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
		glEnable(GL_DEPTH_TEST);
	}

	glPolygonMode(GL_FRONT_AND_BACK, m_wireframe ? GL_LINE : GL_FILL);
	glUseProgram(object->m_program);
	object->loadMatrix("u_model", m_transform.getTransformationMatrix());
	object->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	object->loadMatrix("u_view", m_camera.getViewMatrix());
	object->loadFloat("u_alpha", m_blendAmout);
	object->loadVector("u_color", Vector3f(m_color[0], m_color[1], m_color[2]));
	//Vector4f eyePos_eye(0.0, 0.0, 0.0, 1.0);
	//Matrix4f invView = m_camera.getInvViewMatrix();
	//Vector4f eyePos_model = eyePos_eye ^ invView;
	//Vector3f eyePos_model1 = eyePos_eye * invView;	
	//object->loadVector("u_eyePos", eyePos_model);

	object->loadVector("u_eyePos", m_camera.getPosition());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	// bias LOD to blur cubemap a little
	glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, 2.0f);

	if(cullMode == Culling::FRONT || cullMode == Culling::BACK)
		glEnable(GL_CULL_FACE);

	if (m_blend) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
	}else {
		glEnable(GL_DEPTH_TEST);
	}

	switch (model) {
		case Model::QUAD:
			m_mquad->drawRaw();
			break;
		case Model::SPHERE:
			m_sphere->drawRaw();
			break;
		case Model::TETRA:
			m_tetraedron->drawRaw();
			break;
		case Model::CUBE:
			m_midpoint->drawRaw();
			m_mcube->drawRaw();
			break;
		case Model::TORUS:
			m_torus->drawRaw();
			break;
		case Model::SPIRAL:
			m_spiral->drawRaw();
			break;
		case Model::CYLINDER:
			m_cylinder->drawRaw();
			break;
		case Model::CONE:
			m_cone->drawRaw();
			break;
		case Model::CAPSULE:
			m_capsule->drawRaw();
			break;
		case Model::VENUS:
			venus.drawRaw();
			break;
	}
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_MULTISAMPLE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glUseProgram(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Framebuffer::Unbind();
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

void Game::recompileShader() {
	float *weights;
	int width;
	weights = generateGaussianWeights(m_blurWidth, width);
	generate1DConvolutionFP_filter(blurH, weights, width, false, true, Application::Width / 2, Application::Height / 2);
	generate1DConvolutionFP_filter(blurV, weights, width, true, true, Application::Width / 2, Application::Height / 2);
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

void Game::createBuffers(AttachmentTex::AttachmentTex texFormat, AttachmentRB::AttachmentRB rbFormat, aaInfo aaMode) {
	sceneBuffer.cleanup();
	msaaBuffer.cleanup();

	for (int i = 0; i<DOWNSAMPLE_BUFFERS; i++) {
		downsampleBuffer[i].cleanup();
	}

	for (int i = 0; i<BLUR_BUFFERS; i++) {
		blurBuffer[i].cleanup();
	}

	sceneBuffer.create(Application::Width, Application::Height);
	sceneBuffer.attachTexture(texFormat);
	sceneBuffer.attachRenderbuffer(AttachmentRB::DEPTH24);

	if (aaMode.samples > 0) {
		// create multisampled fbo
		msaaBuffer.create(Application::Width, Application::Height);
		msaaBuffer.attachRenderbuffer(rbFormat, aaMode.samples, aaMode.coverage_samples);
		msaaBuffer.attachRenderbuffer(AttachmentRB::DEPTH24, aaMode.samples, aaMode.coverage_samples);
	}

	for (int i = 0; i < BLUR_BUFFERS; i++) {
		blurBuffer[i].create(Application::Width / 4, Application::Height / 4);
		blurBuffer[i].attachTexture(texFormat);
	}

	int w = Application::Width;
	int h = Application::Height;
	for (int i = 0; i < DOWNSAMPLE_BUFFERS; i++) {
		w /= 2;
		h /= 2;
		downsampleBuffer[i].create(w, h);
		downsampleBuffer[i].attachTexture(texFormat);
	}


}

void Game::panoramaToCubemap() {
	cubeBuffer.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(panToCube->m_program);
	for (int i = 0; i < 6; ++i) {	
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_cubemapEnv, 0);
		

		panToCube->loadInt("u_currentFace", i);
		panToCube->loadInt("u_panorama", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_hdriEnv.getTexture());

		m_quad->drawRaw();
	}
	glUseProgram(0);
	cubeBuffer.unbind();	
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

	ImGui::Checkbox("Glow", &m_glow);
	if (ImGui::Checkbox("Rays", &m_rays)) {
		m_effectAmount = m_rays ? 0.1f : 0.0f;
	}

	ImGui::Checkbox("Draw Skybox", &m_drawSkaybox);
	ImGui::Checkbox("Draw Wirframe", &m_wireframe);
	ImGui::Checkbox("Blend", &m_blend);

	if (ImGui::SliderFloat("Blur Width", &m_blurWidth, 1.0f, 10.0f)) {
		recompileShader();
	}

	ImGui::SliderFloat("Blur Amount", &m_blurAmount, 0.0f, 1.0f);
	ImGui::SliderFloat("Exposure", &m_exposure, 0.0f, m_exposureMax);
	ImGui::SliderFloat("Blend Amount", &m_blendAmout, 0.0f, 1.0f);
	ImGui::SliderFloat("Scale", &m_scale, -2.0f, 2.0f);
	ImGui::SliderFloat3("Translate", &m_translate[0], -10.0f, 10.0f);
	if (ImGui::SliderFloat3("Center of Rot", &m_centerOfRotation[0], -2.0f, 2.0f)) {
		m_trackball.setCenterOfRotation(m_centerOfRotation);
	}
		
	if (ImGui::Combo("Framebuffer format", &currentBuffer, "RGBA8\0RGBA16F\0RGBA32F\0R11F_G11F_B10F\0\0") || ImGui::Combo("Multisample level", &currentMode, aaModesLabel, IM_ARRAYSIZE(aaModesLabel))) {
		createBuffers(bufferTokens[currentBuffer], rbTokens[currentBuffer], aaModes[currentMode]);

		if (aaModes[currentMode].samples > 0) {
			glEnable(GL_MULTISAMPLE);
			glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
		}else {
			glDisable(GL_MULTISAMPLE);
		}
	}

	if (ImGui::Combo("Ansio", &currentAnsio, ansioLabel, IM_ARRAYSIZE(ansioLabel))) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, ansio[currentAnsio]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	int currentModel = model;
	if (ImGui::Combo("Model", &currentModel, "Quad\0Sphere\0Tetra\0Cube\0Torus\0Spiral\0Cylinder\0Cone\0Capsule\0Venus\0\0")) {
		model = static_cast<Model>(currentModel);
	}

	int currentCullMode = cullMode;
	if (ImGui::Combo("Cull", &currentCullMode, "None\0Back\0Front\0\0")) {
		cullMode = static_cast<Culling>(currentCullMode);

		if(cullMode == Culling::BACK)
			glCullFace(GL_BACK);

		if (cullMode == Culling::FRONT)
			glCullFace(GL_FRONT);
	}

	int currentRenderMode = renderMode;
	if (ImGui::Combo("Render", &currentRenderMode, "Hdr Cross\0Hdr Env\0Normal\0Texture\0\0")) {
		renderMode = static_cast<RenderMode>(currentRenderMode);
		if (renderMode == RenderMode::HDR2) {
			cubemap = m_cubemapEnv;
			m_cubemap.setTexture(cubemap);
			m_exposure = 3.0f;
			m_exposureMax = 16.0f;
		}

		if (renderMode == RenderMode::HDR1) {
			cubemap = m_hdriCross.getTexture();
			m_cubemap.setTexture(cubemap);
			m_exposureMax = 512.0f;
		}
	}

	ImGui::ColorEdit3("color", m_color);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}