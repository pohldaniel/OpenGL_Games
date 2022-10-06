#include "Character.h"
#include "TilesetManager.h"
#include "Constants.h"

Character::Character(const CharacterType& characterType) : m_characterType(characterType) {
	wander_radius = 40;
	activeDirection = Enums::Direction::S;

	rect = &m_characterType.m_moveTileSets.at({ getCurActivity(), activeDirection }).getAllTiles()[0].textureRect;
}

void Character::setNumActivities(unsigned short numActivities) {	
	m_numActivities = numActivities;
}

unsigned short Character::getNumActivities() {
	return m_numActivities;
}

void Character::baseOnType(std::string characterType) {
	const CharacterType& other = CharacterTypeManager::Get().getCharacterType(characterType);
	m_numActivities = static_cast<unsigned short>(m_characterType.m_moveTileSets.size()) / 8;
	
	strength = other.strength;
	dexterity = other.dexterity;
	vitality = other.vitality;
	intellect = other.intellect;
	wisdom = other.wisdom;
	max_health = other.max_health;
	max_mana = other.max_mana;
	max_fatigue = other.max_fatigue;
	min_damage = other.min_damage;
	max_damage = other.max_damage;	
	armor = other.armor;
	healthRegen = other.healthRegen;
	manaRegen = other.manaRegen;
	fatigueRegen = other.fatigueRegen;
	damageModifierPoints = other.damageModifierPoints;
	hitModifierPoints = other.hitModifierPoints;
	evadeModifierPoints = other.evadeModifierPoints;
	characterClass = other.characterClass;
	wander_radius = other.wander_radius;
	name = other.name;
	level = other.level;
	experienceValue = other.experienceValue;

	/*setStrength(other.strength);
	setDexterity(other.dexterity);
	setVitality(other.vitality);
	setIntellect(other.intellect);
	setWisdom(other.wisdom);
	setMaxHealth(other.max_health);
	setMaxMana(other.max_mana);
	setMaxFatigue(other.max_fatigue);
	setMinDamage(other.min_damage);
	setMaxDamage(other.max_damage);
	setArmor(other.armor);
	setHealthRegen(other.healthRegen);
	setManaRegen(other.manaRegen);
	setFatigueRegen(other.fatigueRegen);
	setDamageModifierPoints(other.damageModifierPoints);
	setHitModifierPoints(other.hitModifierPoints);
	setEvadeModifierPoints(other.evadeModifierPoints);
	setClass(other.characterClass);
	setWanderRadius(other.wander_radius);
	setName(other.name);
	setLevel(other.level);
	setExperienceValue(other.experienceValue);*/
}

Enums::ActivityType Character::getCurActivity() const {
	Enums::ActivityType curActivity = Enums::ActivityType::Walking;
	
	return curActivity;
}

int Character::getXPos() const {
	return x_pos;
}

int Character::getYPos() const {
	return y_pos;
}


void Character::update(float deltaTime) {
	Enums::ActivityType curActivity = getCurActivity();

	if (isStunned() == true || isMesmerized() == true) {
		rect = &m_characterType.m_moveTileSets.at({curActivity, activeDirection}).getAllTiles()[0].textureRect;
		return;
	}

	Enums::Direction direction = GetDirection();
	if (direction != Enums::Direction::STOP) {
		activeDirection = direction;
	}

	switch (curActivity) {
		case  Enums::ActivityType::Walking: {
			
			if (direction == Enums::Direction::STOP) {
				rect = &m_characterType.m_moveTileSets.at({ curActivity, activeDirection }).getAllTiles()[0].textureRect;
				index = 0;
				return;
			}
			int msPerDrawFrame = 100;


			const TileSet& tileSet = m_characterType.m_moveTileSets.at({ curActivity, activeDirection });

			//index = ((Globals::clock.getElapsedTimeMilli() % (msPerDrawFrame * tileSet.getAllTiles().size())) / msPerDrawFrame);
			progress += deltaTime * 10;
			index = static_cast<int>(std::round(progress));
			index = index % tileSet.getAllTiles().size();

			rect = &tileSet.getAllTiles()[index].textureRect;
			
			
			break;
		}case Enums::ActivityType::Attacking: {

			if (direction == Enums::Direction::STOP) {
				rect = &m_characterType.m_moveTileSets.at({ curActivity, activeDirection }).getAllTiles()[0].textureRect;
				index = 0;
				return;
			}
			int msPerDrawFrame = 80;

			const TileSet& tileSet = m_characterType.m_moveTileSets.at({ curActivity, activeDirection });

			index = ((Globals::clock.getElapsedTimeMilli() % (msPerDrawFrame * tileSet.getAllTiles().size())) / msPerDrawFrame);
			rect = &tileSet.getAllTiles()[index].textureRect;

			break;
		}case Enums::ActivityType::Casting: {

			if (direction == Enums::Direction::STOP) {
				rect = &m_characterType.m_moveTileSets.at({ curActivity, activeDirection }).getAllTiles()[0].textureRect;
				index = 0;
				return;
			}
			int msPerDrawFrame = 80;

			const TileSet& tileSet = m_characterType.m_moveTileSets.at({ curActivity, activeDirection });

			index = ((Globals::clock.getElapsedTimeMilli() % (msPerDrawFrame * tileSet.getAllTiles().size())) / msPerDrawFrame);
			rect = &tileSet.getAllTiles()[index].textureRect;

			break;
		}case Enums::ActivityType::Dying: {

			if (direction == Enums::Direction::STOP) {
				rect = &m_characterType.m_moveTileSets.at({ curActivity, activeDirection }).getAllTiles()[0].textureRect;
				index = 0;
				return;
			}
			int msPerDrawFrame = 80;

			const TileSet& tileSet = m_characterType.m_moveTileSets.at({ curActivity, activeDirection });

			index = ((Globals::clock.getElapsedTimeMilli() % (msPerDrawFrame * tileSet.getAllTiles().size())) / msPerDrawFrame);
			rect = &tileSet.getAllTiles()[index].textureRect;

			break;
		}
		
	}
}

Enums::Direction Character::GetDirectionTexture() {
	if (isStunned() == true || isMesmerized() == true) {
		return activeDirection;
	}

	Enums::Direction direction = GetDirection();
	if (direction != Enums::Direction::STOP) {
		activeDirection = direction;
	}
	
	Enums::ActivityType curActivity = getCurActivity();

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
		return 0.60f;
	}
	else if (isSneaking() == true) { // if we are sneaking, we reduce the movementspeed aswell of the character. good place to do that is here
		return 0.75f;
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
	
	Enums::Direction movingDirection = GetDirection();

	if ((movingDirection != Enums::Direction::STOP) && !mayDoAnythingAffectingSpellActionWithoutAborting()) {
		//CastingAborted();
	}

	if (movingDirection != Enums::Direction::STOP && isChanneling() == true) {
		//removeSpellsWithCharacterState(CharacterStates::Channeling);
	}


	/// if we are feared (fleeing) we run at a random direction. Only choose a direction once for each fear effect.
	if (isFeared() == true) {
		if (hasChoosenFearDirection == false) {
			fearDirection = static_cast<Enums::Direction>(RNG::randomSizeT(1, 8));
			hasChoosenFearDirection = true;
		}
		movingDirection = fearDirection;
	}

	unsigned int movePerStep = 10; // moves one step per movePerStep ms

	// To balance moving diagonally boost, movePerStep = 10*sqrt(2)
	if (movingDirection == Enums::Direction::NW || movingDirection == Enums::Direction::NE || movingDirection == Enums::Direction::SW || movingDirection == Enums::Direction::SE)
		movePerStep = 14;

	// recalculate the movementpoints based on our movementspeed (spells that affect this can be immobolizing spells, snares or movement enhancer
	remainingMovePoints = static_cast<unsigned int>(remainingMovePoints * getMovementSpeed());

	while (remainingMovePoints > movePerStep) {
		remainingMovePoints -= movePerStep;
		switch (movingDirection) {
		case Enums::Direction::NW:
			//MoveLeft(1);
			//MoveUp(1);
			break;
		case Enums::Direction::N:
			//MoveUp(1);
			break;
		case Enums::Direction::NE:
			//MoveRight(1);
			//MoveUp(1);
			break;
		case Enums::Direction::W:
			//MoveLeft(1);
			break;
		case Enums::Direction::E:
			//MoveRight(1);
			break;
		case Enums::Direction::SW:
			//MoveLeft(1);
			//MoveDown(1);
			break;
		case Enums::Direction::S:
			//MoveDown(1);
			break;
		case Enums::Direction::SE:
			//MoveRight(1);
			//MoveDown(1);
			break;
		default:
			break;
		}
	}
}

/*void Character::setStrength(uint16_t newStrength){
	strength = newStrength;
}

void Character::setName(std::string newName) {
	name = newName;
}

void Character::setDexterity(uint16_t newDexterity){
	dexterity = newDexterity;
}

void Character::setVitality(uint16_t newVitality){
	vitality = newVitality;
}

void Character::setIntellect(uint16_t newIntellect){
	intellect = newIntellect;
}

void Character::setWisdom(uint16_t newWisdom){
	wisdom = newWisdom;
}

void Character::setMaxHealth(uint16_t newMaxHealth){
	max_health = newMaxHealth;
	// if ( current_health > getModifiedMaxHealth() )
	// {
	// current_health = getModifiedMaxHealth();
	// }
}

void Character::setMaxMana(uint16_t newMaxMana){
	max_mana = newMaxMana;
	// if ( current_mana > getModifiedMaxMana() )
	// {
	// current_mana = getModifiedMaxMana();
	// }
}

void Character::setMaxFatigue(uint16_t newMaxFatigue){
	max_fatigue = newMaxFatigue;
	// if ( current_fatigue > getModifiedMaxFatigue() )
	// {
	//current_fatigue = getModifiedMaxFatigue();
	// }
}

void Character::setHealthRegen(uint16_t newHealthRegen){
	healthRegen = newHealthRegen;
}

void Character::setManaRegen(uint16_t newManaRegen){
	manaRegen = newManaRegen;
}

void Character::setFatigueRegen(uint16_t newFatigueRegen){
	fatigueRegen = newFatigueRegen;
}

void Character::setMaxDamage(uint16_t newMaxDamage){
	max_damage = newMaxDamage;
}

void Character::setMinDamage(uint16_t newMinDamage){
	min_damage = newMinDamage;
}

void Character::setArmor(uint16_t newArmor){
	armor = newArmor;
}

void Character::setDamageModifierPoints(uint16_t newDamageModifierPoints){
	damageModifierPoints = newDamageModifierPoints;
}

void Character::setHitModifierPoints(uint16_t newHitModifierPoints){
	hitModifierPoints = newHitModifierPoints;
}

void Character::setEvadeModifierPoints(uint16_t newEvadeModifierPoints){
	evadeModifierPoints = newEvadeModifierPoints;
}

void Character::setWanderRadius(uint16_t newWanderRadius){
	wander_radius = newWanderRadius;
}

void Character::setLevel(uint8_t newLevel){
	level = newLevel;
}

void Character::setClass(CharacterClass::CharacterClass characterClass){
	Character::characterClass = characterClass;
}

void Character::setExperienceValue(uint8_t experienceValue){
	Character::experienceValue = experienceValue;
}*/

uint16_t Character::getStrength() const{
	return strength;
}

uint16_t Character::getDexterity() const{
	return dexterity;
}

uint16_t Character::getVitality() const{
	return vitality;
}

uint16_t Character::getIntellect() const{
	return intellect;
}

uint16_t Character::getWisdom() const{
	return wisdom;
}

uint16_t Character::getMaxHealth() const{
	return max_health;
}

uint16_t Character::getMaxMana() const{
	return max_mana;
}

uint16_t Character::getMaxFatigue() const{
	return max_fatigue;
}

uint16_t Character::getMaxDamage() const{
	return max_damage;
}

uint16_t Character::getMinDamage() const{
	return min_damage;
}

uint16_t Character::getArmor() const{
	return armor;
}

uint16_t Character::getHealthRegen() const{
	return healthRegen;
}

uint16_t Character::getManaRegen() const{
	return manaRegen;
}

uint16_t Character::getFatigueRegen() const{
	return fatigueRegen;
}

uint16_t Character::getDamageModifierPoints() const{
	return damageModifierPoints;
}

uint16_t Character::getHitModifierPoints() const{
	return hitModifierPoints;
}

uint16_t Character::getEvadeModifierPoints() const{
	return evadeModifierPoints;
}

Enums::CharacterClass Character::getClass() const{
	return characterClass;
}

std::string Character::getName() const{
	return name;
}

uint8_t Character::getLevel() const{
	return level;
}

uint8_t Character::getExperienceValue() const{
	return experienceValue;
}

int Character::getWidth() const {
	const TextureRect& rect = m_characterType.m_moveTileSets.at({ Enums::ActivityType::Walking, Enums::Direction::S }).getAllTiles()[0].textureRect;
	return useBoundingBox ? boundingBoxW : rect.width;
}

int Character::getHeight() const {
	const TextureRect& rect = m_characterType.m_moveTileSets.at({ Enums::ActivityType::Walking, Enums::Direction::S }).getAllTiles()[0].textureRect;
	return useBoundingBox ? boundingBoxH : rect.height;
}

////////////////////////////////////////////////////////////////////////////////
void CharacterType::addMoveTexture(Enums::ActivityType activity, Enums::Direction direction, int index, std::string filename, int textureOffsetX, int textureOffsetY) {
	TileSet& tileSet = m_moveTileSets[{activity, direction}];
	tileSet.addTile(filename, Enums::TileClassificationType::FLOOR);
}

void CharacterType::setStrength(uint16_t newStrength) {
	strength = newStrength;
}

void CharacterType::setName(std::string newName) {
	name = newName;
}

void CharacterType::setDexterity(uint16_t newDexterity) {
	dexterity = newDexterity;
}

void CharacterType::setVitality(uint16_t newVitality) {
	vitality = newVitality;
}

void CharacterType::setIntellect(uint16_t newIntellect) {
	intellect = newIntellect;
}

void CharacterType::setWisdom(uint16_t newWisdom) {
	wisdom = newWisdom;
}

void CharacterType::setMaxHealth(uint16_t newMaxHealth) {
	max_health = newMaxHealth;
}

void CharacterType::setMaxMana(uint16_t newMaxMana) {
	max_mana = newMaxMana;
}

void CharacterType::setMaxFatigue(uint16_t newMaxFatigue) {
	max_fatigue = newMaxFatigue;
}

void CharacterType::setHealthRegen(uint16_t newHealthRegen) {
	healthRegen = newHealthRegen;
}

void CharacterType::setManaRegen(uint16_t newManaRegen) {
	manaRegen = newManaRegen;
}

void CharacterType::setFatigueRegen(uint16_t newFatigueRegen) {
	fatigueRegen = newFatigueRegen;
}

void CharacterType::setMaxDamage(uint16_t newMaxDamage) {
	max_damage = newMaxDamage;
}

void CharacterType::setMinDamage(uint16_t newMinDamage) {
	min_damage = newMinDamage;
}

void CharacterType::setArmor(uint16_t newArmor) {
	armor = newArmor;
}

void CharacterType::setDamageModifierPoints(uint16_t newDamageModifierPoints) {
	damageModifierPoints = newDamageModifierPoints;
}

void CharacterType::setHitModifierPoints(uint16_t newHitModifierPoints) {
	hitModifierPoints = newHitModifierPoints;
}

void CharacterType::setEvadeModifierPoints(uint16_t newEvadeModifierPoints) {
	evadeModifierPoints = newEvadeModifierPoints;
}

void CharacterType::setWanderRadius(uint16_t newWanderRadius) {
	wander_radius = newWanderRadius;
}

void CharacterType::setLevel(uint8_t newLevel) {
	level = newLevel;
}

void CharacterType::setClass(Enums::CharacterClass characterClass) {
	CharacterType::characterClass = characterClass;
}

void CharacterType::setExperienceValue(uint8_t experienceValue) {
	CharacterType::experienceValue = experienceValue;
}

const TileSet& CharacterType::getTileSet(Enums::ActivityType activity, Enums::Direction direction) const{
	return m_moveTileSets.at({activity, direction});
}

////////////////////////////////////////////////////////////////////////////////
CharacterTypeManager CharacterTypeManager::s_instance;

CharacterTypeManager& CharacterTypeManager::Get() {
	return s_instance;
}

const CharacterType& CharacterTypeManager::getCharacterType(std::string characterType) {
	return m_characterTypes[characterType];
}

std::unordered_map<std::string, CharacterType>& CharacterTypeManager::getCharacterTypes() {
	return m_characterTypes;
}

bool CharacterTypeManager::containsCaracterType(std::string characterType) {
	return m_characterTypes.count(characterType) == 1;
}