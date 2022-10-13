#include "Player.h"

Player Player::s_instance;

Player& Player::Get() {
	return s_instance;
}

Player::Player() {

}

Player::~Player() {

}

void Player::init(int x, int y) {
	x_pos = x;
	y_pos = y;
	direction_texture = Enums::Direction::S;

	activeDirection = Enums::Direction::S;
	lastActiveDirection = activeDirection;
	curActivity = Enums::ActivityType::Walking;

	//remainingMovePoints = 0;
	isPreparing = false;
	alive = true;
	//hasDrawnDyingOnce = false;
	hasChoosenFearDirection = false;
	//hasChoosenDyingDirection = false;
	//curSpellAction = NULL;
	//experience = 0;
	coins = 0;
	//dyingTransparency = 1.0f;
	movementSpeed = 1;
}


void Player::draw() {
	Enums::ActivityType curActivity = getCurActivity();

	int drawX = getXPos();
	int drawY = getYPos();

	if (getUseBoundingBox()){
		drawX -= getBoundingBoxX();
		drawY -= getBoundingBoxY();
	}

	Batchrenderer::Get().bindTexture(TextureManager::GetTextureAtlas("player"), true);
	TextureManager::DrawTexture(*rect, drawX, drawY, true, true);
	Batchrenderer::Get().unbindTexture(true);
}

void Player::update(float deltaTime) {

	//if (curActivity != Enums::ActivityType::Dying) {
		processInput();
	//}

	if (activeDirection != Enums::Direction::STOP && curActivity != Enums::ActivityType::Walking) {		
		interruptCurrentActivityWith(Enums::ActivityType::Walking);
	}

	Move(deltaTime);

	lastActiveDirection = activeDirection != Enums::Direction::STOP ? activeDirection : lastActiveDirection;
	Animate(deltaTime);
}


void Player::Move(float deltaTime) {
	
	// moves one step per movePerStep ms
	float movePerStep = 0.01f * m_currentspeed;

	// To balance moving diagonally boost, movePerStep = 10*sqrt(2)
	if (activeDirection == Enums::Direction::NW || activeDirection == Enums::Direction::NE || activeDirection == Enums::Direction::SW || activeDirection == Enums::Direction::SE)
		movePerStep = 0.014f * m_currentspeed;

	// synch with animation
	m_duration = movePerStep * 10.0f;
	m_elapsedTime += deltaTime;
	
	while (m_elapsedTime > movePerStep) {
		m_elapsedTime -= movePerStep;
		Character::Move(activeDirection);
	}
}

void Player::Animate(float deltaTime) {
	const TileSet& tileSet = m_characterType->m_moveTileSets.at({ curActivity, lastActiveDirection });

	if (activeDirection != Enums::Direction::STOP || ((curActivity == Enums::ActivityType::Dying || curActivity == Enums::ActivityType::Casting) && m_waitForAnimation)) {
		unsigned short numActivityTextures = getNumActivityTextures(curActivity);
	
		m_wanderTime += deltaTime;
		while (m_wanderTime >= m_duration) {
			m_wanderTime -= m_duration;
			if (++currentFrame > numActivityTextures - 1) {				
				currentFrame = curActivity == Enums::ActivityType::Dying ? numActivityTextures - 1 : 0;
				m_waitForAnimation = false;
				
			}
		}
		rect = &tileSet.getAllTiles()[currentFrame].textureRect;

	}else if (curActivity == Enums::ActivityType::Walking) {
		rect = &tileSet.getAllTiles()[0].textureRect;
	}		
}

void Player::processInput() {
	Keyboard &keyboard = Keyboard::instance();

	m_currentspeed = (keyboard.keyDown(Keyboard::KEY_SHIFT) && curActivity == Enums::ActivityType::Walking) ? m_inverseSpeed * 0.7f : m_inverseSpeed;

	if (keyboard.keyDown(Keyboard::KEY_UP) || keyboard.keyDown(Keyboard::KEY_W)) {
		if (keyboard.keyDown(Keyboard::KEY_LEFT) || keyboard.keyDown(Keyboard::KEY_A)) {
			activeDirection = Enums::Direction::NW;
			return;
		} else if (keyboard.keyDown(Keyboard::KEY_RIGHT) || keyboard.keyDown(Keyboard::KEY_D)) {
			activeDirection = Enums::Direction::NE;
			return;
		} else {

			activeDirection = Enums::Direction::N;
			return;
		}
	}

	if (keyboard.keyDown(Keyboard::KEY_DOWN) || keyboard.keyDown(Keyboard::KEY_S)) {
		if (keyboard.keyDown(Keyboard::KEY_LEFT) || keyboard.keyDown(Keyboard::KEY_A)) {
			activeDirection = Enums::Direction::SW;
			return;
		} else if (keyboard.keyDown(Keyboard::KEY_RIGHT) || keyboard.keyDown(Keyboard::KEY_D)) {

			activeDirection = Enums::Direction::SE;
			return;
		} else {

			activeDirection = Enums::Direction::S;
			return;
		}
	}

	if (keyboard.keyDown(Keyboard::KEY_LEFT) || keyboard.keyDown(Keyboard::KEY_A)) {
		activeDirection = Enums::Direction::W;
		return;
	}

	if (keyboard.keyDown(Keyboard::KEY_RIGHT) || keyboard.keyDown(Keyboard::KEY_D)) {
		activeDirection = Enums::Direction::E;
		return;
	}

	if (keyboard.keyDown(Keyboard::KEY_E) && !m_waitForAnimation) {
		curActivity = Enums::ActivityType::Dying;
		currentFrame = 0;
		m_waitForAnimation = true;
	}

	if (keyboard.keyDown(Keyboard::KEY_R) && !m_waitForAnimation) {
		curActivity = Enums::ActivityType::Casting;
		currentFrame = 0;
		m_waitForAnimation = true;
	}

	activeDirection = Enums::Direction::STOP;
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