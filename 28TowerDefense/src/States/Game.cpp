#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"
#include "TileSet.h"

float Game::TileSize = 64.0f;

Game::Game(StateMachine& machine) : State(machine, States::GAME), m_level(m_camera, static_cast<int>(static_cast<float>(Application::Width) / TileSize), static_cast<int>(static_cast<float>(Application::Height) / TileSize)),
									spawnTimer(0.25f), roundTimer(5.0f) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	TileSetManager::Get().getTileSet("sprites").loadTileSetCpu({
		"res/images/Tile Wall.bmp",
		"res/images/Tile Empty.bmp",
		"res/images/Tile Arrow Down.bmp",
		"res/images/Tile Arrow Down Right.bmp",
		"res/images/Tile Arrow Right.bmp",
		"res/images/Tile Arrow Up Right.bmp",
		"res/images/Tile Arrow Up.bmp",
		"res/images/Tile Arrow Up Left.bmp",
		"res/images/Tile Arrow Left.bmp",
		"res/images/Tile Arrow Down Left.bmp",
		"res/images/Tile Target.bmp",	
		"res/images/Tile Enemy Spawner.bmp",
		"res/images/Empty.bmp",
		"res/images/Unit.bmp",
		"res/images/Turret.bmp",
		"res/images/Turret Shadow.bmp",
		"res/images/Projectile.bmp" });


	TileSetManager::Get().getTileSet("sprites").loadTileSetGpu();
	m_sprites = TileSetManager::Get().getTileSet("sprites").getAtlas();
	m_level.init(std::vector<TextureRect> (TileSetManager::Get().getTileSet("sprites").getTextureRects().begin(), TileSetManager::Get().getTileSet("sprites").getTextureRects().begin() + 13 ));
	Unit::Init(TileSetManager::Get().getTileSet("sprites").getTextureRects()[13]);

	Turret::Init(std::vector<TextureRect>(TileSetManager::Get().getTileSet("sprites").getTextureRects().begin() + 14, TileSetManager::Get().getTileSet("sprites").getTextureRects().begin() + 16));
	Projectile::Init(TileSetManager::Get().getTileSet("sprites").getTextureRects()[16]);

	Spritesheet::Bind(m_sprites);

	std::srand(std::time(NULL));
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {

}

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

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}

		if (move) {
			m_camera.move(directrion * m_dt);
		}
	}

	Vector2f posMouse((float)m_mouseX / TileSize, (float)(Application::Height - m_mouseY) / TileSize);

	if (m_mouseDownLeft || m_mouseDownRight) {
		if (m_mouseDownRight) {
			m_level.setTileWall((int)posMouse[0], (int)posMouse[1], false);
			removeTurretsAtMousePosition(posMouse);
		}

		if (m_mouseDownLeft) {
			if (keyboard.keyDown(Keyboard::KEY_CTRL)) {
				addTurret(posMouse);
				m_mouseDownLeft = false;
			}else {
				m_level.setTileWall((int)posMouse[0], (int)posMouse[1], true);
			}
			
		}
	}

	//Update the units.
	updateUnits(m_dt);

	//Update the turrets.
	for (auto& turretSelected : listTurrets)
		turretSelected.update(m_dt, listUnits, listProjectiles);

	//Update the projectiles.
	updateProjectiles(m_dt);

	updateSpawnUnitsIfRequired(m_dt);
}

void Game::updateUnits(float dt) {
	//Loop through the list of units and update all of them.
	auto it = listUnits.begin();
	while (it != listUnits.end()) {
		bool increment = true;

		if ((*it) != nullptr) {
			(*it)->update(dt, m_level, listUnits);

			//Check if the unit is still alive.  If not then erase it and don't increment the iterator.
			if ((*it)->getIsAlive() == false) {
				it = listUnits.erase(it);
				increment = false;
			}
		}

		if (increment)
			it++;
	}
}

void Game::updateProjectiles(float dt) {
	//Loop through the list of projectiles and update all of them.
	auto it = listProjectiles.begin();
	while (it != listProjectiles.end()) {
		(*it).update(dt);

		//Check if the projectile has collided or not, erase it if needed, and update the iterator.
		if ((*it).getCollisionOccurred())
			it = listProjectiles.erase(it);
		else
			it++;
	}
}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_level.draw(TileSize);

	for (auto& unitSelected : listUnits)
		if (unitSelected != nullptr)
			unitSelected->drawBatched(TileSize);

	//Draw the turrets.
	for (auto& turretSelected : listTurrets)
		turretSelected.drawBatched(TileSize);

	//Draw the projectiles.
	for (auto& projectileSelected : listProjectiles)
		projectileSelected.drawBatched(TileSize);

	Batchrenderer::Get().drawBuffer();

	//if (m_drawUi)
		//renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {	
	m_mouseX = event.x;
	m_mouseY = event.y;
} 

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_mouseDownLeft = event.button == 1u;
	m_mouseDownRight = event.button == 2u;
	m_mouseX = event.x;
	m_mouseY = event.y;
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_mouseDownLeft = false;
	m_mouseDownRight = false;
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {
	
}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {

}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
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
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::addUnit(const Vector2f& posMouse) {
	listUnits.push_back(std::make_shared<Unit>(posMouse));
}

void Game::addTurret(const Vector2f& posMouse) {
	Vector2f pos((int)posMouse[0] + 0.5f, (int)posMouse[1] + 0.5f);
	listTurrets.push_back(Turret(pos));
}

void Game::removeTurretsAtMousePosition(const Vector2f& posMouse) {
	for (auto it = listTurrets.begin(); it != listTurrets.end();) {
		if ((*it).checkIfOnTile((int)posMouse[0], (int)posMouse[1]))
			it = listTurrets.erase(it);
		else
			it++;
	}
}

void Game::updateSpawnUnitsIfRequired(float dt) {
	spawnTimer.countDown(dt);

	//Check if the round needs to start.
	if (listUnits.empty() && spawnUnitCount == 0) {
		roundTimer.countDown(dt);
		if (roundTimer.timeSIsZero()) {
			spawnUnitCount = 15;
			roundTimer.resetToMax();
		}
	}

	//Add a unit if needed.
	if (spawnUnitCount > 0 && spawnTimer.timeSIsZero()) {
		addUnit(m_level.getRandomEnemySpawnerLocation());
		spawnUnitCount--;
		spawnTimer.resetToMax();
	}
}