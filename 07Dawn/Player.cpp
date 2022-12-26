#include "Player.h"
#include "Spells.h"
#include "Actions.h"
#include "Statssystem.h"
#include "Luainterface.h"
#include "Item.h"
#include "Inventory.h"

const unsigned short NULLABLE_ATTRIBUTE_MIN = 0;
const unsigned short NON_NULLABLE_ATTRIBUTE_MIN = 1;

static unsigned short getModifiedAttribute(const Inventory& inventory, const Character* character, unsigned short basicAttributeValue, unsigned short(*getItemAttribute)(Item*), unsigned short(*getSpellAttribute)(GeneralBuffSpell*), unsigned short minValue = std::numeric_limits<unsigned short>::min(), unsigned short maxValue = std::numeric_limits<unsigned short>::max()) {
	int attributeModifier = 0;

	std::vector<InventoryItem*> equippedItems = inventory.getEquippedItems();
	size_t numItems = equippedItems.size();
	bool readTwoHandedWeapon = false;
	for (size_t curItemNr = 0; curItemNr<numItems; ++curItemNr) {
		Item* curItem = equippedItems[curItemNr]->getItem();

		if (curItem->isTwoHandedWeapon() == false || readTwoHandedWeapon == false) {
			attributeModifier += getItemAttribute(curItem);
		}

		// we do this because we only want to read the stats from two-handed weapons once and not two times as it would be since we equip two-handed weapons in both main-hand and off-hand slot.
		if (curItem->isTwoHandedWeapon()) {
			readTwoHandedWeapon = true;
		}
	}

	const std::vector<SpellActionBase*>& activeSpells = character->getActiveSpells();
	size_t numSpells = activeSpells.size();
	for (size_t curSpellNr = 0; curSpellNr<numSpells; ++curSpellNr) {
		GeneralBuffSpell *curSpell = dynamic_cast<GeneralBuffSpell*> (activeSpells[curSpellNr]);
		// since more than Buffspells can be active, we want to check to see that we're getting a buff here...
		if (curSpell != NULL) {
			attributeModifier += getSpellAttribute(curSpell);
		}
	}

	if (static_cast<int>(basicAttributeValue) + attributeModifier < static_cast<int>(minValue)) {
		return minValue;
	} else if (static_cast<int>(basicAttributeValue) + attributeModifier > static_cast<int>(maxValue)) {
		return maxValue;
	} else {
		return basicAttributeValue + attributeModifier;
	}
}

static unsigned short getModifiedAttribute(Enums::ElementType elementType, const Inventory &inventory, const Character *character, unsigned short basicAttributeValue, unsigned short(*getItemAttribute)(Enums::ElementType, Item*), unsigned short(*getSpellAttribute)(Enums::ElementType, GeneralBuffSpell*), unsigned short minValue = std::numeric_limits<unsigned short>::min(), unsigned short maxValue = std::numeric_limits<unsigned short>::max()) {
	int attributeModifier = 0;

	std::vector<InventoryItem*> equippedItems = inventory.getEquippedItems();
	size_t numItems = equippedItems.size();

	for (size_t curItemNr = 0; curItemNr<numItems; ++curItemNr) {
		Item* curItem = equippedItems[curItemNr]->getItem();
		attributeModifier += getItemAttribute(elementType, curItem);
	}

	const std::vector<SpellActionBase*>& activeSpells = character->getActiveSpells();

	size_t numSpells = activeSpells.size();
	for (size_t curSpellNr = 0; curSpellNr<numSpells; ++curSpellNr) {
		GeneralBuffSpell* curSpell = dynamic_cast<GeneralBuffSpell*> (activeSpells[curSpellNr]);

		// since more than Buffspells can be active, we want to check to see that we're getting a buff here...
		if (curSpell != NULL) {
			attributeModifier += getSpellAttribute(elementType, curSpell);
		}
	}

	if (static_cast<int>(basicAttributeValue) + attributeModifier < static_cast<int>(minValue)) {
		return minValue;

	} else if (static_cast<int>(basicAttributeValue) + attributeModifier > static_cast<int>(maxValue)) {
		return maxValue;

	} else {
		return basicAttributeValue + attributeModifier;
	}
}

/// helperfunctions for items.
static unsigned short getItemStrengthHelper(Item* item) { return item->getStats(Enums::StatsType::Strength); }
static unsigned short getItemDexterityHelper(Item* item) { return item->getStats(Enums::StatsType::Dexterity); }
static unsigned short getItemVitalityHelper(Item* item) { return item->getStats(Enums::StatsType::Vitality); }
static unsigned short getItemIntellectHelper(Item* item) { return item->getStats(Enums::StatsType::Intellect); }
static unsigned short getItemWisdomHelper(Item* item) { return item->getStats(Enums::StatsType::Wisdom); }
static unsigned short getItemHealthHelper(Item* item) { return item->getStats(Enums::StatsType::Health); }
static unsigned short getItemManaHelper(Item* item) { return item->getStats(Enums::StatsType::Mana); }
static unsigned short getItemFatigueHelper(Item* item) { return item->getStats(Enums::StatsType::Fatigue); }
static unsigned short getItemArmorHelper(Item* item) { return item->getStats(Enums::StatsType::Armor); }
static unsigned short getItemDamageModifierPointsHelper(Item* item) { return item->getStats(Enums::StatsType::DamageModifier); }
static unsigned short getItemHitModifierPointsHelper(Item* item) { return item->getStats(Enums::StatsType::HitModifier); }
static unsigned short getItemEvadeModifierPointsHelper(Item* item) { return item->getStats(Enums::StatsType::EvadeModifier); }
static unsigned short getItemParryModifierPointsHelper(Item* item) { return item->getStats(Enums::StatsType::ParryModifier); }
static unsigned short getItemBlockModifierPointsHelper(Item* item) { return item->getStats(Enums::StatsType::BlockModifier); }
static unsigned short getItemMeleeCriticalModifierPointsHelper(Item* item) { return item->getStats(Enums::StatsType::MeleeCritical); }
static unsigned short getItemResistElementModifierPointsHelper(Enums::ElementType elementType, Item* item) { return item->getResistElementModifierPoints(elementType) + item->getStats(Enums::StatsType::ResistAll); }
static unsigned short getItemSpellEffectElementModifierPointsHelper(Enums::ElementType elementType, Item* item) { return item->getSpellEffectElementModifierPoints(elementType) + item->getStats(Enums::StatsType::SpellEffectAll); }
static unsigned short getItemSpellCriticalModifierPointsHelper(Item* item) { return item->getStats(Enums::StatsType::SpellCritical); }
static unsigned short getItemHealthRegenHelper(Item* item) { return item->getStats(Enums::StatsType::HealthRegen); }
static unsigned short getItemManaRegenHelper(Item * item) { return item->getStats(Enums::StatsType::ManaRegen); }
static unsigned short getItemFatigueRegenHelper(Item * item) { return item->getStats(Enums::StatsType::FatigueRegen); }
static unsigned short getItemMinDamageHelper(Item* item) { return item->getMinDamage(); }
static unsigned short getItemMaxDamageHelper(Item* item) { return item->getMaxDamage(); }

/// helperfunctions for spells (buffs / debuffs)
static unsigned short getSpellStrengthHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::Strength); }
static unsigned short getSpellDexterityHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::Dexterity); }
static unsigned short getSpellVitalityHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::Vitality); }
static unsigned short getSpellIntellectHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::Intellect); }
static unsigned short getSpellWisdomHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::Wisdom); }
static unsigned short getSpellHealthHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::Health); }
static unsigned short getSpellManaHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::Mana); }
static unsigned short getSpellFatigueHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::Fatigue); }
static unsigned short getSpellArmorHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::Armor); }
static unsigned short getSpellDamageModifierPointsHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::DamageModifier); }
static unsigned short getSpellHitModifierPointsHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::HitModifier); }
static unsigned short getSpellEvadeModifierPointsHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::EvadeModifier); }
static unsigned short getSpellParryModifierPointsHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::ParryModifier); }
static unsigned short getSpellBlockModifierPointsHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::BlockModifier); }
static unsigned short getSpellMeleeCriticalModifierPointsHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::MeleeCritical); }
static unsigned short getSpellResistElementModifierPointsHelper(Enums::ElementType elementType, GeneralBuffSpell* spell) { return spell->getResistElementModifierPoints(elementType) + spell->getStats(Enums::StatsType::ResistAll); }
static unsigned short getSpellSpellEffectElementModifierPointsHelper(Enums::ElementType elementType, GeneralBuffSpell* spell) { return spell->getSpellEffectElementModifierPoints(elementType) + spell->getStats(Enums::StatsType::SpellEffectAll); }
static unsigned short getSpellSpellCriticalModifierPointsHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::SpellCritical); }
static unsigned short getSpellHealthRegenHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::HealthRegen); }
static unsigned short getSpellManaRegenHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::ManaRegen); }
static unsigned short getSpellFatigueRegenHelper(GeneralBuffSpell* spell) { return spell->getStats(Enums::StatsType::FatigueRegen); }

static unsigned short getSpellMinDamageHelper(GeneralBuffSpell* spell) { return 0; } // not used yet
static unsigned short getSpellMaxDamageHelper(GeneralBuffSpell* spell) { return 0; } // not used yet

Player Player::s_instance;

Player& Player::Get() {
	return s_instance;
}

Player::Player() { }

Player::~Player() {}

void Player::draw() {

	int drawX = getXPos();
	int drawY = getYPos();

	if (getUseBoundingBox()) {
		drawX -= getBoundingBoxX();
		drawY -= getBoundingBoxY();
	}

	Vector4f color = { 1.0f, 1.0f, 1.0f, 1.0f };

	// if player is invisible we draw at 0.2 transparency.. if sneaking we draw at 0.5 and with darker colors (shade)
	if (isSneaking() == true){
		color[0] = 0.7f;
		color[1] = 0.7f;
		color[2] = 0.7f;
		color[3] = 0.5f;
	}else if (isInvisible() == true) {
		color[3] = 0.2f;
	}

	TextureManager::DrawTextureBatched(*rect, drawX, drawY, color, true, true, 4u);
}

void Player::update(float deltaTime) {
	// making sure our target is still alive, not invisible and still in range while stealthed. if not well set our target to NULL.
	if (getTarget()) {
		double distance = sqrt(pow((getTarget()->getXPos() + getTarget()->getWidth() / 2) - (getXPos() + getWidth() / 2), 2)
			+ pow((getTarget()->getYPos() + getTarget()->getHeight() / 2) - (getYPos() + getHeight() / 2), 2));

		if(getTarget()->isAlive() == false ||
			(getTarget()->isInvisible() == true && canSeeInvisible() == false) ||
			(getTarget()->isSneaking() == true && distance > 260 && canSeeSneaking() == false)) {
			setTarget(NULL);
		}
	}
	
	CalculateStats();
	cleanupActiveSpells();
	cleanupCooldownSpells();

	curActivity = getCurActivity();

	regenerateLifeManaFatigue(deltaTime);

	if (curActivity != Enums::ActivityType::Dying) {
		processInput();
	}

	if (activeDirection != Enums::Direction::STOP && curActivity != Enums::ActivityType::Walking) {
		interruptCurrentActivityWith(Enums::ActivityType::Walking);
		CastingAborted();
	}

	Move(deltaTime);

	lastActiveDirection = activeDirection != Enums::Direction::STOP ? activeDirection : lastActiveDirection;
	Animate(deltaTime);
	continuePreparing();

	// check all active spells for inEffects on our player.
	for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpells.size(); ++curActiveSpellNr) {
		activeSpells[curActiveSpellNr]->inEffect(deltaTime);
	}
}

void Player::setCharacterType(std::string characterTypeStr) {
	m_characterTypeStr = characterTypeStr;
}

void Player::Die() {
	alive = false;
	curActivity = Enums::ActivityType::Dying;
	currentFrame = 0;
	m_waitForAnimation = true;
}

void Player::init() {
	m_characterType = &CharacterTypeManager::Get().getCharacterType(m_characterTypeStr);
	rect = &m_characterType->m_moveTileSets.at({ Enums::ActivityType::Walking, Enums::Direction::S }).getAllTiles()[0].textureRect;

	activeDirection = Enums::Direction::S;
	lastActiveDirection = activeDirection;
	curActivity = Enums::ActivityType::Walking;

	isPreparing = false;
	alive = true;
	curSpellAction = NULL;
	
	m_isPlayer = true;
	updatePortraitOffset();

	inventory = new Inventory();
	inventory->init(10, 4, this);

	Globals::soundManager.get("player").loadChannel("res/sound/walking.ogg", 10);
}

bool Player::hasTarget(Character* target) {
	if (Character::target == target) {
		return true;
	}
	return false;
}

void Player::setTarget(Character *target, Enums::Attitude attitude) {
	this->target = target;
	targetAttitude = attitude;
}

Vector3f Player::getPosition() {
	return Vector3f(static_cast<float>(getXPos()), static_cast<float>(getYPos()), 0.0f);
}

int Player::getDeltaX() {
	return dx;
}

int Player::getDeltaY() {
	return dy;
}

void Player::removeActiveSpell(SpellActionBase* activeSpell) {
	for (size_t curSpell = 0; curSpell < activeSpells.size(); curSpell++) {
		if (activeSpells[curSpell] == activeSpell) {
			activeSpells[curSpell]->markSpellActionAsFinished();
		}
	}
}

std::vector<SpellActionBase*> Player::getCooldownSpells() const {
	return cooldownSpells;
}

Enums::Attitude Player::getTargetAttitude() {
	return targetAttitude;
}

void Player::gainExperience(unsigned long addExp) {
	if (m_isPlayer) {

		if (std::numeric_limits<unsigned long>::max() - addExp < experience) {
			experience = std::numeric_limits<unsigned long>::max();

		} else {
			experience += addExp;
			GLfloat yellow[] = { 1.0f, 1.0f, 0.0f };
			DawnInterface::addTextToLogWindow(yellow, "You gain %d experience.", addExp);
		}

		while (canRaiseLevel()) {
			raiseLevel();
		}
	}
}

unsigned long Player::getExperience() const {
	return experience;
}

unsigned long Player::getExpNeededForLevel(unsigned short level) const {
	unsigned long result = (level*(level - 1) * 50);
	return result;
}

unsigned int Player::getTicksOnCooldownSpell(std::string spellName) const {
	for (size_t curSpell = 0; curSpell < cooldownSpells.size(); curSpell++) {
		if (cooldownSpells[curSpell]->getName() == spellName) {
			return cooldownSpells[curSpell]->m_timer.getElapsedTimeMilli();
		}
	}
	return 0u;
}

bool Player::isSpellOnCooldown(std::string spellName) const {
	for (size_t curSpell = 0; curSpell < cooldownSpells.size(); curSpell++) {
		if (cooldownSpells[curSpell]->getName() == spellName) {
			return true;
		}
	}
	return false;
}

bool Player::canRaiseLevel() const {
	return (experience >= getExpNeededForLevel(getLevel() + 1) && (getExpNeededForLevel(getLevel() + 1) != getExpNeededForLevel(getLevel())));
}

void Player::raiseLevel() {
	if (canRaiseLevel()) {
		setMaxHealth(getMaxHealth() * 1.1f);
		setStrength(getStrength() * 1.1f);
		setLevel(getLevel() + 1);
		GLfloat yellow[] = { 1.0f, 1.0f, 0.0f };
		if (m_isPlayer == true){
			dynamic_cast<Player*>(this)->m_reloadItemTooltip = true;
			dynamic_cast<Player*>(this)->m_reloadSpellTooltip = true;
		}

		DawnInterface::addTextToLogWindow(yellow, "You are now a level %d %s.", getLevel(), getClassName().c_str());	
	}
}

void Player::setExperience(unsigned long experience) {
	this->experience = experience;
}

////////////////////////PRIVATE//////////////////////////////////
void Player::Move(float deltaTime) {
	int oldX = getXPos();
	int oldY = getYPos();

	// moves one step per movePerStep ms
	float movePerStep = 0.01f * getMovementSpeed() * m_currentspeed;

	// To balance moving diagonally boost, movePerStep = 10*sqrt(2)
	if (activeDirection == Enums::Direction::NW || activeDirection == Enums::Direction::NE || activeDirection == Enums::Direction::SW || activeDirection == Enums::Direction::SE)
		movePerStep = 0.014f * getMovementSpeed() * m_currentspeed;

	// synch with animation
	m_duration = movePerStep * 10.0f;
	m_elapsedTime += deltaTime;
	
	if (isFeared() == true) {
		if (hasChoosenFearDirection == false) {
			fearDirection = static_cast<Enums::Direction>(RNG::randomSizeT(1, 8));
			hasChoosenFearDirection = true;
		}
		activeDirection = fearDirection;
	}

	while (m_elapsedTime > movePerStep) {
		m_elapsedTime -= movePerStep;
		Character::Move(activeDirection);
	}

	dx = getXPos() - oldX;
	dy = getYPos() - oldY;

	// if the character has moved, enable walking sound
	if (oldX != getXPos() || oldY != getYPos()) {
		Globals::soundManager.get("player").playChannel(10);
	} else {
		Globals::soundManager.get("player").pauseChannel(10);
	}
}

void Player::Animate(float deltaTime) {
	const TileSet& tileSet = m_characterType->m_moveTileSets.at({ curActivity, lastActiveDirection });
	
	if (activeDirection != Enums::Direction::STOP || (curActivity == Enums::ActivityType::Dying && m_waitForAnimation)) {
		unsigned short numActivityTextures = m_characterType->m_numMoveTexturesPerDirection.at(curActivity);

		m_animationTime += deltaTime;
		while (m_animationTime >= m_duration) {
			m_animationTime -= m_duration;
			if (++currentFrame > numActivityTextures - 1) {
				currentFrame = curActivity == Enums::ActivityType::Dying ? numActivityTextures - 1 : 0;
				m_waitForAnimation = false;

			}
		}
		rect = &tileSet.getAllTiles()[currentFrame].textureRect;

	}else if (curActivity == Enums::ActivityType::Casting) {
		unsigned short numActivityTextures = m_characterType->m_numMoveTexturesPerDirection.at(curActivity);
		currentFrame = static_cast<unsigned short>(floor(getPreparationPercentage() * numActivityTextures));		
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

	activeDirection = Enums::Direction::STOP;
}

void Player::clearCooldownSpells() {
	cooldownSpells.clear();
}

void Player::clearActiveSpells() {
	activeSpells.clear();
}

unsigned short Player::getModifiedArmor() const {
	return getModifiedAttribute(*inventory, this, getArmor(), &getItemArmorHelper, &getSpellArmorHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateDamageReductionPoints(this);
}

unsigned short Player::getModifiedDamageModifierPoints() const {
	return getModifiedAttribute(*inventory, this, getDamageModifierPoints(), &getItemDamageModifierPointsHelper, &getSpellDamageModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateDamageModifierPoints(this);
}

unsigned short Player::getModifiedHitModifierPoints() const {
	return getModifiedAttribute(*inventory, this, getHitModifierPoints(), &getItemHitModifierPointsHelper, &getSpellHitModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateHitModifierPoints(this);
}

unsigned short Player::getModifiedEvadeModifierPoints() const {
	return getModifiedAttribute(*inventory, this, getEvadeModifierPoints(), &getItemEvadeModifierPointsHelper, &getSpellEvadeModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateEvadeModifierPoints(this);
}

unsigned short Player::getModifiedParryModifierPoints() const {
	return getModifiedAttribute(*inventory, this, getParryModifierPoints(), &getItemParryModifierPointsHelper, &getSpellParryModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateParryModifierPoints(this);
}

unsigned short Player::getModifiedBlockModifierPoints() const {
	return getModifiedAttribute(*inventory, this, getBlockModifierPoints(), &getItemBlockModifierPointsHelper, &getSpellBlockModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateBlockModifierPoints(this);
}

unsigned short Player::getModifiedMeleeCriticalModifierPoints() const {
	return getModifiedAttribute(*inventory, this, getMeleeCriticalModifierPoints(), &getItemMeleeCriticalModifierPointsHelper, &getSpellMeleeCriticalModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateMeleeCriticalModifierPoints(this);
}

unsigned short Player::getModifiedResistElementModifierPoints(Enums::ElementType elementType) const {
	return getModifiedAttribute(elementType, *inventory, this, getResistElementModifierPoints(elementType) + getResistAllModifierPoints(), &getItemResistElementModifierPointsHelper, &getSpellResistElementModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateResistElementModifierPoints(elementType, this);
}

unsigned short Player::getModifiedSpellEffectElementModifierPoints(Enums::ElementType elementType) const {
	return getModifiedAttribute(elementType, *inventory, this, getSpellEffectElementModifierPoints(elementType) + getSpellEffectAllModifierPoints(), &getItemSpellEffectElementModifierPointsHelper, &getSpellSpellEffectElementModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateSpellEffectElementModifierPoints(elementType, this);
}

unsigned short Player::getModifiedSpellCriticalModifierPoints() const {
	return getModifiedAttribute(*inventory, this, getSpellCriticalModifierPoints(), &getItemSpellCriticalModifierPointsHelper, &getSpellSpellCriticalModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateSpellCriticalModifierPoints(this);
}

unsigned short Player::getModifiedStrength() const {
	return getModifiedAttribute(*inventory, this, getStrength(), &getItemStrengthHelper, &getSpellStrengthHelper, NON_NULLABLE_ATTRIBUTE_MIN);
}

unsigned short Player::getModifiedDexterity() const {
	return getModifiedAttribute(*inventory, this, getDexterity(), &getItemDexterityHelper, &getSpellDexterityHelper, NON_NULLABLE_ATTRIBUTE_MIN);
}

unsigned short Player::getModifiedVitality() const {
	return getModifiedAttribute(*inventory, this, getVitality(), &getItemVitalityHelper, &getSpellVitalityHelper, NON_NULLABLE_ATTRIBUTE_MIN);
}

unsigned short Player::getModifiedIntellect() const {
	return getModifiedAttribute(*inventory, this, getIntellect(), &getItemIntellectHelper, &getSpellIntellectHelper, NON_NULLABLE_ATTRIBUTE_MIN);
}

unsigned short Player::getModifiedWisdom() const {
	return getModifiedAttribute(*inventory, this, getWisdom(), &getItemWisdomHelper, &getSpellWisdomHelper, NON_NULLABLE_ATTRIBUTE_MIN);
}

unsigned short Player::getModifiedMaxHealth() const {
	return getModifiedAttribute(*inventory, this, getMaxHealth(), &getItemHealthHelper, &getSpellHealthHelper, NON_NULLABLE_ATTRIBUTE_MIN);
}

unsigned short Player::getModifiedMaxMana() const {
	return getModifiedAttribute(*inventory, this, getMaxMana(), &getItemManaHelper, &getSpellManaHelper, NULLABLE_ATTRIBUTE_MIN);
}

unsigned short Player::getModifiedMaxFatigue() const {
	return getModifiedAttribute(*inventory, this, getMaxFatigue(), &getItemFatigueHelper, &getSpellFatigueHelper, NULLABLE_ATTRIBUTE_MIN);
}

unsigned short Player::getModifiedMinDamage() const {
	unsigned short  inventoryMinDamage = getModifiedAttribute(*inventory, this, 0, &getItemMinDamageHelper, &getSpellMinDamageHelper, NON_NULLABLE_ATTRIBUTE_MIN);
	return inventoryMinDamage;
}

unsigned short  Player::getModifiedMaxDamage() const {
	unsigned short  inventoryMaxDamage = getModifiedAttribute(*inventory, this, 0, &getItemMaxDamageHelper, &getSpellMaxDamageHelper, getModifiedMinDamage());
	return inventoryMaxDamage;
}

unsigned short Player::getModifiedHealthRegen() const {
	return getModifiedAttribute(*inventory, this, getHealthRegen(), &getItemHealthRegenHelper, &getSpellHealthRegenHelper, NULLABLE_ATTRIBUTE_MIN);
}

unsigned short Player::getModifiedManaRegen() const {
	return getModifiedAttribute(*inventory, this, getManaRegen(), &getItemManaRegenHelper, &getSpellManaRegenHelper, NULLABLE_ATTRIBUTE_MIN);
}

unsigned short Player::getModifiedFatigueRegen() const {
	return getModifiedAttribute(*inventory, this, getFatigueRegen(), &getItemFatigueRegenHelper, &getSpellFatigueRegenHelper, NULLABLE_ATTRIBUTE_MIN);
}

void Player::updatePortraitOffset() {
	m_portraitOffset = getClass() == Enums::CharacterClass::Warrior ? 14 : getClass() == Enums::CharacterClass::Liche ? 15 : 16;
}

unsigned short Player::getPortraitOffset() const {
	return m_portraitOffset;
}

bool Player::canWearArmorType(Item* item) const {
	if (ArmorType::getHighestArmorTypeByClass(getClass()) < item->getArmorType()) {
		return false;
	}
	return true;
}

bool Player::isSpellInscribedInSpellbook(SpellActionBase* spell) const {
	for (size_t curSpell = 0; curSpell < getSpellbook().size(); curSpell++) {
		if (getSpellbook()[curSpell]->getName() == spell->getName()) {
			if (getSpellbook()[curSpell]->getRank() >= spell->getRank()) {
				return true; // yes, spell is already inscribed in spellbook with same or higher rank.
			}
		}
	}
	return false;
}

void Player::startShopping() {
	isCurrentlyShopping = true;
}

void Player::stopShopping() {
	isCurrentlyShopping = false;
}

bool Player::isShopping() const {
	return isCurrentlyShopping;
}

Inventory* Player::getInventory() {
	return inventory;
}

void Player::clearInventory() {
	inventory->clear();
}

void Player::reduceCoins(unsigned int amountOfCoins) {
	if (coins >= amountOfCoins) {
		coins -= amountOfCoins;
	}else {
		coins = 0;
	}
}

std::string Player::getSaveText() const {
	std::ostringstream oss;
	std::string objectName = "thePlayer";
	oss << "-- General status attributes" << std::endl;
	oss << "local " << objectName << " = DawnInterface.getPlayer();" << std::endl;
	oss << objectName << ":setStrength( " << getStrength() << " );" << std::endl;
	oss << objectName << ":setDexterity( " << getDexterity() << " );" << std::endl;
	oss << objectName << ":setVitality( " << getVitality() << " );" << std::endl;
	oss << objectName << ":setIntellect( " << getIntellect() << " );" << std::endl;
	oss << objectName << ":setWisdom( " << getWisdom() << " );" << std::endl;
	oss << objectName << ":setMaxHealth( " << getMaxHealth() << " );" << std::endl;
	oss << objectName << ":setMaxMana( " << getMaxMana() << " );" << std::endl;
	oss << objectName << ":setMaxFatigue( " << getMaxFatigue() << " );" << std::endl;
	oss << objectName << ":setMinDamage( " << getMinDamage() << " );" << std::endl;
	oss << objectName << ":setMaxDamage( " << getMaxDamage() << " );" << std::endl;

	// don't set activity textures here. That is determined by class of Player / NPC

	oss << objectName << ":setDamageModifierPoints( " << getDamageModifierPoints() << " );" << std::endl;
	oss << objectName << ":setHitModifierPoints( " << getHitModifierPoints() << " );" << std::endl;
	oss << objectName << ":setEvadeModifierPoints( " << getEvadeModifierPoints() << " );" << std::endl;
	oss << objectName << ":setParryModifierPoints( " << getParryModifierPoints() << " );" << std::endl;
	oss << objectName << ":setBlockModifierPoints( " << getBlockModifierPoints() << " );" << std::endl;
	oss << objectName << ":setMeleeCriticalModifierPoints( " << getMeleeCriticalModifierPoints() << " );" << std::endl;
	oss << objectName << ":setResistAllModifierPoints( " << getResistAllModifierPoints() << " );" << std::endl;
	oss << objectName << ":setSpellEffectAllModifierPoints( " << getSpellEffectAllModifierPoints() << " );" << std::endl;

	for (size_t curElement = 0; curElement<static_cast<size_t>(Enums::ElementType::CountET); ++curElement) {
		Enums::ElementType curElementType = static_cast<Enums::ElementType>(curElement);
		oss << objectName << ":setResistElementModifierPoints( "
			<< curElementType << ", "
			<< getResistElementModifierPoints(curElementType) << " );" << std::endl;
		oss << objectName << ":setSpellEffectElementModifierPoints( "
			<< curElementType << ", "
			<< getSpellEffectElementModifierPoints(curElementType) << " );" << std::endl;
	}
	oss << objectName << ":setSpellCriticalModifierPoints( " << getSpellCriticalModifierPoints() << " );" << std::endl;
	oss << objectName << ":setName( \"" << getName() << "\" );" << std::endl;
	// string stream doesn't seem to have a proper overload for uint8_t and makes it the 0-character, so cast to size_t
	oss << objectName << ":setLevel( " << static_cast<size_t>(getLevel()) << " );" << std::endl;
	oss << objectName << ":setExperience( " << getExperience() << " );" << std::endl;
	oss << objectName << ":setClass( Enums." << getClassName() << " );" << std::endl;
	oss << objectName << ":setCharacterType(\"" << m_characterTypeStr << "\" );" << std::endl;
	oss << objectName << ":setCoins( " << getCoins() << " );" << std::endl;
	oss << objectName << ":setPosition( " << getXPos() << ", " << getYPos() << " );" << std::endl;
	oss << objectName << ":setCurrentHealth(" << getCurrentHealth() << ");" << std::endl;
	oss << objectName << ":setCurrentMana(" << getCurrentMana() << ");" << std::endl;
	oss << objectName << ":setCurrentFatigue(" << getCurrentFatigue() << ");" << std::endl;
	oss << objectName << ":init();" << std::endl << std::endl;
	// no current attributes are set here because after reloading the player is completely refreshed again

	return oss.str();
}

void Player::DrawActiveSpells() {
	const std::vector<SpellActionBase*>& activeSpells = Player::Get().getActiveSpells();
	for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpells.size(); ++curActiveSpellNr) {
		if (!activeSpells[curActiveSpellNr]->isEffectComplete()) {
			activeSpells[curActiveSpellNr]->draw();
		}
	}
}