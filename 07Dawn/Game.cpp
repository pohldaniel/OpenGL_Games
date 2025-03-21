#include "Game.h"
#include "Editor.h"
#include "Interface.h"
#include "Player.h"
#include "Spells.h"
#include "Zone.h"
#include "Shop.h"
#include "Quest.h"
#include "InventoryCanvas.h"
#include "TextWindow.h"
#include "OptionsWindow.h"
#include "Message.h"

std::vector<TextureRect> Game::TextureRects;
bool Game::s_init = false;

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {
	Mouse::SetCursorIcon("res/cursors/pointer.cur");

	if (!s_init && !Editor::s_init) {

		LuaFunctions::executeLuaFile("res/_lua/playerdata.lua");
		Player::Get().setCharacterType("player");
		Player::Get().setClass(Enums::CharacterClass::Liche);
		Player::Get().init();
		
		LuaFunctions::executeLuaFile("res/_lua/spells.lua");
		LuaFunctions::executeLuaFile("res/_lua/itemdatabase.lua");
		LuaFunctions::executeLuaFile("res/_lua/mobdata_wolf.lua");
		DawnInterface::enterZone("res/_lua/zone1", 512, 400);
		LuaFunctions::executeLuaFile("res/_lua/loadsave.lua");
	}

	if(!s_init){
		Init();

		Interface::Get().setPlayer(&Player::Get());
		Interface::Get().init({ TextureRects.begin(), TextureRects.begin() + 44 });
		Interface::Get().inscribeActionsToActionsbar();

		Spellbook::Get().setPlayer(&Player::Get());
		Spellbook::Get().init({ TextureRects.begin() + 44, TextureRects.begin() + 48 });
		Spellbook::Get().reloadSpellsFromPlayer();

		CharacterInfo::Get().setPlayer(&Player::Get());
		CharacterInfo::Get().init({ TextureRects.begin() + 48, TextureRects.begin() + 53 });

		InventoryCanvas::Get().setPlayer(&Player::Get());
		InventoryCanvas::Get().init({ TextureRects.begin() + 53, TextureRects.begin() + 71 });

		QuestCanvas::Get().init({ TextureRects[71] });
		DialogCanvas::Init({ TextureRects.begin() + 72, TextureRects.begin() + 81 });
		ShopCanvas::Get().init({ TextureRects.begin() + 81, TextureRects.begin() + 86 });
		InteractionPoint::Init({ TextureRects.begin() + 86, TextureRects.begin() + 92 });
		GroundLoot::Init({ TextureRects.begin() + 92, TextureRects.begin() + 95 });
		OptionsWindow::Get().init({ TextureRects[95], TextureRects[47], TextureRects[46], TextureRects[4] });
		ItemTooltip::Init({ TextureRects.begin() + 55, TextureRects.begin() + 58 });

		LuaFunctions::executeLuaFile("res/_lua/gameinit.lua");
		DawnInterface::clearLogWindow();
		GLfloat color[] = { 1.0f, 1.0f, 0.0f };
		DawnInterface::addTextToLogWindow(color, "Welcome to the world of Dawn, %s.", Player::Get().getName().c_str());
		s_init = true;
	}


	if (Editor::s_init) {
		DawnInterface::enterZone();
	}

	//DawnInterface::enterZone("res/_lua/zone1", 747, 1530);	
	//DawnInterface::enterZone("res/_lua/arinoxGeneralShop", -158, 0);
	//Editor::s_init ? DawnInterface::enterZone() : DawnInterface::enterZone("res/_lua/zone1", 747, 1530);

	//becarefull bind the textures atfer the last glDeleteTextures() call
	TextureManager::BindTexture(Globals::spritesheetManager.getAssetPointer("font")->getAtlas(), true, 0);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("interface"), true, 1);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("interface"), true, 2);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("spells"), true, 3);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("player"), true, 4);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("mobs"), true, 5);
	TextureManager::BindTexture(ZoneManager::Get().getCurrentZone()->getTetureAtlas(), true, 6);

	Globals::musicManager.get("background").play("res/music/loading.ogg");

}

Game::~Game() {}

void Game::fixedUpdate() {

}

void Game::update() {
	processInput();

	TextWindow::Update();
	ZoneManager::Get().getCurrentZone()->update(m_dt);

	// making sure our target is still alive, not invisible and still in range while stealthed. if not well set our target to NULL.
	if (Player::Get().getTarget()) {
		double distance = sqrt(pow((Player::Get().getTarget()->getXPos() + Player::Get().getTarget()->getWidth() / 2) - (Player::Get().getXPos() + Player::Get().getWidth() / 2), 2)
			+ pow((Player::Get().getTarget()->getYPos() + Player::Get().getTarget()->getHeight() / 2) - (Player::Get().getYPos() + Player::Get().getHeight() / 2), 2));

		if (Player::Get().getTarget()->isAlive() == false ||
			(Player::Get().getTarget()->isInvisible() == true && Player::Get().canSeeInvisible() == false) ||
			(Player::Get().getTarget()->isSneaking() == true && distance > 260 && Player::Get().canSeeSneaking() == false)) {
			Player::Get().setTarget(NULL);
		}
	}

	Player::Get().update(m_dt);
	ViewPort::Get().setPosition(Player::Get().getPosition());	
	Message::Get().deleteDecayed();
}

void Game::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Zone::Draw();	
	Zone::DrawActiveAoESpells();
	GroundLoot::Draw();
	InteractionPoint::Draw();
	Zone::DrawNpcs();
	Player::Get().draw();
	Player::DrawActiveSpells();
	Npc::DrawActiveSpells();
	GroundLoot::DrawTooltip(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY());
	InteractionPoint::DrawSymbols();
	Interface::Get().draw();
	Message::Get().draw();
	TextureManager::DrawBuffer();
	//Batchrenderer::PrintStatistic();
}

void Game::resize(int deltaW, int deltaH) {
	Interface::Get().resize(deltaW, deltaH);
	TextWindow::Resize(deltaW, deltaH);
}

void Game::processInput() {
	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_F)) {
		Message::Get().addText(ViewPort::Get().getWidth() / 2, ViewPort::Get().getHeight() / 2, 1.0f, 0.625f, 0.71f, 1.0f, 15u, 3.0f, "Zone saved ...");
	}

	Npc::ProcessInput();
	GroundLoot::ProcessInput(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY());
	ZoneManager::Get().getCurrentZone()->processInput(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY(), Player::Get().getXPos(), Player::Get().getYPos());
	Interface::Get().processInputRightDrag();
	OptionsWindow::Get().processInput();

	if (keyboard.keyPressed(Keyboard::KEY_L)) {
		Interface::Get().closeAll();
		OptionsWindow::Get().close();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Editor(m_machine));
	}

	if (keyboard.keyPressed(Keyboard::KEY_TAB)){

		bool FoundNewTarget = false;
		std::vector<Npc*> NPClist;

		// select next npc on screen
		std::vector<Npc*> zoneNPCs = ZoneManager::Get().getCurrentZone()->getNPCs();
		for (size_t curNPCNr = 0; curNPCNr < zoneNPCs.size(); ++curNPCNr) {
			// if NPC is in on screen (might be changed to line of sight or something)
			// this makes a list of all visible NPCs, easier to select next target this way.
			// also makes sure the NPC isn't invisible or sneaking outside of our vision range.
			Npc* curNPC = zoneNPCs[curNPCNr];
			double distance = sqrt(pow((curNPC->getXPos() + curNPC->getWidth() / 2) - (Player::Get().getXPos() + Player::Get().getWidth() / 2), 2)
				+ pow((curNPC->getYPos() + curNPC->getHeight() / 2) - (Player::Get().getYPos() + Player::Get().getHeight() / 2), 2));

			if (TextureManager::IsRectOnScreen(curNPC->getXPos(), 1, curNPC->getYPos(), 1) &&
				curNPC->isAlive() &&
				(curNPC->isInvisible() == false || (curNPC->isInvisible() == true && Player::Get().canSeeInvisible() == true)) &&
				(curNPC->isSneaking() == false || (curNPC->isSneaking() == true && (distance < 260 || Player::Get().canSeeSneaking() == true)))) {
				NPClist.push_back(curNPC);
			}
		}
		// selects next target in the list, if target = NULL, set target to first NPC on the visible list.
		for (size_t curNPC = 0; curNPC < NPClist.size(); ++curNPC) {
			if (!Player::Get().getTarget()) {
				Player::Get().setTarget(NPClist[0], NPClist[0]->getAttitude());
			}

			if (Player::Get().getTarget() == NPClist[curNPC]) {

				if (curNPC + 1 == NPClist.size()) {
					Player::Get().setTarget(NPClist[0], NPClist[0]->getAttitude());

				} else {
					Player::Get().setTarget(NPClist[curNPC + 1], NPClist[curNPC + 1]->getAttitude());
				}
				FoundNewTarget = true;
				break;
			}
		}

		if (!FoundNewTarget && NPClist.size() > 0) {
			Player::Get().setTarget(NPClist[0], NPClist[0]->getAttitude());
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_P)){
		GLfloat color[] = { 100, 100, 255, 255 };
		DawnInterface::addTextToLogWindow(color, "Player X: %d", Player::Get().getXPos());
		DawnInterface::addTextToLogWindow(color, "Player Y: %d", Player::Get().getYPos());
	}
}

void Game::Init() {
	if (s_init) return;

	TextureAtlasCreator::Get().init("interface", 1024, 1024);

	TextureManager::Loadimage("res/lifebar.tga", 0, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/NPCTarget_background.tga", 1, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/NPCTarget_left.tga", 2, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/NPCTarget_right.tga", 3, TextureRects);
	TextureManager::Loadimage("res/white2x2pixel.png", 4, TextureRects);
	TextureManager::Loadimage("res/interface/BuffWindow/frame.tga", 5, TextureRects);

	TextureManager::Loadimage("res/interface/BuffWindow/background.tga", 6, TextureRects);
	TextureManager::Loadimage("res/fear.tga", 7, TextureRects);
	TextureManager::Loadimage("res/stun.tga", 8, TextureRects);
	TextureManager::Loadimage("res/confused.tga", 9, TextureRects);
	TextureManager::Loadimage("res/mesmerized.tga", 10, TextureRects);

	TextureManager::Loadimage("res/interface/Portrait/base.tga", 11, TextureRects);
	TextureManager::Loadimage("res/interface/Portrait/bar.tga", 12, TextureRects);
	TextureManager::Loadimage("res/interface/Portrait/bar_small.tga", 13, TextureRects);
	TextureManager::Loadimage("res/interface/Portrait/Warrior.tga", 14, TextureRects);
	TextureManager::Loadimage("res/interface/Portrait/Liche.tga", 15, TextureRects);
	TextureManager::Loadimage("res/interface/Portrait/Ranger.tga", 16, TextureRects);
	TextureManager::Loadimage("res/cursors/pointer2.png", 17, TextureRects);
	TextureManager::Loadimage("res/interface/BuffWindow/frame.tga", 18, TextureRects);
	TextureManager::Loadimage("res/interface/BuffWindow/background.tga", 19, TextureRects);
	TextureManager::Loadimage("res/interface/blended_bg.tga", 20, TextureRects);

	TextureManager::Loadimage("res/border.tga", 21, TextureRects);
	TextureManager::Loadimage("res/cursors/circle1_enabled.tga", 22, TextureRects);
	TextureManager::Loadimage("res/cursors/circle1_disabled.tga", 23, TextureRects);

	TextureManager::Loadimage("res/interface/combattext/0small.tga", 24, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/1small.tga", 25, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/2small.tga", 26, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/3small.tga", 27, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/4small.tga", 28, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/5small.tga", 29, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/6small.tga", 30, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/7small.tga", 31, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/8small.tga", 32, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/9small.tga", 33, TextureRects);

	TextureManager::Loadimage("res/interface/combattext/0big.tga", 34, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/1big.tga", 35, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/2big.tga", 36, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/3big.tga", 37, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/4big.tga", 38, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/5big.tga", 39, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/6big.tga", 40, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/7big.tga", 41, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/8big.tga", 42, TextureRects);
	TextureManager::Loadimage("res/interface/combattext/9big.tga", 43, TextureRects);

	TextureManager::Loadimage("res/interface/spellbook/base.tga", 44, TextureRects);
	TextureManager::Loadimage("res/interface/spellbook/placeholder.tga", 45, TextureRects);
	TextureManager::Loadimage("res/interface/spellbook/arrow_right.tga", 46, TextureRects);
	TextureManager::Loadimage("res/interface/spellbook/arrow_left.tga", 47, TextureRects);

	TextureManager::Loadimage("res/interface/CharacterInfoScreen/background.tga", 48, TextureRects);
	TextureManager::Loadimage("res/white2x2pixel.png", 49, TextureRects);
	TextureManager::Loadimage("res/interface/CharacterInfoScreen/meleeSelected.tga", 50, TextureRects);
	TextureManager::Loadimage("res/interface/CharacterInfoScreen/defenseSelected.tga", 51, TextureRects);
	TextureManager::Loadimage("res/interface/CharacterInfoScreen/spellsSelected.tga", 52, TextureRects);

	TextureManager::Loadimage("res/interface/inventory/base.tga", 53, TextureRects);
	TextureManager::Loadimage("res/white2x2pixel.png", 54, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/goldcoin.tga", 55, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/silvercoin.tga", 56, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/coppercoin.tga", 57, TextureRects);

	TextureManager::Loadimage("res/interface/inventory/head.tga", 58, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/amulet.tga", 59, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/main_hand.tga", 60, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/chest.tga", 61, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/belt.tga", 62, TextureRects);

	TextureManager::Loadimage("res/interface/inventory/legs.tga", 63, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/shoulder.tga", 64, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/cloak.tga", 65, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/gloves.tga", 66, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/off_hand.tga", 67, TextureRects);

	TextureManager::Loadimage("res/interface/inventory/ring_one.tga", 68, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/ring_two.tga", 69, TextureRects);
	TextureManager::Loadimage("res/interface/inventory/boots.tga", 70, TextureRects);

	TextureManager::Loadimage("res/interface/QuestScreen/questscreen.tga", 71, TextureRects);

	TextureManager::Loadimage("res/interface/tooltip/lower_left2.tga", 72, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/lower_right2.tga", 73, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/upper_left2.tga", 74, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/upper_right2.tga", 75, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/background2.tga", 76, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/upper2.tga", 77, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/lower2.tga", 78, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/left2.tga", 79, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/right2.tga", 80, TextureRects);

	TextureManager::Loadimage("res/interface/Shop/base.tga", 81, TextureRects);
	TextureManager::Loadimage("res/white2x2pixel.png", 82, TextureRects);
	TextureManager::Loadimage("res/interface/Shop/weapontab.tga", 83, TextureRects);
	TextureManager::Loadimage("res/interface/Shop/armortab.tga", 84, TextureRects);
	TextureManager::Loadimage("res/interface/Shop/misctab.tga", 85, TextureRects);

	TextureManager::Loadimage("res/interaction/talk0.tga", 86, TextureRects);
	TextureManager::Loadimage("res/interaction/talk1.tga", 87, TextureRects);
	TextureManager::Loadimage("res/interaction/shop0.tga", 88, TextureRects);
	TextureManager::Loadimage("res/interaction/shop1.tga", 89, TextureRects);
	TextureManager::Loadimage("res/interaction/zone0.tga", 90, TextureRects);
	TextureManager::Loadimage("res/interaction/zone1.tga", 91, TextureRects);

	TextureManager::Loadimage("res/interface/tooltip/groundloot_background.tga", 92, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/groundloot_left.tga", 93, TextureRects);
	TextureManager::Loadimage("res/interface/tooltip/groundloot_right.tga", 94, TextureRects);
	TextureManager::Loadimage("res/interface/OptionsScreen/optionsScreen_big.tga", 95, TextureRects);


	TextureManager::SetTextureAtlas(TextureAtlasCreator::Get().getName(),
		Spritesheet::Merge(TextureManager::GetTextureAtlas("items"), Spritesheet::Merge(TextureManager::GetTextureAtlas("symbols"), TextureAtlasCreator::Get().getAtlas(), true, true), true, true)
	);

	for (unsigned short layer = 0; layer < TextureRects.size(); layer++) {
		TextureRects[layer].frame += 2;
	}

	LuaFunctions::incrementTableRects("symbols");
}

void Game::stop() {
	m_isRunning = false;
}