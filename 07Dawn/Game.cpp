#include "Game.h"
#include "Magic.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), player(Player::Get()) {
	EventDispatcher::AddMouseListener(this);
	
	Mouse::SetCursorIcon("res/cursors/pointer.cur");
	
	LuaFunctions::executeLuaFile("res/_lua/mobdata_wolf.lua");
	LuaFunctions::executeLuaFile("res/_lua/playerdata_w.lua");

	Player::Get().setCharacterType("player_w");
	Player::Get().setClass(Enums::CharacterClass::Liche);

	player = Player::Get();

	LuaFunctions::executeLuaFile("res/_lua/spells.lua");

	ZoneManager::Get().getZone("res/_lua/zone1").loadZone();
	ZoneManager::Get().setCurrentZone(&ZoneManager::Get().getZone("res/_lua/zone1"));
	zone = ZoneManager::Get().getCurrentZone();

	dawnInterface = &Interface::Get();
	dawnInterface->init();	

	// setting initial actions in the action bar
	const std::vector<CSpellActionBase*> inscribedSpells = Player::Get().getSpellbook();
	for (size_t curEntry = 0; curEntry <= 9 && curEntry < inscribedSpells.size(); ++curEntry) {
		dawnInterface->bindActionToButtonNr(curEntry, inscribedSpells[curEntry]);
	}

	spell = Player::Get().getSpellbook()[1];
}

Game::~Game() {}

void Game::fixedUpdate() {

}

void Game::update() {
	//ViewPort::get().update(m_dt);
	
	

	Mouse &mouse = Mouse::instance();

	if(mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
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
					}else{

						player.setTarget(nullptr);
					}
					break;
				}
			}

		
		}	
	}

	std::vector<Npc*> zoneNPCs = zone->getNPCs();
	for (unsigned int x = 0; x < zoneNPCs.size(); x++) {
		Npc *curNPC = zoneNPCs[x];
		std::vector<std::pair<CSpellActionBase*, uint32_t> > activeSpellActions = curNPC->getActiveSpells();
		for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpellActions.size(); ++curActiveSpellNr) {
			
			activeSpellActions[curActiveSpellNr].first->inEffect(m_dt);
		}
	}

	// check all active spells for inEffects on our player.
	std::vector<std::pair<CSpellActionBase*, uint32_t> > activeSpellActions = player.getActiveSpells();
	for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpellActions.size(); ++curActiveSpellNr) {
		activeSpellActions[curActiveSpellNr].first->inEffect(m_dt);
	}

	player.update(m_dt);
	ViewPort::get().setPosition(Player::Get().getPosition());

	dawnInterface->processInputRightDrag();


	for (unsigned int i = 0; i < zone->MagicMap.size(); ++i) {
		zone->MagicMap[i]->process();
		zone->MagicMap[i]->getSpell()->inEffect(m_dt);
		zone->cleanupActiveAoESpells();

		if (zone->MagicMap[i]->isDone()) {
			zone->MagicMap.erase(zone->MagicMap.begin() + i);
		}
	}
}


float degrees = 0.0f;
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
	for (unsigned int x = 0; x< zoneNPCs.size(); x++){
		Npc *curNPC = zoneNPCs[x];
		/*if (player->getTarget() == curNPC) {
			GUI.drawTargetedNPCText();
		}*/

		// draw the spell effects for our NPCs
		std::vector<std::pair<CSpellActionBase*, uint32_t> > activeSpellActions = curNPC->getActiveSpells();
		for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpellActions.size(); ++curActiveSpellNr){
			if (!activeSpellActions[curActiveSpellNr].first->isEffectComplete()){
				activeSpellActions[curActiveSpellNr].first->draw();
			}
		}
	}

	/*std::vector<std::pair<CSpellActionBase*, uint32_t> > activeSpellActions = player.getActiveSpells();

	for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpellActions.size(); ++curActiveSpellNr) {
		if (!activeSpellActions[curActiveSpellNr].first->isEffectComplete()){
			activeSpellActions[curActiveSpellNr].first->draw();
		}
	}*/

	//spell->draw();

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