#include "Character.h"
#include "TilesetManager.h"
#include "Constants.h"

const uint16_t NULLABLE_ATTRIBUTE_MIN = 0;
const uint16_t NON_NULLABLE_ATTRIBUTE_MIN = 1;

template <class AttributeType, class ModifierType>
AttributeType getModifiedAttributeValue(AttributeType attributeValue, ModifierType modifier,
	AttributeType minValue = std::numeric_limits<AttributeType>::min(),
	AttributeType maxValue = std::numeric_limits<AttributeType>::max()) {

	// is modified value < minValue? => set to minValue
	if (modifier < 0 && static_cast<AttributeType>(-modifier) > attributeValue - minValue)
		return minValue;
	// is modified value > maxValue? => set to maxValue
	else if (modifier > 0 && (maxValue - attributeValue) < modifier)
		return maxValue;
	else
		return (attributeValue + modifier);
}


Character::Character() {
	wander_radius = 40;
	activeDirection = Enums::Direction::S;	
}

void Character::setNumActivities(unsigned short numActivities) {	
	m_numActivities = numActivities;
}

unsigned short Character::getNumActivities() {
	return m_numActivities;
}

void Character::baseOnType(std::string characterType) {
	const CharacterType& other = CharacterTypeManager::Get().getCharacterType(characterType);
		
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
}

Enums::ActivityType Character::getCurActivity() const {
	Enums::ActivityType curActivity = Enums::ActivityType::Walking;
	
	return curActivity;
}

void Character::setBoundingBox(int bbx, int bby, int bbw, int bbh) {
	boundingBoxX = bbx;
	boundingBoxY = bby;
	boundingBoxW = bbw;
	boundingBoxH = bbh;
	useBoundingBox = true;
}

void Character::setUseBoundingBox(bool use) {
	useBoundingBox = use;
}

int Character::getXPos() const {
	return x_pos;
}

int Character::getYPos() const {
	return y_pos;
}

int Character::getBoundingBoxX() const {
	return boundingBoxX;
}

int Character::getBoundingBoxY() const {
	return boundingBoxY;
}

int Character::getBoundingBoxW() const {
	return boundingBoxW;
}

int Character::getBoundingBoxH() const {
	return boundingBoxH;
}

bool Character::getUseBoundingBox() const {
	return useBoundingBox;
}

void Character::update(float deltaTime) {
	
}

void Character::interruptCurrentActivityWith(Enums::ActivityType activity) {
	curActivity = activity;
	m_waitForAnimation = false;
	currentFrame = 0;
}

std::string getCharacterClassPortrait(Enums::CharacterClass characterClass){
	switch (characterClass){
	case Enums::CharacterClass::Liche:
		return "res/interface/Portrait/Liche.tga";
		break;
	case Enums::CharacterClass::Ranger:
		return "res/interface/Portrait/Ranger.tga";
		break;
	case Enums::CharacterClass::Warrior:
		return "res/interface/Portrait/Warrior.tga";
		break;
	default:
		return "";
		break;
	}
}

void Character::giveCoins(unsigned int amountOfCoins) {
	coins += amountOfCoins;
}

uint16_t Character::getWanderRadius() const {
	return wander_radius;
}

uint16_t Character::getWanderRadiusSq() const {
	return wander_radius * wander_radius;
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

bool Character::isConfused() const {
	/*for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell].first->getCharacterState().first == CharacterStates::Confused) {
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
	return true;
}

bool Character::mayDoAnythingAffectingSpellActionWithAborting() const {
	/*if (isPlayer() == true) {
		return (curSpellAction == NULL || isPreparing);
	}else {
		return (curSpellAction == NULL);
	}*/

	return true;
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

void Character::Move(char x, char y) {
	x_pos += x;
	y_pos += y;
}

void Character::Move(Enums::Direction direction, uint8_t n) {
	switch (direction) {
		case Enums::Direction::NW:
			MoveLeft(n);
			MoveUp(n);
			break;
		case Enums::Direction::N:
			MoveUp(n);
			break;
		case Enums::Direction::NE:
			MoveRight(n);
			MoveUp(n);
			break;
		case Enums::Direction::W:
			MoveLeft(n);
			break;
		case Enums::Direction::E:
			MoveRight(n);
			break;
		case Enums::Direction::SW:
			MoveLeft(n);
			MoveDown(n);
			break;
		case Enums::Direction::S:
			MoveDown(n);
			break;
		case Enums::Direction::SE:
			MoveRight(n);
			MoveDown(n);
			break;
		default:
			break;
	}
}

void Character::setStrength(uint16_t newStrength){
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

void Character::setClass(Enums::CharacterClass _characterClass){
	characterClass = _characterClass;
}

void Character::setExperienceValue(uint8_t experienceValue){
	Character::experienceValue = experienceValue;
}

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

uint16_t Character::getCurrentHealth() const {
	if (current_health > getModifiedMaxHealth())
		return getModifiedMaxHealth();

	return current_health;
}

void Character::setCurrentHealth(uint16_t newCurrentHealth) {

	current_health = newCurrentHealth;
}

uint16_t Character::getModifiedMaxHealth() const {
	return getMaxHealth();
}

void Character::modifyCurrentHealth(int16_t currentHealthModifier) {
	setCurrentHealth(getModifiedAttributeValue(getCurrentHealth(), currentHealthModifier, NULLABLE_ATTRIBUTE_MIN, getModifiedMaxHealth()));
}


bool Character::CheckMouseOver(int _x_pos, int _y_pos) {
	int myWidth = getWidth();
	int myHeight = getHeight();
	if (((x_pos < _x_pos) && ((x_pos + myWidth) > _x_pos))
		&& ((y_pos < _y_pos) && ((y_pos + myHeight) > _y_pos))) {
		return true;
	}else {
		return false;
	}
}

bool Character::hasTarget(Character* target) {

	if (Character::target == target) {
		return true;
	}

	return false;
}

void Character::setTarget(Character *target) {
	this->target = target;
}

void Character::setTarget(Character *target, Enums::Attitude attitude) {
	this->target = target;
	targetAttitude = attitude;
}

Character* Character::getTarget() const {
	return target;
}

float Character::getPreparationPercentage() const {
	if (isPreparing) {

		return preparationPercentage;
	}else {

		return 0;
	}
}

std::string Character::getCurrentSpellActionName() const {
	//if (curSpellAction != NULL) {
		//return curSpellAction->getName();
	//}
	return "Spell_Action";
}

Enums::Direction Character::GetOppositeDirection(Enums::Direction direction) {
	switch (direction) {
	case Enums::Direction::NW:
		return Enums::Direction::SE;
		break;
	case Enums::Direction::N:
		return Enums::Direction::S;
		break;
	case Enums::Direction::NE:
		return Enums::Direction::SW;
		break;
	case Enums::Direction::W:
		return Enums::Direction::E;
		break;
	case Enums::Direction::E:
		return Enums::Direction::W;
		break;
	case Enums::Direction::SW:
		return Enums::Direction::NE;
		break;
	case Enums::Direction::S:
		return Enums::Direction::N;
		break;
	case Enums::Direction::SE:
		return Enums::Direction::NW;
		break;
	default:
		return Enums::Direction::STOP;
		break;
	}
}

std::string Character::AttitudeToString(Enums::Attitude attitude) {
	switch (attitude) {
	case Enums::Attitude::HOSTILE:
		return "HOSTILE";
	case Enums::Attitude::NEUTRAL:
		return "NEUTRAL";
	case Enums::Attitude::FRIENDLY:
		return "FRIENDLY";
	}
}

std::string Character::ActivityToString(Enums::ActivityType activity) {
	switch (activity) {
	case Enums::ActivityType::Walking:
		return "Walking";
	case Enums::ActivityType::Dying:
		return "Dying";
	case Enums::ActivityType::Attacking:
		return "ATTACKING";
	case Enums::ActivityType::Casting:
		return "Casting";
	case Enums::ActivityType::Shooting:
		return "Shooting";
	}
}


////////////////////////////////////////////////////////////////////////////////
void CharacterType::addMoveTexture(Enums::ActivityType activity, Enums::Direction direction, int index, std::string filename, unsigned int maxWidth, unsigned int maxHeight, bool reload, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {
	TileSet& tileSet = m_moveTileSets[{activity, direction}];
	tileSet.addTile(filename, maxWidth, maxHeight, reload, paddingLeft, paddingRight, paddingTop, paddingBottom);
}

void CharacterType::calcNumMoveTexturesPerDirection() {
	std::unordered_map<std::pair<int, int>, TileSet, pair_hash>::const_iterator it;

	for (it = m_moveTileSets.begin(); it != m_moveTileSets.end(); it++) {
		Enums::ActivityType currActivity = static_cast<Enums::ActivityType>(it->first.first);
		if (m_numMoveTexturesPerDirection.find(static_cast<Enums::ActivityType>(it->first.first)) == m_numMoveTexturesPerDirection.end()) {
			m_numMoveTexturesPerDirection[static_cast<Enums::ActivityType>(it->first.first)] = static_cast<unsigned short>(m_moveTileSets.at({ static_cast<Enums::ActivityType>(it->first.first), Enums::Direction::S }).getAllTiles().size());
		}

	}
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