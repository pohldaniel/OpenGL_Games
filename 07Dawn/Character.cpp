#include "Character.h"
#include "TilesetManager.h"
#include "Constants.h"

Character::Character() {
	wander_radius = 40;
	activeDirection = S;
}

void Character::setNumActivities(unsigned short numActivities) {	
	m_numActivities = numActivities;
}

TileSet* Character::getTileSet(ActivityType::ActivityType activity, Direction direction) {
	return &m_moveTileSets[{activity, direction}];	
}

void Character::setTileSet(std::unordered_map<std::pair<int, int>, TileSet, pair_hash>& moveTileSets) {
	m_moveTileSets = moveTileSets;
}

unsigned short Character::getNumActivities() {
	return m_numActivities;
}

void Character::setMoveTexture(ActivityType::ActivityType activity, Direction direction, int index, std::string filename, int textureOffsetX, int textureOffsetY){
	TileSet* tileSet = getTileSet(activity, direction);
	tileSet->addTile(filename, TileClassificationType::TileClassificationType::FLOOR);
}

ActivityType::ActivityType Character::getCurActivity() const {
	ActivityType::ActivityType curActivity = ActivityType::Walking;
	
	return curActivity;
}

int Character::getXPos() const {
	return x_pos;
}

int Character::getYPos() const {
	return y_pos;
}

void Character::setName(std::string newName){
	name = newName;
}

void Character::update(float deltaTime) {
	ActivityType::ActivityType curActivity = getCurActivity();

	if (isStunned() == true || isMesmerized() == true) {
		rect = &m_moveTileSets[{curActivity, activeDirection}].getAllTiles()[0]->textureRect;
		return;
	}

	Direction direction = GetDirection();
	if (direction != STOP) {
		activeDirection = direction;
	}

	switch (curActivity) {
		case ActivityType::Walking: {
			
			if (direction == STOP) {
				rect = &m_moveTileSets[{curActivity, activeDirection}].getAllTiles()[0]->textureRect;
				index = 0;
				return;
			}
			int msPerDrawFrame = 100;


			TileSet& tileSet = m_moveTileSets[{curActivity, activeDirection}];

			dumping += deltaTime * 10;

			//index = ((Globals::clock.getElapsedTimeMilli() % (msPerDrawFrame * tileSet.getAllTiles().size())) / msPerDrawFrame);

			index = dumping;
			index = index % tileSet.getAllTiles().size();

			rect = &tileSet.getAllTiles()[index]->textureRect;
			
			
			break;
		}case ActivityType::Attacking: {

			if (direction == STOP) {
				rect = &m_moveTileSets[{curActivity, activeDirection}].getAllTiles()[0]->textureRect;
				index = 0;
				return;
			}
			int msPerDrawFrame = 80;

			TileSet& tileSet = m_moveTileSets[{curActivity, activeDirection}];

			index = ((Globals::clock.getElapsedTimeMilli() % (msPerDrawFrame * tileSet.getAllTiles().size())) / msPerDrawFrame);
			rect = &tileSet.getAllTiles()[index]->textureRect;

			break;
		}case ActivityType::Casting: {

			if (direction == STOP) {
				rect = &m_moveTileSets[{curActivity, activeDirection}].getAllTiles()[0]->textureRect;
				index = 0;
				return;
			}
			int msPerDrawFrame = 80;

			TileSet& tileSet = m_moveTileSets[{curActivity, activeDirection}];

			index = ((Globals::clock.getElapsedTimeMilli() % (msPerDrawFrame * tileSet.getAllTiles().size())) / msPerDrawFrame);
			rect = &tileSet.getAllTiles()[index]->textureRect;

			break;
		}case ActivityType::Dying: {

			if (direction == STOP) {
				rect = &m_moveTileSets[{curActivity, activeDirection}].getAllTiles()[0]->textureRect;
				index = 0;
				return;
			}
			int msPerDrawFrame = 80;

			TileSet& tileSet = m_moveTileSets[{curActivity, activeDirection}];

			index = ((Globals::clock.getElapsedTimeMilli() % (msPerDrawFrame * tileSet.getAllTiles().size())) / msPerDrawFrame);
			rect = &tileSet.getAllTiles()[index]->textureRect;

			break;
		}
		
	}
}

Direction Character::GetDirectionTexture() {
	if (isStunned() == true || isMesmerized() == true) {
		return activeDirection;
	}

	Direction direction = GetDirection();
	if (direction != STOP) {
		activeDirection = direction;
	}
	
	ActivityType::ActivityType curActivity = getCurActivity();

	/*switch (curActivity) {
		case ActivityType::Walking: {

			if (direction == STOP)
				return activeDirection;

			TileSet& tileSet = m_moveTileSets2[curActivity][activeDirection];
			int msPerDrawFrame = 100;
			index = ((Globals::clock.getElapsedTimeMilli() % (msPerDrawFrame * tileSet.getAllTiles().size())) / msPerDrawFrame);

			std::cout << "Size: " << "  " << tileSet.getAllTiles().size() << std::endl;

			return direction;
		}
		break;
	}*/
	
	return direction;
}

uint16_t Character::getWanderRadius() const {
	return wander_radius;
}

bool Character::isStunned() const{
	/*for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell].first->getCharacterState().first == CharacterStates::Stunned) {
			return true;
		}
	}*/
	return false;
}

bool Character::isMesmerized() const {
	/*for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell].first->getCharacterState().first == CharacterStates::Mesmerized) {
			return true;
		}
	}*/
	return false;
}

bool Character::isFeared() const {
	/*for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell].first->getCharacterState().first == CharacterStates::Feared) {
			return true;
		}
	}*/
	return false;
}

bool Character::isCharmed() const {
	/*for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell].first->getCharacterState().first == CharacterStates::Charmed) {
			return true;
		}
	}*/
	return false;
}

bool Character::isChanneling() const {
	/*for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell].first->getCharacterState().first == CharacterStates::Channeling) {
			return true;
		}
	}*/
	return false;
}

bool Character::isSneaking() const {
	/*for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell].first->getCharacterState().first == CharacterStates::Sneaking) {
			return true;
		}
	}*/
	return false;
}

bool Character::getIsPreparing() const {
	//return isPreparing;
	return false;
}

bool Character::continuePreparing() {
	/// if we're preparing a spell while getting stunned, feared, mesmerized or charmed, abort the spellcasting.
	if ((isStunned() == true || isFeared() == true || isMesmerized() == true || isCharmed() == true) && getIsPreparing() == true) {
		//CastingAborted();
	}

	if (isPreparing) {
		/*bool preparationFinished = (curSpellAction->getCastTime() == 0);
		if (!preparationFinished) {
			preparationCurrentTime = SDL_GetTicks();

			// casting_percentage is mostly just for the castbar display, guess we could alter this code.
			uint16_t spellCastTime = curSpellAction->getCastTime();

			// if we're confused while casting, we add 35% more time to our spellcasting.
			if (isConfused() == true) {
				spellCastTime *= 1.35;
			}

			preparationPercentage = (static_cast<float>(preparationCurrentTime - preparationStartTime)) / spellCastTime;
			preparationFinished = (preparationPercentage >= 1.0f);
		}
		if (preparationFinished)
			startSpellAction();*/
	}

	return isPreparing;
}

bool Character::isPlayer() const {
	return false;
}

bool Character::mayDoAnythingAffectingSpellActionWithoutAborting() const {
	//return (curSpellAction == NULL);
	return false;
}

bool Character::mayDoAnythingAffectingSpellActionWithAborting() const {
	/*if (isPlayer() == true) {
		return (curSpellAction == NULL || isPreparing);
	}else {
		return (curSpellAction == NULL);
	}*/

	return false;
}

float Character::getMovementSpeed() const {
	// see if we are affected by movement altering spells. If we are we get the lowest value and return it.
	// if we have no spell lowering the movement, we look for enhancers and return that. If that's not found, 1.0 is returned.
	float lowestMovementSpeed = 1.0f;
	float highestMovementSpeed = 1.0f;
	/*for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell].first->getCharacterState().first == CharacterStates::Movementspeed) {
			if (lowestMovementSpeed > activeSpells[activeSpell].first->getCharacterState().second) {
				lowestMovementSpeed = activeSpells[activeSpell].first->getCharacterState().second;
			}
			if (highestMovementSpeed < activeSpells[activeSpell].first->getCharacterState().second) {
				highestMovementSpeed = activeSpells[activeSpell].first->getCharacterState().second;
			}
		}
	}*/


	if (lowestMovementSpeed < 1.0) {
		return lowestMovementSpeed;
	}
	else if (isFeared() == true) { // if we are feared, we reduce the movementspeed. Mostly so we dont run too far away.
		return 0.60;
	}
	else if (isSneaking() == true) { // if we are sneaking, we reduce the movementspeed aswell of the character. good place to do that is here
		return 0.75;
	}
	else {
		return highestMovementSpeed;
	}
}

void Character::MoveUp(uint8_t n) {
	//if (CollisionCheck(N) == 0) {
		y_pos += n;
		//}
}

void Character::MoveDown(uint8_t n) {
	//if (CollisionCheck(S) == 0) {
		y_pos -= n;
		//}
}

void Character::MoveLeft(uint8_t n) {
	//if (CollisionCheck(W) == 0) {
		x_pos -= n;
		//}
}

void Character::MoveRight(uint8_t n) {
	//if (CollisionCheck(E) == 0) {
		x_pos += n;
		//}
}

void Character::Move(){

	if (isStunned() == true || isMesmerized() == true) {
		remainingMovePoints = 0;
		return;
	}

	if (isFeared() == false) {
		hasChoosenFearDirection = false;
	}

	continuePreparing();
	if (!mayDoAnythingAffectingSpellActionWithoutAborting()) {
		if (!mayDoAnythingAffectingSpellActionWithAborting()) {
			remainingMovePoints = 0;
			return;
		}
	}
	
	Direction movingDirection = GetDirection();

	if ((movingDirection != STOP) && !mayDoAnythingAffectingSpellActionWithoutAborting()) {
		//CastingAborted();
	}

	if (movingDirection != STOP && isChanneling() == true) {
		//removeSpellsWithCharacterState(CharacterStates::Channeling);
	}


	/// if we are feared (fleeing) we run at a random direction. Only choose a direction once for each fear effect.
	if (isFeared() == true) {
		if (hasChoosenFearDirection == false) {
			fearDirection = static_cast<Direction>(RNG::randomSizeT(1, 8));
			hasChoosenFearDirection = true;
		}
		movingDirection = fearDirection;
	}

	unsigned int movePerStep = 10; // moves one step per movePerStep ms

	// To balance moving diagonally boost, movePerStep = 10*sqrt(2)
	if (movingDirection == NW || movingDirection == NE || movingDirection == SW || movingDirection == SE)
		movePerStep = 14;

	// recalculate the movementpoints based on our movementspeed (spells that affect this can be immobolizing spells, snares or movement enhancer
	remainingMovePoints *= getMovementSpeed();

	while (remainingMovePoints > movePerStep) {
		remainingMovePoints -= movePerStep;
		switch (movingDirection) {
		case NW:
			//MoveLeft(1);
			//MoveUp(1);
			break;
		case N:
			//MoveUp(1);
			break;
		case NE:
			//MoveRight(1);
			//MoveUp(1);
			break;
		case W:
			//MoveLeft(1);
			break;
		case E:
			//MoveRight(1);
			break;
		case SW:
			//MoveLeft(1);
			//MoveDown(1);
			break;
		case S:
			//MoveDown(1);
			break;
		case SE:
			//MoveRight(1);
			//MoveDown(1);
			break;
		default:
			break;
		}
	}
}
