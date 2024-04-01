#include <random>
#include "Game.h"
#include "engine/input/EventDispatcher.h"
#include "Application.h"

const float HEIGHTMAP_ROUGHNESS = 1.2f;
const float HEIGHTMAP_SCALE = 2.0f;

const int HEIGHTMAP_RESOLUTION = 128;
const int HEIGHTMAP_WIDTH = 8192;
const float CAMERA_Y_OFFSET = 100.0f;
const Vector3f CAMERA_ACCELERATION(400.0f, 400.0f, 400.0f);
const Vector3f CAMERA_VELOCITY(200.0f, 200.0f, 200.0f);
Vector3f LIGHT_DIRECTION(-100.0f, 100.0f, -100.0f);

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);

	m_quad = new Quad(Vector2f(-1.0f, -1.0f), Vector2f(2.0f, 2.0f));

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 100.0f);
	m_camera.lookAt(Vector3f(-1.30592, 3.5, 4.11601), Vector3f(0.00597954, 3.5, 0.00461888), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	m_statue.loadModel("res/models/statue/statue.obj", false, false, true, false, true);
	m_transform.scale(1.0f, 1.0f, 1.0f);

	lights[0].color = Vector3f(1.2f, 1.2f, 1.2f);
	lights[0].pos = Vector3f(0.0f, 5.0f, 8.0f);
	lights[0].viewDirection = Vector3f::Normalize(m_transform.getTransformationMatrix() * m_statue.getCenter() - lights[0].pos);
	lights[0].m_shadowProjection.perspective(45.0f, 1.0, 1.0f, 100.0f);
	lights[0].m_shadowView.lookAt(lights[0].pos, m_transform.getTransformationMatrix() * m_statue.getCenter(), Vector3f(0.0f, 1.0f, 0.0f));
	lights[0].m_depthRT.create(DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	lights[0].m_depthRT.attachTexture(AttachmentTex::RED32F);
	lights[0].m_depthRT.attachTexture(AttachmentTex::DEPTH32F);

	lights[0].fov = 45.0f * PI_ON_180;
	lights[0].falloffWidth = 0.05f;
	lights[0].attenuation = 1.0f / 128.0f;
	lights[0].farPlane = 100.0f;
	lights[0].bias = -0.01f;

	lights[1].color = Vector3f(0.3f, 0.3f, 0.3f);
	lights[1].pos = Vector3f(0.0f, 5.0f, -8.0f);
	lights[1].viewDirection = Vector3f::Normalize(m_transform.getTransformationMatrix() * m_statue.getCenter() - lights[1].pos);
	lights[1].m_shadowProjection.perspective(45.0f, 1.0, 1.0f, 100.0f);
	lights[1].m_shadowView.lookAt(lights[1].pos, m_transform.getTransformationMatrix() * m_statue.getCenter(), Vector3f(0.0f, 1.0f, 0.0f));
	lights[1].m_depthRT.create(DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	lights[1].m_depthRT.attachTexture(AttachmentTex::RED32F);
	lights[1].m_depthRT.attachTexture(AttachmentTex::DEPTH32F);

	lights[1].fov = 45.0f * PI_ON_180;
	lights[1].falloffWidth = 0.05f;
	lights[1].attenuation = 1.0f / 128.0f;
	lights[1].farPlane = 100.0f;
	lights[1].bias = -0.01f;

	lights[2].color = Vector3f(0.3f, 0.3f, 0.3f);
	lights[2].pos = Vector3f(8.0f, 5.0f, 0.0f);
	lights[2].viewDirection = Vector3f::Normalize(m_transform.getTransformationMatrix() * m_statue.getCenter() - lights[2].pos);
	lights[2].m_shadowProjection.perspective(45.0f, 1.0, 1.0f, 100.0f);
	lights[2].m_shadowView.lookAt(lights[1].pos, m_transform.getTransformationMatrix() * m_statue.getCenter(), Vector3f(0.0f, 1.0f, 0.0f));
	lights[2].m_depthRT.create(DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	lights[2].m_depthRT.attachTexture(AttachmentTex::RED32F);
	lights[2].m_depthRT.attachTexture(AttachmentTex::DEPTH32F);

	lights[2].fov = 45.0f * PI_ON_180;
	lights[2].falloffWidth = 0.05f;
	lights[2].attenuation = 1.0f / 128.0f;
	lights[2].farPlane = 100.0f;
	lights[2].bias = -0.01f;

	lights[3].color = Vector3f(0.3f, 0.3f, 0.3f);
	lights[3].pos = Vector3f(-8.0f, 5.0f, 0.0f);
	lights[3].viewDirection = Vector3f::Normalize(m_transform.getTransformationMatrix() * m_statue.getCenter() - lights[3].pos);
	lights[3].m_shadowProjection.perspective(45.0f, 1.0, 1.0f, 100.0f);
	lights[3].m_shadowView.lookAt(lights[1].pos, m_transform.getTransformationMatrix() * m_statue.getCenter(), Vector3f(0.0f, 1.0f, 0.0f));
	lights[3].m_depthRT.create(DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	lights[3].m_depthRT.attachTexture(AttachmentTex::RED32F);
	lights[3].m_depthRT.attachTexture(AttachmentTex::DEPTH32F);

	lights[3].fov = 45.0f * PI_ON_180;
	lights[3].falloffWidth = 0.05f;
	lights[3].attenuation = 1.0f / 128.0f;
	lights[3].farPlane = 100.0f;
	lights[3].bias = -0.01f;

	m_mainRT.create(Application::Width, Application::Height);
	m_mainRT.attachTexture(AttachmentTex::RGBA);
	m_mainRT.attachTexture(AttachmentTex::RGBA);
	m_mainRT.attachTexture(AttachmentTex::RED32F);
	m_mainRT.attachTexture(AttachmentTex::RG16F);
	m_mainRT.attachTexture(AttachmentTex::RGBA);
	m_mainRT.attachTexture(AttachmentTex::DEPTH_STENCIL);

	m_sssXRT.create(Application::Width, Application::Height);
	m_sssXRT.attachTexture(AttachmentTex::RGBA);

	m_sssYRT.create(Application::Width, Application::Height);

	/**this textures resource could be shared but for comparing the results
	   during development it's more easy to attache it's own texture to the fbo*/
	//m_sssYRT.attachTexture(Attachment::COLOR, m_mainRT.getColorTexture(0));
	m_sssYRT.attachTexture(AttachmentTex::RGBA);
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

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_debug = !m_debug;
	}

	if (keyboard.keyPressed(Keyboard::KEY_M)) {
		m_sss = !m_sss;
	}

	if (keyboard.keyPressed(Keyboard::KEY_B)) {
		m_showBlurRadius = !m_showBlurRadius;
	}

	if (keyboard.keyDown(Keyboard::KEY_ADD)) {
		m_sssWidth = std::max(15.0f, m_sssWidth - 1.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_SUBTRACT)) {
		m_sssWidth = std::min(200.0f, m_sssWidth + 1.0f);
	}

	if (keyboard.keyPressed(Keyboard::KEY_COMMA)) {
		m_translucency = std::max(0.0f, m_translucency - 0.1f);
	}
	
	if (keyboard.keyPressed(Keyboard::KEY_PERIOD)) {
		m_translucency = std::min(1.0f, m_translucency + 0.1f);
	}

	if (keyboard.keyPressed(Keyboard::KEY_K)) {
		m_specularIntensity = std::max(0.0f, m_specularIntensity - 1.0f);
	}

	if (keyboard.keyPressed(Keyboard::KEY_L)) {
		m_specularIntensity = std::min(10.0f, m_specularIntensity + 1.0f);
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
};


void Game::render(unsigned int &frameBuffer) {
	
	shdowPass();
	renderGBuffer();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(m_sss)
		sssPass();
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("quad");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_transform", m_debug ? Matrix4f::Translate(0.25f, 0.0f, 0.0f) : Matrix4f::IDENTITY);
	m_quad->draw(m_sss ? m_sssYRT.getColorTexture(0) : m_mainRT.getColorTexture(0));
	glUseProgram(0);

	if (m_debug) {
		glEnable(GL_SCISSOR_TEST);

		glScissor(0, 0, Application::Width * 0.25, Application::Height);
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader->m_program);
		shader->loadMatrix("u_transform", Matrix4f::Translate(-0.75f, 0.0f, 0.0f));
		m_quad->draw(m_mainRT.getColorTexture(1));
		glUseProgram(0);

		glScissor(Application::Width * 0.25, 0, Application::Width * 0.25, Application::Height);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader->m_program);
		shader->loadMatrix("u_transform", Matrix4f::Translate(-0.25f, 0.0f, 0.0f));
		m_quad->draw(m_mainRT.getColorTexture(0));
		glUseProgram(0);

		glScissor(Application::Width * 0.75f, 0, Application::Width * 0.25, Application::Height);
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader->m_program);
		shader->loadMatrix("u_transform", Matrix4f::Translate(0.75f, 0.0f, 0.0f));
		m_quad->draw(m_mainRT.getColorTexture(4));
		glUseProgram(0);

		glDisable(GL_SCISSOR_TEST);
	}
}

void Game::shdowPass() {
	//glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	for (int i = 0; i < 4; i++) {

		lights[i].m_depthRT.bind();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto shader = Globals::shaderManager.getAssetPointer("depth");
		glUseProgram(shader->m_program);
		shader->loadMatrix("u_viewProjection", lights[i].m_shadowProjection * lights[i].m_shadowView);
		shader->loadMatrix("u_model", m_transform.getTransformationMatrix());
		m_statue.drawRaw();
		lights[i].m_depthRT.unbind();
	}
	glDisable(GL_DEPTH_TEST);
}

void Game::shdowPassAdditive() {
	glDisable(GL_DEPTH_TEST);
	for (int i = 0; i < 4; i++) {

		lights[i].m_depthRT.bind();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		auto shader = Globals::shaderManager.getAssetPointer("depthAdd");
		glUseProgram(shader->m_program);
		shader->loadMatrix("u_viewProjection", lights[i].m_shadowProjection * lights[i].m_shadowView);
		shader->loadMatrix("u_model", m_transform.getTransformationMatrix());

		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);

		//additve blending output_color = 1 * source_color + 1 * destination_color
		glBlendFunc(GL_ONE, GL_ONE);
		shader->loadFloat("depthScale", 1.0f);

		glCullFace(GL_FRONT);
		m_statue.drawRaw();

		shader->loadFloat("depthScale", -1.0f);
		glCullFace(GL_BACK);
		m_statue.drawRaw();

		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		
		lights[i].m_depthRT.unbind();
	}
}

void Game::renderGBuffer() {
	glEnable(GL_DEPTH_TEST);

	m_mainRT.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glClearStencil(0);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//glClearTexImage(m_mainRT.getColorTexture(2), 0, GL_RED, GL_FLOAT, red);

	auto shader = Globals::shaderManager.getAssetPointer("Main");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", Globals::projection);
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", m_transform.getTransformationMatrix());
	shader->loadMatrix("u_normalMatrix", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
	
	shader->loadVector("u_cameraPosition", m_camera.getPosition());
	currViewProj = Globals::projection * m_camera.getViewMatrix();

	shader->loadMatrix("u_currWorldViewProj", currViewProj);
	shader->loadMatrix("u_prevWorldViewProj", prevViewProj);
	
	shader->loadFloat("sssWidth", m_sssWidth);
	shader->loadFloat("translucency", m_translucency);
	shader->loadFloat("specularIntensity", m_specularIntensity);

	for (int i = 0; i < 4; i++) {
		shader->loadMatrix(std::string("u_projectionShadowBias[" + std::to_string(i) + "]").c_str(), Matrix4f::BiasMatrix * lights[i].m_shadowProjection);
		shader->loadMatrix(std::string("u_projectionShadow[" + std::to_string(i) + "]").c_str(), lights[i].m_shadowProjection);
		shader->loadMatrix(std::string("u_viewShadow[" + std::to_string(i) + "]").c_str(), lights[i].m_shadowView);


		shader->loadVector(std::string("lightPosition[" + std::to_string(i) + "]").c_str(), lights[i].pos);
		shader->loadVector(std::string("lightDirection[" + std::to_string(i) + "]").c_str(), lights[i].viewDirection);
		shader->loadVector(std::string("lightColor[" + std::to_string(i) + "]").c_str(), lights[i].color);
		shader->loadFloat(std::string("lightFalloffStart[" + std::to_string(i) + "]").c_str(), cos(0.5f * (45.0f * PI_ON_180)));

		shader->loadInt(std::string("u_shadowMap[" + std::to_string(i) + "]").c_str(), i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, lights[i].m_depthRT.getDepthTexture());
		//glBindTexture(GL_TEXTURE_2D, lights[i].m_depthRT.getColorTexture());
	}

	shader->loadInt("u_albedo", 4);
	Globals::textureManager.get("albedo").bind(4);

	shader->loadInt("u_normal", 5);
	Globals::textureManager.get("normal").bind(5);

	shader->loadInt("u_beckmann", 6);
	Globals::textureManager.get("beckmann").bind(6);
	m_statue.drawRaw();

	glUseProgram(0);

	m_mainRT.unbind();
	prevViewProj = currViewProj;
	glDisable(GL_DEPTH_TEST);
}



void Game::sssPass() {
	m_sssXRT.bind();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("sss");
	glUseProgram(shader->m_program);
	shader->loadVector("dir", Vector2f((GLfloat)Application::Height / (GLfloat)Application::Width, 0.0f));
	shader->loadFloat("u_showBlurRadius", m_showBlurRadius);
	shader->loadFloat("sssWidth", m_sssWidth);

	shader->loadInt("u_colorTex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_mainRT.getColorTexture(0));

	shader->loadInt("u_strengthTex", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_mainRT.getColorTexture(1));

	shader->loadInt("u_depthTex", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_mainRT.getColorTexture(2));

	m_quad->drawRaw();

	m_sssYRT.bind();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shader->loadVector("dir", Vector2f(0.0f, 1.0f));

	shader->loadInt("u_colorTex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_sssXRT.getColorTexture(0));

	m_quad->drawRaw();
	glUseProgram(0);
	m_sssYRT.unbind();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::resize(int deltaW, int deltaH) {
	m_mainRT.resize(Application::Width, Application::Height);
	m_sssXRT.resize(Application::Width, Application::Height);
	m_sssYRT.resize(Application::Width, Application::Height);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 100.0f);
}
