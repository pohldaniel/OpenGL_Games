#include "Game.h"
#include "Magic.h"
#include "TextWindow.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), player(Player::Get()) {
	EventDispatcher::AddMouseListener(this);
	
	Mouse::SetCursorIcon("res/cursors/pointer.cur");
	
	LuaFunctions::executeLuaFile("res/_lua/playerdata_w.lua");
	Player::Get().setCharacterType("player_w");
	Player::Get().setClass(Enums::CharacterClass::Liche);

	player = Player::Get();

	LuaFunctions::executeLuaFile("res/_lua/spells.lua");
	LuaFunctions::executeLuaFile("res/_lua/mobdata_wolf.lua");

	ZoneManager::Get().getZone("res/_lua/zone1").loadZone();
	ZoneManager::Get().setCurrentZone(&ZoneManager::Get().getZone("res/_lua/zone1"));
	zone = ZoneManager::Get().getCurrentZone();

	dawnInterface = &Interface::Get();
	dawnInterface->init();	

	DialogCanvas::initTextures();

	// setting initial actions in the action bar
	const std::vector<CSpellActionBase*> inscribedSpells = Player::Get().getSpellbook();
	for (short curEntry = 0; curEntry <= 9 && curEntry < inscribedSpells.size(); ++curEntry) {
		dawnInterface->bindActionToButtonNr(curEntry, inscribedSpells[curEntry]);
	}

	GLfloat color[] = { 1.0f, 1.0f, 0.0f };
	DawnInterface::addTextToLogWindow(color, "Welcome to the world of Dawn, %s.", player.getName().c_str());
}

Game::~Game() {}

void Game::fixedUpdate() {

}

void Game::update() {
	//ViewPort::get().update(m_dt);

	processInput();
	dawnInterface->processInputRightDrag();
	
	for (auto it = TextWindow::GetTextWindows().begin(); it != TextWindow::GetTextWindows().end(); ++it) {
		short index = static_cast<short>(std::distance(TextWindow::GetTextWindows().begin(), it));
		TextWindow *curTextWindow = *it;		
		if (curTextWindow->canBeDeleted() == true) {
			curTextWindow->close();
			curTextWindow->toggle();

			delete curTextWindow;
			TextWindow::RemoveTextWindow(index);
		}
	}

	for (unsigned int i = 0; i < zone->MagicMap.size(); ++i) {
		zone->MagicMap[i]->process();
		zone->MagicMap[i]->getSpell()->inEffect(m_dt);
		zone->cleanupActiveAoESpells();

		if (zone->MagicMap[i]->isDone()) {
			zone->MagicMap.erase(zone->MagicMap.begin() + i);
		}
	}

	for (const auto& npc : zone->getNPCs()) {
		npc->update(m_dt);	
	}

	// check all active spells for inEffects on our player.
	std::vector<CSpellActionBase*> activeSpellActions = player.getActiveSpells();
	for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpellActions.size(); ++curActiveSpellNr) {
		activeSpellActions[curActiveSpellNr]->inEffect(m_dt);
	}

	player.update(m_dt);
	ViewPort::get().setPosition(Player::Get().getPosition());

	
}

void Game::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);


	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);
	
	zone->drawZoneBatched();
	// draw AoE spells
	std::vector<std::pair<CSpellActionBase*, uint32_t> > activeAoESpells = zone->getActiveAoESpells();
	for (size_t curActiveAoESpellNr = 0; curActiveAoESpellNr < activeAoESpells.size(); ++curActiveAoESpellNr) {
		if (!activeAoESpells[curActiveAoESpellNr].first->isEffectComplete()) {
			activeAoESpells[curActiveAoESpellNr].first->draw();
		}
	}
	zone->drawNpcsBatched();
	player.draw();
	
	std::vector<Npc*> zoneNPCs = zone->getNPCs();
	for (unsigned int x = 0; x < zoneNPCs.size(); x++){
		Npc *curNPC = zoneNPCs[x];
		
		// draw the spell effects for our NPCs
		std::vector<CSpellActionBase*> activeSpellActions = curNPC->getActiveSpells();
		for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpellActions.size(); ++curActiveSpellNr){
			if (!activeSpellActions[curActiveSpellNr]->isEffectComplete()){
				activeSpellActions[curActiveSpellNr]->draw();
			}
		}
	}

	std::vector<CSpellActionBase*> activeSpellActions = player.getActiveSpells();
	for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpellActions.size(); ++curActiveSpellNr) {
		if (!activeSpellActions[curActiveSpellNr]->isEffectComplete()){
			activeSpellActions[curActiveSpellNr]->draw();
		}
	}

	dawnInterface->DrawInterface();
	dawnInterface->DrawFloatingSpell();
	//dawnInterface->DrawCursor(m_drawInGameCursor);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	//m_drawInGameCursor = !event.titleBar;
	//Mouse::instance().hideCursor(m_drawInGameCursor);

}

void Game::resize() {
	dawnInterface->resize();
}

void Game::processInput() {
	Mouse &mouse = Mouse::instance();
	Keyboard &keyboard = Keyboard::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		// get and iterate through the NPCs
		std::vector<Npc*> zoneNPCs = zone->getNPCs();
		for (unsigned int x = 0; x < zoneNPCs.size(); x++) {
			Npc* curNPC = zoneNPCs[x];
			// is the mouse over a NPC and no AoE spell is being prepared?
			if (curNPC->CheckMouseOver(ViewPort::get().getCursorPosX(), ViewPort::get().getCursorPosY())) {
				// is the NPC friendly?
				if (!curNPC->getAttitude() == Enums::Attitude::FRIENDLY) {
					// set a target if the player has none

					if (!player.hasTarget(curNPC)) {
						player.setTarget(curNPC, curNPC->getAttitude());
					}else {
						player.setTarget(nullptr);
					}
					break;
				}
			}


		}
	}

	if (keyboard.keyDown(Keyboard::KEY_F)) {
		Message::Get().addText(ViewPort::get().getWidth() / 2, ViewPort::get().getHeight() / 2, 1.0f, 0.625f, 0.71f, 1.0f, 15u, 3.0f, "Zone saved ...");
		return;
	}


	
}