
#include "Game.h"

const float HEIGHTMAP_ROUGHNESS = 1.2f;
const float HEIGHTMAP_SCALE = 2.0f;

const int HEIGHTMAP_RESOLUTION = 128;
const int HEIGHTMAP_WIDTH = 8192;
const float CAMERA_Y_OFFSET = 100.0f;
const Vector3f CAMERA_ACCELERATION(400.0f, 400.0f, 400.0f);
const Vector3f CAMERA_VELOCITY(200.0f, 200.0f, 200.0f);
Vector3f LIGHT_DIRECTION(-100.0f, 100.0f, -100.0f);

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), m_water(Water(m_dt, m_fdt)), m_skybox(SkyBox(m_dt, m_fdt, 2500.0f)) {
	
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


	std::vector<btCollisionShape*> terrainShape = Physics::CreateStaticCollisionShapes(&m_terrain, 1.0f);
	btRigidBody* body = Globals::physics->addStaticModel(terrainShape, Physics::BtTransform(), false, btVector3(1, 1, 1), Physics::collisiontypes::TERRAIN, Physics::collisiontypes::RAY | Physics::collisiontypes::CAR);

	m_water.create(1, HEIGHTMAP_WIDTH, 200.0f);

	Vector3f pos;
	pos[0] = HEIGHTMAP_WIDTH * 0.5f;
	pos[2] = HEIGHTMAP_WIDTH * 0.5f;
	pos[1] = m_terrain.getHeightMap().heightAt(pos[0], pos[2]) + CAMERA_Y_OFFSET;
	//pos[1] = 0.0f;
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

	m_bottom = new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f, -5.0f, HEIGHTMAP_WIDTH * 0.5f), HEIGHTMAP_WIDTH, 5.0f, HEIGHTMAP_WIDTH);
	m_bottom->setShader(Globals::shaderManager.getAssetPointer("texture"));
	m_bottom->setTexture(&Globals::textureManager.get("marble"));
	m_bottom->setPrecision(1, 1);
	m_bottom->buildMesh4Q();

	std::vector<btCollisionShape*> bottomShape = Physics::CreateStaticCollisionShapes(m_bottom, 1.0f);
	btRigidBody* bottomBody = Globals::physics->addStaticModel(bottomShape, Physics::BtTransform(), false, btVector3(1, 1, 1), Physics::collisiontypes::RENDERABLE_OBJECT, Physics::collisiontypes::RAY | Physics::collisiontypes::CAR);


	//m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 300.0f, 450.1f, HEIGHTMAP_WIDTH * 0.5f + 300.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 600.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 600.0f, HEIGHTMAP_WIDTH * 0.5f + 300.0f) + 50.0f, HEIGHTMAP_WIDTH * 0.5f + 300.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 300.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 300.0f, HEIGHTMAP_WIDTH * 0.5f + 600.0f) + 50.0f, HEIGHTMAP_WIDTH * 0.5f + 600.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 600.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 600.0f, HEIGHTMAP_WIDTH * 0.5f + 600.0f) + 50.0f, HEIGHTMAP_WIDTH * 0.5f + 600.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 900.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 900.0f, HEIGHTMAP_WIDTH * 0.5f + 900.0f) + 50.0f, HEIGHTMAP_WIDTH * 0.5f + 900.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 1200.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 1200.0f, HEIGHTMAP_WIDTH * 0.5f + 12000.0f) + 50.0f, HEIGHTMAP_WIDTH * 0.5f + 1200.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 1500.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 1500.0f, HEIGHTMAP_WIDTH * 0.5f + 1500.0f) + 50.0f, HEIGHTMAP_WIDTH * 0.5f + 1500.0f), 100, 100, 100));
	m_entities.push_back(new MeshCube(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 1800.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 1800.0f, HEIGHTMAP_WIDTH * 0.5f + 1800.0f) + 50.0f, HEIGHTMAP_WIDTH * 0.5f + 1800.0f), 100, 100, 100));

	for (auto entitie : m_entities) {
		entitie->setPrecision(1, 1);
		entitie->buildMesh4Q();
		entitie->setShader(Globals::shaderManager.getAssetPointer("dissolve"));
		entitie->setTexture(&Globals::textureManager.get("marble"));

		std::vector<btCollisionShape*> cubeShape = Physics::CreateStaticCollisionShapes(entitie, 1.0f);
		btRigidBody* body = Globals::physics->addStaticModel(cubeShape, Physics::BtTransform(), false, btVector3(1, 1, 1), Physics::collisiontypes::RENDERABLE_OBJECT | Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::RAY);
		body->setUserPointer(reinterpret_cast<void*>(entitie));
	}

	m_camera.setUpLightTransformation(std::vector<Vector2f>({ Vector2f(1.0f, 250.0f), Vector2f(250.0f, 500.0f), Vector2f(500.0f, 1000.0f), Vector2f(1000.0f, 2500.0f), Vector2f(2500.0f, 5000.0f) }));
	m_camera.calcLightTransformation(LIGHT_DIRECTION);
	m_camera.calcLightTransformation2(LIGHT_DIRECTION);

	m_lightDepthFramebuffer.create(2048, 2048);
	//there are two options use a second color attachment and glEnable(GL_ALPHA_TEST) in the renderShadow() function or use if(alpha < 0.5) discard; inside the depthGS.fs shader
	//m_lightDepthFramebuffer.attachLayerdTexture(Framebuffer::Attachments::COLOR, m_camera.m_numberCascades);
	m_lightDepthFramebuffer.attachLayerdTexture(Framebuffer::Attachments::DEPTH24, m_camera.m_numberCascades);

	
	m_tree = new Tree();
	m_tree->translate(HEIGHTMAP_WIDTH * 0.5f + 800.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 800.0f, HEIGHTMAP_WIDTH * 0.5f + 300.0f), HEIGHTMAP_WIDTH * 0.5f + 300.0f);
	m_tree->scale(10.0f, 10.0f, 10.0f);

	std::vector<btCollisionShape*> treeShape = Physics::CreateStaticCollisionShapes(m_tree->getConvexHull().m_vertexBuffer, m_tree->getConvexHull().m_indexBuffer, 10.0f);
	btRigidBody* treeBody = Globals::physics->addStaticModel(treeShape, Physics::BtTransform(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 800.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 800.0f, HEIGHTMAP_WIDTH * 0.5f + 300.0f), HEIGHTMAP_WIDTH * 0.5f + 300.0f)), false, btVector3(1, 1, 1), Physics::collisiontypes::RENDERABLE_OBJECT, Physics::collisiontypes::RAY);


	m_fernEntities.push_back(new Fern());
	m_fernEntities.back()->translate(HEIGHTMAP_WIDTH * 0.5f + 400.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 400.0f, HEIGHTMAP_WIDTH * 0.5f + 400.0f), HEIGHTMAP_WIDTH * 0.5f + 400.0f);

	std::vector<btCollisionShape*> fernShape1 = Physics::CreateStaticCollisionShapes(m_fernEntities.back()->getConvexHull().m_vertexBuffer, m_fernEntities.back()->getConvexHull().m_indexBuffer, 10.0f);
	btRigidBody* fernBody1 = Globals::physics->addStaticModel(fernShape1, Physics::BtTransform(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 400.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 400.0f, HEIGHTMAP_WIDTH * 0.5f + 400.0f), HEIGHTMAP_WIDTH * 0.5f + 400.0f)), false, btVector3(1, 1, 1), Physics::collisiontypes::RENDERABLE_OBJECT, Physics::collisiontypes::RAY);

	m_fernEntities.push_back(new Fern());
	m_fernEntities.back()->translate(HEIGHTMAP_WIDTH * 0.5f + 500.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 400.0f, HEIGHTMAP_WIDTH * 0.5f + 400.0f), HEIGHTMAP_WIDTH * 0.5f + 400.0f);

	std::vector<btCollisionShape*> fernShape2 = Physics::CreateStaticCollisionShapes(m_fernEntities.back()->getConvexHull().m_vertexBuffer, m_fernEntities.back()->getConvexHull().m_indexBuffer, 10.0f);
	btRigidBody* fernBody2 = Globals::physics->addStaticModel(fernShape2, Physics::BtTransform(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 500.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 400.0f, HEIGHTMAP_WIDTH * 0.5f + 400.0f), HEIGHTMAP_WIDTH * 0.5f + 400.0f)), false, btVector3(1, 1, 1), Physics::collisiontypes::RENDERABLE_OBJECT, Physics::collisiontypes::RAY);
	
	for (auto entitie : m_fernEntities) {		
		entitie->scale(10.0f, 10.0f, 10.0f);
	}
	
	m_barrel = new Barrel();

	std::vector<btCollisionShape*> barrelShape = Physics::CreateStaticCollisionShapes(m_barrel->getConvexHull().m_vertexBuffer, m_barrel->getConvexHull().m_indexBuffer, btVector3(10.0f, 10.0f, 10.0f));
	btRigidBody* barrelBody = Globals::physics->addStaticModel(barrelShape, Physics::BtTransform(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 200.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 200.0f, HEIGHTMAP_WIDTH * 0.5f + 200.0f) + 50.0f, HEIGHTMAP_WIDTH * 0.5f + 200.0f), Vector3f(0.0f, 0.0f, 1.0f), 45.0f), false, btVector3(1, 1, 1), Physics::collisiontypes::RENDERABLE_OBJECT, Physics::collisiontypes::RAY);
	barrelBody->setUserPointer(reinterpret_cast<void*>(m_barrel));

	m_barrel->m_transform.fromMatrix(Physics::MatrixFrom(barrelBody->getWorldTransform(), barrelShape[0]->getLocalScaling()));
	charachterSet.loadFromFile("res/verdana.fnt");

	m_text = new Text(charachterSet);
	m_text->setLabel("In Game Text", 2.0f);
	m_text->setColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	//m_text->setPosition(0.0f, 0.0f);

	glUseProgram(Globals::shaderManager.getAssetPointer("terrain")->m_program);
	Globals::shaderManager.getAssetPointer("terrain")->loadVector("fogColor", Vector4f(0.5f, 0.5f, 0.5f, 1.0f));
	glUseProgram(0);

	glUseProgram(Globals::shaderManager.getAssetPointer("skybox")->m_program);
	Globals::shaderManager.getAssetPointer("skybox")->loadVector("fogColor", Vector4f(0.5f, 0.5f, 0.5f, 1.0f));
	glUseProgram(0);

	
	m_car = new PhysicsCar();
	m_car->Initialize(Globals::physics->GetDynamicsWorld(), Physics::BtTransform(Vector3f(HEIGHTMAP_WIDTH * 0.5f + 100.0f, m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 100.0f, HEIGHTMAP_WIDTH * 0.5f + 100.0f) + 50.0f, HEIGHTMAP_WIDTH * 0.5f + 100.0f), Vector3f(0, 1, 0), -45.0f));

	
	Vector3f position;
	position[0] = HEIGHTMAP_WIDTH * 0.5f + 100.0f;
	position[2] = HEIGHTMAP_WIDTH * 0.5f + 100.0f;
	position[1] = m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 100.0f, HEIGHTMAP_WIDTH * 0.5f + 100.0f) + 50.0f;

	cowboy.loadModel("res/models/cowboy/cowboy.dae", "res/models/cowboy/cowboy.png");
	cowboy.rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f);
	cowboy.translate(position[0], position[1], position[2]);
	cowboy.scale(10.0f);

	cowboy.getAnimator()->startAnimation("");


	position[0] = HEIGHTMAP_WIDTH * 0.5f + 800.0f;
	position[2] = HEIGHTMAP_WIDTH * 0.5f + 300.0f;
	position[1] = m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 800.0f, HEIGHTMAP_WIDTH * 0.5f + 300.0f) + 200.0f;

	dragonCompare = new ObjModel();
	dragonCompare->loadObject("res/models/dragon/dragon.obj", Vector3f(1.0f, 0.0f, 0.0f), 0.0f, Vector3f(0.0f, 0.0f, 0.0f), 1.0f);
	dragonCompare->getTransform().setRotPosScale(Vector3f(1.0f, 0.0f, 0.0f), -90.0f, position[0], position[1], position[2], 10.0f, 10.0f, 10.0f);
	dragonCompare->initAssets(Globals::shaderManager, Globals::textureManager);

	position[0] = HEIGHTMAP_WIDTH * 0.5f + 1000.0f;
	position[2] = HEIGHTMAP_WIDTH * 0.5f + 300.0f;
	position[1] = m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 800.0f, HEIGHTMAP_WIDTH * 0.5f + 300.0f) + 200.0f;

	dragonGN = new ObjModel();
	dragonGN->loadObject("res/models/dragon/dragon.obj", Vector3f(1.0f, 0.0f, 0.0f), 0.0f, Vector3f(0.0f, 0.0f, 0.0f), 1.0f, false, true, true);
	dragonGN->getTransform().setRotPosScale(Vector3f(1.0f, 0.0f, 0.0f), -90.0f, position[0], position[1], position[2], 10.0f, 10.0f, 10.0f);
	dragonGN->initAssets(Globals::shaderManager, Globals::textureManager);

	position[0] = HEIGHTMAP_WIDTH * 0.5f + 1300.0f;
	position[2] = HEIGHTMAP_WIDTH * 0.5f + 300.0f;
	position[1] = m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 800.0f, HEIGHTMAP_WIDTH * 0.5f + 300.0f) + 200.0f;

	dragonStacked = new ObjModel();
	dragonStacked->loadObject("res/models/dragon/dragon.obj", Vector3f(1.0f, 0.0f, 0.0f), 0.0f, Vector3f(0.0f, 0.0f, 0.0f), 1.0f, true, true, false, true);
	dragonStacked->getTransform().setRotPosScale(Vector3f(1.0f, 0.0f, 0.0f), -90.0f, position[0], position[1], position[2], 10.0f, 10.0f, 10.0f);
	dragonStacked->generateNormals();
	dragonStacked->initAssets(Globals::shaderManager, Globals::textureManager);

	position[0] = HEIGHTMAP_WIDTH * 0.5f + 1300.0f;
	position[2] = HEIGHTMAP_WIDTH * 0.5f + 700.0f;
	position[1] = m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 800.0f, HEIGHTMAP_WIDTH * 0.5f + 300.0f) + 200.0f;

	dragonAssimp.loadModel("res/models/dragon/dragon.obj", true);
	dragonAssimp.initAssets(Globals::shaderManager, Globals::textureManager);
	dragonAssimp.getTransform().setRotPosScale(Vector3f(1.0f, 0.0f, 0.0f), -90.0f, position[0], position[1], position[2], 10.0f, 10.0f, 10.0f);

	position[0] = HEIGHTMAP_WIDTH * 0.5f + 50.0f;
	position[2] = HEIGHTMAP_WIDTH * 0.5f + 70.0f;
	position[1] = m_terrain.getHeightMap().heightAt(HEIGHTMAP_WIDTH * 0.5f + 50.0f, HEIGHTMAP_WIDTH * 0.5f + 70.0f + 100.0f) + 50.0f;

	/*Globals::animationManager.loadAnimationDae("cowboy_run", "res/models/cowboy/cowboy.dae", "Armature", "cowboy_run");
	assimpAnimated.loadModel("res/models/cowboy/cowboy.dae", "res/models/cowboy/cowboy.png");
	assimpAnimated.rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f);
	assimpAnimated.translate(position[0], position[1], position[2]);
	assimpAnimated.scale(10.3f, 10.3f, 10.3f);
	assimpAnimated.getAnimator()->addAnimation(Globals::animationManager.getAssetPointer("cowboy_run"));
	assimpAnimated.getAnimator()->startAnimation("cowboy_run");*/
	

	Globals::animationManager.loadAnimationDae("vampire_dance", "res/models/vampire/dancing_vampire.dae", "", "vampire_dance");
	assimpAnimated.loadModel("res/models/vampire/dancing_vampire.dae", "res/models/vampire/textures/Vampire_diffuse.png");
	assimpAnimated.rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f);
	assimpAnimated.translate(position[0], position[1], position[2]);
	assimpAnimated.scale(0.3f, 0.3f, 0.3f);
	assimpAnimated.getAnimator()->addAnimation(Globals::animationManager.getAssetPointer("vampire_dance"));
	assimpAnimated.getAnimator()->startAnimation("vampire_dance");

	/*Globals::animationManager.loadAnimationFbx("player_idle", "res/models/player/player.fbx", "Player", "player_idle", 6u, 82u, 50u);
	Globals::animationManager.loadAnimationFbx("player_run", "res/models/player/player.fbx", "Player", "player_run", 83u, 104u, 50u);

	assimpAnimated.loadModel("res/models/player/player.fbx", "res/models/player/textures/Player_D.tga");
	assimpAnimated.rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f);
	assimpAnimated.translate(position[0], position[1], position[2]);
	assimpAnimated.scale(0.3f, 0.3f, 0.3f);
	assimpAnimated.getAnimator()->addAnimation(Globals::animationManager.getAssetPointer("player_idle"));
	assimpAnimated.getAnimator()->addAnimation(Globals::animationManager.getAssetPointer("player_run"));
	assimpAnimated.getAnimator()->startAnimation("player_idle");*/
}

Game::~Game() {}

void Game::fixedUpdate() {
	m_car->Update(m_fdt);
	Globals::physics->stepSimulation(m_fdt);
};

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

	if (keyboard.keyDown(Keyboard::KEY_UP)) {
		m_car->Forward();
	}

	if (keyboard.keyDown(Keyboard::KEY_DOWN)) {
		m_car->Backward();
	}

	if (keyboard.keyDown(Keyboard::KEY_LEFT)) {
		m_car->TurnLeft();
	}

	if (keyboard.keyDown(Keyboard::KEY_RIGHT)) {
		m_car->TurnRight();
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

	if (keyboard.keyPressed(Keyboard::KEY_4)) {
		m_skybox.toggleDayNight();
	}

	if (keyboard.keyPressed(Keyboard::KEY_5)) {
		m_barrel->toggleLightRotation();
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_debug = !m_debug;
	}

	if (keyboard.keyPressed(Keyboard::KEY_R)) {
		m_debugCount = (m_debugCount < (m_camera.m_numberCascades - 1)) ? m_debugCount + 1 : 0;
	}

	if (keyboard.keyPressed(Keyboard::KEY_N)) {
		m_debugNormal = !m_debugNormal;
	}

	if (keyboard.keyPressed(Keyboard::KEY_H)) {
		assimpAnimated.getAnimator()->startAnimation("player_run");
	}

	if (keyboard.keyPressed(Keyboard::KEY_G)) {
		assimpAnimated.getAnimator()->startAnimation("player_idle");
	}

	

	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::MouseButton::BUTTON_LEFT)) {
		
		float mouseXndc = (2.0f * mouse.xPosAbsolute()) / (float)WIDTH - 1.0f;
		float mouseYndc = 1.0f - (2.0f * mouse.yPosAbsolute()) / (float)HEIGHT;

		Vector4f rayStartEye = Vector4f(mouseXndc, mouseYndc, -1.0f, 1.0f) ^ Globals::invProjection;
		Vector4f rayEndEye = Vector4f(mouseXndc, mouseYndc, 1.0f, 1.0f) ^ Globals::invProjection;
		rayEndEye = rayEndEye * (1.0f / rayEndEye[3]);

		
		Vector3f rayStartWorld = rayStartEye * m_camera.getInvViewMatrix();
		Vector3f rayEndWorld = rayEndEye * m_camera.getInvViewMatrix();

		Vector3f rayDirection = rayEndWorld - rayStartWorld;
		Vector3f::Normalize(rayDirection);

		btVector3 origin = btVector3(rayStartWorld[0], rayStartWorld[1], rayStartWorld[2]);		
		btVector3 target = btVector3(rayEndWorld[0], rayEndWorld[1], rayEndWorld[2]);

		RayResultCallback callback(origin, target, Physics::collisiontypes::RAY, Physics::collisiontypes::PICKABLE_OBJECT);
		
		Globals::physics->GetDynamicsWorld()->rayTest(origin, target, callback);
		if (callback.hasHit()) {
			MeshCube* cube = reinterpret_cast<MeshCube*>(callback.m_collisionObject->getUserPointer());
			cube->dissolve();			
		}
		m_mousePicker.click();
	}

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {		
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

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

	m_skybox.update();
	m_barrel->update(m_dt);
	m_barrel->setDrawBorder(m_mousePicker.getPickedId() == m_barrel->getId());
	
	for (auto entitie : m_entities) {
		entitie->update(m_dt);
	}
	m_tree->setDrawBorder(m_mousePicker.getPickedId() == m_tree->getId());

	for (auto entitie : m_fernEntities) {
		entitie->setDrawBorder(m_mousePicker.getPickedId() == entitie->getId());
	}

	m_mousePicker.update(m_dt);
	cowboy.update(m_dt);
	assimpAnimated.update(m_dt);
	//performCameraCollisionDetection();
};

void Game::render(unsigned int &frameBuffer) {
	BuiltInShader::UpdateViewUbo(m_camera);

	renderOffscreen();
	shadowPass();
	mousePickPass();

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	m_water.render(m_camera, m_water.getReflectionBuffer().getColorTexture(), m_water.getRefractionBuffer().getColorTexture(), m_water.getRefractionBuffer().getDepthTexture());
	auto shader = Globals::shaderManager.getAssetPointer("terrain");
	glUseProgram(shader->m_program);

	shader->loadMatrixArray("u_projectionShadows", m_camera.lightProjections, m_camera.m_numberCascades);
	shader->loadMatrixArray("u_viewShadows", m_camera.lightViews, m_camera.m_numberCascades);
	shader->loadFloatArray("u_cascadeEndClipSpace", m_camera.m_cascadeEndClipSpace, m_camera.m_numberCascades);
	shader->loadBool("u_debug", m_debug);
	
	shader->loadInt("u_texture", 0);
	shader->loadInt("u_shadowMaps", 5);
	shader->loadUnsignedInt("u_numCascades", m_camera.m_numberCascades);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_lightDepthFramebuffer.getDepthTexture());

	m_terrain.drawNormal(m_camera);
	glUseProgram(0);

	m_barrel->draw(m_camera);

	
	for (auto entitie : m_fernEntities) {
		entitie->draw(m_camera);
	}

	m_tree->draw(m_camera);

	for (auto entitie : m_entities) {
		entitie->draw(m_camera);
	}
	//m_bottom->draw(m_camera);

	
	m_car->draw(m_camera);
	//cowboy.draw(m_camera);
	assimpAnimated.draw(m_camera);

	if (!m_debugNormal) {
		dragonCompare->draw(m_camera);
		dragonGN->draw(m_camera);
		dragonStacked->drawStacked(m_camera);
		dragonAssimp.drawStacked(m_camera);
	}

	if (m_debugNormal) {
		auto normalGS = Globals::shaderManager.getAssetPointer("normalGS");
		glUseProgram(normalGS->m_program);
		normalGS->loadMatrix("u_projection", Globals::projection);
		normalGS->loadMatrix("u_view", m_camera.getViewMatrix());
		normalGS->loadMatrix("u_model", m_barrel->getTransformationMatrix());
		normalGS->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_barrel->getTransformationMatrix()));
		m_barrel->drawShadow(m_camera);
		glUseProgram(0);

		shader = Globals::shaderManager.getAssetPointer("normal");
		glUseProgram(shader->m_program);
		shader->loadMatrix("u_transform", Globals::projection * m_camera.getViewMatrix() * dragonCompare->getTransformationMatrix());
		dragonCompare->drawRaw();

		shader->loadMatrix("u_transform", Globals::projection * m_camera.getViewMatrix() * dragonGN->getTransformationMatrix());
		dragonGN->drawRaw();

		shader->loadMatrix("u_transform", Globals::projection * m_camera.getViewMatrix() * dragonStacked->getTransformationMatrix());
		dragonStacked->drawRawStacked();

		shader->loadMatrix("u_transform", Globals::projection * m_camera.getViewMatrix() * dragonAssimp.getTransformationMatrix());
		dragonAssimp.drawRawStacked();

		glUseProgram(0);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_mousePicker.draw(m_camera);

	m_skybox.render(m_camera);

	m_text->render();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	

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
		//m_perlinQuad->render(Globals::textureManager.get("perlin").getTexture());
		m_perlinQuad->render(m_mousePicker.getBuffer().getColorTexture());
		glUseProgram(0);

		glScissor(WIDTH - offsetX, 0, offsetX, offsetY);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		glUseProgram(m_quadArrayShadowShader->m_program);
		m_quadArrayShadowShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		m_quadArrayShadowShader->loadInt("u_layer", m_debugCount);
		m_shadowQuad->render(m_lightDepthFramebuffer.getDepthTexture(), true);
		glUseProgram(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_SCISSOR_TEST);
	}	
}

void Game::renderOffscreen() {
	glEnable(GL_CLIP_DISTANCE0);

	m_water.bindReflectionBuffer();
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(Globals::shaderManager.getAssetPointer("terrain")->m_program);
	Globals::shaderManager.getAssetPointer("terrain")->loadVector("u_plane", Vector4f(0.0f, 1.0f, 0.0f, -m_water.getWaterLevel()));
	glUseProgram(0);
	m_skybox.render(m_camera);
	glEnable(GL_CULL_FACE);
	m_camera.pitchReflection((m_water.getWaterLevel()));
	m_terrain.drawNormal(m_camera);
	m_camera.pitchReflection(-m_water.getWaterLevel());
	glDisable(GL_CULL_FACE);
	Framebuffer::Unbind();

	m_water.bindRefractionBuffer();
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(Globals::shaderManager.getAssetPointer("terrain")->m_program);
	Globals::shaderManager.getAssetPointer("terrain")->loadVector("u_plane", Vector4f(0.0f, -1.0f, 0.0f, m_water.getWaterLevel()));
	glUseProgram(0);
	m_terrain.drawNormal(m_camera);
	Framebuffer::Unbind();

	glDisable(GL_CLIP_DISTANCE0);
	
}

void Game::shadowPass() {
	m_lightDepthFramebuffer.bind();
	glClear(GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("depthGS");
	glUseProgram(shader->m_program);
	shader->loadMatrixArray("u_projectionShadows", m_camera.lightProjections, m_camera.m_numberCascades);
	shader->loadMatrixArray("u_viewShadows", m_camera.lightViews, m_camera.m_numberCascades);

	for (unsigned short j = 0; j < m_camera.m_numberCascades; j++) {
		shader->loadInt("u_numCascades", j);

		shader->loadMatrix("u_model", Matrix4f::IDENTITY);
		for (auto entitie : m_entities) {
			entitie->drawShadow(m_camera);
		}

		shader->loadMatrix("u_model", m_tree->getTransformationMatrix());
		m_tree->drawShadow(m_camera);

		for (auto entitie : m_fernEntities) {
			shader->loadMatrix("u_model", entitie->getTransformationMatrix());
			entitie->drawShadow(m_camera);
		}

		shader->loadMatrix("u_model", m_barrel->getTransformationMatrix());
		m_barrel->drawShadow(m_camera);

	}
	glUseProgram(0);

	Framebuffer::Unbind();
}

void Game::mousePickPass() {
	m_mousePicker.getBuffer().bindWrite();

	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//m_barrel->drawAABB(m_camera);
	m_barrel->drawHull(m_camera);
	/*for (auto entitie : m_fernEntities) {
		entitie->drawAABB(m_camera);
	}*/

	for (auto entitie : m_fernEntities) {
		entitie->drawHull(m_camera);
	}

	m_tree->drawHull(m_camera);

	Framebuffer::UnbindWrite();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_mousePicker.updateObjectId(event.x, event.y);
	m_mousePicker.updatePosition(event.x, event.y, m_camera);
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