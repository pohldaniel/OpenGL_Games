#include "Game.h"
#include "engine/input/EventDispatcher.h"
#include "Application.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);

	m_cube = new Cube();
	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	fluidSys = new FluidSystem(m_width, m_height, m_depth);
	fluidSys->splat();

	trackball.reshape(Application::Width, Application::Height);


	trackball.setDollyPosition(-5.0f);

	applyTransformation(trackball.getTransform());
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

	if (keyboard.keyDown(Keyboard::KEY_C)) {
		fluidSys->reset();
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

	trackball.idle();
	applyTransformation(trackball.getTransform());
};

void Game::render(unsigned int &frameBuffer) {

	//glClearColor(0.576f, 0.709f, 0.949f, 0.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	if ( ((rand() & 0xff) > 220)) {
		fluidSys->splat();
	}
	fluidSys->getStateBuffer()->setFiltering(GL_NEAREST);
	fluidSys->step(1.0f);
	

	fluidSys->getStateBuffer()->setFiltering(GL_LINEAR);
	auto shader = Globals::shaderManager.getAssetPointer("ray_march");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_model);
	shader->loadMatrix("u_invModelView", m_invModel * m_camera.getInvViewMatrix());

	shader->loadFloat("density", 0.01f);
	shader->loadFloat("brightness", 2.0f);

	shader->loadInt("u_texture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, fluidSys->getStateBuffer()->getTexture());

	m_cube->drawRaw();
	glUseProgram(0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	trackball.motion(event.x, event.y);
	nv::matrix4f mtx = trackball.getTransform();
	applyTransformation(trackball.getTransform());
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {	
	trackball.mouse(0, 0, event.x, event.y);
	nv::matrix4f mtx = trackball.getTransform();
	applyTransformation(trackball.getTransform());
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	trackball.mouse(0, 1, event.x, event.y);
	nv::matrix4f mtx = trackball.getTransform();
	applyTransformation(trackball.getTransform());
	
}

void Game::applyTransformation(nv::matrix4f& mtx) {
	m_model.set(mtx._11, mtx._12, mtx._13, mtx._14,
				mtx._21, mtx._22, mtx._23, mtx._24,
				mtx._31, mtx._32, mtx._33, mtx._34,
				mtx._41, mtx._42, mtx._43, mtx._44);

	
	m_invModel.set(m_model[0][0], m_model[1][0], m_model[2][0], 0.0f,
		m_model[0][1], m_model[1][1], m_model[2][1], 0.0f,
		m_model[0][2], m_model[1][2], m_model[2][2], 0.0f,
		-(m_model[3][0] * m_model[0][0] + m_model[3][1] * m_model[0][1] + m_model[3][2] * m_model[0][2]), 
		-(m_model[3][0] * m_model[1][0] + m_model[3][1] * m_model[1][1] + m_model[3][2] * m_model[1][2]),
		-(m_model[3][0] * m_model[2][0] + m_model[3][1] * m_model[2][1] + m_model[3][2] * m_model[2][2]), 1.0f);

}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
}