#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"
#include "Constants.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), m_keySet(m_sphere.getPosition()), m_raySet(), m_respawnPointSet(m_sphere.getPosition()), m_columnSet(m_sphere.getPosition()) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow());

	Init(1);

	m_pos = Vector3f((TERRAIN_SIZE * SCALE) / 2, (Terrain.GetHeight((TERRAIN_SIZE * SCALE) / 2, (TERRAIN_SIZE * SCALE) / 2) + RADIUS) * SCALE, (TERRAIN_SIZE * SCALE) / 2);
	
	m_camera = ThirdPersonCamera();
	m_camera.perspective(45.0f, (float)Application::Width / (float)Application::Height, 0.1f, 5000.0f);
	m_camera.lookAt(m_pos - Vector3f(0.0f, 0.0f, m_offsetDistance), m_pos, Vector3f(0.0f, 1.0f, 0.0f));

	std::vector<btCollisionShape*> terrainShape = Physics::CreateStaticCollisionShapes(&Terrain, SCALE);
	btRigidBody* body = Globals::physics->addStaticModel(terrainShape, Physics::BtTransform(), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::TERRAIN, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA);

	//create dynamic character
	btSphereShape* playerShape = new btSphereShape(0.5f * SCALE);
	btTransform playerTransform;
	playerTransform.setIdentity();
	playerTransform.setOrigin(btVector3(btScalar(m_pos[0]), btScalar(m_pos[1]), btScalar(m_pos[2])));
	btVector3 localInertiaChar(0, 0, 0);
	playerShape->calculateLocalInertia(100.0f, localInertiaChar);


	btDefaultMotionState* playerMotionState = new btDefaultMotionState(playerTransform);
	btRigidBody::btRigidBodyConstructionInfo cInfoChar(100.0f, playerMotionState, playerShape, localInertiaChar);

	m_characterController = new CharacterController();
	m_characterController->create(new btRigidBody(cInfoChar), Physics::GetDynamicsWorld(), Physics::collisiontypes::CHARACTER, Physics::collisiontypes::TERRAIN);
	
	m_characterController->setSlopeAngle(60.0f);
	m_characterController->setJumpDistanceOffset(RADIUS + 0.1f);
	m_characterController->setOnGroundDistanceOffset(RADIUS + 0.01f);

	m_characterController->setAngularFactor(btVector3(1.0f, 0.0f, 1.0f));
	m_characterController->setSleepingThresholds(0.0f, 0.0f);
	m_characterController->setRollingFriction(1.0f);
	m_characterController->setDamping(0.0f, 0.7f);
	m_characterController->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
	m_characterController->setGravity(btVector3(0.0f, -9.81f * 3.0f, 0.0f));

	m_raySet.init();
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {
	m_characterController->preStep();
	Globals::physics->stepSimulation(m_fdt);
	m_characterController->postStep();
}

void Game::update() {
	
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

	Vector3f direction = Vector3f();
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
		direction += Vector3f(-0.5f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(0.5f, 0.0f, 0.0f);
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

	if (mouse.buttonDown(Mouse::BUTTON_RIGHT)) {
		m_characterController->setLinearVelocityXZ(btVector3(0.0f, 0.0f, 0.0f));
		m_characterController->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
	}else if(move){
		direction = m_camera.getViewSpaceDirection(direction);
		m_characterController->setLinearVelocityXZ(Physics::VectorFrom(direction * Vector3f(15.0f, 1.0f, 15.0f)));
	}

	if (keyboard.keyDown(Keyboard::KEY_LALT)) {
		m_characterController->jump(btVector3(0.0f, 1.0f, 0.0f), 20.0f);
	}
	
	btTransform t;
	m_characterController->getWorldTransform(t);
	m_playerPos = Physics::VectorFrom(t.getOrigin());
	m_useThirdCamera ? m_camera.setTarget(m_playerPos) : m_camera.Camera::setTarget(m_playerPos);

	float dx = 0.0f;
	float dy = 0.0f;
	dx = mouse.xPosRelative();
	dy = mouse.yPosRelative();


	if (dx || dy) {
		m_useThirdCamera ? m_camera.rotate(dx, dy) : m_camera.rotate(dx, dy, m_playerPos);		
	}

	btVector3 cameraPosition = Physics::VectorFrom(m_camera.getPosition());

	cameraPosition.setInterpolate3(t.getOrigin(), cameraPosition, Physics::SweepSphere(t.getOrigin(), cameraPosition, 0.2f, Physics::collisiontypes::CAMERA, Physics::collisiontypes::TERRAIN));
	m_camera.setPosition(Physics::VectorFrom(cameraPosition));
	
	m_sphere.setPosition(m_playerPos);
	m_sphere.setOrientation(Physics::QuaternionFrom(t.getRotation()));

	if (m_useThirdCamera)
		m_camera.update(m_dt);

	//comprueba si el player muere
	if (m_playerPos[1] <= Lava.GetHeight() + RADIUS){
		Player.SetY(Lava.GetHeight() + RADIUS);
		Player.SetVel(0.0f, 0.0f, 0.0f);
		pickedkey_id = -1;
		state = STATE_LIVELOSS;
		//Sound.Play(SOUND_SWISH);
	}


	if (pickedkey_id == -1){
		for (unsigned int i = 0; i < m_keySet.getKeyStates().size(); i++){

			if (!m_keySet.isDeployed(i)){
				if ((m_playerPos - m_keySet.getPosition(i)).length() <= RADIUS * 2){
					pickedkey_id = i;					
					m_keySet.setPickedKeyId(pickedkey_id);
					break;
					//Sound.Play(SOUND_PICKUP);
				}
			}
		}
	}else {
		if (m_columnSet.insideGatheringArea(pickedkey_id)) {
			//Sound.Play(SOUND_UNLOCK);
			//Sound.Play(SOUND_ENERGYFLOW);

			const Vector3f& columnPos = m_columnSet.getPosition(pickedkey_id);
			m_keySet.deploy(pickedkey_id, m_columnSet.getHole(pickedkey_id), m_columnSet.getYaw(pickedkey_id));
			m_raySet.deploy(Vector3f(columnPos[0], columnPos[1] + COLUMN_HEIGHT + ENERGY_BALL_RADIUS, columnPos[2]), m_portal.getReceptor(pickedkey_id), pickedkey_id, m_keySet.getNumDeployed());
			
			pickedkey_id = -1;
			if (respawn_id) {
				//Sound.Play(SOUND_SWISH);
				respawn_id = 0;
			}
			portal_activated = m_keySet.getNumDeployed() == 5;
			m_portal.setDisabled(!portal_activated);
			//if (portal_activated) Sound.Play(SOUND_WARP);
		}
	}


	m_portal.update(m_dt);
	m_keySet.update(m_dt);
	m_raySet.update(m_dt);
	m_respawnPointSet.update(m_dt);
}

void Game::render() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getPerspectiveMatrix()[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getViewMatrix()[0][0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_skybox.draw(m_camera);
	m_lava.draw(m_camera);

	auto shader = Globals::shaderManager.getAssetPointer("terrain_new");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::Scale(SCALE, SCALE, SCALE));
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix()));
	
	shader->loadVector("lightPos", Vector3f(50.0f, 50.0f, 50.0f));
	shader->loadVector("lightAmbient", Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	shader->loadVector("lightDiffuse", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	shader->loadVector("lightSpecular", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	shader->loadVector("matAmbient", Vector4f(0.7f, 0.7f, 0.7f, 1.0f));
	shader->loadVector("matDiffuse", Vector4f(0.8f, 0.8f, 0.8f, 1.0f));
	shader->loadVector("matSpecular", Vector4f(0.3f, 0.3f, 0.3f, 1.0f));
	shader->loadVector("matEmission", Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	shader->loadFloat("matShininess", 100.0f);

	shader->loadInt("tex_top", 0);
	shader->loadInt("tex_side", 1);
	shader->loadFloat("height", Lava.GetHeight());
	shader->loadFloat("hmax", Lava.GetHeightMax());

	Globals::textureManager.get("grass").bind(0);
	Globals::textureManager.get("rock").bind(1);
	Terrain.DrawNew();
	shader->unuse();

	m_keySet.draw(m_camera);
	m_raySet.draw(m_camera);
	m_columnSet.draw(m_camera);
	
	if (abs(m_camera.getPositionZ() - m_portal.getZ()) < m_camera.getOffsetDistance()){
		//draw player
		m_sphere.draw(m_camera);

		//draw portal
		m_portal.draw(m_camera);
	}else{
		//draw portal
		m_portal.draw(m_camera);

		//draw player
		m_sphere.draw(m_camera);
	}

	m_respawnPointSet.draw(m_camera);
	
	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {

	if (event.direction == 1u) {
		m_offsetDistance += 2.0f;
		m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
		m_camera.setOffsetDistance(m_offsetDistance);
	}

	if (event.direction == 0u) {
		m_offsetDistance -= 2.0f;
		m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
		m_camera.setOffsetDistance(m_offsetDistance);
	}
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		m_drawUi = false;
		Mouse::instance().attach(Application::GetWindow());
		Keyboard::instance().enable();
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {

}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_SPACE) {
		m_drawUi = true;
		Mouse::instance().detach();
		Keyboard::instance().disable();
	}
}

void Game::resize(int deltaW, int deltaH) {

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
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());

	if (ImGui::SliderFloat("Camera Offset", &m_offsetDistance, 0.0f, 150.0f)) {
		m_camera.setOffsetDistance(m_offsetDistance);
	}

	if (ImGui::Checkbox("Enable Springsystem", &m_enableSpring)) {
		m_camera.enableSpringSystem(m_enableSpring);
	}

	ImGui::Checkbox("Third", &m_useThirdCamera);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool Game::Init(int lvl) {

	bool res = true;
	portal_activated = false;
	level = lvl;
	state = STATE_RUN;
	respawn_id = 0;
	pickedkey_id = -1;

	//Graphics initialization
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)Application::Width / (float)Application::Height, CAMERA_ZNEAR, CAMERA_ZFAR);
	glMatrixMode(GL_MODELVIEW);

	const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_position[] = { 50.0f, 50.0f, 50.0f, 0.0f };
	const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	const GLfloat mat_specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	const GLfloat mat_shininess[] = { 100.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	//Texture initialization
	Data.Load();

	//level initialization(terrain, lava and skybox)
	Scene.LoadLevel(level, &Terrain, &Lava, CAMERA_ZFAR);
	Terrain.createAttribute();
	//Sound initialization
	//Sound.Load();

	//Shader initialization
	Shader.Load();

	//Model initialization
	Model.Load();

	//Player initialization
	Player.SetPos(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2) + RADIUS, TERRAIN_SIZE / 2);
	
	//Sound.Play(SOUND_AMBIENT);
	
	
	m_sphere = RenderableObject("sphere", "texture_new", "player");
	m_sphere.setDrawFunction([&](const Camera& camera) {
		if (m_sphere.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(m_sphere.getShader());
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		shader->loadMatrix("u_model", m_sphere.getTransformationOP());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix() * m_sphere.getTransformationOP()));
		Globals::textureManager.get(m_sphere.getTexture()).bind(0);
		Globals::shapeManager.get(m_sphere.getShape()).drawRaw();
		shader->unuse();
	});

	m_lava = RenderableObject("quad_lava", "texture_new", "lava");
	m_lava.setDrawFunction([&](const Camera& camera) {
		if (m_lava.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(m_lava.getShader());
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		shader->loadMatrix("u_model", m_lava.getTransformationP());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix() * m_lava.getTransformationP()));
		Globals::textureManager.get(m_lava.getTexture()).bind(0);
		Globals::shapeManager.get(m_lava.getShape()).drawRaw();
		shader->unuse();
	});

	m_lava.setPosition(0.0f, 2.5f, 0.0f);

	m_keySet.init(Terrain);
	m_respawnPointSet.init(Terrain);
	m_columnSet.init(Terrain);
	m_portal.init(Terrain);

	m_skybox = RenderableObject("cube", "skybox", "skybox");
	m_skybox.setScale(750.0f);
	m_skybox.setDrawFunction([&](const Camera& camera) {
		if (m_skybox.isDisabled()) return;

		glDisable(GL_DEPTH_TEST);
		glFrontFace(GL_CW);

		auto shader = Globals::shaderManager.getAssetPointer(m_skybox.getShader());
		Matrix4f view = camera.getViewMatrix();
		view[3][0] = 0.0f; view[3][1] = 0.0f; view[3][2] = 0.0f;
		shader->use();
		shader->loadMatrix("projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("view", view);
		shader->loadMatrix("model", m_skybox.getTransformationSO());
		shader->loadVector("lightPos", Vector3f(0.0f, 0.0f, 0.0f));
		shader->loadVector("viewPos", camera.getPosition());
		shader->loadInt("cubemap", 0);

		Globals::textureManager.get(m_skybox.getTexture()).bind(0);
		Globals::shapeManager.get(m_skybox.getShape()).drawRaw();

		Texture::Unbind(GL_TEXTURE_CUBE_MAP);

		shader->unuse();

		glFrontFace(GL_CCW);
		glEnable(GL_DEPTH_TEST);
	});

	return res;
}