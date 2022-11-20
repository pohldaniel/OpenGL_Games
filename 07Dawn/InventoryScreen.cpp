#include "InventoryScreen.h"

InventoryScreen InventoryScreen::s_instance;

InventoryScreen& InventoryScreen::Get() {
	return s_instance;
}

InventoryScreen::InventoryScreen() : Widget(125, 50, 454, 672) {

}

void InventoryScreen::setPlayer(Player* player) {
	m_player = player;
}

void InventoryScreen::init() {

}

void InventoryScreen::draw() {

}

void InventoryScreen::processInput() {

}