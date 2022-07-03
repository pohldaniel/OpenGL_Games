#include "Game.h"

const float HEIGHTMAP_ROUGHNESS = 1.2f;
const float HEIGHTMAP_SCALE = 2.0f;

const int HEIGHTMAP_RESOLUTION = 128;
const int HEIGHTMAP_WIDTH = 8192;
const float CAMERA_Y_OFFSET = 100.0f;
const Vector3f CAMERA_ACCELERATION(400.0f, 400.0f, 400.0f);
const Vector3f CAMERA_VELOCITY(200.0f, 200.0f, 200.0f);

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), m_water(Water(m_dt, m_fdt)) {
	m_quadShader = Globals::shaderManager.getAssetPointer("quad");

	unsigned int rbDepthStencil;
	glGenTextures(1, &m_frameTextureCopy);
	glBindTexture(GL_TEXTURE_2D, m_frameTextureCopy);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);	
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &m_fboCopy);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboCopy);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_frameTextureCopy, 0);

	// buffer for depth and stencil
	glGenRenderbuffers(1, &rbDepthStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, rbDepthStencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbDepthStencil);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int rbDepthStencilReflection;
	glGenTextures(1, &m_frameTextureReflection);
	glBindTexture(GL_TEXTURE_2D, m_frameTextureReflection);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &m_frameTextureReflectionDepth);
	glBindTexture(GL_TEXTURE_2D, m_frameTextureReflectionDepth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &m_fboReflection);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboReflection);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_frameTextureReflection, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_frameTextureReflectionDepth, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_frameTextureReflectionDepth, 0);
	// buffer for depth and stencil
	/*glGenRenderbuffers(1, &rbDepthStencilReflection);
	glBindRenderbuffer(GL_RENDERBUFFER, rbDepthStencilReflection);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbDepthStencilReflection);*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenTextures(1, &m_frameTextureRefraction);
	glBindTexture(GL_TEXTURE_2D, m_frameTextureRefraction);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &m_frameTextureRefractionDepth);
	glBindTexture(GL_TEXTURE_2D, m_frameTextureRefractionDepth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenFramebuffers(1, &m_fboRefraction);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboRefraction);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_frameTextureRefraction, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_frameTextureRefractionDepth, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	m_reflectionQuad = new Quad(Vector2f(-1.0f, 1.0f - size[1]), size);
	m_refractionQuad = new Quad(Vector2f(1.0f - size[0], 1.0f - size[1]), size);
	offsetX = WIDTH * size[0] * 0.5f;
	offsetY = HEIGHT * size[1] * 0.5f;
	
	m_terrain.create(Globals::textureManager.get("height_map"), HEIGHTMAP_WIDTH, HEIGHTMAP_SCALE);
	//m_terrain.create(HEIGHTMAP_RESOLUTION, HEIGHTMAP_WIDTH, HEIGHTMAP_SCALE, HEIGHTMAP_ROUGHNESS);
	//m_terrain.createProcedural(HEIGHTMAP_RESOLUTION, HEIGHTMAP_WIDTH, HEIGHTMAP_SCALE, HEIGHTMAP_ROUGHNESS);
	

	//m_terrain.setGridPosition(0, 0);
	//m_terrain.setGridPosition(0, 1);
	//m_terrain.setGridPosition(1, 0);
	//m_terrain.setGridPosition(1, 1);
	//m_terrain.createInstances();
	m_terrain.scaleRegions(HEIGHTMAP_SCALE);

	m_water.create(1, HEIGHTMAP_WIDTH);

	Vector3f pos;

	pos[0] = HEIGHTMAP_WIDTH * 0.5f;
	pos[2] = HEIGHTMAP_WIDTH * 0.5f;
	pos[1] = m_terrain.getHeightMap().heightAt(pos[0], pos[2]) + CAMERA_Y_OFFSET;

	//setup the camera.
	m_camera = Camera();
	m_camera.lookAt(pos, Vector3f(pos[0] + 100.0f, pos[1] + 10.0f, pos[2] + 100.0f), Vector3f(0.0f, 1.0f, 0.0f));
	//m_camera.lookAt(Vector3f(4.0f, 0.0f, 20.0f), Vector3f(4.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
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

	if (keyboard.keyPressed(Keyboard::KEY_1) ) {
		m_terrain.toggleDisableColorMaps();
	}

	if (keyboard.keyPressed(Keyboard::KEY_2)) {
		m_terrain.toggleColorMode();
	}

	if (keyboard.keyPressed(Keyboard::KEY_3)) {
		m_terrain.generateUsingDiamondSquareFractal(HEIGHTMAP_ROUGHNESS);
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_debug = !m_debug;
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
	
	glEnable(GL_CLIP_DISTANCE0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fboReflection);
	
	glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(Globals::shaderManager.getAssetPointer("terrain")->m_program);
	Globals::shaderManager.getAssetPointer("terrain")->loadVector("u_plane", Vector4f(0.0f, 1.0f, 0.0f, -200.0f + 1.0f));
	glUseProgram(0);

	m_camera.pitchReflection(200.0f);
	m_terrain.draw(m_camera);
	m_camera.pitchReflection(-200.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fboRefraction);

	glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(Globals::shaderManager.getAssetPointer("terrain")->m_program);
	Globals::shaderManager.getAssetPointer("terrain")->loadVector("u_plane", Vector4f(0.0f, -1.0f, 0.0f, 200.0f));
	glUseProgram(0);

	m_terrain.draw(m_camera);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_CLIP_DISTANCE0);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glEnable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_terrain.draw(m_camera);
	m_water.render(m_camera, m_frameTextureReflection, m_frameTextureRefraction, m_frameTextureRefractionDepth);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_BLEND);


	if (m_debug) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fboCopy);
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_SCISSOR_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		glScissor(0, HEIGHT - offsetY, offsetX, offsetY);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(m_quadShader->m_program);
		m_quadShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		m_reflectionQuad->render(m_frameTextureReflection);
		glUseProgram(0);

		glScissor(WIDTH - offsetX, HEIGHT - offsetY, offsetX, offsetY);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(m_quadShader->m_program);
		m_quadShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		m_refractionQuad->render(m_frameTextureRefraction);
		glUseProgram(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_SCISSOR_TEST);
	}
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