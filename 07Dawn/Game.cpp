#include "Game.h"
#include "Interface.h"
#include "Player.h"
#include "Spells.h"
#include "Zone.h"
#include "Shop.h"
#include "Quest.h"
#include "InventoryCanvas.h"
#include "TextWindow.h"
#include "Message.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {
	Mouse::SetCursorIcon("res/cursors/pointer.cur");
	
	LuaFunctions::executeLuaFile("res/_lua/playerdata_w.lua");
	Player::Get().setCharacterType("player_w");
	Player::Get().setClass(Enums::CharacterClass::Liche);

	Interface::Get().setPlayer(&Player::Get());
	Spellbook::Get().setPlayer(&Player::Get());
	
	LuaFunctions::executeLuaFile("res/_lua/spells.lua");
	LuaFunctions::executeLuaFile("res/_lua/mobdata_wolf.lua");
	LuaFunctions::executeLuaFile("res/_lua/itemdatabase.lua");

	//DialogCanvas::Init();
	//InventoryCanvas::Get().init();
	//ShopCanvas::Get().init();
	//QuestCanvas::Get().init();
	Interface::Get().init();

	DawnInterface::enterZone("res/_lua/zone1", 512, 400);
	//DawnInterface::enterZone("res/_lua/zone1", 747, 1530);	
	//DawnInterface::enterZone("res/_lua/arinoxGeneralShop", -158, 0);

	LuaFunctions::executeLuaFile("res/_lua/gameinit.lua");
	DawnInterface::clearLogWindow();

	Spellbook::Get().reloadSpellsFromPlayer();

	GLfloat color[] = { 1.0f, 1.0f, 0.0f };
	DawnInterface::addTextToLogWindow(color, "Welcome to the world of Dawn, %s.", Player::Get().getName().c_str());


}

Game::~Game() {}

void Game::fixedUpdate() {

}

void Game::update() {
	processInput();

	TextWindow::Update();
	ZoneManager::Get().getCurrentZone()->update(m_dt);
	Player::Get().update(m_dt);
	ViewPort::Get().setPosition(Player::Get().getPosition());	
	Message::Get().deleteDecayed();
}

void Game::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Zone::Draw();
	GroundLoot::Draw();
	InteractionPoint::Draw();
	Zone::DrawActiveAoESpells();
	Zone::DrawNpcs();
	Player::Get().draw();
	Npc::DrawActiveSpells();
	
	Interface::Get().draw();
	Message::Get().draw();
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
}