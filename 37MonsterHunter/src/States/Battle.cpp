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
m_canCatch(true)
{
	Globals::musicManager.get("background").play("res/audio/battle.ogg");
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
	
	Mouse &mouse = Mouse::instance();

	if (m_playAbility) {
		m_elapsedTime += 6.0f * m_dt;
		m_currentFrame = static_cast <int>(std::floor(m_elapsedTime));
		if (m_currentFrame > m_frameCount - 1) {
			m_currentFrame = 0;
			m_abilityOffset = 0;
			m_elapsedTime = 0.0f;

			onDrawAbilityUI(false);
			onAbilityEnd();
		}
	}

	m_opponentTimer.update(m_dt);
	m_exitTimer.update(m_dt);
	m_fade.update(m_dt);

	for (auto& monster : m_monsters) {
		monster->update(m_dt);
	}

	if (m_playAbility) 	
		updateAbilityAnimation();
	
	if (m_exit)
		return;

	for (auto& monster = m_monsters.begin(); monster != m_monsters.end(); ++monster) {
		if ((*monster)->getInitiative() >= 100.0f && !(*monster)->getPause()) {

			std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setHighlight), std::placeholders::_1, false));
			(*monster)->setInitiative(0.0f);
			(*monster)->setHighlight(true, 300);
			(*monster)->setDefending(false);
			std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::pause));

			m_currentSelectedMonster = std::distance(m_monsters.begin(), monster);
			m_currentMax = 4;

			if ((*monster)->getCell().flipped) {
				onDrawGeneralUI(true);
			}else {
				if (std::count_if(m_monsters.begin(), m_monsters.end(), [](Monster* monster) { return monster->getCell().flipped && monster->getHealth() > 0.0f; }) != 0) {
					m_opponentTimer.start(600u, false);
				}
			}
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
	
	if (m_drawGeneralUi) 		
		updateGeneral();
	
	if (m_drawAtacksUi) 		
		updateAttacks();
	
	if (m_drawSwitchUi) 	
		updateSwitch();
}

void Battle::render() {
	m_mainRenderTarget.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ui::Widget::draw();

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
}

void Battle::setMapHeight(float mapHeight) {
	m_mapHeight = mapHeight;
}

void Battle::setViewHeight(float viewHeight) {
	m_viewHeight = viewHeight;
}

void Battle::updateGeneral() {

	Monster* currentMonster = m_monsters[m_currentSelectedMonster];

	ui::Empty* generalUI = findChild<ui::Empty>("general");
	unsigned int i = 0u;
	std::shared_ptr<ui::IconAnimated> currentWidget;
	for (auto& children : generalUI->getChildren()) {
		currentWidget = std::static_pointer_cast<ui::IconAnimated>(children);
		currentWidget->setCurrentFrame(i == m_currentSelectedOption ?
			i + 4 + ((i == 0) && !currentMonster->getCanAttack()) * 8 + ((i == 2) && !m_canSwitch) * 8 + ((i == 3) && !m_canCatch) * 8 :
			i + 8);
		i++;
	}
}

void Battle::updateAbilityAnimation() {
	ui::IconAnimated* abilityUI = findChild<ui::IconAnimated>("ability");
	abilityUI->setCurrentFrame(m_abilityOffset + m_currentFrame);
}

void Battle::updateAttacks() {	
	eraseAbilities();
	addAbilities();
}

void Battle::updateSwitch() {	
	eraseListMonsters();
	addListMonsters();
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
				if (currentMonster->getHealth() >= currentMonster->getMaxHealth() * 0.8f) {
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
				m_canSwitch = canSwitch();

			}else{

				m_drawTargetUI = false;
				m_monsters[m_currentSelectedMonster]->reduceEnergy(MonsterIndex::_AttackData[m_currentAbility.first]);
				m_monsters[m_currentSelectedMonster]->playAttackAnimation();
				m_monsters[m_currentSelectedMonster]->canAttack();

				float amount = m_monsters[m_currentSelectedMonster]->getBaseDamage(m_currentAbility.first);
				playAttackSound(MonsterIndex::_AttackData[m_currentAbility.first].animation);
				m_monsters[m_currentSelectedOption + m_cutOff]->applyAttack(amount, MonsterIndex::_AttackData[m_currentAbility.first]);
				
				m_abilityPosX = m_monsters[m_currentSelectedOption + m_cutOff]->getCell().centerX;
				m_abilityPosY = m_monsters[m_currentSelectedOption + m_cutOff]->getCell().centerY;
				if (std::count_if(m_monsters.begin(), m_monsters.end(), [](Monster* monster) { return !monster->getCell().flipped && monster->getHealth() > 0.0f; }) == 0) {
					std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setCanAttack), std::placeholders::_1, false));
				}
				onDrawAbilityUI(true);
			}
		}

		if (m_drawSwitchUi) {		
			MonsterEntry& monsterEntry = m_filteredMonsters[m_currentOffset + m_currentSelectedOption];
			MonsterEntry& currentMonsterEntry = m_monsters[m_currentSelectedMonster]->getMonsterEntry();
			std::swap(monsterEntry, currentMonsterEntry);
			
			m_monsters[m_currentSelectedMonster]->setGraphic(static_cast<int>(MonsterIndex::MonsterData[currentMonsterEntry.name].graphic * 16u));
			m_monsters[m_currentSelectedMonster]->calculateStates(currentMonsterEntry);
			m_monsters[m_currentSelectedMonster]->setInitiative(0.0f);
			m_monsters[m_currentSelectedMonster]->setHighlight(false);
			m_monsters[m_currentSelectedMonster]->canAttack();

			std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));
			
			m_visibleItems = 4;
			m_currentSelectedOption = 0;
			m_currentOffset = 0;
			m_currentMax = 4;
			m_currentSelectedMonster = -1;		
			onDrawSwitchUI(false);
		}

		if (m_drawAtacksUi) {
			m_currentAbility = *(m_abilitiesFiltered.begin() + m_currentSelectedOption);
			m_currentTarget = MonsterIndex::_AttackData[m_currentAbility.first].target;	
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
			onDrawAttacksUI(false);
		}

		if (m_drawGeneralUi) {
			if (m_currentSelectedOption == 0) {
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

				onDrawGeneralUI(false);
				onDrawAttacksUI(true);

				if (m_currentMax == 0 || opponentMonsterCount == 0) {
					m_visibleItems = 4;
					m_currentMax = 4;
					onDrawAttacksUI(false);
					onDrawGeneralUI(true);
				}

			}else if (m_currentSelectedOption == 1) {
				m_monsters[m_currentSelectedMonster]->setDefending(true);
								
				m_visibleItems = 4;
				m_currentSelectedOption = 0;
				m_currentOffset = 0;
				m_currentMax = 4;
				m_currentSelectedMonster = -1;
				std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));

				onDrawGeneralUI(false);

			}else if (m_currentSelectedOption == 2) {
				
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

				onDrawGeneralUI(false);
				
				if (!m_canSwitch) {				
					m_currentMax = 4;
					m_currentSelectedOption = 2;

					onDrawGeneralUI(true);
				}else {
					onDrawSwitchUI(true);
				}

			}else if (m_currentSelectedOption == 3) {
				
				m_drawTargetUI = true;

				int playerMonsterCount = std::count_if(m_monsters.begin(), m_monsters.end(), [](const Monster* monster) { return monster->getCell().flipped; });

				m_currentMax = m_currentTarget == "player" ? playerMonsterCount : static_cast<int>(m_monsters.size()) - playerMonsterCount;
				m_visibleItems = m_currentMax;

				m_currentSelectedOption = 0;
				m_currentOffset = 0;
				m_cutOff = playerMonsterCount;
				m_catchMonster = true;

				onDrawGeneralUI(false);

				if (!m_canCatch) {
					m_catchMonster = false;				
					m_drawTargetUI = false;
					m_currentMax = 4;
					m_visibleItems = 4;
					m_currentSelectedOption = 3;
					m_cutOff = 0;

					onDrawGeneralUI(true);
				}
			}
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_ESCAPE)) {
		if (m_drawGeneralUi) {
			m_monsters[m_currentSelectedMonster]->setDefending(true);
			
			m_visibleItems = 4;
			m_currentSelectedOption = 0;
			m_currentOffset = 0;
			m_currentMax = 4;
			m_currentSelectedMonster = -1;
			std::for_each(m_monsters.begin(), m_monsters.end(), std::mem_fn(&Monster::unPause));

			onDrawGeneralUI(false);
		}

		if (m_drawAtacksUi || m_drawSwitchUi || m_drawTargetUI) {
			if (m_drawTargetUI) {
				std::for_each(m_monsters.begin(), m_monsters.end(), std::bind(std::mem_fn<void(bool)>(&Monster::setHighlight), std::placeholders::_1, false));
				m_monsters[m_currentSelectedMonster]->setHighlight(true);
				m_currentSelectedOption = 0;
			}

			if (m_drawAtacksUi) {
				m_currentSelectedOption = 0;
				onDrawAttacksUI(false);
			}

			if (m_drawSwitchUi) {
				m_currentSelectedOption = 2;
				onDrawSwitchUI(false);
			}
			
			m_drawTargetUI = false;
			m_catchMonster = false;	
			m_currentMax = 4;		
			m_currentOffset = 0;	

			onDrawGeneralUI(true);
			
		}
	}
}

void Battle::onAbilityEnd() {
	
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

	m_drawTargetUI = false;
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
			if (m_monsters[index]->getCell().flipped && growSupplyIndexPlayer()) {
							
				m_monsters[index]->setGraphic(static_cast<int>(MonsterIndex::MonsterData[MonsterIndex::Monsters[m_supplyIndexPlayer].name].graphic * 16u));
				m_monsters[index]->setMonsterEntry(MonsterIndex::Monsters[m_supplyIndexPlayer]);
				m_monsters[index]->setInitiative(0.0f);
				m_monsters[index]->setKilled(false);
				m_monsters[index]->setHighlight(false);
				m_monsters[index]->canAttack();
				
				m_canSwitch = canSwitch();
			}else if(m_supplyIndexOpponent < static_cast<int>(m_opponentMonsters.size()) - 1 && !m_monsters[index]->getCell().flipped) {
				m_supplyIndexOpponent++;
					
				m_monsters[index]->setGraphic(static_cast<int>(MonsterIndex::MonsterData[m_opponentMonsters[m_supplyIndexOpponent].name].graphic * 16u));
				m_monsters[index]->setMonsterEntry(m_opponentMonsters[m_supplyIndexOpponent]);
				m_monsters[index]->setInitiative(0.0f);
				m_monsters[index]->setKilled(false);
				m_monsters[index]->setHighlight(false);
				m_monsters[index]->canAttack();

			}else {
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
	playAttackSound(MonsterIndex::_AttackData[ability->first].animation);
	target->applyAttack(amount, MonsterIndex::_AttackData[ability->first]);
		
	m_abilityPosX = target->getCell().centerX;
	m_abilityPosY = target->getCell().centerY;
	m_abilityOffset = MonsterIndex::_AttackData[ability->first].graphic;
	currentMonster->reduceEnergy(MonsterIndex::_AttackData[ability->first]);

	onDrawAbilityUI(true);
}

void Battle::exit() {
	m_fade.setOnFadeOut([&m_isRunning = m_isRunning]() {
		m_isRunning = false;
	});
	m_fade.fadeOut();
	Globals::musicManager.get("background").stop();
}

Fade& Battle::getFade() {
	return m_fade;
}

void Battle::setOpponentMonsters() {

	m_opponentMonsters.push_back({ "Atrox", 3u, 3.0f, 200.0f, 0.0f, false });
	m_opponentMonsters.push_back({ "Finiette", 4u, 0.0f, 20.0f, 0.0f, false });
	m_opponentMonsters.push_back({ "Pouch", 5u, 3.0f, 20.0f, 0.0f, false });
	m_opponentMonsters.push_back({ "Finsta", 2u, 0.0f, 20.0f, 0.0f, false });
	m_opponentMonsters.push_back({ "Cleaf", 2u, 3.0f, 20.0f, 0.0f, false });
	m_opponentMonsters.push_back({ "Friolera", 5u, 0.0f, 20.0f, 0.0f, false });

	int defeated = std::count_if(MonsterIndex::Monsters.begin(), MonsterIndex::Monsters.end(), [](const MonsterEntry monsterEntry) { return monsterEntry.health <= 0.0f; });

	MonsterIndex::Monsters.reserve(MonsterIndex::Monsters.size() + m_opponentMonsters.size());
	m_supplyIndexPlayer = std::max(std::min(2, static_cast<int>(MonsterIndex::Monsters.size() - defeated) - 1), 0);

	Monster* monster; int k = 0;
	for (int i = 0; i < std::min(static_cast<int>(MonsterIndex::Monsters.size() - defeated), 3); i++) {
		if (MonsterIndex::Monsters[i + k].health <= 0) {
			m_supplyIndexPlayer++;
			k++;
			i--;
			continue;
		}

		m_cells.push_back({ positions[i][0], positions[i][1], 192.0f, 192.0f, static_cast<int>(MonsterIndex::MonsterData[MonsterIndex::Monsters[i + k].name].graphic * 16u), centers[i][0], centers[i][1], true, true });
		monster = addChild<Monster>(m_cells.back(), MonsterIndex::Monsters[i + k], Vector2f(positions[i][0], positions[i][1]));
		m_monsters.push_back(monster);
	}

	std::for_each(m_opponentMonsters.begin(), m_opponentMonsters.end(), std::mem_fn(&MonsterEntry::resetStats));
	m_supplyIndexOpponent = std::max(std::min(2, static_cast<int>(m_opponentMonsters.size()) - 1), 0);
	for (int i = 0; i < std::min(static_cast<int>(m_opponentMonsters.size()), 3); i++) {
		m_cells.push_back({ positions[i + 3][0], positions[i + 3][1], 192.0f, 192.0f, static_cast<int>(MonsterIndex::MonsterData[m_opponentMonsters[i].name].graphic * 16u), centers[i + 3][0], centers[i + 3][1], true, false });
		monster = addChild<Monster>(m_cells.back(), m_opponentMonsters[i], Vector2f(positions[i + 3][0], positions[i][1]));
		m_monsters.push_back(monster);
	}

	m_canSwitch = canSwitch();
}

void Battle::setOpponentMonsters(const std::vector<MonsterEntry>& monsters, bool canCatch) {

	m_opponentMonsters.assign(monsters.begin(), monsters.end());
	int defeated = std::count_if(MonsterIndex::Monsters.begin(), MonsterIndex::Monsters.end(), [](const MonsterEntry monsterEntry) { return monsterEntry.health <= 0.0f; });

	MonsterIndex::Monsters.reserve(MonsterIndex::Monsters.size() + m_opponentMonsters.size());
	m_supplyIndexPlayer = std::max(std::min(2, static_cast<int>(MonsterIndex::Monsters.size() - defeated) - 1), 0);

	Monster* monster; int k = 0;
	for (int i = 0; i < std::min(static_cast<int>(MonsterIndex::Monsters.size() - defeated), 3); i++) {
		if (MonsterIndex::Monsters[i + k].health <= 0) {
			m_supplyIndexPlayer++;
			k++;
			i--;		
			continue;		
		}

		m_cells.push_back({ positions[i][0], positions[i][1], 192.0f, 192.0f, static_cast<int>(MonsterIndex::MonsterData[MonsterIndex::Monsters[i + k].name].graphic * 16u), centers[i][0], centers[i][1], true, true });
		monster = addChild<Monster>(m_cells.back(), MonsterIndex::Monsters[i + k], Vector2f(positions[i][0], positions[i][1]));
		m_monsters.push_back(monster);
	}

	std::for_each(m_opponentMonsters.begin(), m_opponentMonsters.end(), std::mem_fn(&MonsterEntry::resetStats));
	m_supplyIndexOpponent = std::max(std::min(2, static_cast<int>(m_opponentMonsters.size()) - 1), 0);
	for (int i = 0; i < std::min(static_cast<int>(m_opponentMonsters.size()), 3); i++) {
		m_cells.push_back({ positions[i + 3][0], positions[i + 3][1], 192.0f, 192.0f, static_cast<int>(MonsterIndex::MonsterData[m_opponentMonsters[i].name].graphic * 16u), centers[i + 3][0], centers[i + 3][1], true, false });
		monster = addChild<Monster>(m_cells.back(), m_opponentMonsters[i], Vector2f(positions[i + 3][0], positions[i][1]));
		m_monsters.push_back(monster);
	}

	m_canSwitch = canSwitch();
	m_canCatch = canCatch;
}

void Battle::setBiomeBackground(const std::string& biomeBackground) {
	if(biomeBackground == "forest")
		BackgroundRect.frame = 0u;
	else if(biomeBackground == "ice")
		BackgroundRect.frame = 1u;
	else if(biomeBackground == "sand")
		BackgroundRect.frame = 2u;
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

	ui::Surface* atacksUI = findChild<ui::Surface>("attacks");

	ui::Surface* surface = atacksUI->addChild<ui::Surface>();
	surface->setPosition(0.0f, 1.0f - (position + 1) * invLimiter);
	surface->setScale(1.0f, invLimiter);
	surface->setColor(Vector4f(0.78431f, 0.78431f, 0.78431f, 1.0f));
	surface->setEdge(position == 0 ? ui::Edge::TOP : position == std::min(m_visibleItems, m_currentMax - m_currentOffset) - 1 ? ui::Edge::BOTTOM : ui::Edge::EDGE_NONE);
	surface->setBorderRadius(12.0f);

	ui::Label* label; int index = 0;
	for (auto& ability = m_abilitiesFiltered.begin() + m_currentOffset; ability != m_abilitiesFiltered.begin() + m_currentOffset + std::min(m_visibleItems, m_currentMax - m_currentOffset); ++ability) {
		label = atacksUI->addChild<ui::Label>(Globals::fontManager.get("dialog"));
		label->setPosition(0.5f, 1.0f - (index + 1) * invLimiter + 0.5 * invLimiter);
		label->translateRelative(-0.5f * Globals::fontManager.get("dialog").getWidth((*ability).first) * 0.045f, -lineHeight * 0.5);
		label->setScale(1.0f, 1.0f);
		label->setSize(0.045f);
		label->setLabel((*ability).first);
		label->setTextColor((index == m_currentSelectedOption - m_currentOffset) ? MonsterIndex::_AttackData[(*ability).first].element == "normal" ? Vector4f(0.0f, 0.0f, 0.0f, 1.0f) : MonsterIndex::ColorMap[MonsterIndex::_AttackData[(*ability).first].element] : Vector4f(0.78431f, 0.78431f, 0.78431f, 1.0f));
		index++;
	}
}

void Battle::eraseAbilities() {
	findChild<ui::Surface>("attacks")->eraseAllChildren();
}

void Battle::addListMonsters() {
	const std::vector<TextureRect>& iconRects = TileSetManager::Get().getTileSet("monster_icon").getTextureRects();

	float invLimiter = 0.25f;
	int position = m_currentSelectedOption - m_currentOffset;
	float lineHeight = Globals::fontManager.get("dialog").lineHeight * 0.045f;

	ui::Surface* switchUI = findChild<ui::Surface>("switch");

	ui::Surface* surface = switchUI->addChild<ui::Surface>();
	surface->setPosition(0.0f, 1.0f - (position + 1) * invLimiter);
	surface->setScale(1.0f, invLimiter);
	surface->setColor(Vector4f(0.78431f, 0.78431f, 0.78431f, 1.0f));
	surface->setEdge(position == 0 ? ui::Edge::TOP : position == 3 ? ui::Edge::BOTTOM : ui::Edge::EDGE_NONE);
	surface->setBorderRadius(12.0f);

	ui::Icon* icon; ui::Label* label; ui::Bar* bar;

	for (size_t index = m_currentOffset; index < m_currentOffset + std::min(m_visibleItems, m_currentMax - m_currentOffset); index++) {
		
		const TextureRect& iconRect = iconRects[MonsterIndex::MonsterData[m_filteredMonsters[index].get().name].graphic];
		icon = switchUI->addChild<ui::Icon>(iconRect);
		icon->setPosition(0.0f, 1.0f - (index - m_currentOffset + 1) * invLimiter + 0.5 * invLimiter);
		icon->translateRelative(10.0f + 0.5f * iconRect.width, 0.0f);
		icon->setAligned(true);
		icon->setSpriteSheet(TileSetManager::Get().getTileSet("monster_icon").getAtlas());

		label = switchUI->addChild<ui::Label>(Globals::fontManager.get("dialog"));
		label->setPosition(0.0f, 1.0f - (index - m_currentOffset + 1) * invLimiter + 0.5 * invLimiter);
		label->translateRelative(90.0f, -lineHeight * 0.5);
		label->setScale(1.0f, 1.0f);
		label->setSize(0.045f);
		label->setLabel(m_filteredMonsters[index].get().name + " " + "(" + std::to_string(m_filteredMonsters[index].get().level) + ")");
		label->setTextColor(index == m_currentSelectedOption ? Vector4f(0.94117f, 0.19215f, 0.19215f, 1.0f) : Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

		bar = switchUI->addChild<ui::Bar>(TileSetManager::Get().getTileSet("bars"));
		bar->setPosition(0.0f, 1.0f - (index - m_currentOffset + 1) * invLimiter + 0.5 * invLimiter);
		bar->translateRelative(90.0f, -lineHeight * 0.5 - 9.0f);
		bar->setRadius(0.0f);
		bar->setBgColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
		bar->setColor(Vector4f(0.94117f, 0.19215f, 0.19215f, 1.0f));
		bar->setWidth(100.0f);
		bar->setHeight(5.0f);
		bar->setMaxValue(static_cast<float>(m_filteredMonsters[index].get().level * MonsterIndex::MonsterData[m_filteredMonsters[index].get().name].maxHealth));
		bar->setValue(m_filteredMonsters[index].get().health);

		bar = switchUI->addChild<ui::Bar>(TileSetManager::Get().getTileSet("bars"));
		bar->setPosition(0.0f, 1.0f - (index - m_currentOffset + 1) * invLimiter + 0.5 * invLimiter);
		bar->translateRelative(90.0f, -lineHeight * 0.5 - 16.0f);
		bar->setRadius(0.0f);
		bar->setBgColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
		bar->setColor(Vector4f(0.4f, 0.84313f, 0.93333f, 1.0f));
		bar->setWidth(100.0f);
		bar->setHeight(5.0f);
		bar->setMaxValue(static_cast<float>(m_filteredMonsters[index].get().level * MonsterIndex::MonsterData[m_filteredMonsters[index].get().name].maxEnergy));
		bar->setValue(m_filteredMonsters[index].get().energy);
	}
}

void Battle::eraseListMonsters() {
	findChild<ui::Surface>("switch")->eraseAllChildren();
}

void Battle::addGeneralUI(float posX, float posY) {

	ui::Empty* generalUI = addChild<ui::Empty>();
	generalUI->setName("general");
	generalUI->setPosition(0.0f, 0.0f);
	generalUI->translateRelative(posX, posY);
	
	ui::IconAnimated* iconAnimated = generalUI->addChild<ui::IconAnimated>(TileSetManager::Get().getTileSet("battle_icon").getTextureRects());
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("battle_icon").getAtlas());
	iconAnimated->setAligned(true);
	iconAnimated->setPosition(0.0f, 0.0f);
	iconAnimated->translateRelative(30.0f, 60.0f);

	iconAnimated = generalUI->addChild<ui::IconAnimated>(TileSetManager::Get().getTileSet("battle_icon").getTextureRects());
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("battle_icon").getAtlas());
	iconAnimated->setAligned(true);
	iconAnimated->setPosition(0.0f, 0.0f);
	iconAnimated->translateRelative(40.0f, 20.0f);

	iconAnimated = generalUI->addChild<ui::IconAnimated>(TileSetManager::Get().getTileSet("battle_icon").getTextureRects());
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("battle_icon").getAtlas());
	iconAnimated->setAligned(true);
	iconAnimated->setPosition(0.0f, 0.0f);
	iconAnimated->translateRelative(40.0f, -20.0f);

	iconAnimated = generalUI->addChild<ui::IconAnimated>(TileSetManager::Get().getTileSet("battle_icon").getTextureRects());
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("battle_icon").getAtlas());
	iconAnimated->setAligned(true);
	iconAnimated->setPosition(0.0f, 0.0f);
	iconAnimated->translateRelative(30.0f, -60.0f);
}

void Battle::eraseGeneralUI() {
	findChild<ui::Empty>("general")->eraseSelf();
}

void Battle::addAttacksUI(float posX, float posY, float scaleX, float scaleY) {
	ui::Surface* attacksUI = addChild<ui::Surface>();
	attacksUI->setName("attacks");
	attacksUI->setPosition(0.0f, 0.0f);
	attacksUI->translateRelative(posX, posY);
	attacksUI->setBorderRadius(12.0f);
	attacksUI->setEdge(ui::Edge::ALL);
	attacksUI->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	attacksUI->setScale(1.0f, 1.0f);
	attacksUI->scaleAbsolute(scaleX, scaleY);
}

void Battle::eraseAttacksUI() {
	findChild<ui::Surface>("attacks")->eraseSelf();
}

void Battle::addSwitchUI(float posX, float posY, float scaleX, float scaleY) {
	ui::Surface* switchUI = addChild<ui::Surface>();
	switchUI->setName("switch");
	switchUI->setPosition(0.0f, 0.0f);
	switchUI->translateRelative(posX, posY);
	switchUI->setBorderRadius(12.0f);
	switchUI->setEdge(ui::Edge::ALL);
	switchUI->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	switchUI->setScale(1.0f, 1.0f);
	switchUI->scaleAbsolute(scaleX, scaleY);
}

void Battle::eraseSwitchUI() {
	findChild<ui::Surface>("switch")->eraseSelf();
}

void Battle::addAbilityUI(float posX, float posY) {
	ui::IconAnimated* abilityUI = addChild<ui::IconAnimated>(TileSetManager::Get().getTileSet("attacks").getTextureRects());
	abilityUI->setName("ability");
	abilityUI->setSpriteSheet(TileSetManager::Get().getTileSet("attacks").getAtlas());
	abilityUI->setAligned(true);
	abilityUI->setPosition(0.0f, 0.0f);
	abilityUI->translateRelative(posX, posY);
}

void Battle::eraseAbilityUI() {
	findChild<ui::IconAnimated>("ability")->eraseSelf();
}

void Battle::onDrawGeneralUI(bool flag) {
	m_drawGeneralUi = flag;

	if (m_drawGeneralUi) {
		Monster* currentMonster = m_monsters[m_currentSelectedMonster];
		const Cell& cell = currentMonster->getCell();
		addGeneralUI(cell.posX + cell.width, cell.posY + 0.5f * cell.height);
	}else {
		eraseGeneralUI();
	}
}

void Battle::onDrawAttacksUI(bool flag) {
	m_drawAtacksUi = flag;

	if (m_drawAtacksUi) {
		Monster* currentMonster = m_monsters[m_currentSelectedMonster];
		const Cell& cell = currentMonster->getCell();
		float width = 150.0f;
		float height = 200.0f;
		int limiter = std::min(m_visibleItems, m_currentMax);
		height = limiter <= 1 ? 50.0f : limiter == 2 ? 100.0f : limiter == 3 ? 150.0f : 200.0f;

		addAttacksUI(cell.posX + cell.width + 20.0f, cell.posY + 0.5f * cell.height - 0.5f * height, width, height);
	}else {
		eraseAttacksUI();
	}
}

void Battle::onDrawSwitchUI(bool flag) {
	m_drawSwitchUi = flag;

	if (m_drawSwitchUi) {
		Monster* currentMonster = m_monsters[m_currentSelectedMonster];
		const Cell& cell = currentMonster->getCell();
		float width = 300.0f;
		float height = 320.0f;

		addSwitchUI(cell.posX + cell.width + 20.0f, cell.posY + 0.5f * cell.height - 0.5f * height, width, height);
	}else {
		eraseSwitchUI();
	}
}

void Battle::onDrawAbilityUI(bool flag) {
	m_playAbility = flag;

	if (m_playAbility) {
		addAbilityUI(m_abilityPosX, m_abilityPosY);
	}else {
		eraseAbilityUI();
	}
}

bool Battle::canSwitch() {
	return std::count_if(MonsterIndex::Monsters.begin() + m_supplyIndexPlayer + 1, MonsterIndex::Monsters.end(), [](const MonsterEntry& monsterEntry) { return monsterEntry.health > 0.0f; }) != 0;
}

int Battle::getDefeated() {
	return std::count_if(MonsterIndex::Monsters.begin(), MonsterIndex::Monsters.end(), [](const MonsterEntry monsterEntry) { return monsterEntry.health <= 0.0f; });
}

bool Battle::growSupplyIndexPlayer() {
	if (m_supplyIndexPlayer == static_cast<int>(MonsterIndex::Monsters.size()) - 1)
		return false;

	for (unsigned int index = m_supplyIndexPlayer + 1; index < MonsterIndex::Monsters.size(); index++) {
		if (MonsterIndex::Monsters[index].health == 0.0f) {		
			continue;
		}else {
			m_supplyIndexPlayer = index;
			return true;
		}
	}
	return false;
}

void Battle::playAttackSound(const std::string& attackName) {
	if (attackName == "scratch" || attackName == "ice")
		Globals::soundManager.get("game").play("res/audio/" + attackName + ".mp3");
	else
		Globals::soundManager.get("game").play("res/audio/" + attackName + ".wav");
}