#include "Player.h"

Player Player::s_instance;

Player& Player::Get() {
	return s_instance;
}

Player::Player() {

}

Player::~Player() {

}


void Player::draw() {

}

void Player::Move(float deltaTime) {

}

Enums::Direction Player::GetDirection() {
	return Enums::Direction::S;
}

Enums::Direction Player::GetDirectionRNG() {
	return Enums::Direction::S;
}

int Player::getWidth() const {
	const TextureRect& rect = m_characterType->m_moveTileSets.at({ Enums::ActivityType::Walking, Enums::Direction::S }).getAllTiles()[0].textureRect;
	return useBoundingBox ? boundingBoxW : rect.width;
}

int Player::getHeight() const  {
	const TextureRect& rect = m_characterType->m_moveTileSets.at({ Enums::ActivityType::Walking, Enums::Direction::S }).getAllTiles()[0].textureRect;
	return useBoundingBox ? boundingBoxH : rect.height;
}
unsigned short Player::getNumActivityTextures(Enums::ActivityType activity) {
	return m_characterType->m_numMoveTexturesPerDirection.at(activity);
}

void Player::setCharacterType(std::string characterType) {
	m_characterType = &CharacterTypeManager::Get().getCharacterType(characterType);
	rect = &m_characterType->m_moveTileSets.at({ getCurActivity(), activeDirection }).getAllTiles()[0].textureRect;

}