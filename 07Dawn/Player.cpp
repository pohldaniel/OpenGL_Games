#include "Player.h"
#include "Spells.h"
#include "Actions.h"
#include "Constants.h"
#include "Statssystem.h"
#include "Luainterface.h"
#include "Item.h"
#include "Inventory.h"

const unsigned short NULLABLE_ATTRIBUTE_MIN = 0;
const unsigned short NON_NULLABLE_ATTRIBUTE_MIN = 1;

static unsigned short getModifiedAttribute(const Character* character, unsigned short basicAttributeValue, unsigned short(*getSpellAttribute)(GeneralBuffSpell*), unsigned short minValue = std::numeric_limits<unsigned short>::min(), unsigned short maxValue = std::numeric_limits<unsigned short>::max()) {
	int attributeModifier = 0;
	/*std::vector<InventoryItem*> equippedItems = inventory.getEquippedItems();
	size_t numItems = equippedItems.size();
	bool readTwoHandedWeapon = false;
	for (size_t curItemNr = 0; curItemNr<numItems; ++curItemNr) {
	Item* curItem = equippedItems[curItemNr]->getItem();
	assert(curItem != NULL);
	if (curItem->isTwoHandedWeapon() == false || readTwoHandedWeapon == false) {
	attributeModifier += getItemAttribute(curItem);
	}

	// we do this because we only want to read the stats from two-handed weapons once and not two times as it would be since we equip two-handed weapons in both main-hand and off-hand slot.
	if (curItem->isTwoHandedWeapon()){
	readTwoHandedWeapon = true;
	}
	}*/

	std::vector<SpellActionBase*> activeSpells = character->getActiveSpells();
	size_t numSpells = activeSpells.size();
	for (size_t curSpellNr = 0; curSpellNr<numSpells; ++curSpellNr) {
		GeneralBuffSpell *curSpell = dynamic_cast<GeneralBuffSpell*> (activeSpells[curSpellNr]);
		// since more than Buffspells can be active, we want to check to see that we're getting a buff here...
		if (curSpell != NULL) {
			attributeModifier += getSpellAttribute(curSpell);
		}
	}

	if (static_cast<int32_t>(basicAttributeValue) + attributeModifier < static_cast<int32_t>(minValue)) {
		return minValue;
	}
	else if (static_cast<int32_t>(basicAttributeValue) + attributeModifier > static_cast<int32_t>(maxValue)) {
		return maxValue;
	}
	else {
		return basicAttributeValue + attributeModifier;
	}
}

static unsigned short getModifiedAttribute(Enums::ElementType elementType, const Character *character, unsigned short basicAttributeValue, unsigned short(*getSpellAttribute)(Enums::ElementType, GeneralBuffSpell*), unsigned short minValue = std::numeric_limits<unsigned short>::min(), unsigned short maxValue = std::numeric_limits<unsigned short>::max()) {
	int32_t attributeModifier = 0;
	/*std::vector<InventoryItem*> equippedItems = inventory.getEquippedItems();
	size_t numItems = equippedItems.size();
	for (size_t curItemNr = 0; curItemNr<numItems; ++curItemNr)
	{
	Item* curItem = equippedItems[curItemNr]->getItem();
	assert(curItem != NULL);
	attributeModifier += getItemAttribute(elementType, curItem);
	}*/

	std::vector<SpellActionBase*> activeSpells;
	activeSpells = character->getActiveSpells();
	size_t numSpells = activeSpells.size();
	for (size_t curSpellNr = 0; curSpellNr<numSpells; ++curSpellNr) {
		GeneralBuffSpell* curSpell = dynamic_cast<GeneralBuffSpell*> (activeSpells[curSpellNr]);

		// since more than Buffspells can be active, we want to check to see that we're getting a buff here...
		if (curSpell != NULL) {
			attributeModifier += getSpellAttribute(elementType, curSpell);
		}
	}

	if (static_cast<int32_t>(basicAttributeValue) + attributeModifier < static_cast<int32_t>(minValue)) {
		return minValue;

	}
	else if (static_cast<int32_t>(basicAttributeValue) + attributeModifier > static_cast<int32_t>(maxValue)) {
		return maxValue;

	}
	else {
		return basicAttributeValue + attributeModifier;
	}
}

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

Player::Player() : inventory(Inventory()) {
}

Player::~Player() {

}

void Player::draw() {

	int drawX = getXPos();
	int drawY = getYPos();

	if (getUseBoundingBox()) {
		drawX -= getBoundingBoxX();
		drawY -= getBoundingBoxY();
	}

	TextureManager::BindTexture(TextureManager::GetTextureAtlas("player"), true);
	TextureManager::DrawTexture(*rect, drawX, drawY, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), true, true);
	TextureManager::UnbindTexture(true);
}

void Player::update(float deltaTime) {
	//std::cout << "Activity: " << ActivityToString(curActivity) << std::endl;

	cleanupActiveSpells();
	cleanupCooldownSpells();

	curActivity = getCurActivity();

	regenerateLifeManaFatigue(deltaTime);

	//if (curActivity != Enums::ActivityType::Dying) {
	processInput();
	//}

	if (activeDirection != Enums::Direction::STOP && curActivity != Enums::ActivityType::Walking) {
		interruptCurrentActivityWith(Enums::ActivityType::Walking);
		CastingAborted();
	}

	Move(deltaTime);

	lastActiveDirection = activeDirection != Enums::Direction::STOP ? activeDirection : lastActiveDirection;
	Animate(deltaTime);
	continuePreparing();
}

void Player::init(int x, int y) {
	x_pos = x;
	y_pos = y;


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
	//coins = 0;
	//dyingTransparency = 1.0f;
	movementSpeed = 1;
	setName("Enylyn");

	m_isPlayer = true;

	inventory.init(10, 4, this);
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

		}
		else {
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

unsigned int Player::getTicketForItemTooltip() const {
	return ticketForItemTooltip;
}

unsigned int Player::getTicketForSpellTooltip() const {
	return ticketForSpellTooltip;
}

void Player::setTicketForItemTooltip() {
	ticketForItemTooltip = Globals::clock.getElapsedTimeMilli();
}

void Player::setTicketForSpellTooltip() {
	ticketForSpellTooltip = Globals::clock.getElapsedTimeMilli();
}

unsigned int Player::getTicksOnCooldownSpell(std::string spellName) const {
	for (size_t curSpell = 0; curSpell < cooldownSpells.size(); curSpell++) {
		if (cooldownSpells[curSpell]->getName() == spellName) {
			return cooldownSpells[curSpell]->m_timer.getElapsedTimeMilli();
		}
	}
	return 0u;
}

unsigned short Player::getModifiedMinDamage() const {
	unsigned short inventoryMinDamage = getModifiedAttribute(this, 0, &getSpellMinDamageHelper, NON_NULLABLE_ATTRIBUTE_MIN);
	return inventoryMinDamage;
}

unsigned short Player::getModifiedMaxDamage() const {
	unsigned short inventoryMaxDamage = getModifiedAttribute(this, 0, &getSpellMaxDamageHelper, getModifiedMinDamage());
	return inventoryMaxDamage;
}

unsigned short Player::getModifiedDamageModifierPoints() const {
	return getModifiedAttribute(this, getDamageModifierPoints(), &getSpellDamageModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateDamageModifierPoints(this);
}

unsigned short Player::getModifiedSpellEffectElementModifierPoints(Enums::ElementType elementType) const {
	return getModifiedAttribute(elementType, this, getSpellEffectElementModifierPoints(elementType) + getSpellEffectAllModifierPoints(), &getSpellSpellEffectElementModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateSpellEffectElementModifierPoints(elementType, this);
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
		setMaxHealth(getMaxHealth() * 1.1);
		setStrength(getStrength() * 1.1);
		setLevel(getLevel() + 1);
		GLfloat yellow[] = { 1.0f, 1.0f, 0.0f };
		if (m_isPlayer == true){
			dynamic_cast<Player*>(this)->setTicketForItemTooltip();
			dynamic_cast<Player*>(this)->setTicketForSpellTooltip();
		}

		DawnInterface::addTextToLogWindow(yellow, "Welcome to the world of Dawn, %s.", getLevel(), getClassName().c_str());
	}
}

void Player::setExperience(unsigned long experience) {
	this->experience = experience;
}

////////////////////////PRIVATE//////////////////////////////////
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
	return getModifiedAttribute(this, getArmor(), &getSpellArmorHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateDamageReductionPoints(this);
}

unsigned short Player::getModifiedHitModifierPoints() const {
	return getModifiedAttribute(this, getHitModifierPoints(), &getSpellHitModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateHitModifierPoints(this);
}

unsigned short Player::getModifiedEvadeModifierPoints() const {
	return getModifiedAttribute(this, getEvadeModifierPoints(), &getSpellEvadeModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateEvadeModifierPoints(this);
}

unsigned short Player::getModifiedParryModifierPoints() const {
	return getModifiedAttribute(this, getParryModifierPoints(), &getSpellParryModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateParryModifierPoints(this);
}

unsigned short Player::getModifiedBlockModifierPoints() const {
	return getModifiedAttribute(this, getBlockModifierPoints(), &getSpellBlockModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateBlockModifierPoints(this);
}

unsigned short Player::getModifiedMeleeCriticalModifierPoints() const {
	return getModifiedAttribute(this, getMeleeCriticalModifierPoints(), &getSpellMeleeCriticalModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateMeleeCriticalModifierPoints(this);
}

unsigned short Player::getModifiedResistElementModifierPoints(Enums::ElementType elementType) const {
	return getModifiedAttribute(elementType, this, getResistElementModifierPoints(elementType) + getResistAllModifierPoints(), &getSpellResistElementModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateResistElementModifierPoints(elementType, this);
}

unsigned short Player::getModifiedSpellCriticalModifierPoints() const {
	return getModifiedAttribute(this, getSpellCriticalModifierPoints(), &getSpellSpellCriticalModifierPointsHelper, NULLABLE_ATTRIBUTE_MIN) + StatsSystem::getStatsSystem()->calculateSpellCriticalModifierPoints(this);
}

unsigned short Player::getModifiedStrength() const {
	return getModifiedAttribute(this, getStrength(), &getSpellStrengthHelper, NON_NULLABLE_ATTRIBUTE_MIN);
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
	return &inventory;
}

void Player::clearInventory() {
	inventory.clear();
}

void Player::reduceCoins(unsigned int amountOfCoins) {
	if (coins >= amountOfCoins) {
		coins -= amountOfCoins;
	}else {
		coins = 0;
	}
}