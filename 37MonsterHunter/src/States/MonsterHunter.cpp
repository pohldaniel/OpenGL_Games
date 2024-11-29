#include <numeric>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <engine/Batchrenderer.h>
#include <engine/Fontrenderer.h>
#include <engine/TileSet.h>
#include <States/Menu.h>
#include <States/Battle.h>

#include "MonsterHunter.h"
#include "Application.h"
#include "Globals.h"

MonsterHunter::MonsterHunter(StateMachine& machine) : State(machine, States::MONSTER_HUNTER), m_zone(m_camera), m_dialogTree(m_camera), m_indexOpen(false), m_evolveOpen(false), m_blockIndex(false), m_lastCharacter(nullptr) {

	m_viewWidth = 1280.0f;
	m_viewHeight= 720.0f;
	m_movingSpeed = 250.0f;
	m_screeBorder = 0.0f;

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(m_movingSpeed);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("font");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("dialog");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	m_zone.loadTileSetData("res/tilesets.json");
	m_zone.loadZone("res/tmx/data/maps/world.tmx", "world", "house");	
	m_zone.setDebugCollision(m_debugCollision);
	m_zone.getPlayer().setMovingSpeed(m_movingSpeed);
	m_zone.getPlayer().setViewWidth(m_viewWidth);
	m_zone.getPlayer().setViewHeight(m_viewHeight);
	m_zone.getPlayer().setMapHeight(m_zone.getMapHeight());
	m_zone.getPlayer().adjustCamera();
	m_mapHeight = m_zone.getMapHeight();

	m_monsterIndex.setViewWidth(m_viewWidth);
	m_monsterIndex.setViewHeight(m_viewHeight);
	m_monsterIndex.initUI(m_viewWidth, m_viewHeight);

	m_evolve.setViewWidth(m_viewWidth);
	m_evolve.setViewHeight(m_viewHeight);
	m_evolve.initUI(m_viewWidth, m_viewHeight);

	Sprite::Init(static_cast<unsigned int>(m_viewWidth), static_cast<unsigned int>(m_viewHeight));

	m_evolve.setOnEvolveEnd([this] {
		if (!m_evolveQueue.empty()) {
			const EvolveEntry& evolveEntry = m_evolveQueue.front();
			m_evolve.setCurrentMonster(evolveEntry.m_startMonster);
			m_evolve.setStartMonster(evolveEntry.m_startMonster);
			m_evolve.setEndMonster(evolveEntry.m_endMonster);
			m_evolve.setCurentMonsterIndex(evolveEntry.index);

			m_delayEvolve.setOnTimerEnd([&m_evolveOpen = m_evolveOpen, &m_evolve = m_evolve, &m_zone = m_zone] {
				m_zone.setAlpha(1.0f - 0.784f);
				m_evolveOpen = true;
				m_evolve.startEvolution();
			});
			m_delayEvolve.start(800u, false);
			m_zone.getPlayer().block();
			m_blockIndex = true;
			m_evolveQueue.pop();
		}else {
			
			m_blockIndex = false;
			m_evolveOpen = false;
			m_zone.setAlpha(1.0f);
			m_zone.getPlayer().unblock();
			m_dialogTree.setBlockInput(false);
			m_dialogTree.setFinished(true);
		}
	});

	TileSetManager::Get().getTileSet("bars").createBarRects(1024u, 256u, 200u, 5u);
	//TileSetManager::Get().getTileSet("bars").setLinear();
}

MonsterHunter::~MonsterHunter() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void MonsterHunter::fixedUpdate() {

}

void MonsterHunter::update() {
	
	if (m_zone.getBiomeIndex() > -1) {
		int biomeIndex = m_zone.getBiomeIndex();
		m_zone.getPlayer().block();
		m_zone.getFade().setOnFadeOut([this, biomeIndex = biomeIndex]() {
			m_machine.addStateAtTop(new Battle(m_machine));
			static_cast<Battle*>(m_machine.getStates().top())->setMapHeight(m_mapHeight);
			static_cast<Battle*>(m_machine.getStates().top())->setViewHeight(m_viewHeight);
			static_cast<Battle*>(m_machine.getStates().top())->setOpponentMonsters(m_zone.getBiomes()[biomeIndex].monsters, true);
			static_cast<Battle*>(m_machine.getStates().top())->setBiomeBackground(m_zone.getBiomes()[biomeIndex].biome);
			EventDispatcher::RemoveKeyboardListener(this);
			EventDispatcher::RemoveMouseListener(this);
		});
		m_zone.getFade().fadeOut();
	}

	Keyboard &keyboard = Keyboard::instance();

	Rect playerRect = { m_zone.getPlayer().getCell().posX + 32.0f, m_zone.getPlayer().getCell().posY - (128.0f - 30.0f) , 128.0f - 64.0f, 128.0f - 60.0f };
	for (const Transition& transition : m_zone.getTransitions()) {

		if (SpriteEntity::HasCollision(transition.collisionRect.posX, transition.collisionRect.posY, transition.collisionRect.posX + transition.collisionRect.width, transition.collisionRect.posY + transition.collisionRect.height, playerRect.posX, playerRect.posY, playerRect.posX + playerRect.width, playerRect.posY + playerRect.height)) {			
			m_zone.getPlayer().block();
			m_zone.getFade().setOnFadeIn([&m_zone = m_zone]() {
				m_zone.getPlayer().unblock();
			});

			m_zone.getFade().setOnFadeOut([&m_zone = m_zone, &transition = transition, &m_mapHeight = m_mapHeight, m_movingSpeed = m_movingSpeed, m_viewWidth = m_viewWidth, m_viewHeight = m_viewHeight]() {
				if(transition.target == "hospital2")
					m_zone.loadZone("res/tmx/data/maps/" + transition.target + ".tmx", "hospital", transition.newPos);
				else
					m_zone.loadZone("res/tmx/data/maps/" + transition.target + ".tmx", transition.target, transition.newPos);
				m_mapHeight = m_zone.getMapHeight();
				m_zone.getPlayer().block();				
				m_zone.getPlayer().setMovingSpeed(m_movingSpeed);
				m_zone.getPlayer().setViewWidth(m_viewWidth);
				m_zone.getPlayer().setViewHeight(m_viewHeight);
				m_zone.getPlayer().setMapHeight(m_zone.getMapHeight());
				m_zone.getPlayer().adjustCamera();

				m_zone.getFade().fadeIn();
			});
			m_zone.getFade().fadeOut();

			break;
		}	
	}

	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {

		for (Character& character : m_zone.getCharacters()) {
			if (SpriteEntity::CheckConnection(m_zone.getPlayer().getCell(), character.getCell(), m_zone.getPlayer().getLastViewDirection())) {
				m_zone.getPlayer().block();
				character.changeFacingDirection(m_zone.getPlayer());
				character.setRayCast(false);
				character.stopLookAroundTimer();
				Trainer& trainer = DialogTree::Trainers[character.getCharacterId()];

				if (m_dialogTree.isFinished()) {
					for (auto& dialog : !character.isDefeated() ? trainer.dialog.undefeated : trainer.dialog.defeated) {
						m_dialogTree.addDialog(character.getCell().posX, m_mapHeight - (character.getCell().posY - 128.0f), 0.0f, 0.0f, dialog);
					}
					m_dialogTree.setFinished(false);

					if (character.getCharacterId() == "Nurse") {
						m_dialogTree.setOnDialogFinished([&m_monsterIndex = m_monsterIndex, &m_zone = m_zone]() {
							m_zone.getPlayer().unblock();
							m_monsterIndex.resetStats();
						});
					}else {
						
						m_dialogTree.setOnDialogFinished([this, &character = character]() {
							if (!character.isDefeated()) {
								character.setDefeated(true);
								m_dialogTree.setBlockInput(true);
								m_dialogTree.setFinished(false);
								m_lastCharacter = &character;							
								m_zone.getFade().setOnFadeOut([this, &character = character]() {
									m_machine.addStateAtTop(new Battle(m_machine));
									static_cast<Battle*>(m_machine.getStates().top())->setMapHeight(m_mapHeight);
									static_cast<Battle*>(m_machine.getStates().top())->setViewHeight(m_viewHeight);
									static_cast<Battle*>(m_machine.getStates().top())->setOpponentMonsters(DialogTree::Trainers[character.getCharacterId()].monsters);
									static_cast<Battle*>(m_machine.getStates().top())->setBiomeBackground(DialogTree::Trainers[character.getCharacterId()].biome);
									EventDispatcher::RemoveKeyboardListener(this);
									EventDispatcher::RemoveMouseListener(this);
								});
								m_zone.getFade().fadeOut();
							}else {
								m_zone.getPlayer().unblock();
							}
						});
					}
				}
			}
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_ENTER) && MonsterIndex::Monsters.size() > 0) {
		if (m_blockIndex)
			return;
		m_indexOpen = !m_indexOpen;
		if (m_indexOpen) {
			m_zone.setAlpha(1.0f - 0.784f);
			m_zone.getPlayer().block();	
			m_monsterIndex.addAbilities();
			m_monsterIndex.addMonsters();
		}else {
			m_zone.setAlpha(1.0f);
			m_zone.getPlayer().unblock();
			m_monsterIndex.reset();
		}
	}

	if (m_indexOpen) {
		m_monsterIndex.processInput();
	}

	if (keyboard.keyPressed(Keyboard::KEY_E)) {
		checkForEvolution();
	}

	for (Character& character : m_zone.getCharacters()) {
		if (character.raycast(m_zone.getPlayer())) {
			character.setRayCast(false);
			character.stopLookAroundTimer();
			if(!m_zone.getPlayer().isBlocked())
				m_zone.getPlayer().changeFacingDirection(character);
			m_zone.getPlayer().block();
			m_zone.getPlayer().setIsNoticed(true);
			
			m_dialogTree.setBlockInput(true);
			m_dialogTree.setFinished(false);

			m_dialogTree.setOnDialogFinished([this, &character = character]() {	
				if (!character.isDefeated()) {
					character.setDefeated(true);
					m_dialogTree.setBlockInput(true);
					m_dialogTree.setFinished(false);
					m_zone.getFade().setOnFadeOut([this, &character = character]() {						
						m_machine.addStateAtTop(new Battle(m_machine));
						static_cast<Battle*>(m_machine.getStates().top())->setMapHeight(m_mapHeight);
						static_cast<Battle*>(m_machine.getStates().top())->setViewHeight(m_viewHeight);
						static_cast<Battle*>(m_machine.getStates().top())->setOpponentMonsters(DialogTree::Trainers[character.getCharacterId()].monsters);
						static_cast<Battle*>(m_machine.getStates().top())->setBiomeBackground(DialogTree::Trainers[character.getCharacterId()].biome);
						EventDispatcher::RemoveKeyboardListener(this);
						EventDispatcher::RemoveMouseListener(this);
					});
					m_zone.getFade().fadeOut();
				}
			});

			character.startMove({ m_zone.getPlayer().getCell().posX, m_zone.getPlayer().getCell().posY });
			character.setOnMoveEnd([&m_dialogTree = m_dialogTree, &m_trainers = DialogTree::Trainers, &character = character, &m_zone = m_zone, m_mapHeight = m_mapHeight]() {
				Trainer& trainer = m_trainers[character.getCharacterId()];
				for (auto& dialog : trainer.dialog.undefeated) {
					m_dialogTree.addDialog(character.getCell().posX, m_mapHeight - (character.getCell().posY - 128.0f), 0.0f, 0.0f, dialog, 0);
				}
				m_dialogTree.setBlockInput(false);
				m_zone.getPlayer().setIsNoticed(false);
			});
			m_lastCharacter = &character;
			break;
		}
	}

	m_dialogTree.processInput();
	m_delayEvolve.update(m_dt);
	for (auto&& spriteEntity : m_zone.getSpriteEntities()) {
		spriteEntity->update(m_dt);
	}
	m_zone.update(m_dt);
	if (m_indexOpen) {
		m_monsterIndex.update(m_dt);
	}
	m_evolve.update(m_dt);
}

void MonsterHunter::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_zone.draw();
	m_dialogTree.draw();

	if (m_indexOpen) {
		m_monsterIndex.draw();
	}

	if (m_evolveOpen) {
		m_evolve.draw();
	}

	if (m_drawUi)
		renderUi();
}

void MonsterHunter::OnReEnter(unsigned int prevState) {
	if (prevState == States::BATTLE) {
		EventDispatcher::AddKeyboardListener(this);
		EventDispatcher::AddMouseListener(this);
		m_zone.setAlpha(0.0f);

		bool _evolve = false;
		m_zone.getFade().setOnFadeIn([this, _evolve = _evolve]() {
			if (m_lastCharacter ) {
				m_zone.getPlayer().block();
				Trainer& trainer = DialogTree::Trainers[m_lastCharacter->getCharacterId()];
				for (auto& dialog : trainer.dialog.defeated) {
					m_dialogTree.addDialog(m_lastCharacter->getCell().posX, m_mapHeight - (m_lastCharacter->getCell().posY - 128.0f), 0.0f, 0.0f, dialog, 0);
				}
				
				m_dialogTree.setBlockInput(false);
				m_lastCharacter = nullptr;
				m_dialogTree.setOnDialogFinished([this, _evolve = _evolve]() {									
					m_zone.getPlayer().unblock();
					if (checkForEvolution()) {
						m_dialogTree.setBlockInput(true);
						m_dialogTree.setFinished(false);
					}
				});

			}else {
				m_zone.getPlayer().unblock();
				checkForEvolution();
			}

			});
		m_zone.getFade().fadeIn();
	}
}

void MonsterHunter::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void MonsterHunter::OnMouseWheel(Event::MouseWheelEvent& event) {
	
}

void MonsterHunter::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void MonsterHunter::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void MonsterHunter::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void MonsterHunter::OnKeyUp(Event::KeyboardEvent& event) {

}

void MonsterHunter::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);
	m_zone.resize();

	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("font");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("dialog");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();
}

void MonsterHunter::renderUi() {
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
	if(ImGui::Checkbox("Draw Center", &m_drawCenter)) {
		m_zone.setDrawCenter(m_drawCenter);
	}
		
	if(ImGui::Checkbox("Use Culling", &m_useCulling)) {
		m_zone.setUseCulling(m_useCulling);
	}

	if (ImGui::Checkbox("Draw Screen Border", &m_drawScreenBorder)) {
		m_zone.setDrawScreenBorder(m_drawScreenBorder);
	}

	if (ImGui::SliderFloat("Screen Border", &m_screeBorder, -5.0f, 450.0f)) {
		m_zone.setScreeBorder(m_screeBorder);
	}
	if (ImGui::Checkbox("Debug Collision", &m_debugCollision)) {
		m_zone.setDebugCollision(m_debugCollision);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool MonsterHunter::checkForEvolution() {

	for (int i = 0; i < MonsterIndex::Monsters.size(); i++) {
		if (std::get<1>(MonsterIndex::MonsterData[MonsterIndex::Monsters[i].name].evolve) && MonsterIndex::Monsters[i].level >= std::get<1>(MonsterIndex::MonsterData[MonsterIndex::Monsters[i].name].evolve)) {
			m_evolveQueue.push({ MonsterIndex::Monsters[i].name,  std::get<0>(MonsterIndex::MonsterData[MonsterIndex::Monsters[i].name].evolve), i });
		}
	}
	bool result = !m_evolveQueue.empty();
	if (!m_evolveQueue.empty()) {
		const EvolveEntry& evolveEntry = m_evolveQueue.front();
		m_evolve.setCurrentMonster(evolveEntry.m_startMonster);
		m_evolve.setStartMonster(evolveEntry.m_startMonster);
		m_evolve.setEndMonster(evolveEntry.m_endMonster);
		m_evolve.setCurentMonsterIndex(evolveEntry.index);

		m_delayEvolve.setOnTimerEnd([&m_evolveOpen = m_evolveOpen, &m_evolve = m_evolve, &m_zone = m_zone] {
			m_zone.setAlpha(1.0f - 0.784f);
			m_evolveOpen = true;
			m_evolve.startEvolution();
			});
		m_delayEvolve.start(800u, false);
		m_zone.getPlayer().block();
		m_blockIndex = true;
		m_evolveQueue.pop();
	}
	return result;
}