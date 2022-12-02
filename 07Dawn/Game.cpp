#include "Game.h"
#include "Magic.h"
#include "TextWindow.h"
#include "Shop.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), m_player(Player::Get()) {
	EventDispatcher::AddMouseListener(this);
	
	Mouse::SetCursorIcon("res/cursors/pointer.cur");
	
	
	LuaFunctions::executeLuaFile("res/_lua/playerdata_w.lua");
	Player::Get().setCharacterType("player_w");
	Player::Get().setClass(Enums::CharacterClass::Liche);

	Interface::Get().setPlayer(&Player::Get());
	Spellbook::Get().setPlayer(&Player::Get());
	
	LuaFunctions::executeLuaFile("res/_lua/spells.lua");
	LuaFunctions::executeLuaFile("res/_lua/mobdata_wolf.lua");
	LuaFunctions::executeLuaFile("res/_lua/itemdatabase.lua");

	DialogCanvas::initTextures();
	ShopCanvas::Get().init();
	QuestCanvas::Get().init();
	Interface::Get().init();
	//DawnInterface::enterZone("res/_lua/zone1", 512, 400);
	//DawnInterface::enterZone("res/_lua/zone1", 747, 1530);	
	DawnInterface::enterZone("res/_lua/arinoxGeneralShop", -158, 0);
	
	LuaFunctions::executeLuaFile("res/_lua/gameinit.lua");
	DawnInterface::clearLogWindow();


	Spellbook::Get().reloadSpellsFromPlayer();

	GLfloat color[] = { 1.0f, 1.0f, 0.0f };
	DawnInterface::addTextToLogWindow(color, "Welcome to the world of Dawn, %s.", m_player.getName().c_str());
}

Game::~Game() {}

void Game::fixedUpdate() {

}

void Game::update() {
	//ViewPort::get().update(m_dt);

	processInput();
	Interface::Get().processInputRightDrag();
		
	for (auto it = TextWindow::GetTextWindows().begin(); it != TextWindow::GetTextWindows().end();) {
		short index = static_cast<short>(std::distance(TextWindow::GetTextWindows().begin(), it));
		TextWindow *curTextWindow = *it;		

		if (curTextWindow->canBeDeleted() == true) {
			curTextWindow->close();

			delete curTextWindow;
			it = TextWindow::GetTextWindows().erase(TextWindow::GetTextWindows().begin() + index);
			//TextWindow::Get().RemoveTextWindow(index);
		}else {
			++it;
		}
	}

	for (unsigned int i = 0; i < ZoneManager::Get().getCurrentZone()->MagicMap.size(); ++i) {
		ZoneManager::Get().getCurrentZone()->MagicMap[i]->process();
		ZoneManager::Get().getCurrentZone()->MagicMap[i]->getSpell()->inEffect(m_dt);
		ZoneManager::Get().getCurrentZone()->cleanupActiveAoESpells();

		if (ZoneManager::Get().getCurrentZone()->MagicMap[i]->isDone()) {
			ZoneManager::Get().getCurrentZone()->MagicMap.erase(ZoneManager::Get().getCurrentZone()->MagicMap.begin() + i);
		}
	}

	for (const auto& npc : ZoneManager::Get().getCurrentZone()->getNPCs()) {
		npc->update(m_dt);	
	}

	// check all active spells for inEffects on our player.
	std::vector<SpellActionBase*> activeSpellActions = m_player.getActiveSpells();
	for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpellActions.size(); ++curActiveSpellNr) {
		activeSpellActions[curActiveSpellNr]->inEffect(m_dt);
	}

	m_player.update(m_dt);
	ViewPort::get().setPosition(Player::Get().getPosition());

	
}

void Game::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);


	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);
	
	ZoneManager::Get().getCurrentZone()->drawZoneBatched();
	ZoneManager::Get().getCurrentZone()->getGroundLoot()->draw();

	// draw the interactions on screen
	std::vector<InteractionPoint*> zoneInteractionPoints = ZoneManager::Get().getCurrentZone()->getInteractionPoints();
	for (size_t curInteractionNr = 0; curInteractionNr<zoneInteractionPoints.size(); ++curInteractionNr){
		InteractionPoint* curInteraction = zoneInteractionPoints[curInteractionNr];
		curInteraction->draw();
	}

	// draw AoE spells
	std::vector<std::pair<SpellActionBase*, uint32_t> > activeAoESpells = ZoneManager::Get().getCurrentZone()->getActiveAoESpells();
	for (size_t curActiveAoESpellNr = 0; curActiveAoESpellNr < activeAoESpells.size(); ++curActiveAoESpellNr) {
		if (!activeAoESpells[curActiveAoESpellNr].first->isEffectComplete()) {
			activeAoESpells[curActiveAoESpellNr].first->draw();
		}
	}
	ZoneManager::Get().getCurrentZone()->drawNpcsBatched();
	m_player.draw();
	
	std::vector<Npc*> zoneNPCs = ZoneManager::Get().getCurrentZone()->getNPCs();
	for (unsigned int x = 0; x < zoneNPCs.size(); x++){
		Npc *curNPC = zoneNPCs[x];
		
		// draw the spell effects for our NPCs
		std::vector<SpellActionBase*> activeSpellActions = curNPC->getActiveSpells();
		for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpellActions.size(); ++curActiveSpellNr){
			if (!activeSpellActions[curActiveSpellNr]->isEffectComplete()){
				activeSpellActions[curActiveSpellNr]->draw();
			}
		}
	}

	std::vector<SpellActionBase*> activeSpellActions = m_player.getActiveSpells();
	for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpellActions.size(); ++curActiveSpellNr) {
		if (!activeSpellActions[curActiveSpellNr]->isEffectComplete()){
			activeSpellActions[curActiveSpellNr]->draw();
		}
	}

	ZoneManager::Get().getCurrentZone()->getGroundLoot()->drawTooltip(ViewPort::get().getCursorPosX(), ViewPort::get().getCursorPosY());

	for (size_t curInteractionNr = 0; curInteractionNr<zoneInteractionPoints.size(); ++curInteractionNr){
		InteractionPoint *curInteraction = zoneInteractionPoints[curInteractionNr];
		curInteraction->drawInteractionSymbol(ViewPort::get().getCursorPosX(), ViewPort::get().getCursorPosY(), m_player.getXPos(), m_player.getYPos());	
	}

	for (auto it = TextWindow::GetTextWindows().begin(); it != TextWindow::GetTextWindows().end(); ++it) {
		(*it)->draw();
	}

	Interface::Get().draw();
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	//m_drawInGameCursor = !event.titleBar;
	//Mouse::instance().hideCursor(m_drawInGameCursor);

}

void Game::resize(int deltaW, int deltaH) {
	Interface::Get().resize(deltaW, deltaH);
}

void Game::processInput() {
	
	Mouse &mouse = Mouse::instance();
	Keyboard &keyboard = Keyboard::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		ZoneManager::Get().getCurrentZone()->getGroundLoot()->searchForItems(ViewPort::get().getCursorPosX(), ViewPort::get().getCursorPosY());
		// get and iterate through the NPCs
		std::vector<Npc*> zoneNPCs = ZoneManager::Get().getCurrentZone()->getNPCs();
		for (unsigned int x = 0; x < zoneNPCs.size(); x++) {
			Npc* curNPC = zoneNPCs[x];
			// is the mouse over a NPC and no AoE spell is being prepared?
			if (curNPC->CheckMouseOver(ViewPort::get().getCursorPosX(), ViewPort::get().getCursorPosY())) {
				// is the NPC friendly?
				if (!curNPC->getAttitude() == Enums::Attitude::FRIENDLY) {
					// set a target if the player has none
					if (!m_player.hasTarget(curNPC)) {
						m_player.setTarget(curNPC, curNPC->getAttitude());
					}else {
						m_player.setTarget(NULL);
					}
					break;
				}
			}


		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_F)) {
		Message::Get().addText(ViewPort::get().getWidth() / 2, ViewPort::get().getHeight() / 2, 1.0f, 0.625f, 0.71f, 1.0f, 15u, 3.0f, "Zone saved ...");
		return;
	}


	if (keyboard.keyDown(Keyboard::KEY_LALT)){
		ZoneManager::Get().getCurrentZone()->getGroundLoot()->enableTooltips();
	}else {
		ZoneManager::Get().getCurrentZone()->getGroundLoot()->disableTooltips();
	}

	std::vector<InteractionPoint*> zoneInteractionPoints = ZoneManager::Get().getCurrentZone()->getInteractionPoints();
	for (size_t curInteractionNr = 0; curInteractionNr< zoneInteractionPoints.size(); ++curInteractionNr) {
		InteractionPoint *curInteraction = zoneInteractionPoints[curInteractionNr];
		curInteraction->processInput(ViewPort::get().getCursorPosX(), ViewPort::get().getCursorPosY(), m_player.getXPos(), m_player.getYPos());
	}

	for (auto it = TextWindow::GetTextWindows().begin(); it != TextWindow::GetTextWindows().end(); ++it) {
		(*it)->processInput();
	}
}