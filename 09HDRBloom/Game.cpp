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

	m_quadShader = Globals::shaderManager.getAssetPointer("quad");

	m_copyFramebuffer.create(Application::Width, Application::Height);
	m_copyFramebuffer.attachTexture(AttachmentTex::RGBA);
	m_copyFramebuffer.attachRenderbuffer(AttachmentRB::DEPTH_STENCIL);

	m_leftBottom = new Quad(Vector2f(-1.0f, -1.0f), size);
	m_rightBottom = new Quad(Vector2f(1.0f - size[0], -1.0f), size);
	m_leftTop = new Quad(Vector2f(-1.0f, 1.0f - size[1]), size);
	m_rightTop = new Quad(Vector2f(1.0f - size[0], 1.0f - size[1]), size);

	m_quad = new Quad(Vector2f(-1.0f, -1.0f), Vector2f(2.0f, 2.0f));
	m_cube = new Cube();

	offsetX = Application::Width * size[0] * 0.5f;
	offsetY = Application::Height * size[1] * 0.5f;

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 100.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 5.0f), Vector3f(0.0f, 0.0f, -5.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setAcceleration(CAMERA_ACCELERATION);
	m_camera.setVelocity(CAMERA_VELOCITY);
	m_camera.setRotationSpeed(0.1f);

	m_hdrBuffer.create(Application::Width, Application::Height);
	m_hdrBuffer.attachTexture(AttachmentTex::RGBA16F);
	m_hdrBuffer.attachTexture(AttachmentTex::RGBA16F);
	m_hdrBuffer.attachRenderbuffer(AttachmentRB::DEPTH24);
	
	m_pingPongBuffer[0].create(Application::Width, Application::Height);
	m_pingPongBuffer[0].attachTexture(AttachmentTex::RGBA16F);

	m_pingPongBuffer[1].create(Application::Width, Application::Height);
	m_pingPongBuffer[1].attachTexture(AttachmentTex::RGBA16F);

	lightPositions.push_back(Vector3f(0.0f, 0.5f, 1.5f));
	lightPositions.push_back(Vector3f(-4.0f, 0.5f, -3.0f));
	lightPositions.push_back(Vector3f(3.0f, 0.5f, 1.0f));
	lightPositions.push_back(Vector3f(-.8f, 2.4f, -1.0f));

	lightColors.push_back(Vector3f(5.0f, 5.0f, 5.0f));
	lightColors.push_back(Vector3f(10.0f, 0.0f, 0.0f));
	lightColors.push_back(Vector3f(0.0f, 0.0f, 15.0f));
	lightColors.push_back(Vector3f(0.0f, 5.0f, 0.0f));

	auto shader = Globals::shaderManager.getAssetPointer("bloom");
	glUseProgram(shader->m_program);
	shader->loadInt("diffuseTexture", 0);
	glUseProgram(0);

	shader = Globals::shaderManager.getAssetPointer("bloom_blur");
	glUseProgram(shader->m_program);
	shader->loadInt("image", 0);
	glUseProgram(0);

	shader = Globals::shaderManager.getAssetPointer("bloom_final");
	glUseProgram(shader->m_program);
	shader->loadInt("scene", 0);
	shader->loadInt("bloomBlur", 1);
	glUseProgram(0);

	bloomRenderer.Init(Application::Width, Application::Height);
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

	if (keyboard.keyPressed(Keyboard::KEY_1)) {
		programChoice = 1;
	}

	if (keyboard.keyPressed(Keyboard::KEY_2)) {
		programChoice = 2;
	}

	if (keyboard.keyPressed(Keyboard::KEY_3)) {
		programChoice = 3;
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
};


void Game::render(unsigned int &frameBuffer) {
	
	renderOffscreen();

	if (programChoice < 1 || programChoice > 3) { programChoice = 1; }
	bloom = (programChoice == 1) ? false : true;
	bool horizontal = true;

	// 1) bloom is disabled
	// ----------------------
	if (programChoice == 1) {

	}
	// 2) blur bright fragments with two-pass Gaussian Blur
	// ------------------------------------------------------
	else if (programChoice == 2) {
		bool first_iteration = true;
		unsigned int amount = 10;
		auto shader = Globals::shaderManager.getAssetPointer("bloom_blur");
		glUseProgram(shader->m_program);
		for (unsigned int i = 0; i < amount; i++) {
			m_pingPongBuffer[horizontal].bind();

			shader->loadInt("horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? m_hdrBuffer.getColorTexture(1) : m_pingPongBuffer[!horizontal].getColorTexture());  // bind texture of other framebuffer (or scene if first iteration)
			m_quad->drawRaw();
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		Framebuffer::Unbind();
	}

	// 3) use unthresholded bloom with progressive downsample/upsampling
	// -------------------------------------------------------------------
	else if (programChoice == 3) {
		bloomRenderer.RenderBloomTexture(m_hdrBuffer.getColorTexture(1), bloomFilterRadius);
	}

	// render
	// ------
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("bloom_final");
	glUseProgram(shader->m_program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_hdrBuffer.getColorTexture(0));
	glActiveTexture(GL_TEXTURE1);
	if (programChoice == 1) {
		glBindTexture(GL_TEXTURE_2D, 0); // trick to bind invalid texture "0", we don't care either way!
	}

	if (programChoice == 2) {
		glBindTexture(GL_TEXTURE_2D, m_pingPongBuffer[!horizontal].getColorTexture());
	} else if (programChoice == 3) {
		glBindTexture(GL_TEXTURE_2D, bloomRenderer.BloomTexture());
	}
	shader->loadInt("programChoice", programChoice);
	shader->loadFloat("exposure", exposure);
	m_quad->drawRaw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::renderOffscreen() {
	// 1. render scene into floating point framebuffer
	// -----------------------------------------------
	m_hdrBuffer.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("bloom");
	glUseProgram(shader->m_program);
	shader->loadMatrix("projection", Globals::projection);
	shader->loadMatrix("view", m_camera.getViewMatrix());
	
	// set lighting uniforms
	for (unsigned int i = 0; i < lightPositions.size(); i++) {
		shader->loadVector(std::string("lights[" + std::to_string(i) + "].Position").c_str(), lightPositions[i]);
		shader->loadVector(std::string("lights[" + std::to_string(i) + "].Color").c_str(), lightColors[i]);
	}
	shader->loadVector("viewPos", m_camera.getPosition());
	// create one large cube that acts as the floor
	m_model.reset();
	m_model.translate(0.0f, -1.0f, 0.0f);
	m_model.scale(12.5f, 0.5f, 12.5f);
	shader->loadMatrix("model", m_model.getTransformationMatrix());
	m_cube->draw(Globals::textureManager.get("wood").getTexture());
	// then create multiple cubes as the scenery
	glBindTexture(GL_TEXTURE_2D, Globals::textureManager.get("container").getTexture());

	m_model.reset();
	m_model.translate(0.0f, 1.5f, 0.0);
	m_model.scale(0.5f, 0.5f, 0.5f);
	shader->loadMatrix("model", m_model.getTransformationMatrix());
	m_cube->drawRaw();

	m_model.reset();
	m_model.translate(2.0f, 0.0f, 1.0);
	m_model.scale(0.5f, 0.5f, 0.5f);
	shader->loadMatrix("model", m_model.getTransformationMatrix());
	m_cube->drawRaw();

	m_model.reset();
	m_model.translate(-1.0f, -1.0f, 2.0);
	m_model.rotate(Vector3f::Normalize(Vector3f(1.0, 0.0, 1.0)), 60.0f);
	shader->loadMatrix("model", m_model.getTransformationMatrix());
	m_cube->drawRaw();

	m_model.reset();
	m_model.translate(0.0f, 2.7f, 4.0);
	m_model.scale(0.5f, 0.5f, 0.5f);
	shader->loadMatrix("model", m_model.getTransformationMatrix());
	m_cube->drawRaw();

	m_model.reset();
	m_model.translate(-2.0f, 1.0f, -3.0);
	m_model.scale(0.5f, 0.5f, 0.5f);
	shader->loadMatrix("model", m_model.getTransformationMatrix());
	m_cube->drawRaw();

	m_model.reset();
	m_model.translate(-3.0f, 0.0f, 0.0);
	m_model.rotate(Vector3f::Normalize(Vector3f(1.0, 0.0, 1.0)), 124.0f);
	shader->loadMatrix("model", m_model.getTransformationMatrix());
	m_cube->drawRaw();

	// finally show all the light sources as bright cubes
	shader = Globals::shaderManager.getAssetPointer("bloom_light");
	glUseProgram(shader->m_program);
	shader->loadMatrix("projection", Globals::projection);
	shader->loadMatrix("view", m_camera.getViewMatrix());

	for (unsigned int i = 0; i < lightPositions.size(); i++) {
		m_model.reset();
		m_model.translate(lightPositions[i][0], lightPositions[i][1], lightPositions[i][2]);
		m_model.scale(0.25f, 0.25f, 0.25f);
		shader->loadMatrix("model", m_model.getTransformationMatrix());
		shader->loadVector("lightColor", lightColors[i]);
		m_cube->drawRaw();
	}
	m_hdrBuffer.unbind();
}

void Game::resize(int deltaW, int deltaH) {

	m_copyFramebuffer.resize(Application::Width, Application::Height);
	m_hdrBuffer.resize(Application::Width, Application::Height);
	offsetX = Application::Width * size[0] * 0.5f;
	offsetY = Application::Height * size[1] * 0.5f;

	bloomRenderer.resize(Application::Width, Application::Height);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 100.0f);
}
