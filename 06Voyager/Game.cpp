#include "Game.h"

const float HEIGHTMAP_ROUGHNESS = 1.2f;
const float HEIGHTMAP_SCALE = 2.0f;

const int HEIGHTMAP_RESOLUTION = 128;
const int HEIGHTMAP_WIDTH = 8192;
const float CAMERA_Y_OFFSET = 100.0f;
const Vector3f CAMERA_ACCELERATION(400.0f, 400.0f, 400.0f);
const Vector3f CAMERA_VELOCITY(200.0f, 200.0f, 200.0f);

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	//m_terrain.create("res/heightmap.png", HEIGHTMAP_WIDTH, HEIGHTMAP_SCALE);
	m_terrain.create(Globals::textureManager.get("height_map"), HEIGHTMAP_WIDTH, HEIGHTMAP_SCALE);
	//m_terrain.create(HEIGHTMAP_RESOLUTION, HEIGHTMAP_WIDTH, HEIGHTMAP_SCALE, HEIGHTMAP_ROUGHNESS);
	//m_terrain.createProcedural(HEIGHTMAP_RESOLUTION, HEIGHTMAP_WIDTH, HEIGHTMAP_SCALE, HEIGHTMAP_ROUGHNESS);
	m_terrain.scaleRegions(HEIGHTMAP_SCALE);

	//m_terrain.setGridPosition(0, 0);
	//m_terrain.setGridPosition(0, 1);
	//m_terrain.setGridPosition(1, 0);
	//m_terrain.setGridPosition(1, 1);
	//m_terrain.createInstances();

	Vector3f pos;

	pos[0] = HEIGHTMAP_WIDTH * 0.5f;
	pos[2] = HEIGHTMAP_WIDTH * 0.5f;
	pos[1] = m_terrain.getHeightMap().heightAt(pos[0], pos[2]) +  CAMERA_Y_OFFSET;

	//setup the camera.
	m_camera = Camera();
	m_camera.lookAt(pos, Vector3f(pos[0] + 100.0f, pos[1] + 10.0f, pos[2] + 100.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setAcceleration(CAMERA_ACCELERATION);
	m_camera.setVelocity(CAMERA_VELOCITY);
	m_camera.setRotationSpeed(0.1f);
	float gridSpacing = static_cast<float>(HEIGHTMAP_WIDTH) / static_cast<float>(HEIGHTMAP_RESOLUTION);
	float upperBounds = HEIGHTMAP_WIDTH - gridSpacing;
	float lowerBounds = gridSpacing;

	m_cameraBoundsMax[0] = upperBounds;
	m_cameraBoundsMax[1] = HEIGHTMAP_WIDTH * 2.0f;
	m_cameraBoundsMax[2] = upperBounds;

	m_cameraBoundsMin[0] = lowerBounds;
	m_cameraBoundsMin[1] = 0.0f;
	m_cameraBoundsMin[2] = lowerBounds;
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

	if (keyboard.keyPressed(Keyboard::KEY_T) ) {
		m_terrain.toggleDisableColorMaps();
	}

	if (keyboard.keyPressed(Keyboard::KEY_R)) {
		m_terrain.toggleColorMode();
	}

	if (keyboard.keyPressed(Keyboard::KEY_M)) {
		m_terrain.generateUsingDiamondSquareFractal(HEIGHTMAP_ROUGHNESS);
	}

	Mouse &mouse = Mouse::instance();
	dx = mouse.xPosRelative();
	dy = mouse.yPosRelative();

	if (move || dx != 0.0f || dy != 0.0f) {

		// rotate camera
		if (dx || dy) {
			m_camera.rotateSmoothly(dx, dy, 0.0f);
			
		} // end if any rotation

		if (move) {
			//float speed = 1.3f;
			m_camera.updatePosition(directrion, m_dt);
		}

	}// end if any movement

	//performCameraCollisionDetection();
};

void Game::render(unsigned int &frameBuffer) {

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	
	glClearColor(0.3f, 0.5f, 0.9f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	m_terrain.draw(m_camera);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::performCameraCollisionDetection(){

	const Vector3f &pos = m_camera.getPosition();
	Vector3f newPos(pos);

	if (pos[0] > m_cameraBoundsMax[0])
		newPos[0] = m_cameraBoundsMax[0];

	if (pos[0] < m_cameraBoundsMin[0])
		newPos[0] = m_cameraBoundsMin[0];

	if (pos[2] > m_cameraBoundsMax[2])
		newPos[2] = m_cameraBoundsMax[2];

	if (pos[2] < m_cameraBoundsMin[2])
		newPos[2] = m_cameraBoundsMin[2];

	newPos[1] = m_terrain.getHeightMap().heightAt(newPos[0], newPos[2]) + CAMERA_Y_OFFSET;
	m_camera.setPosition(newPos);
}