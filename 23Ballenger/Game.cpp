#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"
#include "Constants.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	Init(1);

	m_pos = Vector3f((TERRAIN_SIZE * SCALE) / 2, (Terrain.GetHeight((TERRAIN_SIZE * SCALE) / 2, (TERRAIN_SIZE * SCALE) / 2) + RADIUS) * SCALE, (TERRAIN_SIZE * SCALE) / 2);
	
	m_camera = ThirdPersonCamera();
	m_camera.perspective(45.0f, (float)Application::Width / (float)Application::Height, 0.1f, 1000.0f);
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
}

void Game::render() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getPerspectiveMatrix()[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getViewMatrix()[0][0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*auto shader = Globals::shaderManager.getAssetPointer("terrain");
	shader->use();
	shader->loadInt("tex_top", 0);
	shader->loadInt("tex_side", 1);
	shader->loadFloat("height", Lava.GetHeight());
	shader->loadFloat("hmax", Lava.GetHeightMax());

	glEnable(GL_TEXTURE_2D);
	Globals::textureManager.get("grass").bind(0);
	Globals::textureManager.get("rock").bind(1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	Terrain.Draw();

	shader->unuse();*/

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
	glDisable(GL_TEXTURE_2D);

	Terrain.DrawNew();

	shader->unuse();

	m_sphere.draw(m_camera);

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
	key.SetPos(883, Terrain.GetHeight(883, 141), 141);
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
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix() * m_sphere.getTransformationP()));
		Globals::textureManager.get(m_sphere.getTexture()).bind(0);
		Globals::shapeManager.get(m_sphere.getShape()).drawRaw();
		shader->unuse();
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