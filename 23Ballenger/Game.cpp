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

	m_pos = Vector3f(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2) + RADIUS, TERRAIN_SIZE / 2);
	m_pos2 = Vector3f(TERRAIN_SIZE / 2, Terrain.GetHeight(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2) + RADIUS, TERRAIN_SIZE / 2);

	m_camera = Camera();
	m_camera.perspective(45.0, (float)Application::Width / (float)Application::Height, 1.0f, 1000.0f);
	m_camera.lookAt(m_pos - Vector3f(0.0f, 0.0f, m_offsetDistance), m_pos + Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	//m_camera.lookAt(m_pos, m_pos + Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {

}

void Game::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
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
		direction += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(1.0f, 0.0f, 0.0f);
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

	Mouse &mouse = Mouse::instance();

	dx = mouse.xPosRelative();
	dy = mouse.yPosRelative();
	
	if (move || dx != 0.0f || dy != 0.0f) {
		if (move) {
			m_pos += m_camera.getViewSpaceDirection(direction) * 20.0f * m_dt;	
			m_camera.moveRelative(direction * 20.0f * m_dt);
		}
		
		if (dx || dy) {
			m_camera.rotateSmoothly(dx, dy, 0.0f, m_pos, m_offsetDistance);
		}
	}
	m_sphere.setPosition(m_pos);

	if (mouse.wheelPos() < 0.0f) {
		m_offsetDistance += 2.0f;
		m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
		m_camera.setPosition(m_pos - m_camera.getViewDirection() * m_offsetDistance);
	}

	if (mouse.wheelPos() > 0.0f) {
		m_offsetDistance -= 2.0f;
		m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
		m_camera.setPosition(m_pos - m_camera.getViewDirection() * m_offsetDistance);
	}
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
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
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
		m_camera.setPosition(m_pos - m_camera.getViewDirection() * m_offsetDistance);
	}

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
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
		shader->loadMatrix("u_model", m_sphere.getTransformationP());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_sphere.getTransformationP()));
		Globals::textureManager.get(m_sphere.getTexture()).bind(0);
		Globals::shapeManager.get(m_sphere.getShape()).drawRaw();
		shader->unuse();
	});

	return res;
}