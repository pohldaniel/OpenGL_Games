#include "Game.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), player(Player::Get()) {
	Mouse::SetCursorIcon("res/cursors/pointer.cur");
	
	LuaFunctions::executeLuaFile("res/_lua/mobdata_wolf.lua");
	LuaFunctions::executeLuaFile("res/_lua/playerdata.lua");
	Player::Get().setCharacterType("player_w");

	ZoneManager::Get().getZone("res/_lua/zone1").loadZone();
	ZoneManager::Get().setCurrentZone(&ZoneManager::Get().getZone("res/_lua/zone1"));
	
	zone = ZoneManager::Get().getCurrentZone();
	dawnInterface = &Interface::Get();
	dawnInterface->init();	
}

Game::~Game() {}

void Game::fixedUpdate() {

}

void Game::update() {
	//ViewPort::get().update(m_dt);
	player.update(m_dt);
	ViewPort::get().setPosition(Player::Get().getPosition());

	Mouse &mouse = Mouse::instance();

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
					}else{

						player.setTarget(nullptr);
					}
					break;
				}
			}
		}	
	}
}

void Game::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);


	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);


	zone->drawZoneBatched();
	Player::Get().draw();
	dawnInterface->DrawInterface();
	//dawnInterface->DrawCursor(m_drawInGameCursor);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	//m_drawInGameCursor = !event.titleBar;
	//Mouse::instance().hideCursor(m_drawInGameCursor);
}