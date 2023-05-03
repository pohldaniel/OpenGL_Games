#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"


Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {

}

void Game::update() {

}

void Game::render() {
	
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {

}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {

}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	
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

	return res;
}