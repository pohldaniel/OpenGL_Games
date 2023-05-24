#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"
#include "Constants.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) , m_key(m_sphere.getPosition()) {
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

	lineColors = { Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Vector4f(1.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.2f, 0.2f, 1.0f, 1.0f) , Vector4f(1.0f, 0.0f, 1.0f, 1.0f) };
	
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

	Coord P; P.x = m_playerPos[0]; P.y = m_playerPos[1]; P.z = m_playerPos[2];
	float r = RADIUS;

	//comprueba si el player entra en algun Respawn Point
	float cr = CIRCLE_RADIUS, ah = AURA_HEIGHT;
	for (unsigned int i = 0; i<respawn_points.size(); i++){
		Coord RP; RP.x = respawn_points[i].GetX(); RP.y = respawn_points[i].GetY(); RP.z = respawn_points[i].GetZ();
		if (sqrt((P.x - RP.x)*(P.x - RP.x) + (P.y - RP.y)*(P.y - RP.y) + (P.z - RP.z)*(P.z - RP.z)) <= RADIUS + CIRCLE_RADIUS){
			//if (respawn_id != i) Sound.Play(SOUND_SWISH);
			respawn_id = i;
		}
	}

	m_colors.clear();
	m_activate.clear();

	//1. sizeof(bool) = 1 --> we have a padwidth of 16 to match the std 140 layout
	//2. I highly recomand to use the fourth componente of color and set the 0.6 inside the shader insteed of passing a second bool array
	for (unsigned int i = 0; i<respawn_points.size(); i++) {
		m_colors.push_back(i == respawn_id ? Vector4f(1.0f, 0.4f, 0.0f, 0.6f) : Vector4f(0.5f, 0.5f, 1.0f, 0.6f));
		m_activate.push_back({ i == respawn_id, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false });
	}

	if (pickedkey_id == -1){

		for (unsigned int i = 0; i<target_keys.size(); i++){

			if (!target_keys[i].IsDeployed()){

				Coord K; K.x = target_keys[i].GetX(); K.y = target_keys[i].GetY(); K.z = target_keys[i].GetZ();
				if (sqrt((P.x - K.x)*(P.x - K.x) + (P.y - K.y)*(P.y - K.y) + (P.z - K.z)*(P.z - K.z)) <= RADIUS * 2){
					pickedkey_id = i;
					
					target_keys.erase(target_keys.begin() + pickedkey_id);
					
					m_key.setPickedKeyId(pickedkey_id);

					//Sound.Play(SOUND_PICKUP);
				}
			}
		}
	}else {
		if (columns[pickedkey_id].InsideGatheringArea(P.x, P.y, P.z)) {
			//Sound.Play(SOUND_UNLOCK);
			//Sound.Play(SOUND_ENERGYFLOW);
			//target_keys[pickedkey_id].Deploy();
			target_keys[pickedkey_id].Deploy();
			m_key.deploy(pickedkey_id, Vector3f(columns[pickedkey_id].GetHoleX(), columns[pickedkey_id].GetHoleY(), columns[pickedkey_id].GetHoleZ()), columns[pickedkey_id].GetYaw());
			

			float r = ENERGY_BALL_RADIUS / 2.0f;
			int numrays = 6;

			for (int j = 0; j<numrays; j++) {
				float ang_rad = (ang + j*(360 / numrays))*(PI / 180);
				m_line.addToBuffer(Vector3f(columns[pickedkey_id].GetX() + cos(ang_rad)*r, columns[pickedkey_id].GetY() + COLUMN_HEIGHT + ENERGY_BALL_RADIUS + sin(ang_rad)*r, columns[pickedkey_id].GetZ()), Vector3f(Portal.GetReceptorX(pickedkey_id), Portal.GetReceptorY(pickedkey_id), Portal.GetZ()));
				colors.push_back(lineColors[pickedkey_id]);
				colors.push_back(lineColors[pickedkey_id]);
			}
			m_line.addVec4Attribute(colors);

			pickedkey_id = -1;
			if (respawn_id) {
				//Sound.Play(SOUND_SWISH);
				respawn_id = 0;
			}
			bool all_keys_deployed = true;
			for (unsigned int i = 0; all_keys_deployed && i < target_keys.size(); i++) all_keys_deployed = target_keys[i].IsDeployed();
			portal_activated = all_keys_deployed;
			//if (portal_activated) Sound.Play(SOUND_WARP);
		}
	}

	glBindBuffer(GL_UNIFORM_BUFFER, Globals::colorUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 128, &m_colors[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, Globals::activateUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 128, &m_activate[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_vortex.update(m_dt);
	m_key.update(m_dt, 1.0f);
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

	m_key.draw(m_camera);

	shader = Globals::shaderManager.getAssetPointer("line");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	m_line.drawRaw();
	shader->unuse();

	
	m_column.draw(m_camera);
	m_energyBallCl.draw(m_camera);

	if (abs(m_camera.getPositionZ() - Portal.GetZ()) < m_camera.getOffsetDistance()){
		//draw player
		m_sphere.draw(m_camera);

		//draw portal
		m_vortex.draw(m_camera);
		m_portal.draw(m_camera);
		m_energyBallP.draw(m_camera);
	}else{
		//draw portal
		m_vortex.draw(m_camera);
		m_portal.draw(m_camera);
		m_energyBallP.draw(m_camera);

		//draw player
		m_sphere.draw(m_camera);
	}

	glEnable(GL_BLEND);
	m_respawnPoint.draw(m_camera);

	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	m_cylinder.draw(m_camera);
	m_disk.draw(m_camera);

	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

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
	noclip = false;
	portal_activated = false;
	time = ang = 0.0f;
	noclipSpeedF = 1.0f;
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

	//target keys initialization
	cKey key;
	//key.SetPos(883, Terrain.GetHeight(883, 141), 141);
	key.SetPos(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2 + 10.0f), TERRAIN_SIZE / 2 + 10.0f);
	target_keys.push_back(key);
	key.SetPos(345, Terrain.GetHeight(345, 229), 229);
	target_keys.push_back(key);
	key.SetPos(268, Terrain.GetHeight(268, 860), 860);
	target_keys.push_back(key);
	key.SetPos(780, Terrain.GetHeight(780, 858), 858);
	target_keys.push_back(key);
	key.SetPos(265, Terrain.GetHeight(265, 487), 487);
	target_keys.push_back(key);

	//columns initialization
	cColumn col;
	col.SetColumn(TERRAIN_SIZE / 2 + 18, Terrain.GetHeight(TERRAIN_SIZE / 2 + 18, TERRAIN_SIZE / 2 + 8), TERRAIN_SIZE / 2 + 8, 90);
	columns.push_back(col);
	col.SetColumn(TERRAIN_SIZE / 2 + 14, Terrain.GetHeight(TERRAIN_SIZE / 2 + 14, TERRAIN_SIZE / 2 - 8), TERRAIN_SIZE / 2 - 8, 90);
	columns.push_back(col);
	col.SetColumn(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2 - 16), TERRAIN_SIZE / 2 - 16, 180);
	columns.push_back(col);
	col.SetColumn(TERRAIN_SIZE / 2 - 14, Terrain.GetHeight(TERRAIN_SIZE / 2 - 14, TERRAIN_SIZE / 2 - 8), TERRAIN_SIZE / 2 - 8, -90);
	columns.push_back(col);
	col.SetColumn(TERRAIN_SIZE / 2 - 18, Terrain.GetHeight(TERRAIN_SIZE / 2 - 18, TERRAIN_SIZE / 2 + 8), TERRAIN_SIZE / 2 + 8, -90);
	columns.push_back(col);

	//Player initialization
	Player.SetPos(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2) + RADIUS, TERRAIN_SIZE / 2);

	//Portal initialization
	Portal.SetPos(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2 + 32), TERRAIN_SIZE / 2 + 32);

	//respawn points initialization
	cRespawnPoint rp;
	rp.SetPos(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2), TERRAIN_SIZE / 2);
	respawn_points.push_back(rp);
	rp.SetPos(256, Terrain.GetHeight(256, 160), 160);
	respawn_points.push_back(rp);
	rp.SetPos(840, Terrain.GetHeight(840, 184), 184);
	respawn_points.push_back(rp);
	rp.SetPos(552, Terrain.GetHeight(552, 760), 760);
	respawn_points.push_back(rp);
	rp.SetPos(791, Terrain.GetHeight(791, 850), 850);
	respawn_points.push_back(rp);
	rp.SetPos(152, Terrain.GetHeight(152, 832), 832);
	respawn_points.push_back(rp);
	rp.SetPos(448, Terrain.GetHeight(448, 944), 944);
	respawn_points.push_back(rp);
	rp.SetPos(816, Terrain.GetHeight(816, 816), 816);
	respawn_points.push_back(rp);

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

	Globals::shapeManager.get("quad_rp").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2), TERRAIN_SIZE / 2) * Matrix4f::Scale(CIRCLE_RADIUS, CIRCLE_RADIUS, CIRCLE_RADIUS));
	Globals::shapeManager.get("quad_rp").addInstance(Matrix4f::Translate(256.0f, Terrain.GetHeight(256.0f, 160.0f), 160.0f) * Matrix4f::Scale(CIRCLE_RADIUS, CIRCLE_RADIUS, CIRCLE_RADIUS));
	Globals::shapeManager.get("quad_rp").addInstance(Matrix4f::Translate(840.0f, Terrain.GetHeight(840.0f, 184.0f), 184.0f) * Matrix4f::Scale(CIRCLE_RADIUS, CIRCLE_RADIUS, CIRCLE_RADIUS));
	Globals::shapeManager.get("quad_rp").addInstance(Matrix4f::Translate(552.0f, Terrain.GetHeight(552.0f, 760.0f), 760.0f) * Matrix4f::Scale(CIRCLE_RADIUS, CIRCLE_RADIUS, CIRCLE_RADIUS));
	Globals::shapeManager.get("quad_rp").addInstance(Matrix4f::Translate(791.0f, Terrain.GetHeight(791.0f, 850.0f), 850.0f) * Matrix4f::Scale(CIRCLE_RADIUS, CIRCLE_RADIUS, CIRCLE_RADIUS));
	Globals::shapeManager.get("quad_rp").addInstance(Matrix4f::Translate(152.0f, Terrain.GetHeight(152.0f, 832.0f), 832.0f) * Matrix4f::Scale(CIRCLE_RADIUS, CIRCLE_RADIUS, CIRCLE_RADIUS));
	Globals::shapeManager.get("quad_rp").addInstance(Matrix4f::Translate(448.0f, Terrain.GetHeight(448.0f, 944.0f), 944.0f) * Matrix4f::Scale(CIRCLE_RADIUS, CIRCLE_RADIUS, CIRCLE_RADIUS));
	Globals::shapeManager.get("quad_rp").addInstance(Matrix4f::Translate(816.0f, Terrain.GetHeight(816.0f, 816.0f), 816.0f) * Matrix4f::Scale(CIRCLE_RADIUS, CIRCLE_RADIUS, CIRCLE_RADIUS));

	m_respawnPoint = RenderableObject("quad_rp", "respawn", "circle");
	m_respawnPoint.setDrawFunction([&](const Camera& camera) {
		if (m_respawnPoint.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(m_respawnPoint.getShader());
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));
		Globals::spritesheetManager.getAssetPointer(m_respawnPoint.getTexture())->bind(0);
		Globals::shapeManager.get(m_respawnPoint.getShape()).drawRawInstanced();
		shader->unuse();
	});

	Globals::shapeManager.get("cylinder").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2), TERRAIN_SIZE / 2));
	Globals::shapeManager.get("cylinder").addInstance(Matrix4f::Translate(256.0f, Terrain.GetHeight(256.0f, 160.0f), 160.0f));
	Globals::shapeManager.get("cylinder").addInstance(Matrix4f::Translate(840.0f, Terrain.GetHeight(840.0f, 184.0f), 184.0f));
	Globals::shapeManager.get("cylinder").addInstance(Matrix4f::Translate(552.0f, Terrain.GetHeight(552.0f, 760.0f), 760.0f));
	Globals::shapeManager.get("cylinder").addInstance(Matrix4f::Translate(791.0f, Terrain.GetHeight(791.0f, 850.0f), 850.0f));
	Globals::shapeManager.get("cylinder").addInstance(Matrix4f::Translate(152.0f, Terrain.GetHeight(152.0f, 832.0f), 832.0f));
	Globals::shapeManager.get("cylinder").addInstance(Matrix4f::Translate(448.0f, Terrain.GetHeight(448.0f, 944.0f), 944.0f));
	Globals::shapeManager.get("cylinder").addInstance(Matrix4f::Translate(816.0f, Terrain.GetHeight(816.0f, 816.0f), 816.0f));

	m_cylinder = RenderableObject("cylinder", "cylinder", "null");
	m_cylinder.setDrawFunction([&](const Camera& camera) {
		if (m_cylinder.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(m_cylinder.getShader());
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));
		shader->loadFloat("hmax", 3.0f);
		Globals::textureManager.get(m_cylinder.getTexture()).bind(0);
		Globals::shapeManager.get(m_cylinder.getShape()).drawRawInstanced();
		shader->unuse();
	});

	Globals::shapeManager.get("disk").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2), TERRAIN_SIZE / 2));
	Globals::shapeManager.get("disk").addInstance(Matrix4f::Translate(256.0f, Terrain.GetHeight(256.0f, 160.0f), 160.0f));
	Globals::shapeManager.get("disk").addInstance(Matrix4f::Translate(840.0f, Terrain.GetHeight(840.0f, 184.0f), 184.0f));
	Globals::shapeManager.get("disk").addInstance(Matrix4f::Translate(552.0f, Terrain.GetHeight(552.0f, 760.0f), 760.0f));
	Globals::shapeManager.get("disk").addInstance(Matrix4f::Translate(791.0f, Terrain.GetHeight(791.0f, 850.0f), 850.0f));
	Globals::shapeManager.get("disk").addInstance(Matrix4f::Translate(152.0f, Terrain.GetHeight(152.0f, 832.0f), 832.0f));
	Globals::shapeManager.get("disk").addInstance(Matrix4f::Translate(448.0f, Terrain.GetHeight(448.0f, 944.0f), 944.0f));
	Globals::shapeManager.get("disk").addInstance(Matrix4f::Translate(816.0f, Terrain.GetHeight(816.0f, 816.0f), 816.0f));

	m_disk = RenderableObject("disk", "disk", "null");
	m_disk.setDrawFunction([&](const Camera& camera) {
		if (m_disk.isDisabled()) return;
		auto shader = Globals::shaderManager.getAssetPointer(m_disk.getShader());
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		Globals::shapeManager.get(m_disk.getShape()).drawRawInstanced();
		shader->unuse();

	});

	m_key.init(Terrain);

	Globals::shapeManager.get("column").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2 + 18.0f, Terrain.GetHeight(TERRAIN_SIZE / 2 + 18.0f, TERRAIN_SIZE / 2 + 8), TERRAIN_SIZE / 2 + 8) * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), 90.0f));
	Globals::shapeManager.get("column").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2 + 14.0f, Terrain.GetHeight(TERRAIN_SIZE / 2 + 14.0f, TERRAIN_SIZE / 2 - 8), TERRAIN_SIZE / 2 - 8)* Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), 90.0f));
	Globals::shapeManager.get("column").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2 - 16.0f), TERRAIN_SIZE / 2 - 16.0f)* Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f));
	Globals::shapeManager.get("column").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2 - 14.0f, Terrain.GetHeight(TERRAIN_SIZE / 2 - 14.0f, TERRAIN_SIZE / 2 - 8.0f), TERRAIN_SIZE / 2 - 8.0f)* Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), -90.0f));
	Globals::shapeManager.get("column").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2 - 18.0f, Terrain.GetHeight(TERRAIN_SIZE / 2 - 18.0f, TERRAIN_SIZE / 2 + 8.0f), TERRAIN_SIZE / 2 + 8.0f)* Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), -90.0f));
	Globals::shapeManager.get("column").addVec4Attribute({Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Vector4f(1.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.1f, 0.1f, 1.0f, 1.0f) , Vector4f(1.0f, 0.0f, 1.0f, 1.0f) }, 1);

	m_column = RenderableObject("column", "column", "null");
	m_column.setDrawFunction([&](const Camera& camera) {
		if (m_column.isDisabled()) return;
		auto shader = Globals::shaderManager.getAssetPointer(m_column.getShader());
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));

		shader->loadVector("u_lightPos", Vector3f(50.0f, 50.0f, 50.0f));

		shader->loadFloat("invRadius", 0.0f);
		shader->loadFloat("alpha", 1.0f);
		shader->loadInt("u_texture", 0);
		shader->loadInt("u_normalMap", 1);

		Globals::textureManager.get("column").bind(0);
		Globals::textureManager.get("column_nmp").bind(1);

		Globals::shapeManager.get(m_column.getShape()).drawRawInstanced();
		shader->unuse();
	});

	Globals::shapeManager.get("sphere_cl").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2 + 18.0f, Terrain.GetHeight(TERRAIN_SIZE / 2 + 18.0f, TERRAIN_SIZE / 2 + 8), TERRAIN_SIZE / 2 + 8));
	Globals::shapeManager.get("sphere_cl").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2 + 14.0f, Terrain.GetHeight(TERRAIN_SIZE / 2 + 14.0f, TERRAIN_SIZE / 2 - 8), TERRAIN_SIZE / 2 - 8));
	Globals::shapeManager.get("sphere_cl").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2 - 16.0f), TERRAIN_SIZE / 2 - 16.0f));
	Globals::shapeManager.get("sphere_cl").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2 - 14.0f, Terrain.GetHeight(TERRAIN_SIZE / 2 - 14.0f, TERRAIN_SIZE / 2 - 8.0f), TERRAIN_SIZE / 2 - 8.0f));
	Globals::shapeManager.get("sphere_cl").addInstance(Matrix4f::Translate(TERRAIN_SIZE / 2 - 18.0f, Terrain.GetHeight(TERRAIN_SIZE / 2 - 18.0f, TERRAIN_SIZE / 2 + 8.0f), TERRAIN_SIZE / 2 + 8.0f));
	Globals::shapeManager.get("sphere_cl").addVec4Attribute({ Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Vector4f(1.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.1f, 0.1f, 1.0f, 1.0f) , Vector4f(1.0f, 0.0f, 1.0f, 1.0f) }, 1);

	m_energyBallCl = RenderableObject("sphere_cl", "energy", "null");
	m_energyBallCl.setDrawFunction([&](const Camera& camera) {
		if (m_energyBallCl.isDisabled()) return;
		glEnable(GL_BLEND);
		auto shader = Globals::shaderManager.getAssetPointer(m_energyBallCl.getShader());
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));

		Globals::textureManager.get(m_energyBallCl.getTexture()).bind(0);
		Globals::shapeManager.get(m_energyBallCl.getShape()).drawRawInstanced();
		shader->unuse();
		glDisable(GL_BLEND);
	});


	m_portal = RenderableObject("portal", "portal", "null");

	m_portal.setDrawFunction([&](const Camera& camera) {
		if (m_portal.isDisabled()) return;
		auto shader = Globals::shaderManager.getAssetPointer(m_portal.getShader());
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		shader->loadMatrix("u_model", m_portal.getTransformationP());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix() * m_portal.getTransformationP()));

		shader->loadVector("u_lightPos", Vector3f(50.0f, 50.0f, 50.0f));

		shader->loadFloat("invRadius", 0.0f);
		shader->loadFloat("alpha", 1.0f);
		shader->loadInt("u_texture", 0);
		shader->loadInt("u_normalMap", 1);

		Globals::textureManager.get("portal").bind(0);
		Globals::textureManager.get("portal_nmp").bind(1);

		Globals::shapeManager.get(m_portal.getShape()).drawRaw();
		shader->unuse();
	});
	
	m_portal.setPosition(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2 + 32.0f), TERRAIN_SIZE / 2 + 32.0f);

	Globals::shapeManager.get("sphere_portal").addInstance(Matrix4f::Translate(0.0f,			    PORTAL_SIDE * 1.5f,  0.0f));
	Globals::shapeManager.get("sphere_portal").addInstance(Matrix4f::Translate( PORTAL_SIDE * 0.5f, PORTAL_SIDE * 1.0f,  0.0f));
	Globals::shapeManager.get("sphere_portal").addInstance(Matrix4f::Translate(-PORTAL_SIDE * 0.5f, PORTAL_SIDE * 1.0f,  0.0f));
	Globals::shapeManager.get("sphere_portal").addInstance(Matrix4f::Translate(-PORTAL_SIDE * 0.5f, PORTAL_SIDE * 0.33f, 0.0f));
	Globals::shapeManager.get("sphere_portal").addInstance(Matrix4f::Translate( PORTAL_SIDE * 0.5f, PORTAL_SIDE * 0.33f, 0.0f));
	Globals::shapeManager.get("sphere_portal").addVec4Attribute({ Vector4f(0.0f, 1.0f, 0.0f, 1.0f), Vector4f(1.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.2f, 0.2f, 1.0f, 1.0f) , Vector4f(1.0f, 0.0f, 1.0f, 1.0f) , Vector4f(1.0f, 0.0f, 0.0f, 1.0f) }, 1);

	m_energyBallP = RenderableObject("sphere_portal", "energy", "null");
	m_energyBallP.setDrawFunction([&](const Camera& camera) {
		if (m_energyBallP.isDisabled()) return;
		glEnable(GL_BLEND);
		auto shader = Globals::shaderManager.getAssetPointer(m_energyBallP.getShader());
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));

		Globals::textureManager.get(m_energyBallP.getTexture()).bind(0);
		Globals::shapeManager.get(m_energyBallP.getShape()).drawRawInstanced();
		shader->unuse();
		glDisable(GL_BLEND);
	});

	m_vortex = RenderableObject("vortex", "texture_new", "vortex");
	m_vortex.setDrawFunction([&](const Camera& camera) {
		if (m_vortex.isDisabled()) return;
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		auto shader = Globals::shaderManager.getAssetPointer(m_vortex.getShader());
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		shader->loadMatrix("u_model", m_vortex.getTransformationSOP());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix() * m_vortex.getTransformationSOP()));

		Globals::textureManager.get(m_vortex.getTexture()).bind(0);
		Globals::shapeManager.get(m_vortex.getShape()).drawRaw();
		shader->unuse();
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
	});
	m_vortex.setScale(1.5f, 1.5f, 0.0f);
	m_vortex.setPosition(512.0f, 13.75f + PORTAL_SIDE * 0.5f, 544.0f);

	m_vortex.setUpdateFunction(
		[&](const float dt) {
		m_vortex.rotate(0.0f, 0.0f, 0.5f);
	});

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

	m_skybox.setUpdateFunction(
		[&](const float dt) {
		m_skybox.rotate(0.0f, 10.5f * PI_ON_180 * m_dt, 0.0f);
	});

	return res;
}

void Game::Physics(cBicho &object) {
	Coord initialPos; initialPos.x = object.GetX(); initialPos.y = object.GetY(); initialPos.z = object.GetZ();
	Coord center; center.x = object.GetX() + object.GetVX(); center.y = object.GetY() + object.GetVY(); center.z = object.GetZ() + object.GetVZ();
	std::vector<Vector> cnormals = Terrain.GetCollisionNormals(center, RADIUS);
	object.SetPos(center.x, center.y, center.z); //despues de GetCollisionNormals la posicion center sera una posicion valida sobre la superficie

												 //actualizo angulos de rotacion por movimiento
	if (object.GetZ() != initialPos.z || object.GetX() != initialPos.x) {
		float yaw, pitch;
		float dx = abs(abs(object.GetX()) - abs(initialPos.x)), dz = abs(abs(object.GetZ()) - abs(initialPos.z));
		if (object.GetZ() > initialPos.z && object.GetX() >= initialPos.x) yaw = atan(dx / dz); //primer cuadrante
		if (object.GetZ() <= initialPos.z && object.GetX() > initialPos.x) yaw = PI / 2 + atan(dz / dx); //segundo cuadrante
		if (object.GetZ() < initialPos.z && object.GetX() <= initialPos.x) yaw = PI + atan(dx / dz);//tercer cuadrante
		if (object.GetZ() >= initialPos.z && object.GetX() < initialPos.x) yaw = PI * 3 / 2 + atan(dz / dx);//cuarto cuadrante
		object.SetYaw(yaw*(180 / PI));

		float perimeter = PI * 2 * RADIUS;
		float dy = abs(abs(object.GetY()) - abs(initialPos.y));
		float travel_dist = sqrt(dx*dx + dy*dy + dz*dz);
		if (cos(yaw) >= 0.0f) pitch = object.GetPitch() + (travel_dist / perimeter) * 360.0f;
		else pitch = object.GetPitch() - (travel_dist / perimeter) * 360.0f;
		if (pitch < 0.0f) pitch = 360.0f - abs(pitch);
		object.SetPitch(fmod(pitch, 360.0f));
	}

	if (cnormals.empty()) object.SetVY(object.GetVY() - GRAVITY);
	else {
		Vector G, F, G1, F1, cNormal;
		float rz, rx; //angulos de rotacion
		float factor, N = 0.0f;

		G.x = 0.0f; G.y = -GRAVITY; G.z = 0.0f;
		F.x = object.GetVX(); F.y = object.GetVY(); F.z = object.GetVZ();
		cNormal.x = 0.0f; cNormal.y = 0.0f; cNormal.z = 0.0f;

		for (unsigned int i = 0; i<cnormals.size(); i++) {
			if (cnormals[i].x == 0.0f) rz = 0.0f;
			else if (cnormals[i].x >  0.0f) rz = -PI / 2 + atan(cnormals[i].y / cnormals[i].x);
			else rz = PI / 2 + atan(cnormals[i].y / cnormals[i].x);

			if (cnormals[i].z == 0.0f) rx = 0.0f;
			else if (cnormals[i].z >  0.0f) rx = PI / 2 - atan(cnormals[i].y / cnormals[i].z);
			else rx = -PI / 2 - atan(cnormals[i].y / cnormals[i].z);

			//Transformamos las fuerzas definidas en el sistema de coordenadas de OpenGL al sistema de coordenadas definido por cnormal(eje y) 
			G1.x = cos(-rz)*G.x - sin(-rz)*G.y;
			G1.y = cos(-rx)*sin(-rz)*G.x + cos(-rx)*cos(-rz)*G.y - sin(-rx)*G.z;
			G1.z = sin(-rx)*sin(-rz)*G.x + sin(-rx)*cos(-rz)*G.y + cos(-rx)*G.z;

			F1.x = cos(-rz)*F.x - sin(-rz)*F.y;
			F1.y = cos(-rx)*sin(-rz)*F.x + cos(-rx)*cos(-rz)*F.y - sin(-rx)*F.z;
			F1.z = sin(-rx)*sin(-rz)*F.x + sin(-rx)*cos(-rz)*F.y + cos(-rx)*F.z;

			//consideramos la fuerza normal para un unico triangulo
			float cN = 0.0f;
			if (G1.y < 0.0f) { cN -= G1.y; G1.y = 0.0f; }
			if (F1.y < 0.0f) { cN -= F1.y; F1.y = 0.0f; }
			N += cN; //actualizo la fuerza normal global

					 //actualizo la fuerza de cnormal global
			cNormal.x += cnormals[i].x;
			cNormal.y += cnormals[i].y;
			cNormal.z += cnormals[i].z;

			//consideramos la posible friccion
			if (cN > 0.0f && abs(F1.x) + abs(F1.z) > 0.0f) {
				factor = sqrt(((FRICTION*cN)*(FRICTION*cN)) / (F1.x*F1.x + F1.z*F1.z));

				if (abs(F1.x) < abs(F1.x*factor)) F1.x = 0.0f;
				else F1.x -= F1.x*factor;

				if (abs(F1.z) < abs(F1.z*factor)) F1.z = 0.0f;
				else F1.z -= F1.z*factor;
			}

			//volvemos a Transformar las fuerzas del sistema de coordenadas de cnormal(eje y) al sistema de coordenadas de OpenGL
			G.x = cos(rz)*G1.x - sin(rz)*cos(rx)*G1.y + sin(rz)*sin(rx)*G1.z;
			G.y = sin(rz)*G1.x + cos(rz)*cos(rx)*G1.y - cos(rz)*sin(rx)*G1.z;
			G.z = sin(rx)*G1.y + cos(rx)*G1.z;

			F.x = cos(rz)*F1.x - sin(rz)*cos(rx)*F1.y + sin(rz)*sin(rx)*F1.z;
			F.y = sin(rz)*F1.x + cos(rz)*cos(rx)*F1.y - cos(rz)*sin(rx)*F1.z;
			F.z = sin(rx)*F1.y + cos(rx)*F1.z;
		}

		float nextVX = F.x + G.x;
		float nextVY = F.y + G.y;
		float nextVZ = F.z + G.z;

		//limitaremos la velocidad para que la esfera no se salte triangulos
		float limitation_factor;
		if (sqrt(nextVX*nextVX + nextVY*nextVY + nextVZ*nextVZ) <= MAX_MOVEMENT) limitation_factor = 1.0f;
		else limitation_factor = sqrt((MAX_MOVEMENT*MAX_MOVEMENT) / (nextVX*nextVX + nextVY*nextVY + nextVZ*nextVZ));

		nextVX *= limitation_factor;
		nextVY *= limitation_factor;
		nextVZ *= limitation_factor;

		//consideramos el rebote
		if (N > GRAVITY * 4) factor = sqrt((N*N) / (cNormal.x*cNormal.x + cNormal.y*cNormal.y + cNormal.z*cNormal.z));
		else factor = 0.0f;

		nextVX += cNormal.x*factor*ELASTICITY;
		nextVY += cNormal.y*factor*ELASTICITY;
		nextVZ += cNormal.z*factor*ELASTICITY;

		float bounceForce = sqrt((cNormal.x*factor*ELASTICITY)*(cNormal.x*factor*ELASTICITY) + (cNormal.y*factor*ELASTICITY)*(cNormal.y*factor*ELASTICITY) + (cNormal.z*factor*ELASTICITY)*(cNormal.z*factor*ELASTICITY));
		//if (bounceForce >= PLAYER_JUMP_SPEED) Sound.PlayBounce(1.0f);
		//else if (bounceForce / PLAYER_JUMP_SPEED > 0.2f) Sound.PlayBounce(bounceForce / PLAYER_JUMP_SPEED);

		//actualizamos velocidad
		object.SetVel(nextVX, nextVY, nextVZ);
	}
}