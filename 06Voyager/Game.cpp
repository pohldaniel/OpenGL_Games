#include "Game.h"

const float     HEIGHTMAP_ROUGHNESS = 1.2f;
const float     HEIGHTMAP_SCALE = 2.0f;
const float     HEIGHTMAP_TILING_FACTOR = 12.0f;
const int       HEIGHTMAP_SIZE = 128;
const int       HEIGHTMAP_GRID_SPACING = 16;
const float     CAMERA_Y_OFFSET = 25.0f;

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	m_terrain.create(HEIGHTMAP_SIZE, HEIGHTMAP_GRID_SPACING, HEIGHTMAP_SCALE);
	m_terrain.generateUsingDiamondSquareFractal(HEIGHTMAP_ROUGHNESS);

	Vector3f pos;

	pos[0] = HEIGHTMAP_SIZE * HEIGHTMAP_GRID_SPACING * 0.5f;
	pos[2] = HEIGHTMAP_SIZE * HEIGHTMAP_GRID_SPACING * 0.5f;
	pos[1] = m_terrain.getHeightMap().heightAt(pos[0], pos[2]) + CAMERA_Y_OFFSET;

	//setup the camera.
	camera = Camera();
	//camera.lookAt(pos, Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	camera.setPosition(pos);

}

Game::~Game() {}

void Game::fixedUpdate() {
	
};

void Game::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f directrion = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;
	if (keyboard.keyDown(Keyboard::KEY_W) || keyboard.keyDown(Keyboard::KEY_UP)) {		
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S) || keyboard.keyDown(Keyboard::KEY_DOWN)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A) || keyboard.keyDown(Keyboard::KEY_LEFT)) {
		directrion += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D) || keyboard.keyDown(Keyboard::KEY_RIGHT)) {
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
	dx = mouse.xPosRelative() * 0.1f;
	dy = mouse.yPosRelative() * 0.1f;

	if (move || dx != 0.0f || dy != 0.0f) {

		// rotate camera
		if (dx || dy) {
			camera.rotate(dx, dy, 0.0f);
			
		} // end if any rotation

		if (move) {
			float speed = 1.3f;
			camera.move(directrion[0] * speed, directrion[1] * speed, directrion[2] * speed);
		}

	}// end if any movement
};

void Game::render(unsigned int &frameBuffer) {
	//glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.3f, 0.5f, 0.9f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	m_terrain.draw(camera);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}