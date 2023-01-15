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
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
	m_camera.lookAt(Vector3f(5.0f, 2.0f, -7.0f), Vector3f(5.0f, 2.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	m_terrain = new TerrainClass();
	m_terrain->Initialize("res/heightmap01.bmp", &Globals::textureManager.get("dirt"));

	m_quadTree = new QuadTree();
	m_quadTree->Initialize(m_terrain);
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
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("texture");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", Globals::projection);
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Globals::textureManager.get("dirt").getTexture());

	m_quadTree->Render();

	glUseProgram(0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
}
