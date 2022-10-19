#include "Game.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), player(Player::Get()) {
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

	spell = dynamic_cast<GeneralRayDamageSpell*>(Player::Get().getSpellbook()[1]);
}

Game::~Game() {}

void Game::fixedUpdate() {

}

void Game::update() {
	//ViewPort::get().update(m_dt);
	spell->update(m_dt);
	player.update(m_dt);
	ViewPort::get().setPosition(Player::Get().getPosition());

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

	if (mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {
		spell->startAnimation();
	}
}


float degrees = 0.0f;
void Game::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);


	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);
	

	zone->drawZoneBatched();
	player.draw();


	if (!spell->waitForAnimation()) {
		degrees = asin((player.getYPos() - ViewPort::get().getCursorPosY()) / sqrt((pow(player.getXPos() - ViewPort::get().getCursorPosX(), 2) + pow(player.getYPos() - ViewPort::get().getCursorPosY(), 2)))) * 57.296;
		degrees += 90;

		if (player.getXPos() < ViewPort::get().getCursorPosX()) {
			degrees = -degrees;
		}
	}
	

	spell->draw(player.getXPos() - 128.0f, player.getYPos() + 32.0f, degrees);

	dawnInterface->DrawInterface();
	//dawnInterface->DrawCursor(m_drawInGameCursor);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	//m_drawInGameCursor = !event.titleBar;
	//Mouse::instance().hideCursor(m_drawInGameCursor);
}