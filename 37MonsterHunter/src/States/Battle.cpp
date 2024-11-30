#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <engine/Fontrenderer.h>
#include <States/Menu.h>
#include <States/MonsterHunter.h>

#include "Battle.h"
#include "Zone.h"
#include "Application.h"
#include "Globals.h"

std::random_device Battle::RandomDevice;
std::mt19937 Battle::MersenTwist(RandomDevice());
Sprite Battle::SurfaceBar;
TextureRect Battle::BackgroundRect = {0.0f, 0.0f, 1.0f, 1.0f, 1280.0f, 720.0f, 0u};

Battle::Battle(StateMachine& machine) : State(machine, States::BATTLE), Icon(BackgroundRect),
m_mapHeight(0.0f), 
m_viewWidth(1280.0f), 
m_viewHeight(720.0f), 
m_drawGeneralUi(false),
m_drawAtacksUi(false),
m_drawSwitchUi(false),
m_drawTargetUI(false),
m_currentSelectedMonster(-1),
m_currentSelectedOption(0),
m_currentMax(4),
m_currentOffset(0),
m_visibleItems(4),
m_cutOff(0),
m_elapsedTime(0.0f),
m_currentFrame(0),
m_frameCount(4),
m_playAbility(false),
m_abilityOffset(0),
m_abilityPosX(0.0f),
m_abilityPosY(0.0f),
m_removeDefeteadMonster(false),
m_exit(false),
m_catchMonster(false),
m_canSwitch(true),
m_alpha(0.0f),
m_fade(m_alpha),
m_canCatch(true),
m_rotate(false)
{

	m_viewWidth = 1280.0f;
	m_viewHeight = 720.0f;

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);
	m_camera.setRotationSpeed(0.1f);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	positions.push_back({ 360.0f - 96.0f , m_viewHeight - 260.0f - 96.0f });
	positions.push_back({ 190.0f - 96.0f , m_viewHeight - 400.0f - 96.0f });
	positions.push_back({ 410.0f - 96.0f , m_viewHeight - 520.0f - 96.0f });

	positions.push_back({ 900.0f - 96.0f  , m_viewHeight - 260.0f - 96.0f });
	positions.push_back({ 1110.0f - 96.0f , m_viewHeight - 390.0f - 96.0f });
	positions.push_back({ 900.0f - 96.0f  , m_viewHeight - 550.0f - 96.0f });

	centers.push_back({ 360.0f, m_viewHeight - 260.0f });
	centers.push_back({ 190.0f, m_viewHeight - 400.0f });
	centers.push_back({ 410.0f, m_viewHeight - 520.0f });

	centers.push_back({ 900.0f , m_viewHeight - 260.0f });
	centers.push_back({ 1110.0f, m_viewHeight - 390.0f });
	centers.push_back({ 900.0f , m_viewHeight - 550.0f });
	m_cells.reserve(6);
	//setOpponentMonsters();

	TextureAtlasCreator::Get().init(256u, 32u);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/sword.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/shield.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/arrows.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/hand.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/sword_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/shield_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/arrows_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/hand_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/sword_gray.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/shield_gray.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/arrows_gray.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/hand_gray.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/sword_gray_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/shield_gray_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/arrows_gray_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileCpu("res/tmx/graphics/ui/hand_gray_highlight.png", false, true, false);
	TileSetManager::Get().getTileSet("battle_icon").loadTileSetGpu();
	//Spritesheet::Safe("battle_icon", TileSetManager::Get().getTileSet("battle_icon").getAtlas());
	m_atlasBattleIcon = TileSetManager::Get().getTileSet("battle_icon").getAtlas();

	TextureAtlasCreator::Get().init(768u, 1152u);
	TileSetManager::Get().getTileSet("attacks").loadTileSetCpu("res/tmx/graphics/attacks/explosion.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("attacks").loadTileSetCpu("res/tmx/graphics/attacks/fire.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("attacks").loadTileSetCpu("res/tmx/graphics/attacks/green.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("attacks").loadTileSetCpu("res/tmx/graphics/attacks/ice.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("attacks").loadTileSetCpu("res/tmx/graphics/attacks/scratch.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("attacks").loadTileSetCpu("res/tmx/graphics/attacks/splash.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("attacks").loadTileSetGpu();
	//Spritesheet::Safe("attacks", TileSetManager::Get().getTileSet("attacks").getAtlas());
	m_atlasAbilities = TileSetManager::Get().getTileSet("attacks").getAtlas();

	TextureAtlasCreator::Get().init(1280u, 720u);
	TileSetManager::Get().getTileSet("backgrounds").loadTileCpu("res/tmx/graphics/backgrounds/forest.png", false, true, false);
	TileSetManager::Get().getTileSet("backgrounds").loadTileCpu("res/tmx/graphics/backgrounds/ice.png", false, true, false);
	TileSetManager::Get().getTileSet("backgrounds").loadTileCpu("res/tmx/graphics/backgrounds/sand.png", false, true, false);
	TileSetManager::Get().getTileSet("backgrounds").loadTileSetGpu();
	//Spritesheet::Safe("backgrounds", TileSetManager::Get().getTileSet("backgrounds").getAtlas());
	m_backgrounds = TileSetManager::Get().getTileSet("backgrounds").getAtlas();
	
	initUI();

	m_opponentTimer.setOnTimerEnd(std::bind(&Battle::opponentAttack, this));
	m_exitTimer.setOnTimerEnd(std::bind(&Battle::exit, this));

	m_mainRenderTarget.create(Application::Width, Application::Height);
	m_mainRenderTarget.attachTexture2D(AttachmentTex::RGBA);
	m_mainRenderTarget.attachTexture2D(AttachmentTex::DEPTH24);
	
	m_biomeBackground = "forest";

	m_fade.setTransitionSpeed(2.353f);
	m_fade.fadeIn();
}

Battle::~Battle() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	MonsterIndex::Monsters.shrink_to_fit();
}

void Battle::fixedUpdate() {

}

void Battle::update() {
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

	if (keyboard.keyPressed(Keyboard::KEY_R)) {
		m_rotate = !m_rotate;
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		exit();
	}

	if (keyboard.keyPressed(Keyboard::KEY_P)) {
		std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));
		std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setHighlight), std::placeholders::_1, false));

		m_drawGeneralUi = false;
		m_currentSelectedMonster = -1;
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
			m_camera.move(direction * m_dt);
		}
	}

	if (m_playAbility) {
		m_elapsedTime += 6.0f * m_dt;
		m_currentFrame = static_cast <int>(std::floor(m_elapsedTime));
		if (m_currentFrame > m_frameCount - 1) {
			m_currentFrame = 0;
			m_abilityOffset = 0;
			m_elapsedTime = 0.0f;
			m_playAbility = false;
			onAbilityEnd();
		}
	}

	m_opponentTimer.update(m_dt);
	m_exitTimer.update(m_dt);
	m_fade.update(m_dt);

	for (auto& monster : m_monsters) {
		monster->update(m_dt);
	}

	if (m_exit)
		return;

	for (auto& monster = m_monsters.begin(); monster != m_monsters.end(); ++monster) {
		if ((*monster)->getInitiative() >= 100.0f && !(*monster)->getPause()) {

			std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setHighlight), std::placeholders::_1, false));
			(*monster)->setInitiative(0.0f);
			(*monster)->setHighlight(true, 300);
			(*monster)->setDefending(false);
			std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::pause));

			if ((*monster)->getCell().flipped) {
				m_drawGeneralUi = true;
			}else {
				if (std::count_if(m_monsters.begin(), m_monsters.end(), [](Monster* monster) { return monster->getCell().flipped && monster->getHealth() > 0.0f; }) != 0) {
					m_opponentTimer.start(600u, false);
				}
			}

			m_currentSelectedMonster = std::distance(m_monsters.begin(), monster);
			m_currentMax = 4;

		}
	}

	if (m_drawGeneralUi || m_drawAtacksUi || m_drawSwitchUi || m_drawTargetUI) {
		processInput();
		if (m_drawTargetUI) {
			std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setHighlight), std::placeholders::_1, false));
			//std::for_each(m_monsters.begin(), m_monsters.end(), MonsterFunctor(&Monster::setHighlight, true, 500u));	
			m_monsters[m_currentSelectedOption + m_cutOff]->setHighlight(true);
		}
	}
	removeDefeteadMonster();
	
	if (m_rotate) {
		rotate(10.0f * m_dt);
	}
}

void Battle::render() {
	m_mainRenderTarget.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Icon::draw();
	{
		std::list<std::shared_ptr<NodeUI>>::iterator  it, itInner;
		std::shared_ptr<WidgetMH> currentWidget;
		for (it = getChildren().begin(); it != getChildren().end(); ++it) {
			currentWidget = std::dynamic_pointer_cast<Monster>(*it);
			if(currentWidget)
				currentWidget->draw();			
		}
	}

	eraseAbilityUI();
	if (m_playAbility) {
		addAbilityUI(m_abilityPosX, m_abilityPosY);
		drawAbilityAnimation();
	}
	
	eraseGeneralUI();
	if (m_drawGeneralUi) {
		Monster* currentMonster = m_monsters[m_currentSelectedMonster];
		const Cell& cell = currentMonster->getCell();
		addGeneralUI(cell.posX + cell.width, cell.posY + 0.5f * cell.height);
		drawGeneral();
	}

	eraseAttacksUI();
	if (m_drawAtacksUi) {
		Monster* currentMonster = m_monsters[m_currentSelectedMonster];
		const Cell& cell = currentMonster->getCell();
		float width = 150.0f;
		float height = 200.0f;
		int limiter = std::min(m_visibleItems, m_currentMax);
		height = limiter <= 1 ? 50.0f : limiter == 2 ? 100.0f : limiter == 3 ? 150.0f : 200.0f;

		addAttacksUI(cell.posX + cell.width + 20.0f, cell.posY + 0.5f * cell.height - 0.5f * height, width, height);
		drawAttacks();
	}

	eraseSwitchUI();
	if (m_drawSwitchUi){
		Monster* currentMonster = m_monsters[m_currentSelectedMonster];
		const Cell& cell = currentMonster->getCell();
		float width = 300.0f;
		float height = 320.0f;

		int limiter = std::min(m_visibleItems, m_currentMax);
		//height = limiter <= 1 ? 80.0f : limiter == 2 ? 160.0f : limiter == 3 ? 240.0f : 320.0f;

		addSwitchUI(cell.posX + cell.width + 20.0f, cell.posY + 0.5f * cell.height - 0.5f * height, width, height);
		drawSwitch();
	}

	if (m_drawUi)
		renderUi();

	m_mainRenderTarget.unbind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_mainRenderTarget.bindColorTexture(0u, 0u);
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, m_alpha));
	Globals::shapeManager.get("quad").drawRaw();
	shader->unuse();
}

void Battle::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Battle::OnMouseButtonDown(Event::MouseButtonEvent& event) {

}

void Battle::OnMouseButtonUp(Event::MouseButtonEvent& event) {

}

void Battle::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Battle::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
}

void Battle::OnKeyUp(Event::KeyboardEvent& event) {

}

void Battle::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);

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

void Battle::renderUi() {
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

void Battle::setMapHeight(float mapHeight) {
	m_mapHeight = mapHeight;
}

void Battle::setViewHeight(float viewHeight) {
	m_viewHeight = viewHeight;
}

void Battle::drawGeneral() {

	Monster* currentMonster = m_monsters[m_currentSelectedMonster];

	Empty* generalUI = findChild<Empty>("general");
	unsigned int i = 0u;
	std::shared_ptr<IconAnimated> currentWidget;
	for (auto& children : generalUI->getChildren()) {
		currentWidget = std::static_pointer_cast<IconAnimated>(children);
		currentWidget->setCurrentFrame(i == m_currentSelectedOption ?
			i + 4 + ((i == 0) && !currentMonster->getCanAttack()) * 8 + ((i == 2) && !m_canSwitch) * 8 + ((i == 3) && !m_canCatch) * 8 :
			i + 8);

		currentWidget->draw();
		i++;
	}
}

void Battle::drawAbilityAnimation() {
	IconAnimated* abilityUI = findChild<IconAnimated>("ability");
	abilityUI->setCurrentFrame(m_abilityOffset + m_currentFrame);
	abilityUI->draw();
}


void Battle::drawAttacks() {
	
	eraseAbilities();
	addAbilities();

	Surface* attacksUI = findChild<Surface>("attacks");
	attacksUI->draw();
	for (auto& children : attacksUI->getChildren()) {
		std::static_pointer_cast<WidgetMH>(children)->draw();
	}
}

void Battle::drawSwitch() {
	
	eraseMonsters();
	addMonsters();
	
	Surface* switchUI = findChild<Surface>("switch");
	switchUI->draw();
	for (auto& children : switchUI->getChildren()) {
		std::static_pointer_cast<WidgetMH>(children)->draw();
	}
}

void Battle::processInput() {
	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_UP)) {
		m_currentSelectedOption--;

		if (m_currentOffset > 0 && m_currentSelectedOption - m_currentOffset < 0) {
			m_currentOffset--;
		}
		
		if (m_currentSelectedOption < 0) {
			m_currentOffset = std::max(m_currentMax - m_visibleItems, 0);
			m_currentSelectedOption = m_currentMax - 1;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_DOWN)) {
		m_currentSelectedOption++;

		if (m_currentSelectedOption - m_currentOffset - (m_visibleItems - 1) > 0) {
			m_currentOffset++;
		}
	
		if (m_currentSelectedOption > m_currentMax - 1) {
			m_currentSelectedOption = 0;
			m_currentOffset = 0;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {

		if (m_drawTargetUI) {
			if(m_catchMonster){
				m_catchMonster = false;
				m_drawTargetUI = false;
				m_monsters[m_currentSelectedMonster]->setHighlight(false);
				std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));

				Monster* currentMonster = m_monsters[m_currentSelectedOption + m_cutOff];
				if (currentMonster->getHealth() >= currentMonster->getMaxHealth() * 0.1f) {
					currentMonster->showMissing();
					m_currentSelectedMonster = -1;
					m_currentSelectedOption = 0;
					m_cutOff = 0;
					return;
				}
								
				MonsterIndex::Monsters.push_back({ currentMonster->getName(), currentMonster->getLevel(), currentMonster->getHealth(), currentMonster->getEnergy(), currentMonster->getExperience(), false });
				if (m_supplyIndexOpponent < static_cast<int>(m_opponentMonsters.size()) - 1) {
					m_supplyIndexOpponent++;
					m_monsters[m_currentSelectedOption + m_cutOff]->setGraphic(static_cast<int>(MonsterIndex::MonsterData[m_opponentMonsters[m_supplyIndexOpponent].name].graphic * 16u));
					m_monsters[m_currentSelectedOption + m_cutOff]->setMonsterEntry(m_opponentMonsters[m_supplyIndexOpponent]);
					m_monsters[m_currentSelectedOption + m_cutOff]->setInitiative(0.0f);

				}else {
					m_monsters[m_currentSelectedOption + m_cutOff]->eraseSelf();
					m_monsters.erase(m_monsters.begin() + m_currentSelectedOption + m_cutOff);
				}

				m_currentSelectedMonster = -1;
				m_currentSelectedOption = 0;
				m_cutOff = 0;

				if (std::count_if(m_monsters.begin(), m_monsters.end(), [](Monster* monster) { return !monster->getCell().flipped && monster->getHealth() > 0.0f; }) == 0) {
					m_exitTimer.start(1000u, false, true);
					m_exit = true;
					std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::pause));
				}
				m_canSwitch = m_supplyIndexPlayer < static_cast<int>(MonsterIndex::Monsters.size()) - 1;

			}else{
				m_playAbility = true;
				m_drawTargetUI = false;
				m_monsters[m_currentSelectedMonster]->reduceEnergy(MonsterIndex::_AttackData[m_currentAbility.first]);
				m_monsters[m_currentSelectedMonster]->playAttackAnimation();
				m_monsters[m_currentSelectedMonster]->canAttack();

				float amount = m_monsters[m_currentSelectedMonster]->getBaseDamage(m_currentAbility.first);
				m_monsters[m_currentSelectedOption + m_cutOff]->applyAttack(amount, MonsterIndex::_AttackData[m_currentAbility.first]);

				m_abilityPosX = m_monsters[m_currentSelectedOption + m_cutOff]->getCell().centerX;
				m_abilityPosY = m_monsters[m_currentSelectedOption + m_cutOff]->getCell().centerY;
				if (std::count_if(m_monsters.begin(), m_monsters.end(), [](Monster* monster) { return !monster->getCell().flipped && monster->getHealth() > 0.0f; }) == 0) {
					std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setCanAttack), std::placeholders::_1, false));
				}
			}
		}

		if (m_drawSwitchUi) {		
			MonsterEntry& monsterEntry = m_filteredMonsters[m_currentOffset + m_currentSelectedOption];
			MonsterEntry& currentMonsterEntry = m_monsters[m_currentSelectedMonster]->getMonsterEntry();
			std::swap(monsterEntry, currentMonsterEntry);
			
			m_monsters[m_currentSelectedMonster]->setGraphic(static_cast<int>(MonsterIndex::MonsterData[currentMonsterEntry.name].graphic * 16u));
			m_monsters[m_currentSelectedMonster]->calculateStates(currentMonsterEntry);
			m_monsters[m_currentSelectedMonster]->setInitiative(0.0f);

			std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));

			m_drawSwitchUi = false;
			m_visibleItems = 4;
			m_currentSelectedOption = 0;
			m_currentOffset = 0;
			m_currentMax = 4;
			m_currentSelectedMonster = -1;		
		}

		if (m_drawAtacksUi) {
			m_currentAbility = *(m_abilitiesFiltered.begin() + m_currentSelectedOption);
			m_currentTarget = MonsterIndex::_AttackData[m_currentAbility.first].target;
			m_drawAtacksUi = false;
			m_drawTargetUI = true;

			int playerMonsterCount = std::count_if(m_monsters.begin(), m_monsters.end(), [](Monster* monster) { return monster->getCell().flipped; });

			m_currentMax = m_currentTarget == "player" ? playerMonsterCount : static_cast<int>(m_monsters.size()) - playerMonsterCount;
			m_visibleItems = m_currentMax;
			
			m_currentSelectedOption = 0;
			m_currentOffset = 0;
			m_cutOff = m_currentTarget == "player" ? 0 : playerMonsterCount;
			Monster* currentMonster = m_monsters[m_currentSelectedMonster];
			if (m_currentTarget == "player")
				currentMonster->setHighlight(false);

			m_abilityOffset = MonsterIndex::_AttackData[m_currentAbility.first].graphic;
		}

		if (m_drawGeneralUi) {
			if (m_currentSelectedOption == 0) {
				m_drawGeneralUi = false;
				m_drawAtacksUi = true;
				m_visibleItems = 4;
				Monster* currentMonster = m_monsters[m_currentSelectedMonster];
				const tsl::ordered_map<std::string, unsigned int>& abilities = MonsterIndex::MonsterData[m_monsters[m_currentSelectedMonster]->getName()].abilities;
				m_abilitiesFiltered.clear();
				std::copy_if(abilities.begin(), abilities.end(), std::inserter(m_abilitiesFiltered, m_abilitiesFiltered.end()), [currentMonster = currentMonster](std::pair<std::string, unsigned int> ability) {
					return  MonsterIndex::_AttackData[ability.first].cost <= currentMonster->getEnergy();
				});

				m_currentMax = std::count_if(m_abilitiesFiltered.begin(), m_abilitiesFiltered.end(), [&currentMonster = currentMonster](const std::pair<std::string, unsigned int>& ability) { return ability.second <= currentMonster->getLevel(); });
				int opponentMonsterCount = std::count_if(m_monsters.begin(), m_monsters.end(), [](Monster* monster) { return !monster->getCell().flipped && monster->getHealth() > 0.0f; });
				m_currentSelectedOption = 0;
				m_currentOffset = 0;
				
				if (m_currentMax == 0 || opponentMonsterCount == 0) {
					m_drawAtacksUi = false;
					m_drawGeneralUi = true;
					m_visibleItems = 4;
					m_currentMax = 4;				
				}

			}else if (m_currentSelectedOption == 1) {
				m_monsters[m_currentSelectedMonster]->setDefending(true);
				m_drawGeneralUi = false;
				m_visibleItems = 4;
				m_currentSelectedOption = 0;
				m_currentOffset = 0;
				m_currentMax = 4;
				m_currentSelectedMonster = -1;
				std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));

			}else if (m_currentSelectedOption == 2) {
				m_drawGeneralUi = false;
				m_drawSwitchUi = true;			
				m_visibleItems = 4;
				const Monster* currentMonster = m_monsters[m_currentSelectedMonster];
				m_filteredMonsters.clear();
				
				for (size_t index = 0; index < MonsterIndex::Monsters.size(); index++) {
					if (MonsterIndex::Monsters[index].health > 0 && index > m_supplyIndexPlayer)
						m_filteredMonsters.push_back(MonsterIndex::Monsters[index]);
				}

				m_currentMax = static_cast<int>(m_filteredMonsters.size());
				m_currentSelectedOption = 0;
				m_currentOffset = 0;

				if (!m_canSwitch) {
					m_drawSwitchUi = false;
					m_drawGeneralUi = true;
					m_currentMax = 4;
					m_currentSelectedOption = 2;
				}

			}else if (m_currentSelectedOption == 3) {
				m_drawGeneralUi = false;
				m_drawTargetUI = true;

				int playerMonsterCount = std::count_if(m_monsters.begin(), m_monsters.end(), [](const Monster* monster) { return monster->getCell().flipped; });

				m_currentMax = m_currentTarget == "player" ? playerMonsterCount : static_cast<int>(m_monsters.size()) - playerMonsterCount;
				m_visibleItems = m_currentMax;

				m_currentSelectedOption = 0;
				m_currentOffset = 0;
				m_cutOff = playerMonsterCount;
				m_catchMonster = true;

				if (!m_canCatch) {
					m_catchMonster = false;
					m_drawGeneralUi = true;
					m_drawTargetUI = false;
					m_currentMax = 4;
					m_visibleItems = 4;
					m_currentSelectedOption = 3;
					m_cutOff = 0;
				}
			}
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_ESCAPE)) {
		if (m_drawGeneralUi) {
			m_monsters[m_currentSelectedMonster]->setDefending(true);
			m_drawGeneralUi = false;
			m_visibleItems = 4;
			m_currentSelectedOption = 0;
			m_currentOffset = 0;
			m_currentMax = 4;
			m_currentSelectedMonster = -1;
			std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));		
		}

		if (m_drawAtacksUi || m_drawSwitchUi || m_drawTargetUI) {
			if (m_drawTargetUI) {
				std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setHighlight), std::placeholders::_1, false));
				m_monsters[m_currentSelectedMonster]->setHighlight(true);
				m_currentSelectedOption = 0;
			}

			if (m_drawAtacksUi)
				m_currentSelectedOption = 0;

			if(m_drawSwitchUi)
				m_currentSelectedOption = 2;

			m_drawAtacksUi = false;
			m_drawSwitchUi = false;
			m_drawTargetUI = false;
			m_catchMonster = false;
			m_drawGeneralUi = true;
			m_currentMax = 4;
			
			m_currentOffset = 0;			
		}
	}
}

void Battle::onAbilityEnd() {
	m_drawGeneralUi = false;
	m_drawAtacksUi = false;
	m_drawTargetUI = false;
	m_drawSwitchUi = false;

	std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));
	std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setHighlight), std::placeholders::_1, false));

	m_currentSelectedMonster = -1;
	m_currentSelectedOption = 0;
	m_currentOffset = 0;

	if (std::count_if(m_monsters.begin(), m_monsters.end(), [](const Monster* monster) { return !monster->getCell().flipped && monster->getHealth() > 0.0f; }) == 0 ||
		std::count_if(m_monsters.begin(), m_monsters.end(), [](const Monster* monster) { return monster->getCell().flipped && monster->getHealth() > 0.0f; }) == 0) {
		m_exitTimer.start(1000u, false, true);
		m_exit = true;
		std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::pause));
	}
}

void Battle::removeDefeteadMonster() {

	for (size_t index = 0; index < m_monsters.size(); index++) {

		if (m_monsters[index]->getHealth() <= 0.0f) {
			if (!m_monsters[index]->getCell().flipped && !m_monsters[index]->getKilled()) {
				float currentExperience = static_cast<float>(m_monsters[index]->getLevel() * 100u) / static_cast<float>(std::count_if(m_monsters.begin(), m_monsters.end(), [](const Monster* monster) { return monster->getCell().flipped && monster->getHealth() > 0.0f; }));
				
				std::for_each(m_monsters.begin(),
					m_monsters.end(),
					[&](Monster* monster) { if (monster->getCell().flipped)
						monster->updateExperience(currentExperience);
					});
			}
			m_monsters[index]->startDelayedKill();
		}
		if (m_monsters[index]->getDelayedKill()) {
			
			m_monsters[index]->setDelayedKill(false);
			
			if (m_supplyIndexPlayer < static_cast<int>(MonsterIndex::Monsters.size()) - 1 && m_monsters[index]->getCell().flipped) {
				m_supplyIndexPlayer++;
					
				m_monsters[index]->setGraphic(static_cast<int>(MonsterIndex::MonsterData[MonsterIndex::Monsters[m_supplyIndexPlayer].name].graphic * 16u));
				m_monsters[index]->setMonsterEntry(MonsterIndex::Monsters[m_supplyIndexPlayer]);
				m_monsters[index]->setInitiative(0.0f);
				m_monsters[index]->setKilled(false);

				m_canSwitch = m_supplyIndexPlayer < static_cast<int>(MonsterIndex::Monsters.size()) - 1;
			}else if(m_supplyIndexOpponent < static_cast<int>(m_opponentMonsters.size()) - 1 && !m_monsters[index]->getCell().flipped) {
				m_supplyIndexOpponent++;
					
				m_monsters[index]->setGraphic(static_cast<int>(MonsterIndex::MonsterData[m_opponentMonsters[m_supplyIndexOpponent].name].graphic * 16u));
				m_monsters[index]->setMonsterEntry(m_opponentMonsters[m_supplyIndexOpponent]);
				m_monsters[index]->setInitiative(0.0f);
				m_monsters[index]->setKilled(false);

			}else {
				if (m_currentSelectedMonster >= index)
					m_currentSelectedMonster--;
				m_monsters[index]->eraseSelf();
				m_monsters.erase(m_monsters.begin() + index);
				
			}
			break;
		}
	}	
}

void Battle::opponentAttack() {

	m_opponentTimer.stop();	
	Monster* currentMonster = m_monsters[m_currentSelectedMonster];
	const tsl::ordered_map<std::string, unsigned int>& abilities = MonsterIndex::MonsterData[currentMonster->getName()].abilities;
	
	tsl::ordered_map<std::string, unsigned int> abilitiesFiltered;
	std::copy_if(abilities.begin(), abilities.end(), std::inserter(abilitiesFiltered, abilitiesFiltered.end()), [currentMonster = currentMonster](std::pair<std::string, unsigned int> ability) {
		return  MonsterIndex::_AttackData[ability.first].cost <= currentMonster->getEnergy();
	});

	int upperBound = std::count_if(abilitiesFiltered.begin(), abilitiesFiltered.end(), [&currentMonster = currentMonster](const std::pair<std::string, unsigned int>& ability) { return ability.second <= currentMonster->getLevel(); });
	
	//Energy empty
	if (upperBound == 0) {
		std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));
		return;
	}
	
	size_t index = 0;
	std::uniform_int_distribution<size_t> dist(0, upperBound - 1);
	index = dist(MersenTwist);
	tsl::ordered_map<std::string, unsigned int>::iterator ability = abilitiesFiltered.begin();
	std::advance(ability, index);
	
	std::string currentTarget = MonsterIndex::_AttackData[ability->first].target;
	int playerMonsterCount = std::count_if(m_monsters.begin(), m_monsters.end(), [](const Monster* monster) { return monster->getCell().flipped; });
	upperBound = currentTarget == "opponent" ? playerMonsterCount : static_cast<int>(m_monsters.size()) - playerMonsterCount;

	std::uniform_int_distribution<size_t> dist2(0, upperBound - 1);
	index = dist2(MersenTwist);
	if (currentTarget == "player")
		index += playerMonsterCount;

	Monster* target = m_monsters[index];
	float amount = currentMonster->getBaseDamage(ability->first);
	target->applyAttack(amount, MonsterIndex::_AttackData[ability->first]);
	m_playAbility = true;
	m_abilityPosX = target->getCell().centerX;
	m_abilityPosY = target->getCell().centerY;
	m_abilityOffset = MonsterIndex::_AttackData[ability->first].graphic;
	currentMonster->reduceEnergy(MonsterIndex::_AttackData[ability->first]);
}

void Battle::exit() {
	m_fade.setOnFadeOut([&m_isRunning = m_isRunning]() {
		m_isRunning = false;
	});
	m_fade.fadeOut();
}

Fade& Battle::getFade() {
	return m_fade;
}

void Battle::setOpponentMonsters() {

	m_opponentMonsters.push_back({ "Atrox", 3u, 3.0f, 200.0f, 0.0f, false });
	m_opponentMonsters.push_back({ "Finiette", 4u, 3.0f, 20.0f, 0.0f, false });
	m_opponentMonsters.push_back({ "Pouch", 5u, 3.0f, 20.0f, 0.0f, false });
	//m_opponentMonsters.push_back({ "Finsta", 2u, 3.0f, 20.0f, 0.0f, false });
	//m_opponentMonsters.push_back({ "Cleaf", 2u, 3.0f, 20.0f, 0.0f, false });
	//m_opponentMonsters.push_back({ "Friolera", 5u, 3.0f, 20.0f, 0.0f, false });

	MonsterIndex::Monsters.reserve(MonsterIndex::Monsters.size() + m_opponentMonsters.size());
	m_supplyIndexPlayer = std::max(std::min(2, static_cast<int>(MonsterIndex::Monsters.size()) - 1), 0);

	Monster* monster;
	for (int i = 0; i < std::min(static_cast<int>(MonsterIndex::Monsters.size()), 3); i++) {
		m_cells.push_back({ positions[i][0], positions[i][1], 192.0f, 192.0f, static_cast<int>(MonsterIndex::MonsterData[MonsterIndex::Monsters[i].name].graphic * 16u), centers[i][0], centers[i][1], true, true });

		monster = addChild<Monster>(m_cells.back(), MonsterIndex::Monsters[i]);
		monster->translateRelative(positions[i][0], positions[i][1]);
		monster->updateWorldTransformation();
		monster->initUI();
		m_monsters.push_back(monster);
	}

	std::for_each(m_opponentMonsters.begin(), m_opponentMonsters.end(), std::mem_fn(&MonsterEntry::resetStats));
	m_supplyIndexOpponent = std::max(std::min(2, static_cast<int>(m_opponentMonsters.size()) - 1), 0);
	for (int i = 0; i < std::min(static_cast<int>(m_opponentMonsters.size()), 3); i++) {
		m_cells.push_back({ positions[i + 3][0], positions[i + 3][1], 192.0f, 192.0f, static_cast<int>(MonsterIndex::MonsterData[m_opponentMonsters[i].name].graphic * 16u), centers[i + 3][0], centers[i + 3][1], true, false });
		monster = addChild<Monster>(m_cells.back(), m_opponentMonsters[i]);
		monster->translateRelative(positions[i + 3][0], positions[i][1]);
		monster->updateWorldTransformation();
		monster->initUI();
		m_monsters.push_back(monster);
	}

	m_canSwitch = std::count_if(MonsterIndex::Monsters.begin() + m_supplyIndexPlayer + 1, MonsterIndex::Monsters.end(), [](const MonsterEntry& monsterEntry) { return monsterEntry.health > 0.0f; }) != 0;
}

void Battle::setOpponentMonsters(const std::vector<MonsterEntry>& monsters, bool canCatch) {

	m_opponentMonsters.assign(monsters.begin(), monsters.end());

	MonsterIndex::Monsters.reserve(MonsterIndex::Monsters.size() + m_opponentMonsters.size());
	m_supplyIndexPlayer = std::max(std::min(2, static_cast<int>(MonsterIndex::Monsters.size()) - 1), 0);

	Monster* monster;
	for (int i = 0; i < std::min(static_cast<int>(MonsterIndex::Monsters.size()), 3); i++) {
		m_cells.push_back({ positions[i][0], positions[i][1], 192.0f, 192.0f, static_cast<int>(MonsterIndex::MonsterData[MonsterIndex::Monsters[i].name].graphic * 16u), centers[i][0], centers[i][1], true, true });

		monster = addChild<Monster>(m_cells.back(), MonsterIndex::Monsters[i]);
		monster->translateRelative(positions[i][0], positions[i][1]);
		monster->updateWorldTransformation();
		monster->initUI();
		m_monsters.push_back(monster);
	}
	
	std::for_each(m_opponentMonsters.begin(), m_opponentMonsters.end(), std::mem_fn(&MonsterEntry::resetStats));
	m_supplyIndexOpponent = std::max(std::min(2, static_cast<int>(m_opponentMonsters.size()) - 1), 0);
	for (int i = 0; i < std::min(static_cast<int>(m_opponentMonsters.size()), 3); i++) {
		m_cells.push_back({ positions[i + 3][0], positions[i + 3][1], 192.0f, 192.0f, static_cast<int>(MonsterIndex::MonsterData[m_opponentMonsters[i].name].graphic * 16u), centers[i + 3][0], centers[i + 3][1], true, false });
		monster = addChild<Monster>(m_cells.back(), m_opponentMonsters[i]);
		monster->translateRelative(positions[i + 3][0], positions[i][1]);
		monster->updateWorldTransformation();
		monster->initUI();
		m_monsters.push_back(monster);
	}

	m_canSwitch = std::count_if(MonsterIndex::Monsters.begin() + m_supplyIndexPlayer + 1, MonsterIndex::Monsters.end(), [](const MonsterEntry& monsterEntry) { return monsterEntry.health > 0.0f; }) != 0;
	m_canCatch = canCatch;
}

void Battle::setBiomeBackground(const std::string& biomeBackground) {
	m_biomeBackground = biomeBackground;
}

void Battle::initUI() {
	setSpriteSheet(TileSetManager::Get().getTileSet("backgrounds").getAtlas());
	scale(BackgroundRect.width, BackgroundRect.height);	
	setOrigin(static_cast<float>(Application::Width) * 0.5f, static_cast<float>(Application::Height) * 0.5f);
}

void Battle::addAbilities() {
	Monster* currentMonster = m_monsters[m_currentSelectedMonster];
	const Cell& cell = currentMonster->getCell();

	float width = 150.0f;
	float height = 200.0f;
	int limiter = std::min(m_visibleItems, m_currentMax);
	int position = m_currentSelectedOption - m_currentOffset;
	float invLimiter = 1.0f / static_cast<float>(limiter);
	float lineHeight = Globals::fontManager.get("dialog").lineHeight * 0.045f;

	Surface* atacksUI = findChild<Surface>("attacks");

	Surface* surface = atacksUI->addChild<Surface>();
	surface->setPosition(0.0f, 1.0f - (position + 1) * invLimiter);
	surface->setScale(1.0f, invLimiter);
	surface->updateWorldTransformation();
	surface->setShader(Globals::shaderManager.getAssetPointer("list"));
	surface->setColor(Vector4f(0.78431f, 0.78431f, 0.78431f, 1.0f));
	surface->setEdge(position == 0 ? Edge::TOP : position == std::min(m_visibleItems, m_currentMax - m_currentOffset) - 1 ? Edge::BOTTOM : Edge::EDGE_NONE);

	Label* label; int index = 0;
	for (auto& ability = m_abilitiesFiltered.begin() + m_currentOffset; ability != m_abilitiesFiltered.begin() + m_currentOffset + std::min(m_visibleItems, m_currentMax - m_currentOffset); ++ability) {
		label = atacksUI->addChild<Label>(Globals::fontManager.get("dialog"));
		label->setPosition(0.5f, 1.0f - (index + 1) * invLimiter + 0.5 * invLimiter);
		label->translateRelative(-0.5f * Globals::fontManager.get("dialog").getWidth((*ability).first) * 0.045f, -lineHeight * 0.5);
		label->setScale(1.0f, 1.0f);
		label->updateWorldTransformation();
		label->setSize(0.045f);
		label->setLabel((*ability).first);
		label->setTextColor((index == m_currentSelectedOption - m_currentOffset) ? MonsterIndex::_AttackData[(*ability).first].element == "normal" ? Vector4f(0.0f, 0.0f, 0.0f, 1.0f) : MonsterIndex::ColorMap[MonsterIndex::_AttackData[(*ability).first].element] : Vector4f(0.78431f, 0.78431f, 0.78431f, 1.0f));
		index++;
	}
}

void Battle::eraseAbilities() {
	Surface* attacksUI = findChild<Surface>("attacks");
	attacksUI->eraseAllChildren();
}

void Battle::addMonsters() {
	const std::vector<TextureRect>& iconRects = TileSetManager::Get().getTileSet("monster_icon").getTextureRects();

	float invLimiter = 0.25f;
	int position = m_currentSelectedOption - m_currentOffset;
	float lineHeight = Globals::fontManager.get("dialog").lineHeight * 0.045f;

	Surface* switchUI = findChild<Surface>("switch");

	Surface* surface = switchUI->addChild<Surface>();
	surface->setPosition(0.0f, 1.0f - (position + 1) * invLimiter);
	surface->setScale(1.0f, invLimiter);
	surface->updateWorldTransformation();
	surface->setShader(Globals::shaderManager.getAssetPointer("list"));
	surface->setColor(Vector4f(0.78431f, 0.78431f, 0.78431f, 1.0f));
	surface->setEdge(position == 0 ? Edge::TOP : position == 3 ? Edge::BOTTOM : Edge::EDGE_NONE);
	surface->setBorderRadius(5.0f);

	Icon* icon; Label* label; Bar* bar;
	for (size_t index = m_currentOffset; index < m_currentOffset + std::min(m_visibleItems, m_currentMax - m_currentOffset); index++) {
		const TextureRect& iconRect = iconRects[MonsterIndex::MonsterData[m_filteredMonsters[index].get().name].graphic];
		icon = switchUI->addChild<Icon>(iconRect);
		icon->setPosition(0.0f, 1.0f - (index - m_currentOffset + 1) * invLimiter + 0.5 * invLimiter);
		icon->translateRelative(10.0f + 0.5f * iconRect.width, 0.0f);
		icon->setAligned(true);
		icon->setSpriteSheet(TileSetManager::Get().getTileSet("monster_icon").getAtlas());

		label = switchUI->addChild<Label>(Globals::fontManager.get("dialog"));
		label->setPosition(0.0f, 1.0f - (index - m_currentOffset + 1) * invLimiter + 0.5 * invLimiter);
		label->translateRelative(90.0f, -lineHeight * 0.5);
		label->setScale(1.0f, 1.0f);
		label->setSize(0.045f);
		label->setLabel(m_filteredMonsters[index].get().name + " " + "(" + std::to_string(m_filteredMonsters[index].get().level) + ")");
		label->setTextColor(index == m_currentSelectedOption ? Vector4f(0.94117f, 0.19215f, 0.19215f, 1.0f) : Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

		bar = switchUI->addChild<Bar>(TileSetManager::Get().getTileSet("bars"));
		bar->setPosition(0.0f, 1.0f - (index - m_currentOffset + 1) * invLimiter + 0.5 * invLimiter);
		bar->translateRelative(90.0f, -lineHeight * 0.5 - 9.0f);
		bar->updateWorldTransformation();
		bar->setRadius(0.0f);
		bar->setBgColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
		bar->setColor(Vector4f(0.94117f, 0.19215f, 0.19215f, 1.0f));
		bar->setWidth(100.0f);
		bar->setHeight(5.0f);
		bar->setMaxValue(static_cast<float>(m_filteredMonsters[index].get().level * MonsterIndex::MonsterData[m_filteredMonsters[index].get().name].maxHealth));
		bar->setValue(m_filteredMonsters[index].get().health);

		bar = switchUI->addChild<Bar>(TileSetManager::Get().getTileSet("bars"));
		bar->setPosition(0.0f, 1.0f - (index - m_currentOffset + 1) * invLimiter + 0.5 * invLimiter);
		bar->translateRelative(90.0f, -lineHeight * 0.5 - 16.0f);
		bar->updateWorldTransformation();
		bar->setRadius(0.0f);
		bar->setBgColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
		bar->setColor(Vector4f(0.4f, 0.84313f, 0.93333f, 1.0f));
		bar->setWidth(100.0f);
		bar->setHeight(5.0f);
		bar->setMaxValue(static_cast<float>(m_filteredMonsters[index].get().level * MonsterIndex::MonsterData[m_filteredMonsters[index].get().name].maxEnergy));
		bar->setValue(m_filteredMonsters[index].get().energy);
	}
}

void Battle::eraseMonsters() {
	Surface* switchUI = findChild<Surface>("switch");
	switchUI->eraseAllChildren();
}

void Battle::addGeneralUI(float posX, float posY) {

	Empty* generalUI = addChild<Empty>();
	generalUI->setName("general");
	generalUI->setPosition(0.0f, 0.0f);
	generalUI->translateRelative(posX, posY);
	
	IconAnimated* iconAnimated = generalUI->addChild<IconAnimated>(TileSetManager::Get().getTileSet("battle_icon").getTextureRects());
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("battle_icon").getAtlas());
	iconAnimated->setAligned(true);
	iconAnimated->setPosition(0.0f, 0.0f);
	iconAnimated->translateRelative(30.0f, 60.0f);

	iconAnimated = generalUI->addChild<IconAnimated>(TileSetManager::Get().getTileSet("battle_icon").getTextureRects());
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("battle_icon").getAtlas());
	iconAnimated->setAligned(true);
	iconAnimated->setPosition(0.0f, 0.0f);
	iconAnimated->translateRelative(40.0f, 20.0f);

	iconAnimated = generalUI->addChild<IconAnimated>(TileSetManager::Get().getTileSet("battle_icon").getTextureRects());
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("battle_icon").getAtlas());
	iconAnimated->setAligned(true);
	iconAnimated->setPosition(0.0f, 0.0f);
	iconAnimated->translateRelative(40.0f, -20.0f);

	iconAnimated = generalUI->addChild<IconAnimated>(TileSetManager::Get().getTileSet("battle_icon").getTextureRects());
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("battle_icon").getAtlas());
	iconAnimated->setAligned(true);
	iconAnimated->setPosition(0.0f, 0.0f);
	iconAnimated->translateRelative(30.0f, -60.0f);
}

void Battle::eraseGeneralUI() {
	Empty* generalUI = findChild<Empty>("general");
	if (generalUI)
		generalUI->eraseSelf();
}

void Battle::addAttacksUI(float posX, float posY, float scaleX, float scaleY) {
	Surface* attacksUI = addChild<Surface>();
	attacksUI->setName("attacks");
	attacksUI->setPosition(0.0f, 0.0f);
	attacksUI->translateRelative(posX, posY);
	attacksUI->setShader(Globals::shaderManager.getAssetPointer("list"));
	attacksUI->setBorderRadius(5.0f);
	attacksUI->setEdge(Edge::ALL);
	attacksUI->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	attacksUI->setScale(1.0f, 1.0f);
	attacksUI->scaleAbsolute(scaleX, scaleY);
}

void Battle::eraseAttacksUI() {
	Surface* attacksUI = findChild<Surface>("attacks");
	if (attacksUI)
		attacksUI->eraseSelf();
}

void Battle::addSwitchUI(float posX, float posY, float scaleX, float scaleY) {
	Surface* switchUI = addChild<Surface>();
	switchUI->setName("switch");
	switchUI->setPosition(0.0f, 0.0f);
	switchUI->translateRelative(posX, posY);
	switchUI->setShader(Globals::shaderManager.getAssetPointer("list"));
	switchUI->setBorderRadius(5.0f);
	switchUI->setEdge(Edge::ALL);
	switchUI->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	switchUI->setScale(1.0f, 1.0f);
	switchUI->scaleAbsolute(scaleX, scaleY);
}

void Battle::eraseSwitchUI() {
	Surface* switchUI = findChild<Surface>("switch");
	if (switchUI)
		switchUI->eraseSelf();
}

void Battle::addAbilityUI(float posX, float posY) {
	IconAnimated* abilityUI = addChild<IconAnimated>(TileSetManager::Get().getTileSet("attacks").getTextureRects());
	abilityUI->setName("ability");
	abilityUI->setSpriteSheet(TileSetManager::Get().getTileSet("attacks").getAtlas());
	abilityUI->setAligned(true);
	abilityUI->setPosition(0.0f, 0.0f);
	abilityUI->translateRelative(posX, posY);
}

void Battle::eraseAbilityUI() {
	IconAnimated* abilityUI = findChild<IconAnimated>("ability");
	if (abilityUI)
		abilityUI->eraseSelf();
}