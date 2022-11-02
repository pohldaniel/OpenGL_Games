#include "Character.h"
#include "Player.h"
#include "TilesetManager.h"
#include "Constants.h"
#include "Spells.h"
#include "Actions.h"
#include "Magic.h"
#include "Interface.h"
#include "Statssystem.h"
#include "Zone.h"

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

Character::Character() : strength(1),
dexterity(1),
vitality(1),
intellect(1),
wisdom(1),
max_health(1),
current_health(1),
max_mana(0),
current_mana(0),
healthRegen(0),
manaRegen(0),
armor(0),
damageModifierPoints(0),
hitModifierPoints(0),
evadeModifierPoints(0),
blockModifierPoints(0),
meleeCriticalModifierPoints(0),
resistElementModifierPoints(NULL),
resistAllModifierPoints(0),
spellEffectElementModifierPoints(NULL),
spellEffectAllModifierPoints(0),
spellCriticalModifierPoints(0),
experienceValue(0),
level(1) {

	wander_radius = 40;
	activeDirection = Enums::Direction::S;	

	resistElementModifierPoints = new uint16_t[static_cast<size_t>(Enums::ElementType::CountET)];
	spellEffectElementModifierPoints = new uint16_t[static_cast<size_t>(Enums::ElementType::CountET)];
	for (unsigned int curElement = 0; curElement<static_cast<size_t>(Enums::ElementType::CountET); ++curElement) {
		resistElementModifierPoints[curElement] = 0;
		spellEffectElementModifierPoints[curElement] = 0;
	}
}

void Character::setNumActivities(unsigned short numActivities) {	
	m_numActivities = numActivities;
}

unsigned short Character::getNumActivities() {
	return m_numActivities;
}

void Character::baseOnType(std::string characterType) {
	const CharacterType& other = CharacterTypeManager::Get().getCharacterType(characterType);
		
	setStrength(other.strength);
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
	setExperienceValue(other.experienceValue);

	for (const auto& spell : other.spellbook) {
		inscribeSpellInSpellbook(spell);
	}
}

Enums::ActivityType Character::getCurActivity() const {
	//Enums::ActivityType curActivity = Enums::ActivityType::Walking;
	if (curSpellAction != NULL) {
		if (dynamic_cast<CSpell*>(curSpellAction) != NULL) {
			return Enums::ActivityType::Casting;
		}
		else if (dynamic_cast<RangedDamageAction*>(curSpellAction) != NULL) {
			return Enums::ActivityType::Shooting;
		}
		else if (dynamic_cast<MeleeDamageAction*>(curSpellAction) != NULL) {
			return Enums::ActivityType::Attacking;
		}
	}

	/*if (isAlive() == false && (dyingStartFrame < SDL_GetTicks() + 10000)) {
		curActivity = ActivityType::Dying;
	}*/

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
	for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell]->getCharacterState().first == Enums::CharacterStates::Stunned) {
			return true;
		}
	}
	return false;
}

bool Character::isMesmerized() const {
	for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell]->getCharacterState().first == Enums::CharacterStates::Mesmerized) {
			return true;
		}
	}
	return false;
}

bool Character::isFeared() const {
	for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell]->getCharacterState().first == Enums::CharacterStates::Feared) {
			return true;
		}
	}
	return false;
}

bool Character::isConfused() const {
	for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell]->getCharacterState().first == Enums::CharacterStates::Confused) {
			return true;
		}
	}
	return false;
}

bool Character::isCharmed() const {
	for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell]->getCharacterState().first == Enums::CharacterStates::Charmed) {
			return true;
		}
	}
	return false;
}

bool Character::isChanneling() const {
	for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell]->getCharacterState().first == Enums::CharacterStates::Channeling) {
			return true;
		}
	}
	return false;
}

bool Character::isSneaking() const {
	for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell]->getCharacterState().first == Enums::CharacterStates::Sneaking) {
			return true;
		}
	}
	return false;
}

bool Character::isInvisible() const {
	for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell]->getCharacterState().first == Enums::CharacterStates::Invisible) {
			return true;
		}
	}
	return false;
}

bool Character::getIsPreparing() const {
	//return isPreparing;
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
	 current_health = getModifiedMaxHealth();
	// }
}

void Character::setMaxMana(uint16_t newMaxMana){
	max_mana = newMaxMana;
	// if ( current_mana > getModifiedMaxMana() )
	// {
	 current_mana = getModifiedMaxMana();
	// }
}

void Character::setMaxFatigue(uint16_t newMaxFatigue){
	max_fatigue = newMaxFatigue;
	// if ( current_fatigue > getModifiedMaxFatigue() )
	// {
	current_fatigue = getModifiedMaxFatigue();
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

uint16_t Character::getParryModifierPoints() const {
	return parryModifierPoints;
}

uint16_t Character::getBlockModifierPoints() const {
	return blockModifierPoints;
}

uint16_t Character::getMeleeCriticalModifierPoints() const {
	return meleeCriticalModifierPoints;
}

uint16_t Character::getSpellCriticalModifierPoints() const {
	return spellCriticalModifierPoints;
}

uint16_t Character::getSpellEffectElementModifierPoints(Enums::ElementType elementType) const {
	return spellEffectElementModifierPoints[static_cast<size_t>(elementType)];
}

uint16_t Character::getSpellEffectAllModifierPoints() const {
	return spellEffectAllModifierPoints;
}

uint16_t Character::getResistElementModifierPoints(Enums::ElementType elementType) const {
	return resistElementModifierPoints[static_cast<size_t>(elementType)];
}

uint16_t Character::getResistAllModifierPoints() const {
	return resistAllModifierPoints;
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

uint16_t Character::getModifiedStrength() const {
	return getStrength();
}

uint16_t Character::getModifiedDexterity() const {
	return getDexterity();
}

uint16_t Character::getModifiedVitality() const {
	return getVitality();
}

uint16_t Character::getModifiedIntellect() const{
	return getIntellect();
}

uint16_t Character::getModifiedWisdom() const {
	return getWisdom();
}

int Character::getDeltaX() {
	return dx;
}

int Character::getDeltaY() {
	return dy;
}

void Character::setExperience(uint64_t experience) {
	this->experience = experience;
}

uint64_t Character::getExperience() const {
	return experience;
}

void Character::addDamageDisplayToGUI(int amount, bool critical, uint8_t damageType) {
	if (isPlayer()){
		Interface::Get().addCombatText(amount, critical, damageType, 140, ViewPort::get().getHeight() - 40, true);
	}else {
		Interface::Get().addCombatText(amount, critical, damageType, getXPos() + getWidth() / 2, getYPos() + getHeight() + 52, false);
	}
}

uint16_t Character::getModifiedSpellEffectElementModifierPoints(Enums::ElementType elementType) const {
	return getSpellEffectElementModifierPoints(elementType) + getSpellEffectAllModifierPoints() + StatsSystem::getStatsSystem()->calculateSpellEffectElementModifierPoints(elementType, this);
}

uint16_t Character::getModifiedResistElementModifierPoints(Enums::ElementType elementType) const {
	return getResistElementModifierPoints(elementType) + getResistAllModifierPoints() + StatsSystem::getStatsSystem()->calculateResistElementModifierPoints(elementType, this);
}

uint16_t Character::getModifiedSpellCriticalModifierPoints() const {
	return getSpellCriticalModifierPoints() + StatsSystem::getStatsSystem()->calculateSpellCriticalModifierPoints(this);
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

Enums::Attitude Character::getTargetAttitude() {
	return targetAttitude;
}

float Character::getPreparationPercentage() const {
	if (isPreparing) {

		return preparationPercentage;
	}else {

		return 0;
	}
}

std::string Character::getCurrentSpellActionName() const {
	if (curSpellAction != NULL) {
		return curSpellAction->getName();
	}
	return "";
}

void Character::setClass(Enums::CharacterClass characterClass) {
	this->characterClass = characterClass;
	switch (characterClass) {
		/// all caster classes here...
	case Enums::CharacterClass::Liche:
		characterArchType = Enums::CharacterArchType::Caster;
		break;

		/// and all other fighter classes here...
	case Enums::CharacterClass::Ranger:
	case Enums::CharacterClass::Warrior:
		characterArchType = Enums::CharacterArchType::Fighter;
		break;
	}

	/// loading our portrait for the class
	/*if (portrait != NULL){
		delete portrait;
	}

	portrait = new CTexture;
	portrait->LoadIMG(CharacterClass::getCharacterClassPortrait(getClass()), 0);*/
}

Enums::CharacterArchType Character::getArchType() const {
	return characterArchType;
}

uint16_t Character::getModifiedMaxMana() const {
	return getMaxMana();
}


void Character::modifyMaxMana(int16_t maxManaModifier) {
	setMaxMana(getModifiedAttributeValue(max_mana, maxManaModifier, NULLABLE_ATTRIBUTE_MIN));
}

uint16_t Character::getCurrentMana() const {
	if (current_mana > getModifiedMaxMana())
		return getModifiedMaxMana();

	return current_mana;
}

void Character::setCurrentMana(uint16_t newCurrentMana) {
	current_mana = newCurrentMana;
}

void Character::modifyCurrentMana(int16_t currentManaModifier) {
	setCurrentMana(getModifiedAttributeValue(getCurrentMana(), currentManaModifier, NULLABLE_ATTRIBUTE_MIN, getModifiedMaxMana()));
}


uint16_t Character::getModifiedMaxFatigue() const {
	return getMaxFatigue();
}

void Character::modifyMaxFatigue(int16_t maxFatigueModifier) {
	setMaxFatigue(getModifiedAttributeValue(max_fatigue, maxFatigueModifier, NULLABLE_ATTRIBUTE_MIN));
}

uint16_t Character::getCurrentFatigue() const {
	if (current_fatigue > getModifiedMaxFatigue())
		return getModifiedMaxFatigue();

	return current_fatigue;
}

void Character::setCurrentFatigue(uint16_t newCurrentFatigue) {
	current_fatigue = newCurrentFatigue;
}

void Character::modifyCurrentFatigue(int16_t currentFatigueModifier) {
	setCurrentFatigue(getModifiedAttributeValue(getCurrentFatigue(), currentFatigueModifier, NULLABLE_ATTRIBUTE_MIN, getModifiedMaxFatigue()));
}

void Character::regenerateLifeManaFatigue(uint32_t regenPoints) {
	/** Regenerate life, mana and fatigue every 1000 ms. **/

	/*remainingRegenPoints += regenPoints;

	if (remainingRegenPoints > 1000) {
		modifyCurrentMana(getModifiedManaRegen());
		modifyCurrentHealth(getModifiedHealthRegen());
		modifyCurrentFatigue(getModifiedFatigueRegen());
		remainingRegenPoints -= 1000;
	}*/
}

bool Character::isAlive() const {
	return alive;
}

bool Character::canBeDamaged() const {
	return true;
}

void Character::addActiveSpell(CSpellActionBase *spell) {
	if (!canBeDamaged())
		return;

	// here we check to see if the current spell cast is already on the character. if it is, then we refresh it.
	for (size_t curSpell = 0; curSpell < activeSpells.size(); curSpell++) {
		if (activeSpells[curSpell]->getID() == spell->getID())
		{
			// we replace the old spell with a new, in case a more powerful spell is cast (a higher rank)
			activeSpells[curSpell] = spell;
			activeSpells[curSpell]->m_timer.reset();
			return;
		}
	}

	// add new spell on character.
	activeSpells.push_back(spell);
	activeSpells.back()->m_timer.reset();
	// if we're an NPC and the spell is hostile, we want to set the caster to hostile.
	if (isPlayer() == false && spell->isSpellHostile() == true) {
		// in the future when having more than one player playing, this function needs to be reworked.
		dynamic_cast<Npc*>(this)->chasePlayer(&Player::Get());
	}
}

void Character::cleanupActiveSpells() {
	size_t curSpell = 0;
	while (curSpell < activeSpells.size()) {
		if (activeSpells[curSpell]->isEffectComplete() == true) {
			delete activeSpells[curSpell];
			activeSpells.erase(activeSpells.begin() + curSpell);
		}else {
			curSpell++;
		}
	}
}

void Character::clearActiveSpells() {
	activeSpells.clear();
}

void Character::removeSpellsWithCharacterState(Enums::CharacterStates characterState) {
	// we remove spells based on what character states they have.
	// removing active spells can cause NULL pointers, because they can be active in some damage cycle or other functions.
	// Therefor in order to "remove" these spells we just mark them as completed, and let the cleanup-function handle the removal of the spells.
	for (size_t curSpell = 0; curSpell < activeSpells.size(); curSpell++) {
		if (activeSpells[curSpell]->getCharacterState().first == characterState) {
			activeSpells[curSpell]->markSpellActionAsFinished();
		}
	}
}

void Character::removeActiveSpell(CSpellActionBase* activeSpell) {
	for (size_t curSpell = 0; curSpell < activeSpells.size(); curSpell++) {
		if (activeSpells[curSpell] == activeSpell) {
			activeSpells[curSpell]->markSpellActionAsFinished();
		}
	}
}

std::vector<CSpellActionBase*> Character::getActiveSpells() const {
	return activeSpells;
}

void Character::addCooldownSpell(CSpellActionBase *spell) {

	if (spell->getCooldown() > 0) {
		cooldownSpells.push_back(spell);
		cooldownSpells.back()->m_timer.reset();
	}
}

void Character::cleanupCooldownSpells() {
	size_t curSpell = 0;
	while (curSpell < cooldownSpells.size()) {
		if (cooldownSpells[curSpell]->m_timer.getElapsedTimeMilli() > cooldownSpells[curSpell]->getCooldown() * 1000u) {
			delete cooldownSpells[curSpell];
			cooldownSpells.erase(cooldownSpells.begin() + curSpell);
			if (isPlayer() == true) {
				// this will seed a new ticket for the itemtooltip and spelltooltips, causing them to reload.
				dynamic_cast<Player*>(this)->setTicketForItemTooltip();
				dynamic_cast<Player*>(this)->setTicketForSpellTooltip();
			}
		}else {
			curSpell++;
		}
	}
}

void Character::clearCooldownSpells() {
	cooldownSpells.clear();
}

std::vector<CSpellActionBase*> Character::getCooldownSpells() const {
	return cooldownSpells;
}

uint32_t Character::getTicksOnCooldownSpell(std::string spellName) const {
	for (size_t curSpell = 0; curSpell < cooldownSpells.size(); curSpell++) {
		if (cooldownSpells[curSpell]->getName() == spellName) {
			return cooldownSpells[curSpell]->m_timer.getElapsedTimeMilli();
		}
	}
	return 0u;
}

bool Character::isSpellOnCooldown(std::string spellName) const {
	for (size_t curSpell = 0; curSpell < cooldownSpells.size(); curSpell++) {
		if (cooldownSpells[curSpell]->getName() == spellName) {
			return true;
		}
	}
	return false;
}

void Character::Die() {
	/*if (hasChoosenDyingDirection == false) {
		dyingDirection = static_cast<Direction>(activeDirection);
		dyingStartFrame = SDL_GetTicks();
		reduceDyingTranspFrame = SDL_GetTicks() + 7000;
	}*/
	curActivity = Enums::ActivityType::Dying;
}

void Character::Damage(int amount, bool criticalHit) {
	if (isFeared() == true) { // if we're feared and taking damage, we have a 20% chance to break from the fear
		if (RNG::randomSizeT(0, 100) <= 20) {
			removeSpellsWithCharacterState(Enums::CharacterStates::Feared);
		}
	}

	if (isChanneling() == true) { // if we're channeling something while taking damage, we loose focus and abort the channeling.
		removeSpellsWithCharacterState(Enums::CharacterStates::Channeling);
	}

	if (isSneaking() == true) { // if we're sneaking while taking damage, we loose the sneak state
		removeSpellsWithCharacterState(Enums::CharacterStates::Sneaking);
	}

	if (isInvisible() == true) { // if we're invisible while taking damage, we loose the invisible state
		removeSpellsWithCharacterState(Enums::CharacterStates::Invisible);
	}

	if (isMesmerized() == true) { // if we're mesmerized while taking damage, we loose the mesmerize state
		removeSpellsWithCharacterState(Enums::CharacterStates::Mesmerized);
	}

	/// here we check for equipped items if they have any trigger spells which is used in TriggerType::TAKING_DAMAGE
	/*if (isPlayer() == true) {
		std::vector<InventoryItem*> inventory = Globals::getPlayer()->getInventory()->getEquippedItems();
		for (size_t curItem = 0; curItem < inventory.size(); curItem++) {
			std::vector<TriggerSpellOnItem*> triggerSpells = inventory[curItem]->getItem()->getTriggerSpells();
			for (size_t curSpell = 0; curSpell < triggerSpells.size(); curSpell++)
			{
				/// searches for spells on the item with the triggertype TAKING_DAMAGE.
				if (triggerSpells[curSpell]->getTriggerType() == TriggerType::TAKING_DAMAGE) {
					/// found one, check to see if we manages to trigger the spell.

					if ((RNG::randomSizeT(0, 10000) <= triggerSpells[curSpell]->getChanceToTrigger() * 10000) == true) {
						/// we triggered this spell, now we cast it based on if it's a self-cast spell or a cast on our target spell.
						if (triggerSpells[curSpell]->getCastOnSelf() == true) {
							/// cast spell on self
							executeSpellWithoutCasting(triggerSpells[curSpell]->getSpellToTrigger(), this);
						}
						else {
							if (this->getTarget() != NULL) {
								/// cast spell on the character's target.
								executeSpellWithoutCasting(triggerSpells[curSpell]->getSpellToTrigger(), this->getTarget());
							}
						}
					}
				}
			}
		}
	}*/

	if (alive) {

		addDamageDisplayToGUI(amount, criticalHit, 0);
		if (current_health <= amount) {
			current_health = 0;
			Die();
			if (isPlayer() == false) {
				Player *player = &Player::Get();
				player->gainExperience(getExperienceValue());
			}
		}else {
			modifyCurrentHealth(-amount);
		}
	}
}

uint64_t Character::getExpNeededForLevel(uint8_t level) const {
	uint64_t result = (level*(level - 1) * 50);
	return result;
}

bool Character::canRaiseLevel() const {
	return (experience >= getExpNeededForLevel(getLevel() + 1) && (getExpNeededForLevel(getLevel() + 1) != getExpNeededForLevel(getLevel())));
}

void Character::raiseLevel() {
	if (canRaiseLevel()) {
		setMaxHealth(getMaxHealth() * 1.1);
		setStrength(getStrength() * 1.1);
		setLevel(getLevel() + 1);
		GLfloat yellow[] = { 1.0f, 1.0f, 0.0f };
		if (isPlayer() == true)
		{
			//dynamic_cast<Player*>(this)->setTicketForItemTooltip();
			//dynamic_cast<Player*>(this)->setTicketForSpellTooltip();
		}
		//DawnInterface::addTextToLogWindow(yellow, "You are now a level %d %s.", getLevel(), getClassName().c_str());
	}
}

void Character::gainExperience(uint64_t addExp) {
	if (isPlayer()) {

		if (std::numeric_limits<uint64_t>::max() - addExp < experience) {
			experience = std::numeric_limits<uint64_t>::max();

		}
		else {
			experience += addExp;
			GLfloat yellow[] = { 1.0f, 1.0f, 0.0f };
			//DawnInterface::addTextToLogWindow(yellow, "You gain %d experience.", addExp);
		}

		while (canRaiseLevel()) {
			raiseLevel();
		}
	}
}

void Character::Heal(int amount) {
	if (alive) {
		uint16_t modifiedDiff = getModifiedMaxHealth() - getCurrentHealth();
		if (modifiedDiff <= amount) {
			amount = modifiedDiff;
		}
		addDamageDisplayToGUI(amount, false, 1);
		modifyCurrentHealth(amount);
	}
}

void Character::executeSpellWithoutCasting(CSpellActionBase *spell, Character *target) {

	CSpellActionBase *newSpell = NULL;

	newSpell = spell->cast(this, target, false);

	if (newSpell != NULL) {
		newSpell->startEffect();
	}
}

void Character::inscribeSpellInSpellbook(CSpellActionBase *spell) {
	if (spell->getRequiredClass() == getClass() || spell->getRequiredClass() == Enums::CharacterClass::ANYCLASS) {
		for (size_t curSpell = 0; curSpell < spellbook.size(); curSpell++) {
			if (spellbook[curSpell]->getName() == spell->getName()) {
				if (spellbook[curSpell]->getRank() < spell->getRank()) {
					spellbook[curSpell] = spell;
					if (isPlayer() == true) {
						// this will seed a new ticket for the itemtooltip, causing it to reload. We might need this because of the tooltip message displaying already known spells and ranks.
						//dynamic_cast<Player*>(this)->setTicketForItemTooltip();
					}
				}
				return;
			}
		}
		spellbook.push_back(spell);

		if (isPlayer() == true) {
			// this will seed a new ticket for the itemtooltip, causing it to reload. We might need this because of the tooltip message displaying already known spells and ranks.
			//dynamic_cast<Player*>(this)->setTicketForItemTooltip();
		}
	}
}

std::vector<CSpellActionBase*> Character::getSpellbook() const {
	return spellbook;
}

bool Character::castSpell(CSpellActionBase *spell) {

	if (isStunned() == true || isFeared() == true || isMesmerized() == true || isCharmed() == true) {
		/// can't cast, we're stunned, feared, mesmerized or charmed. Should perhaps display message about it.
		return false;
	}
	
	if (dynamic_cast<CAction*>(spell) != NULL) {		
		if (spell->getSpellCost() > getCurrentFatigue()) {
			
			/// can't cast. cost more fatigue than we can afford. Display message here about it.
			return false;
		}
	}else if (dynamic_cast<CSpell*>(spell) != NULL) {
		if (spell->getSpellCost() > getCurrentMana()) {
			
			/// can't cast. not enough mana. Display message here about it.
			return false;
		}
	}
	
	if (spell->getEffectType() != Enums::EffectType::SelfAffectingSpell && getTarget() != NULL) {
		uint16_t distance = sqrt(pow((getXPos() + getWidth() / 2) - (getTarget()->getXPos() + getTarget()->getWidth() / 2), 2) + pow((getYPos() + getHeight() / 2) - (getTarget()->getYPos() + getTarget()->getHeight() / 2), 2));
		if (spell->isInRange(distance) == false) {
			/// can't cast, not in range. Display message here about it...
			return false;
		}
	}


	if (isPlayer() == true) {
		if (spell->getRequiredWeapons() != 0) {
			//if ((spell->getRequiredWeapons() & (dynamic_cast<Player*>(this)->getInventory()->getWeaponTypeBySlot(Enums::ItemSlot::MAIN_HAND) | dynamic_cast<Player*>(this)->getInventory()->getWeaponTypeBySlot(Enums::ItemSlot::OFF_HAND))) == 0) {
				/// can't cast spell, not wielding required weapon. Display message here about it...
				return false;
			//}
		}
	}

	for (size_t curSpell = 0; curSpell < cooldownSpells.size(); curSpell++){

		if (cooldownSpells[curSpell]->getID() == spell->getID()){
			
			if (cooldownSpells[curSpell]->m_timer.getElapsedTimeMilli() < spell->getCooldown() * 1000){
				/// can't cast, spell has a cooldown on it. Display message about it.
				return false;
			}
		}
	}

	// if we're invisible, sneaking or channeling while casting, we remove that spell.
	if (isSneaking() == true) {
		removeSpellsWithCharacterState(Enums::CharacterStates::Sneaking);
	}

	if (isInvisible() == true) {
		removeSpellsWithCharacterState(Enums::CharacterStates::Invisible);
	}

	if (isChanneling() == true) {
		removeSpellsWithCharacterState(Enums::CharacterStates::Channeling);
	}
	
	giveToPreparation(spell);
	return true;
}

void Character::giveToPreparation(CSpellActionBase *toPrepare) {
	if (curSpellAction != NULL) {
		// don't cast / execute. Enqueue in the list of coming actions / spells ?
		delete toPrepare;
	}else {
		// setup all variables for casting / executing
		isPreparing = true;
		curSpellAction = toPrepare;
		curSpellAction->playSoundSpellCasting();
		toPrepare->beginPreparationOfSpellAction();
		preparationStartTime = Globals::clock.getElapsedTimeMilli();
		continuePreparing();
	}
}

bool Character::continuePreparing() {
	
	/// if we're preparing a spell while getting stunned, feared, mesmerized or charmed, abort the spellcasting.
	if ((isStunned() == true || isFeared() == true || isMesmerized() == true || isCharmed() == true) && getIsPreparing() == true) {
		CastingAborted();
	}
	
	if (isPreparing) {
		bool preparationFinished = (curSpellAction->getCastTime() == 0);
		if (!preparationFinished) {
			preparationCurrentTime = Globals::clock.getElapsedTimeMilli();

			// casting_percentage is mostly just for the castbar display, guess we could alter this code.
			uint16_t spellCastTime = curSpellAction->getCastTime();

			// if we're confused while casting, we add 35% more time to our spellcasting.
			if (isConfused() == true) {
				spellCastTime *= 1.35;
			}
			
			preparationPercentage = (static_cast<float>(preparationCurrentTime - preparationStartTime)) / spellCastTime;
			preparationFinished = (preparationPercentage >= 1.0f);
			interruptCurrentActivityWith(Enums::ActivityType::Walking);
		}

		if(preparationFinished)
			startSpellAction();
	}

	return isPreparing;
}

void Character::CastingAborted() {
	// if we moved, got stunned, or in some way unable to complete the spell ritual, spellcasting will fail.
	// If we are following the above instructions to use a pointer to a spell and so on, we should clear that pointer here.
	abortCurrentSpellAction();
}

void Character::abortCurrentSpellAction() {
	if (isPreparing) {
		curSpellAction->stopSoundSpellCasting();
		delete curSpellAction;
		curSpellAction = NULL;
		isPreparing = false;
	}
}

void Character::startSpellAction() {
	isPreparing = false;
	preparationCurrentTime = 0;
	preparationStartTime = 0;
	
	/// here we check for equipped items if they have any trigger spells which is used in TriggerType::EXECUTING_ACTION
	/*if (isPlayer() == true) {
		std::vector<InventoryItem*> inventory = Globals::getPlayer()->getInventory()->getEquippedItems();
		for (size_t curItem = 0; curItem < inventory.size(); curItem++) {
			std::vector<TriggerSpellOnItem*> triggerSpells = inventory[curItem]->getItem()->getTriggerSpells();
			for (size_t curSpell = 0; curSpell < triggerSpells.size(); curSpell++)
			{
				/// searches for spells on the item with the triggertype EXECUTING_ACTION.
				if (triggerSpells[curSpell]->getTriggerType() == TriggerType::EXECUTING_ACTION) {
					/// found one, check to see if we manages to trigger the spell.

					if ((RNG::randomSizeT(0, 10000) <= triggerSpells[curSpell]->getChanceToTrigger() * 10000) == true) {
						/// we triggered this spell, now we cast it based on if it's a self-cast spell or a cast on our target spell.
						if (triggerSpells[curSpell]->getCastOnSelf() == true) {
							/// cast spell on self
							executeSpellWithoutCasting(triggerSpells[curSpell]->getSpellToTrigger(), this);
						}
						else {
							if (this->getTarget() != NULL) {
								/// cast spell on the character's target.
								executeSpellWithoutCasting(triggerSpells[curSpell]->getSpellToTrigger(), this->getTarget());
							}
						}
					}
				}
			}
		}
	}*/

	// are we casting an AoE spell?
	if (curSpellAction->getRadius() > 0) {
		ZoneManager::Get().getCurrentZone()->MagicMap.push_back(new CMagic(curSpellAction));
		ZoneManager::Get().getCurrentZone()->MagicMap.back()->setCreator(this);
		ZoneManager::Get().getCurrentZone()->MagicMap.back()->getSpell()->startEffect();

		isPreparing = false;
		preparationCurrentTime = 0;
		preparationStartTime = 0;

	} else {
		curSpellAction->stopSoundSpellCasting();
		curSpellAction->startEffect();
	}
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

void CharacterType::baseOnType(std::string name) {
	const CharacterType& other = CharacterTypeManager::Get().getCharacterType(name);

	m_moveTileSets = other.m_moveTileSets;
	m_numMoveTexturesPerDirection = other.m_numMoveTexturesPerDirection;
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
	spellbook = other.spellbook;
	
}

void CharacterType::inscribeSpellInSpellbook(CSpellActionBase *spell) {	
	spellbook.push_back(spell);	
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

void CharacterType::modifyStrength(int strengthModifier) {
	strength += strengthModifier;
}

void CharacterType::modifyDexterity(int dexterityModifier) {
	dexterity += dexterityModifier;
}

void CharacterType::modifyVitality(int vitalityModifier) {
	vitality += vitalityModifier;
}

void CharacterType::modifyIntellect(int intellectModifier) {
	intellect += intellectModifier;
}

void CharacterType::modifyWisdom(int wisdomModifier) {
	wisdom += wisdomModifier;
}

void CharacterType::modifyMaxHealth(int maxHealthModifier) {
	max_health += maxHealthModifier;
}

void CharacterType::modifyMaxFatigue(int maxFatigueModifier) {
	max_fatigue += maxFatigueModifier;
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