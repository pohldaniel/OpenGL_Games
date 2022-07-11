
#include "Game.h"

const float HEIGHTMAP_ROUGHNESS = 1.2f;
const float HEIGHTMAP_SCALE = 2.0f;

const int HEIGHTMAP_RESOLUTION = 128;
const int HEIGHTMAP_WIDTH = 8192;
const float CAMERA_Y_OFFSET = 200.0f;
const Vector3f CAMERA_ACCELERATION(400.0f, 400.0f, 400.0f);
const Vector3f CAMERA_VELOCITY(200.0f, 200.0f, 200.0f);
Vector3f LIGHT_DIRECTION(-100.0f, 100.0f, -100.0f);

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), m_water(Water(m_dt, m_fdt)) {
	
	m_copyFramebuffer.create(WIDTH, HEIGHT);
	m_copyFramebuffer.attachTexture(Framebuffer::Attachments::COLOR);
	m_copyFramebuffer.attachRenderbuffer(Framebuffer::Attachments::DEPTH_STENCIL);

	m_quadShader = Globals::shaderManager.getAssetPointer("quad");
	m_quadShadow = Globals::shaderManager.getAssetPointer("quad_shadow");
	m_quadArrayShader = Globals::shaderManager.getAssetPointer("quad_array");
	m_quadArrayShadowShader = Globals::shaderManager.getAssetPointer("quad_array_shadow");

	m_reflectionQuad = new Quad(Vector2f(-1.0f, 1.0f - size[1]), size);
	m_refractionQuad = new Quad(Vector2f(1.0f - size[0], 1.0f - size[1]), size);
	m_perlinQuad = new Quad(Vector2f(-1.0f, -1.0f), size);
	m_shadowQuad = new Quad(Vector2f(1.0f - size[0], -1.0f), size);

	offsetX = WIDTH * size[0] * 0.5f;
	offsetY = HEIGHT * size[1] * 0.5f;
	
	m_terrain.create(Globals::textureManager.get("perlin"), HEIGHTMAP_WIDTH, HEIGHTMAP_SCALE);
	//m_terrain.create(HEIGHTMAP_RESOLUTION, HEIGHTMAP_WIDTH, HEIGHTMAP_SCALE, HEIGHTMAP_ROUGHNESS);
	//m_terrain.createProcedural(HEIGHTMAP_RESOLUTION, HEIGHTMAP_WIDTH, HEIGHTMAP_SCALE, HEIGHTMAP_ROUGHNESS);
	

	//m_terrain.setGridPosition(0, 0);
	//m_terrain.setGridPosition(0, 1);
	//m_terrain.setGridPosition(1, 0);
	//m_terrain.setGridPosition(1, 1);
	//m_terrain.createInstances();
	m_terrain.scaleRegions(HEIGHTMAP_SCALE);

	m_water.create(1, HEIGHTMAP_WIDTH, 200.0f);

	Vector3f pos;
	pos[0] = HEIGHTMAP_WIDTH * 0.5f;
	pos[2] = HEIGHTMAP_WIDTH * 0.5f;
	pos[1] = m_terrain.getHeightMap().heightAt(pos[0], pos[2]) + CAMERA_Y_OFFSET;

	//setup the camera.
	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 1.0f, 5000.0f);
	m_camera.lookAt(pos, Vector3f(pos[0] + 100.0f, pos[1] + 10.0f, pos[2] + 100.0f), Vector3f(0.0f, 1.0f, 0.0f));
	//m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

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

	m_meshQuad = new MeshQuad(8192, 8192, 400);
	m_meshQuad->setPrecision(1, 1);
	m_meshQuad->buildMesh();
	m_meshQuad->setShader(Globals::shaderManager.getAssetPointer("shadowQuad"));
	m_meshQuad->setTexture(&Globals::textureManager.get("gray"));

	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 300.0f, 450.1f, HEIGHTMAP_WIDTH * 0.5f + 300.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 600.0f, 450.1f, HEIGHTMAP_WIDTH * 0.5f + 300.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 300.0f, 450.1f, HEIGHTMAP_WIDTH * 0.5f + 600.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 600.0f, 450.1f, HEIGHTMAP_WIDTH * 0.5f + 600.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 900.0f, 450.1f, HEIGHTMAP_WIDTH * 0.5f + 900.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 1200.0f, 450.1f, HEIGHTMAP_WIDTH * 0.5f + 1200.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 1500.0f, 450.1f, HEIGHTMAP_WIDTH * 0.5f + 1500.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 1800.0f, 450.1f, HEIGHTMAP_WIDTH * 0.5f + 1800.0f), 100, 100, 100));

	for (auto entitie : m_entities) {
		entitie->setPrecision(1, 1);
		entitie->buildMesh4Q();
		entitie->setShader(Globals::shaderManager.getAssetPointer("texture"));
		entitie->setTexture(&Globals::textureManager.get("marbel"));
	}

	m_camera.setUpLightTransformation(1500.0f);
	m_camera.calcLightTransformation(LIGHT_DIRECTION);
	m_camera.calcLightTransformation2(LIGHT_DIRECTION);


	glGenTextures(1, &m_lightDepthMaps);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_lightDepthMaps);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT24, 2048, 2048, m_camera.m_numberCascades, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glGenFramebuffers(1, &m_lightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_lightDepthMaps, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	if (keyboard.keyPressed(Keyboard::KEY_R)) {
		m_debugCount = (m_debugCount < (m_camera.m_numberCascades - 1)) ? m_debugCount + 1 : 0;
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

	m_camera.calcLightTransformation(LIGHT_DIRECTION);
	m_camera.calcLightTransformation2(LIGHT_DIRECTION);
	//performCameraCollisionDetection();
};

void Game::render(unsigned int &frameBuffer) {
	//glEnable(GL_BLEND);
	renderOffscreen();
	
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	m_terrain.draw(m_camera);	
	m_water.render(m_camera, m_water.getReflectionBuffer().getColorTexture(), m_water.getRefractionBuffer().getColorTexture(), m_water.getRefractionBuffer().getDepthTexture());
	auto shader = Globals::shaderManager.getAssetPointer("shadowQuad");
	glUseProgram(shader->m_program);

	shader->loadMatrixArray("u_projectionShadows", m_camera.lightProjections, m_camera.m_numberCascades);
	shader->loadMatrixArray("u_viewShadows", m_camera.lightViews, m_camera.m_numberCascades);
	shader->loadFloatArray("u_cascadeEndClipSpace", m_camera.m_cascadeEndClipSpace, m_camera.m_numberCascades);
	shader->loadInt("u_shadowMaps", 1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_lightDepthMaps);

	m_meshQuad->draw(m_camera);

	for (auto entitie : m_entities) {
		entitie->draw(m_camera);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);	
	//glDisable(GL_BLEND);

	if (m_debug) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_copyFramebuffer.getFramebuffer());
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
		m_reflectionQuad->render(m_water.getReflectionBuffer().getColorTexture());
		glUseProgram(0);

		glScissor(WIDTH - offsetX, HEIGHT - offsetY, offsetX, offsetY);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(m_quadShader->m_program);
		m_quadShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		m_refractionQuad->render(m_water.getRefractionBuffer().getColorTexture());
		glUseProgram(0);


		glScissor(0, 0, offsetX, offsetY);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(m_quadShader->m_program);
		m_quadShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		m_perlinQuad->render(Globals::textureManager.get("perlin").getTexture());
		glUseProgram(0);

		glScissor(WIDTH - offsetX, 0, offsetX, offsetY);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		glUseProgram(m_quadArrayShadowShader->m_program);
		m_quadArrayShadowShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		m_quadArrayShadowShader->loadInt("u_layer", m_debugCount);
		m_shadowQuad->render(m_lightDepthMaps, true);
		glUseProgram(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_SCISSOR_TEST);
	}
}

void Game::renderOffscreen() {
	glEnable(GL_CLIP_DISTANCE0);

	m_water.bindReflectionBuffer();
	glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);

	glUseProgram(Globals::shaderManager.getAssetPointer("terrain")->m_program);
	Globals::shaderManager.getAssetPointer("terrain")->loadVector("u_plane", Vector4f(0.0f, 1.0f, 0.0f, -m_water.getWaterLevel()));
	glUseProgram(0);

	m_camera.pitchReflection((m_water.getWaterLevel()));
	m_terrain.drawNormal(m_camera);
	m_camera.pitchReflection(-(m_water.getWaterLevel()));
	glDisable(GL_CULL_FACE);
	Framebuffer::Unbind();

	m_water.bindRefractionBuffer();
	glClearColor(0.3f, 0.5f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(Globals::shaderManager.getAssetPointer("terrain")->m_program);
	Globals::shaderManager.getAssetPointer("terrain")->loadVector("u_plane", Vector4f(0.0f, -1.0f, 0.0f, m_water.getWaterLevel()));
	glUseProgram(0);

	m_terrain.draw(m_camera);
	Framebuffer::Unbind();

	glDisable(GL_CLIP_DISTANCE0);

	auto shader = Globals::shaderManager.getAssetPointer("depthGS");
	glUseProgram(shader->m_program);
	glViewport(0, 0, 2048, 2048);
	glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
	glClearColor(0.3f, 1.0f, 0.9f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	shader->loadMatrixArray("u_projectionShadows", m_camera.lightProjections, m_camera.m_numberCascades);
	shader->loadMatrixArray("u_viewShadows", m_camera.lightViews, m_camera.m_numberCascades);
	for (auto entitie : m_entities) {
		entitie->drawShadow(m_camera);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
	glUseProgram(0);
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

	m_flag = newPos[1] < m_water.getWaterLevel() + CAMERA_Y_OFFSET;
}