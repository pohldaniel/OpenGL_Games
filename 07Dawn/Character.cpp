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

	activeDirection = Enums::Direction::S;	

	resistElementModifierPoints = new uint16_t[static_cast<size_t>(Enums::ElementType::CountET)];
	spellEffectElementModifierPoints = new uint16_t[static_cast<size_t>(Enums::ElementType::CountET)];
	for (unsigned int curElement = 0; curElement<static_cast<size_t>(Enums::ElementType::CountET); ++curElement) {
		resistElementModifierPoints[curElement] = 0;
		spellEffectElementModifierPoints[curElement] = 0;
	}
}

void Character::setCharacterType(std::string characterType) {
	
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
			if (m_isPlayer == false) {
				Player *player = &Player::Get();
				player->gainExperience(getExperienceValue());
			}
		}
		else {
			modifyCurrentHealth(-amount);
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

void Character::Die() {
	/*if (hasChoosenDyingDirection == false) {
	dyingDirection = static_cast<Direction>(activeDirection);
	dyingStartFrame = SDL_GetTicks();
	reduceDyingTranspFrame = SDL_GetTicks() + 7000;
	}*/
	curActivity = Enums::ActivityType::Dying;
}

bool Character::castSpell(SpellActionBase *spell) {

	if (isStunned() == true || isFeared() == true || isMesmerized() == true || isCharmed() == true) {
		/// can't cast, we're stunned, feared, mesmerized or charmed. Should perhaps display message about it.
		return false;
	}

	if (dynamic_cast<Action*>(spell) != NULL) {
		if (spell->getSpellCost() > getCurrentFatigue()) {

			/// can't cast. cost more fatigue than we can afford. Display message here about it.
			return false;
		}
	}else if (dynamic_cast<Spell*>(spell) != NULL) {
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


	if (m_isPlayer == true) {
		if (spell->getRequiredWeapons() != 0) {
			//if ((spell->getRequiredWeapons() & (dynamic_cast<Player*>(this)->getInventory()->getWeaponTypeBySlot(Enums::ItemSlot::MAIN_HAND) | dynamic_cast<Player*>(this)->getInventory()->getWeaponTypeBySlot(Enums::ItemSlot::OFF_HAND))) == 0) {
			/// can't cast spell, not wielding required weapon. Display message here about it...
			return false;
			//}
		}
	}

	for (size_t curSpell = 0; curSpell < cooldownSpells.size(); curSpell++) {

		if (cooldownSpells[curSpell]->getID() == spell->getID()) {

			if (cooldownSpells[curSpell]->m_timer.getElapsedTimeMilli() < spell->getCooldown() * 1000u) {
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

void Character::executeSpellWithoutCasting(SpellActionBase *spell, Character *target) {

	SpellActionBase *newSpell = NULL;

	newSpell = spell->cast(this, target, false);

	if (newSpell != NULL) {
		newSpell->startEffect();
	}
}

void Character::addCooldownSpell(SpellActionBase *spell) {

	if (spell->getCooldown() > 0) {
		cooldownSpells.push_back(spell);
	}
}

void Character::addActiveSpell(SpellActionBase *spell) {
	if (!canBeDamaged())
		return;

	// here we check to see if the current spell cast is already on the character. if it is, then we refresh it.
	for (size_t curSpell = 0; curSpell < activeSpells.size(); curSpell++) {
		if (activeSpells[curSpell]->getID() == spell->getID()) {
			// we replace the old spell with a new, in case a more powerful spell is cast (a higher rank)
			activeSpells[curSpell] = spell;
			return;
		}
	}

	// add new spell on character.
	activeSpells.push_back(spell);
	// if we're an NPC and the spell is hostile, we want to set the caster to hostile.
	if (m_isPlayer == false && spell->isSpellHostile() == true) {
		// in the future when having more than one player playing, this function needs to be reworked.
		dynamic_cast<Npc*>(this)->chasePlayer(&Player::Get());
	}
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

void Character::giveCoins(unsigned int amountOfCoins) {
	coins += amountOfCoins;
}

unsigned int Character::getCoins() const {
	return coins;
}

float Character::getPreparationPercentage() const {
	return isPreparing ? preparationPercentage : 0;
}

bool Character::getIsPreparing() const {
	return isPreparing;
}


std::string Character::getCurrentSpellActionName() const {
	if (curSpellAction != NULL) {
		return curSpellAction->getName();
	}
	return "";
}

bool Character::isStunned() const {
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

bool Character::isAlive() const {
	return alive;
}

bool Character::continuePreparing() {

	/// if we're preparing a spell while getting stunned, feared, mesmerized or charmed, abort the spellcasting.
	if ((isStunned() == true || isFeared() == true || isMesmerized() == true || isCharmed() == true) && getIsPreparing() == true) {
		CastingAborted();
	}

	if (isPreparing) {
		bool preparationFinished = (curSpellAction->getCastTime() == 0);
		if (!preparationFinished) {

			// casting_percentage is mostly just for the castbar display, guess we could alter this code.
			uint16_t spellCastTime = curSpellAction->getCastTime();

			// if we're confused while casting, we add 35% more time to our spellcasting.
			if (isConfused() == true) {
				spellCastTime *= 1.35;
			}
	
			preparationPercentage = static_cast<float>(m_preparationTimer.getElapsedTimeMilli()) / spellCastTime;
			preparationFinished = (preparationPercentage >= 1.0f);

		}

		if (preparationFinished) {
			startSpellAction();
			if(m_isPlayer)
				interruptCurrentActivityWith(Enums::ActivityType::Walking);
		}
	}

	return isPreparing;
}

const CharacterType*  Character::getCharacterType() {
	return m_characterType;
}

std::string Character::getClassName() const{
	return Character::GetCharacterClassName(getClass());
}

std::vector<SpellActionBase*> Character::getSpellbook() const {
	return spellbook;
}

std::vector<SpellActionBase*> Character::getActiveSpells() const {
	return activeSpells;
}

std::vector<SpellActionBase*>& Character::getCooldownSpells() {
	return cooldownSpells;
}

Enums::CharacterArchType Character::getArchType() const {
	return characterArchType;
}

Character* Character::getTarget() const {
	return target;
}

void Character::setTarget(Character *target) {
	this->target = target;
}

void Character::setPosition(int xpos, int ypos) {
	this->x_pos = xpos;
	this->y_pos = ypos;
}

int Character::getXPos() const {
	return x_pos;
}

int Character::getYPos() const {
	return y_pos;
}

int Character::getWidth() const {
	const TextureRect& rect = m_characterType->m_moveTileSets.at({ Enums::ActivityType::Walking, Enums::Direction::S }).getAllTiles()[0].textureRect;
	return useBoundingBox ? boundingBoxW : rect.width;
}

int Character::getHeight() const {
	const TextureRect& rect = m_characterType->m_moveTileSets.at({ Enums::ActivityType::Walking, Enums::Direction::S }).getAllTiles()[0].textureRect;
	return useBoundingBox ? boundingBoxH : rect.height;
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

unsigned short Character::getCurrentHealth() const {
	if (current_health > getModifiedMaxHealth())
		return getModifiedMaxHealth();

	return current_health;
}

unsigned short Character::getCurrentMana() const {
	if (current_mana > getModifiedMaxMana())
		return getModifiedMaxMana();

	return current_mana;
}

unsigned short Character::getCurrentFatigue() const {
	if (current_fatigue > getModifiedMaxFatigue())
		return getModifiedMaxFatigue();

	return current_fatigue;
}

unsigned short Character::getModifiedHealthRegen() const {
	return getHealthRegen();
}


unsigned short Character::getModifiedManaRegen() const {
	return getManaRegen();
}

unsigned short Character::getModifiedFatigueRegen() const {
	return getFatigueRegen();
}

unsigned short Character::getModifiedMaxHealth() const {
	return getMaxHealth();
}

unsigned short Character::getModifiedMaxMana() const {
	return getMaxMana();
}

unsigned short Character::getModifiedMaxFatigue() const {
	return getMaxFatigue();
}

unsigned short Character::getModifiedStrength() const {
	return getStrength();
}

unsigned short Character::getModifiedDexterity() const {
	return getDexterity();
}

unsigned short Character::getModifiedVitality() const {
	return getVitality();
}

unsigned short Character::getModifiedIntellect() const {
	return getIntellect();
}

unsigned short Character::getModifiedWisdom() const {
	return getWisdom();
}

unsigned short Character::getModifiedMinDamage() const {
	return getMinDamage();
}

unsigned short Character::getModifiedMaxDamage() const {
	return getMaxDamage();
}

unsigned short Character::getModifiedSpellEffectElementModifierPoints(Enums::ElementType elementType) const {
	return getSpellEffectElementModifierPoints(elementType) + getSpellEffectAllModifierPoints() + StatsSystem::getStatsSystem()->calculateSpellEffectElementModifierPoints(elementType, this);
}

unsigned short Character::getModifiedResistElementModifierPoints(Enums::ElementType elementType) const {
	return getResistElementModifierPoints(elementType) + getResistAllModifierPoints() + StatsSystem::getStatsSystem()->calculateResistElementModifierPoints(elementType, this);
}

unsigned short Character::getModifiedSpellCriticalModifierPoints() const {
	return getSpellCriticalModifierPoints() + StatsSystem::getStatsSystem()->calculateSpellCriticalModifierPoints(this);
}

unsigned short Character::getModifiedDamageModifierPoints() const {
	return getDamageModifierPoints() + StatsSystem::getStatsSystem()->calculateDamageModifierPoints(this);
}

unsigned short Character::getModifiedHitModifierPoints() const {
	return getHitModifierPoints() + StatsSystem::getStatsSystem()->calculateHitModifierPoints(this);
}

unsigned short Character::getModifiedMeleeCriticalModifierPoints() const {
	return getMeleeCriticalModifierPoints() + StatsSystem::getStatsSystem()->calculateMeleeCriticalModifierPoints(this);
}

unsigned short Character::getModifiedEvadeModifierPoints() const {
	return getEvadeModifierPoints() + StatsSystem::getStatsSystem()->calculateEvadeModifierPoints(this);
}

unsigned short Character::getModifiedParryModifierPoints() const {
	return getParryModifierPoints() + StatsSystem::getStatsSystem()->calculateParryModifierPoints(this);
}

unsigned short Character::getModifiedBlockModifierPoints() const {
	return getBlockModifierPoints() + StatsSystem::getStatsSystem()->calculateBlockModifierPoints(this);
}

unsigned short Character::getModifiedArmor() const {
	return getArmor() + StatsSystem::getStatsSystem()->calculateDamageReductionPoints(this);
}

unsigned short Character::getParryModifierPoints() const {
	return parryModifierPoints;
}

unsigned short Character::getBlockModifierPoints() const {
	return blockModifierPoints;
}

unsigned short Character::getMeleeCriticalModifierPoints() const {
	return meleeCriticalModifierPoints;
}

unsigned short Character::getSpellCriticalModifierPoints() const {
	return spellCriticalModifierPoints;
}

unsigned short Character::getSpellEffectElementModifierPoints(Enums::ElementType elementType) const {
	return spellEffectElementModifierPoints[static_cast<size_t>(elementType)];
}

unsigned short Character::getSpellEffectAllModifierPoints() const {
	return spellEffectAllModifierPoints;
}

unsigned short Character::getResistElementModifierPoints(Enums::ElementType elementType) const {
	return resistElementModifierPoints[static_cast<size_t>(elementType)];
}

unsigned short Character::getResistAllModifierPoints() const {
	return resistAllModifierPoints;
}

////////////////////////////////////////////////////LUA/////////////////////////////////////
Enums::CharacterClass Character::getClass() const {
	return characterClass;
}

std::string Character::getName() const {
	return name;
}

unsigned short Character::getStrength() const {
	return strength;
}

unsigned short Character::getDexterity() const {
	return dexterity;
}

unsigned short Character::getVitality() const {
	return vitality;
}

unsigned short Character::getIntellect() const {
	return intellect;
}

unsigned short Character::getWisdom() const {
	return wisdom;
}

unsigned short Character::getMaxHealth() const {
	return max_health;
}

unsigned short Character::getMaxMana() const {
	return max_mana;
}

unsigned short Character::getMaxFatigue() const {
	return max_fatigue;
}

unsigned short Character::getMaxDamage() const {
	return max_damage;
}

unsigned short Character::getMinDamage() const {
	return min_damage;
}

unsigned short Character::getArmor() const {
	return armor;
}

unsigned short Character::getHealthRegen() const {
	return healthRegen;
}

unsigned short Character::getManaRegen() const {
	return manaRegen;
}

unsigned short Character::getFatigueRegen() const {
	return fatigueRegen;
}

unsigned short Character::getDamageModifierPoints() const {
	return damageModifierPoints;
}

unsigned short Character::getHitModifierPoints() const {
	return hitModifierPoints;
}

unsigned short Character::getEvadeModifierPoints() const {
	return evadeModifierPoints;
}

unsigned short Character::getLevel() const {
	return level;
}

unsigned short Character::getExperienceValue() const {
	return experienceValue;
}

void Character::inscribeSpellInSpellbook(SpellActionBase *spell) {

	if (spell->getRequiredClass() == getClass() || spell->getRequiredClass() == Enums::CharacterClass::ANYCLASS) {
		for (size_t curSpell = 0; curSpell < spellbook.size(); curSpell++) {
			if (spellbook[curSpell]->getName() == spell->getName()) {
				if (spellbook[curSpell]->getRank() < spell->getRank()) {
					spellbook[curSpell] = spell;
					if (m_isPlayer == true) {
						// this will seed a new ticket for the itemtooltip, causing it to reload. We might need this because of the tooltip message displaying already known spells and ranks.
						//dynamic_cast<Player*>(this)->setTicketForItemTooltip();
						dynamic_cast<Player*>(this)->m_reloadItemTooltip = true;
					}
				}
				return;
			}
		}
		spellbook.push_back(spell);

		if (m_isPlayer == true) {
			// this will seed a new ticket for the itemtooltip, causing it to reload. We might need this because of the tooltip message displaying already known spells and ranks.
			//dynamic_cast<Player*>(this)->setTicketForItemTooltip();
			dynamic_cast<Player*>(this)->m_reloadItemTooltip = true;
		}
	}
}

std::string getCharacterClassPortrait(Enums::CharacterClass characterClass) {
	switch (characterClass) {
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

void Character::setClass(Enums::CharacterClass characterClass) {
	this->characterClass = characterClass;
	switch (characterClass) {
		/// all caster classes here...
		case Enums::CharacterClass::Liche:
			characterArchType = Enums::CharacterArchType::Caster;
			break;

			/// and all other fighter classes here...
		case Enums::CharacterClass::Ranger: case Enums::CharacterClass::Warrior:
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

void Character::setName(std::string newName) {
	name = newName;
}

void Character::setStrength(unsigned short newStrength) {
	strength = newStrength;
}


void Character::setDexterity(unsigned short newDexterity) {
	dexterity = newDexterity;
}

void Character::setVitality(unsigned short newVitality) {
	vitality = newVitality;
}

void Character::setIntellect(unsigned short newIntellect) {
	intellect = newIntellect;
}

void Character::setWisdom(unsigned short newWisdom) {
	wisdom = newWisdom;
}

void Character::setMaxHealth(unsigned short newMaxHealth) {
	max_health = newMaxHealth;
	// if ( current_health > getModifiedMaxHealth() )
	// {
	current_health = getModifiedMaxHealth();
	// }
}

void Character::setMaxMana(unsigned short newMaxMana) {
	max_mana = newMaxMana;
	// if ( current_mana > getModifiedMaxMana() )
	// {
	current_mana = getModifiedMaxMana();
	// }
}

void Character::setMaxFatigue(unsigned short newMaxFatigue) {
	max_fatigue = newMaxFatigue;
	// if ( current_fatigue > getModifiedMaxFatigue() )
	// {
	current_fatigue = getModifiedMaxFatigue();
	// }
}

void Character::setHealthRegen(unsigned short newHealthRegen) {
	healthRegen = newHealthRegen;
}

void Character::setManaRegen(unsigned short newManaRegen) {
	manaRegen = newManaRegen;
}

void Character::setFatigueRegen(unsigned short newFatigueRegen) {
	fatigueRegen = newFatigueRegen;
}

void Character::setMaxDamage(unsigned short newMaxDamage) {
	max_damage = newMaxDamage;
}

void Character::setMinDamage(unsigned short newMinDamage) {
	min_damage = newMinDamage;
}

void Character::setArmor(unsigned short newArmor) {
	armor = newArmor;
}

void Character::setDamageModifierPoints(unsigned short newDamageModifierPoints) {
	damageModifierPoints = newDamageModifierPoints;
}

void Character::setHitModifierPoints(unsigned short newHitModifierPoints) {
	hitModifierPoints = newHitModifierPoints;
}

void Character::setEvadeModifierPoints(unsigned short newEvadeModifierPoints) {
	evadeModifierPoints = newEvadeModifierPoints;
}

void Character::setParryModifierPoints(unsigned short newParryModifierPoints){
	parryModifierPoints = newParryModifierPoints;
}

void Character::setBlockModifierPoints(unsigned short newBlockModifierPoints){
	blockModifierPoints = newBlockModifierPoints;
}

void Character::setMeleeCriticalModifierPoints(unsigned short newMeleeCriticalModifierPoints){
	meleeCriticalModifierPoints = newMeleeCriticalModifierPoints;
}

void Character::setResistElementModifierPoints(Enums::ElementType elementType, unsigned short newResistElementModifierPoints){
	resistElementModifierPoints[static_cast<size_t>(elementType)] = newResistElementModifierPoints;
}

void Character::setResistAllModifierPoints(unsigned short newResistAllModifierPoints){
	resistAllModifierPoints = newResistAllModifierPoints;
}

void Character::setSpellEffectElementModifierPoints(Enums::ElementType elementType, unsigned short newSpellEffectElementModifierPoints){
	spellEffectElementModifierPoints[static_cast<size_t>(elementType)] = newSpellEffectElementModifierPoints;
}

void Character::setSpellEffectAllModifierPoints(unsigned short newSpellEffectAllModifierPoints) {
	spellEffectAllModifierPoints = newSpellEffectAllModifierPoints;
}

void Character::setSpellCriticalModifierPoints(unsigned short newSpellCriticalModifierPoints) {
	spellCriticalModifierPoints = newSpellCriticalModifierPoints;
}

void Character::setLevel(unsigned short newLevel) {
	level = newLevel;
}

void Character::setExperienceValue(unsigned short experienceValue) {
	Character::experienceValue = experienceValue;
}

void Character::setCoins(unsigned int amountOfCoins) {
	coins = amountOfCoins;
}
////////////////////////////////////////////////////LUA-END/////////////////////////////////////

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

std::string Character::ElementToString(Enums::ElementType elementType) {
	switch (elementType) {
		case Enums::ElementType::Fire:
			return "Fire";
			break;
		case Enums::ElementType::Water:
			return "Water";
			break;
		case Enums::ElementType::Air:
			return "Air";
			break;
		case Enums::ElementType::Earth:
			return "Earth";
			break;
		case Enums::ElementType::Light:
			return "Light";
			break;
		case Enums::ElementType::Dark:
			return "Dark";
			break;
		default:
			return "";
	}
}

std::string Character::ElementToLowerString(Enums::ElementType elementType) {
	switch (elementType) {
		case Enums::ElementType::Fire:
			return "fire";
			break;
		case Enums::ElementType::Water:
			return "water";
			break;
		case Enums::ElementType::Air:
			return "air";
			break;
		case Enums::ElementType::Earth:
			return "earth";
			break;
		case Enums::ElementType::Light:
			return "light";
			break;
		case Enums::ElementType::Dark:
			return "dark";
			break;
		default:
			return "";
	}
}

std::string Character::GetCharacterClassName(Enums::CharacterClass characterClass) {

	switch (characterClass) {
		case Enums::CharacterClass::NOCLASS:
			return "NOCLASS";
			break;
		case Enums::CharacterClass::ANYCLASS:
			return "ANYCLASS";
			break;
		case Enums::CharacterClass::Liche:
			return "Liche";
			break;
		case Enums::CharacterClass::Ranger:
			return "Ranger";
			break;
		case Enums::CharacterClass::Warrior:
			return "Warrior";
			break;
	}
	return "";
}

bool Character::HasIntersection(int r1_l, int r1_r, int r1_b, int r1_t, int r2_l, int r2_r, int r2_b, int r2_t) {
	return (!((r1_t < r2_b) || (r1_b > r2_t) || (r1_l > r2_r) || (r1_r < r2_l)));
}

////////////////////////////////////////////////////PROTECTED/////////////////////////////////////
bool Character::canBeDamaged() const {
	return true;
}

void Character::regenerateLifeManaFatigue(float deltaTime) {
	/** Regenerate life, mana and fatigue every 1000 ms. **/
	m_regenerationRate = m_regenerationRate >= 1.0f ? 0.0f : m_regenerationRate + deltaTime;

	if (m_regenerationRate == 0.0f) {
		modifyCurrentMana(getModifiedManaRegen());
		modifyCurrentHealth(getModifiedHealthRegen());
		modifyCurrentFatigue(getModifiedFatigueRegen());
	}
}

bool Character::hasIntersection(int r1_l, int r1_r, int r1_b, int r1_t, int r2_l, int r2_r, int r2_b, int r2_t) {
	return (!((r1_t < r2_b) || (r1_b > r2_t) || (r1_l > r2_r) || (r1_r < r2_l)));
}

int Character::CheckForCollision(int x_pos, int y_pos) {
	std::vector<CollisionRect>& collisionMap = ZoneManager::Get().getCurrentZone()->getCollisionMap();

	int character_l = x_pos, character_r = x_pos + getWidth(), character_b = y_pos, character_t = y_pos + getHeight();
	for (unsigned int t = 0; t < collisionMap.size(); t++) {
		int other_l = collisionMap[t].x, other_r = collisionMap[t].x + collisionMap[t].w;
		int other_b = collisionMap[t].y, other_t = collisionMap[t].y + collisionMap[t].h;
		if (hasIntersection(other_l, other_r, other_b, other_t,
			character_l, character_r, character_b, character_t)) {
			return 1;
		}
	}

	// check for collision with other characters
	std::vector<Npc*>& zoneNPCs = ZoneManager::Get().getCurrentZone()->getNPCs();
	for (size_t curNPCNr = 0; curNPCNr < zoneNPCs.size(); ++curNPCNr) {
		Character *curNPC = zoneNPCs[curNPCNr];
		if (curNPC == this || !curNPC->isAlive())
			continue;

		int other_l = curNPC->getXPos(), other_r = curNPC->getXPos() + curNPC->getWidth();
		int other_b = curNPC->getYPos(), other_t = curNPC->getYPos() + curNPC->getHeight();

		if (hasIntersection(other_l, other_r, other_b, other_t,
			character_l, character_r, character_b, character_t)) {
			return 1;
		}

	}

	// check for collision with player
	Character *curNPC = &Player::Get();
	if (curNPC != this && curNPC->isAlive()) {
		int other_l = curNPC->getXPos(), other_r = curNPC->getXPos() + curNPC->getWidth();
		int other_b = curNPC->getYPos(), other_t = curNPC->getYPos() + curNPC->getHeight();

		if (hasIntersection(other_l, other_r, other_b, other_t,
			character_l, character_r, character_b, character_t)) {
			return 1;
		}
	}
	
	return 0;
}

int Character::CollisionCheck(Enums::Direction direction) {

	switch (direction) {
	case Enums::Direction::N:
		// check upper left corner
		if (CheckForCollision(x_pos, y_pos + 1) == 1) {
			return 1;
		}
		break;

	case Enums::Direction::E:
		// check upper right corner
		if (CheckForCollision(x_pos + 1, y_pos) == 1) {
			return 1;
		}
		break;

	case Enums::Direction::S:
		// check lower left corner
		if (CheckForCollision(x_pos, y_pos - 1) == 1) {
			return 1;
		}
		break;

	case Enums::Direction::W:
		// check upper left corner
		if (CheckForCollision(x_pos - 1, y_pos) == 1) {
			return 1;
		}
		break;

	default:
		break;
	}
	return 0;
}

void Character::MoveUp(unsigned short n) {
	if (CollisionCheck(Enums::Direction::N) == 0) {
	y_pos += n;
	}
}

void Character::MoveDown(unsigned short n) {
	if (CollisionCheck(Enums::Direction::S) == 0) {
	y_pos -= n;
	}
}

void Character::MoveLeft(unsigned short n) {
	if (CollisionCheck(Enums::Direction::W) == 0) {
	x_pos -= n;
	}
}

void Character::MoveRight(unsigned short n) {
	if (CollisionCheck(Enums::Direction::E) == 0) {
	x_pos += n;
	}
}

void Character::Move(Enums::Direction direction, unsigned short n) {
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

void Character::CastingAborted() {
	// if we moved, got stunned, or in some way unable to complete the spell ritual, spellcasting will fail.
	// If we are following the above instructions to use a pointer to a spell and so on, we should clear that pointer here.
	abortCurrentSpellAction();
}

void Character::interruptCurrentActivityWith(Enums::ActivityType activity) {
	curActivity = activity;
	m_waitForAnimation = false;
	currentFrame = 0;
}

Enums::ActivityType Character::getCurActivity() const {
	Enums::ActivityType curActivity = Enums::ActivityType::Walking;
	if (curSpellAction != NULL) {
		if (dynamic_cast<Spell*>(curSpellAction) != NULL) {
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

void Character::cleanupCooldownSpells() {
	size_t curSpell = 0;
	while (curSpell < cooldownSpells.size()) {
		if (cooldownSpells[curSpell]->m_timer.getElapsedTimeMilli() > cooldownSpells[curSpell]->getCooldown() * 1000u) {
			delete cooldownSpells[curSpell];
			cooldownSpells.erase(cooldownSpells.begin() + curSpell);
			if (m_isPlayer == true) {
				// this will seed a new ticket for the itemtooltip and spelltooltips, causing them to reload.
				dynamic_cast<Player*>(this)->m_reloadItemTooltip = true;
				dynamic_cast<Player*>(this)->m_reloadSpellTooltip = true;
			}
		} else {
			curSpell++;
		}
	}
}

void Character::cleanupActiveSpells() {
	size_t curSpell = 0;
	while (curSpell < activeSpells.size()) {
		if (activeSpells[curSpell]->isEffectComplete() == true) {
			delete activeSpells[curSpell];
			activeSpells.erase(activeSpells.begin() + curSpell);
		} else {
			curSpell++;
		}
	}
}


////////////////////////////////////////////////////PRIVATE/////////////////////////////////////
void Character::startSpellAction() {
	isPreparing = false;
	m_preparationTimer.restart();

	/// here we check for equipped items if they have any trigger spells which is used in TriggerType::EXECUTING_ACTION
	if (m_isPlayer == true) {
		std::vector<InventoryItem*> inventory = dynamic_cast<Player*>(this)->getInventory()->getEquippedItems();
		for (size_t curItem = 0; curItem < inventory.size(); curItem++) {
			std::vector<TriggerSpellOnItem*> triggerSpells = inventory[curItem]->getItem()->getTriggerSpells();
			for (size_t curSpell = 0; curSpell < triggerSpells.size(); curSpell++) {
				/// searches for spells on the item with the triggertype EXECUTING_ACTION.
				if (triggerSpells[curSpell]->getTriggerType() == Enums::TriggerType::EXECUTING_ACTION) {
					/// found one, check to see if we manages to trigger the spell.

					if ((RNG::randomSizeT(0, 10000) <= triggerSpells[curSpell]->getChanceToTrigger() * 10000) == true) {
						/// we triggered this spell, now we cast it based on if it's a self-cast spell or a cast on our target spell.
						if (triggerSpells[curSpell]->getCastOnSelf() == true) {
							/// cast spell on self
							executeSpellWithoutCasting(triggerSpells[curSpell]->getSpellToTrigger(), this);
						} else {
							if (this->getTarget() != NULL) {
								/// cast spell on the character's target.
								executeSpellWithoutCasting(triggerSpells[curSpell]->getSpellToTrigger(), this->getTarget());
							}
						}
					}
				}
			}
		}
	}

	// are we casting an AoE spell?
	if (curSpellAction->getRadius() > 0) {
		ZoneManager::Get().getCurrentZone()->MagicMap.push_back(new Magic(curSpellAction));
		ZoneManager::Get().getCurrentZone()->MagicMap.back()->setCreator(this);
		ZoneManager::Get().getCurrentZone()->MagicMap.back()->getSpell()->startEffect();

		isPreparing = false;
		m_preparationTimer.restart();

	}else {
		curSpellAction->stopSoundSpellCasting();
		curSpellAction->startEffect();
	}
}

void Character::giveToPreparation(SpellActionBase *toPrepare) {
	if (curSpellAction != NULL) {
		// don't cast / execute. Enqueue in the list of coming actions / spells ?
		delete toPrepare;
	} else {
		// setup all variables for casting / executing
		isPreparing = true;
		curSpellAction = toPrepare;
		curSpellAction->playSoundSpellCasting();
		toPrepare->beginPreparationOfSpellAction();
		m_preparationTimer.reset();
		continuePreparing();
	}
}

void Character::addDamageDisplayToGUI(int amount, bool critical, uint8_t damageType) {
	if (m_isPlayer) {
		Interface::Get().addCombatText(amount, critical, damageType, 140, ViewPort::Get().getHeight() - 40, true);
	} else {
		Interface::Get().addCombatText(amount, critical, damageType, getXPos() + getWidth() / 2, getYPos() + getHeight() + 52, false);
	}
}

void Character::abortCurrentSpellAction() {
	if (isPreparing) {
		curSpellAction->stopSoundSpellCasting();
		delete curSpellAction;
		curSpellAction = NULL;
		isPreparing = false;
	}
}

bool Character::mayDoAnythingAffectingSpellActionWithoutAborting() const {
	return (curSpellAction == NULL);
}

bool Character::mayDoAnythingAffectingSpellActionWithAborting() const {
	if (m_isPlayer) {
		return (curSpellAction == NULL || isPreparing);
	}
	else {
		return (curSpellAction == NULL);
	}

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

void Character::setCurrentHealth(unsigned short newCurrentHealth) {

	current_health = newCurrentHealth;
}

void Character::setCurrentMana(unsigned short newCurrentMana) {
	current_mana = newCurrentMana;
}

void Character::setCurrentFatigue(unsigned short newCurrentFatigue) {
	current_fatigue = newCurrentFatigue;
}

void Character::modifyCurrentHealth(short currentHealthModifier) {
	setCurrentHealth(getModifiedAttributeValue(getCurrentHealth(), currentHealthModifier, NULLABLE_ATTRIBUTE_MIN, getModifiedMaxHealth()));
}

void Character::modifyCurrentMana(short currentManaModifier) {
	setCurrentMana(getModifiedAttributeValue(getCurrentMana(), currentManaModifier, NULLABLE_ATTRIBUTE_MIN, getModifiedMaxMana()));
}

void Character::modifyCurrentFatigue(short currentFatigueModifier) {
	setCurrentFatigue(getModifiedAttributeValue(getCurrentFatigue(), currentFatigueModifier, NULLABLE_ATTRIBUTE_MIN, getModifiedMaxFatigue()));
}

void Character::modifyMaxHealth(short maxHealthModifier){
	setMaxHealth(getModifiedAttributeValue(max_health, maxHealthModifier, NON_NULLABLE_ATTRIBUTE_MIN));
}

void Character::modifyMaxMana(short maxManaModifier) {
	setMaxMana(getModifiedAttributeValue(max_mana, maxManaModifier, NULLABLE_ATTRIBUTE_MIN));
}

void Character::modifyMaxFatigue(short maxFatigueModifier) {
	setMaxFatigue(getModifiedAttributeValue(max_fatigue, maxFatigueModifier, NULLABLE_ATTRIBUTE_MIN));
}


////////////////////////////////////////////////////////////////////////////////
void CharacterType::addMoveTexture(Enums::ActivityType activity, Enums::Direction direction, int index, std::string filename, unsigned int maxWidth, unsigned int maxHeight, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {
	TileSet& tileSet = m_moveTileSets[{activity, direction}];
	tileSet.addTile(filename, maxWidth, maxHeight, paddingLeft, paddingRight, paddingTop, paddingBottom);
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

void CharacterType::inscribeSpellInSpellbook(SpellActionBase *spell) {
	spellbook.push_back(spell);
}

void CharacterType::addItemToLootTable(Item *item, double dropChance) {
	lootTable.push_back(LootTable(item, dropChance));
}

void CharacterType::setCoinDrop(unsigned int minCoinDrop, unsigned int maxCoinDrop, double dropChance) {
	this->minCoinDrop = minCoinDrop;
	this->maxCoinDrop = maxCoinDrop;
	this->coinDropChance = dropChance;
}

void CharacterType::setClass(Enums::CharacterClass characterClass) {
	CharacterType::characterClass = characterClass;
}

void CharacterType::setName(std::string newName) {
	name = newName;
}

void CharacterType::setStrength(unsigned short newStrength) {
	strength = newStrength;
}

void CharacterType::setDexterity(unsigned short newDexterity) {
	dexterity = newDexterity;
}

void CharacterType::setVitality(unsigned short newVitality) {
	vitality = newVitality;
}

void CharacterType::setIntellect(unsigned short newIntellect) {
	intellect = newIntellect;
}

void CharacterType::setWisdom(unsigned short newWisdom) {
	wisdom = newWisdom;
}

void CharacterType::setMaxHealth(unsigned short newMaxHealth) {
	max_health = newMaxHealth;
}

void CharacterType::setMaxMana(unsigned short newMaxMana) {
	max_mana = newMaxMana;
}

void CharacterType::setMaxFatigue(unsigned short newMaxFatigue) {
	max_fatigue = newMaxFatigue;
}

void CharacterType::setHealthRegen(unsigned short newHealthRegen) {
	healthRegen = newHealthRegen;
}

void CharacterType::setManaRegen(unsigned short newManaRegen) {
	manaRegen = newManaRegen;
}

void CharacterType::setFatigueRegen(unsigned short newFatigueRegen) {
	fatigueRegen = newFatigueRegen;
}

void CharacterType::setMaxDamage(unsigned short newMaxDamage) {
	max_damage = newMaxDamage;
}

void CharacterType::setMinDamage(unsigned short newMinDamage) {
	min_damage = newMinDamage;
}

void CharacterType::setArmor(unsigned short newArmor) {
	armor = newArmor;
}

void CharacterType::setDamageModifierPoints(unsigned short newDamageModifierPoints) {
	damageModifierPoints = newDamageModifierPoints;
}

void CharacterType::setHitModifierPoints(unsigned short newHitModifierPoints) {
	hitModifierPoints = newHitModifierPoints;
}

void CharacterType::setEvadeModifierPoints(unsigned short newEvadeModifierPoints) {
	evadeModifierPoints = newEvadeModifierPoints;
}

void CharacterType::setWanderRadius(unsigned short newWanderRadius) {
	wander_radius = newWanderRadius;
}

void CharacterType::setLevel(unsigned short newLevel) {
	level = newLevel;
}

void CharacterType::setExperienceValue(unsigned short experienceValue) {
	CharacterType::experienceValue = experienceValue;
}

unsigned short CharacterType::getLevel() {
	return level;
}

Enums::CharacterClass CharacterType::getClass() {
	return characterClass;
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