#include "Npc.h"
#include "TilesetManager.h"
#include "Zone.h"
#include "Groundloot.h"
#include "Quest.h"
#include "Item.h"
#include "SpellActionBase.h"
#include "Spells.h"
#include "Actions.h"
#include "Constants.h"
#include "Astar.h"

Npc::Npc() {
	alive = true;
	respawn_thisframe = 0.0f;
	respawn_lastframe = 0.0f; // helps us count when to respawn the NPC.
	wander_thisframe = 0.0f;
	wander_lastframe = 0.0f; // helping us decide when the mob will wander.
	wander_every_seconds = 3; // this mob wanders every 1 seconds.
	wandering = false;

	MovingDirection = Enums::Direction::STOP;
	WanderDirection = Enums::Direction::STOP;
	activeDirection = Enums::Direction::STOP;
	lastActiveDirection = activeDirection;
	curActivity = Enums::ActivityType::Walking;
	attitudeTowardsPlayer = Enums::Attitude::NEUTRAL;
	chasingPlayer = false;
	setTarget(NULL);
	markedAsDeleted = false;
	lastPathCalculated = 0;
	wander_points_left = 0;
	
	m_isPlayer = false;
}

Npc::~Npc() {}

void Npc::draw() {
	if (m_updated) {
		
		int drawX = getXPos();
		int drawY = getYPos();

		if (getUseBoundingBox()) {
			drawX -= getBoundingBoxX();
			drawY -= getBoundingBoxY();
		}

		Vector4f color = { 1.0f, 1.0f, 1.0f, 1.0f };

		// if sneaking and character is less than 260 pixels away we draw at 0.5 and with darker colors (shade)
		// if NPC is invisible or sneaking with more than 260 pixels away we don't draw the NPC at all.
		// if the character can see invisible or sneaking, we draw with 0.5 transparency.
		Player& player = Player::Get();
		double distance = sqrt(pow((getXPos() + getWidth() / 2) - (player.getXPos() + player.getWidth() / 2), 2)
			+ pow((getYPos() + getHeight() / 2) - (player.getYPos() + player.getHeight() / 2), 2));

		if (isSneaking() == true && (distance < 260 || player.canSeeSneaking() == true)) {
			color[0] = 0.7f;
			color[1] = 0.7f;
			color[2] = 0.7f;
			color[3] = 0.5f;
		}

		if (isInvisible() == true && player.canSeeInvisible() == true) {
			color[3] = 0.5f;
		}

		if ((isInvisible() == true && player.canSeeInvisible() == false) || (isSneaking() == true && distance > 260 && player.canSeeSneaking() == false)) {
			return;
		}

		if (curActivity == Enums::ActivityType::Dying) {
			if (reduceDyingTranspFrame < Globals::clock.getElapsedTimeMilli() - 50) {
				dyingTransparency -= 0.025;
				reduceDyingTranspFrame = Globals::clock.getElapsedTimeMilli();
			}
			color[3] = dyingTransparency;
		}

		TextureManager::DrawTextureBatched(*rect, drawX, drawY, color, true, true, 5u);
	}
}

void Npc::update(float deltaTime) {
	
	m_updated = TextureManager::IsRectOnScreen(getXPos(), rect->width, getYPos(), rect->height);
	regenerateLifeManaFatigue(deltaTime);
	if (m_updated) {
		CalculateStats();
		cleanupActiveSpells();
		cleanupCooldownSpells();
		Respawn();
		curActivity = m_waitForAnimation ? curActivity : getCurActivity();
		lastActiveDirection = activeDirection != Enums::Direction::STOP ? activeDirection : lastActiveDirection;

		processInput();
		Move(deltaTime);
		Animate(deltaTime);

		const std::vector<SpellActionBase*>& activeSpellActions = getActiveSpells();
		for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpellActions.size(); ++curActiveSpellNr) {
			activeSpellActions[curActiveSpellNr]->inEffect(deltaTime);
		}
		
	}

}

void Npc::setCharacterType(std::string characterType) {
	m_characterType = &CharacterTypeManager::Get().getCharacterType(characterType);
	m_characterTypeStr = characterType;

	setStrength(m_characterType->strength);
	setDexterity(m_characterType->dexterity);
	setVitality(m_characterType->vitality);
	setIntellect(m_characterType->intellect);
	setWisdom(m_characterType->wisdom);
	setMaxHealth(m_characterType->max_health);
	setMaxMana(m_characterType->max_mana);
	setMaxFatigue(m_characterType->max_fatigue);
	setMinDamage(m_characterType->min_damage);
	setMaxDamage(m_characterType->max_damage);
	setArmor(m_characterType->armor);

	setHealthRegen(m_characterType->healthRegen);
	setManaRegen(m_characterType->manaRegen);
	setFatigueRegen(m_characterType->fatigueRegen);
	setDamageModifierPoints(m_characterType->damageModifierPoints);
	setHitModifierPoints(m_characterType->hitModifierPoints);
	setEvadeModifierPoints(m_characterType->evadeModifierPoints);
	setClass(m_characterType->characterClass);
	setName(m_characterType->name);
	setLevel(m_characterType->level);
	setExperienceValue(m_characterType->experienceValue);
	setCoinDrop(m_characterType->minCoinDrop, m_characterType->maxCoinDrop, m_characterType->coinDropChance);

	for (const auto& spell : m_characterType->spellbook) {
		inscribeSpellInSpellbook(spell);
	}

	for (const auto& loot : m_characterType->lootTable) {
		addItemToLootTable(loot.item, loot.dropChance);
	}

	rect = &m_characterType->m_moveTileSets.at({ getCurActivity(), GetDirectionRNG() }).getAllTiles()[0].textureRect;
	wander_radius = m_characterType->wander_radius;
}

std::string Npc::getCharacterTypeStr() {
	return m_characterTypeStr;
}

int Npc::getSpawnPosX() {
	return x_spawn_pos;
}

int Npc::getSpawnPosY() {
	return y_spawn_pos;
}

int Npc::getDoRespawn() {
	return do_respawn;
}

int Npc::getSecondsToRespawn() {
	return seconds_to_respawn;
}

std::string Npc::getAttitudeStr() {
	return "Enums." + Character::AttitudeToString(attitudeTowardsPlayer);
}

std::vector<CallIndirection*>& Npc::getOnDieEventHandlers() {
	return onDieEventHandlers;
}

void Npc::addItemToLootTable(Item *item, double dropChance) {
	lootTable.push_back(LootTable(item, dropChance));
}

void Npc::setCoinDrop(unsigned int minCoinDrop, unsigned int maxCoinDrop, double dropChance) {
	this->minCoinDrop = minCoinDrop;
	this->maxCoinDrop = maxCoinDrop;
	this->coinDropChance = dropChance;
}

bool Npc::CheckMouseOver(int _x_pos, int _y_pos) {
	int myWidth = getWidth();
	int myHeight = getHeight();
	if (((x_pos < _x_pos) && ((x_pos + myWidth) > _x_pos))
		&& ((y_pos < _y_pos) && ((y_pos + myHeight) > _y_pos))) {
		return true;
	}
	else {
		return false;
	}
}


Enums::Direction Npc::GetDirectionRNG() {
	return static_cast<Enums::Direction>(RNG::randomSizeT(1, 8));
}

Enums::Direction Npc::GetOppositeDirection(Enums::Direction direction) {
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

Enums::Direction Npc::GetDirection() {
	Player *player = &Player::Get();
	if (chasingPlayer == true) {
		if (waypoints.size() > 0) {
			// follow waypoints
			std::array<int,2> nextWP = waypoints.back();
			return getDirectionTowardsWaypointAt(nextWP[0], nextWP[1]);
		}else {
			// run directly towards the player
			return getDirectionTowards((player->getXPos() + player->getWidth()) / 2, (player->getYPos() + player->getHeight()) / 2);
		}
	}
	if (wandering) {
		return WanderDirection;
	} else {
		return MovingDirection;
	}
}

Enums::Direction Npc::getDirectionTowardsWaypointAt(int x_pos, int y_pos) const {
	int dx = x_pos - this->x_pos;
	int dy = y_pos - this->y_pos;

	if (dx > 0) {
		if (dy > 0) {
			return Enums::NE;
		}
		else if (dy < 0) {
			return Enums::SE;
		}
		else {
			return Enums::E;
		}
	}
	else if (dx < 0) {
		if (dy > 0) {
			return Enums::NW;
		}
		else if (dy < 0) {
			return Enums::SW;
		}
		else {
			return Enums::W;
		}
	}
	else {
		if (dy > 0) {
			return Enums::N;
		}
		else if (dy < 0) {
			return Enums::S;
		}
		else {
			return Enums::STOP;
		}
	}
}

Enums::Direction Npc::getDirectionTowards(int x_pos, int y_pos) const {
	int dx = x_pos - (this->x_pos + this->getWidth()) / 2;
	int dy = y_pos - (this->y_pos + this->getHeight()) / 2;

	if (dx > 0) {
		if (dy > 0) {
			return Enums::NE;
		}
		else if (dy < 0) {
			return Enums::SE;
		}
		else {
			return Enums::E;
		}
	}
	else if (dx < 0) {
		if (dy > 0) {
			return Enums::NW;
		}
		else if (dy < 0) {
			return Enums::SW;
		}
		else {
			return Enums::W;
		}
	}
	else {
		if (dy > 0) {
			return Enums::N;
		}
		else if (dy < 0) {
			return Enums::S;
		}
		else {
			return Enums::STOP;
		}
	}
}

void Npc::chasePlayer(Character *player) {
	chasingPlayer = true;
	setTarget(player);

	// This can be used to deactivate pathfinding, because it might cause performance problems
	bool dontUsePathfinding = false;
	if (!dontUsePathfinding) {
		// check whether we need to calculate a path for this NPC
		// if so calculate the path and set a list of waypoints

		// don't calculate a path too often
		bool neverCalculatePath = dontUsePathfinding || (Globals::clock.getElapsedTimeMilli() - lastPathCalculated < 2000);
		// don't calculate a path if we are very close to the player
		int rawDistance = ((std::sqrt(std::pow(getXPos() + getWidth() / 2 - (player->getXPos() + player->getWidth() / 2), 2)
			+ std::pow(getYPos() + getHeight() / 2 - (player->getYPos() + player->getHeight() / 2), 2)))
			- std::sqrt(std::pow((getWidth() + player->getWidth()) / 2, 2)
				+ std::pow((getHeight() + player->getHeight()) / 2, 2)));
		if (rawDistance < 50) {
			waypoints.clear();
			neverCalculatePath = true;
		}
		bool calculatePath = (waypoints.size() == 0);
		// recalculate the path if the player has moved too far from the endpoint of the path
		if (!calculatePath && !neverCalculatePath) {
			std::array<int, 2> lastPoint = waypoints.front();
			if (std::pow(lastPoint[0] - player->getXPos(), 2) + std::pow(lastPoint[1] - player->getYPos(), 2) > std::max(waypoints.size() * 100, static_cast<size_t>(1000))) {
				calculatePath = true;
			}
		}
		if (calculatePath && !neverCalculatePath) {
			waypoints = aStar(std::array<int, 2>{getXPos(), getYPos()}, std::array<int, 2>{player->getXPos() + player->getWidth() / 2, player->getYPos() + player->getHeight() / 2}, getWidth(), getHeight());
			if (waypoints.size() > 0) {
				waypoints.pop_back();
				lastPathCalculated = Globals::clock.getElapsedTimeMilli();
			}
		}
	}
}

bool Npc::canBeDamaged() const {
	return attitudeTowardsPlayer != Enums::Attitude::FRIENDLY;
}

void Npc::Damage(int amount, bool criticalHit) {
	// don't damage quest characters etc.
	if (!canBeDamaged())
		return;

	chasePlayer(&Player::Get());
	Character::Damage(amount, criticalHit);
}

void Npc::Die() {

	if (hasChoosenDyingDirection == false) {
		dyingStartFrame = Globals::clock.getElapsedTimeMilli();
		reduceDyingTranspFrame = Globals::clock.getElapsedTimeMilli() + 7000;
	}

	curActivity = Enums::ActivityType::Dying;
	currentFrame = 0;
	m_animationTime = 0.0f;
	m_waitForAnimation = true;
	m_stopped = true;
	wander_points_left = 0;
	chasingPlayer = false;
	waypoints.clear();
	dropItems();
	alive = false;
	respawn_lastframe = Globals::clock.getElapsedTimeMilli();
	onDie();
}

void Npc::dropItems() {
	// iterate through the loot table and see if we should drop any items.
	Zone *curZone = ZoneManager::Get().getCurrentZone();

	for (size_t tableID = 0; tableID < lootTable.size(); ++tableID) {
		// if the item in the table is a questitem, we first need to see if we have a quest which will generate such a drop
		if (lootTable[tableID].item->getItemType() == Enums::ItemType::QUESTITEM) {
			if (QuestCanvas::Get().anyQuestNeedThis(lootTable[tableID].item) == false) {
				// yes, it's a quest item, but not needed in any quest so aborting here.
				break;
			}
		}

		double dropChance = RNG::randomDouble(0, 1);;
		if (dropChance <= lootTable[tableID].dropChance) {
			curZone->getGroundLoot().addItem(getXPos(), getYPos(), lootTable[tableID].item);
		}
	}

	
	double dropChance = RNG::randomDouble(0, 1);
	if (dropChance <= coinDropChance) {
		curZone->getGroundLoot().addItem(getXPos(), getYPos(), new GoldHeap(RNG::randomSizeT(minCoinDrop, maxCoinDrop)));
	}
}

void Npc::onDie() {
	for (size_t curEventHandlerNr = 0; curEventHandlerNr < onDieEventHandlers.size(); ++curEventHandlerNr) {
		onDieEventHandlers[curEventHandlerNr]->call();
	}
}

void Npc::Respawn() {
	if (alive == false && do_respawn == true) {
		respawn_thisframe = Globals::clock.getElapsedTimeMilli();
		if ((respawn_thisframe - respawn_lastframe) >(seconds_to_respawn * 1000)) {
			setTarget(NULL);
			alive = true;
			
			x_pos = x_spawn_pos;
			y_pos = y_spawn_pos;
			respawn_thisframe = 0.0f;
			respawn_lastframe = 0.0f;
			setCurrentHealth(getModifiedMaxHealth());
			setCurrentMana(getModifiedMaxMana());

			activeDirection = Enums::Direction::STOP;
			lastActiveDirection = activeDirection;
			wandering = false;
			curActivity = Enums::ActivityType::Walking;
			m_handleAnimation = false;
			m_waitForAnimation = false;
			m_stopped = false;
			isPreparing = false;
			alive = true;
			hasDrawnDyingOnce = false;
			hasChoosenFearDirection = false;
			hasChoosenDyingDirection = false;
			curSpellAction = NULL;
			coins = 0;
			dyingTransparency = 1.0f;
		}
	}
}

Enums::Attitude Npc::getAttitude() const {
	return this->attitudeTowardsPlayer;
}

void Npc::markAsDeleted() {
	markedAsDeleted = true;
}

bool Npc::isMarkedAsDeletable() const {
	return markedAsDeleted;
}

void Npc::setWanderRadius(unsigned short newWanderRadius) {
	wander_radius = newWanderRadius;
}

void Npc::setSpawnInfo(int _x_spawn_pos, int _y_spawn_pos, int _seconds_to_respawn, int _do_respawn) {
	x_pos = _x_spawn_pos;
	y_pos = _y_spawn_pos;
	x_spawn_pos = _x_spawn_pos;
	y_spawn_pos = _y_spawn_pos;
	do_respawn = _do_respawn;
	seconds_to_respawn = _seconds_to_respawn;
}

void Npc::setAttitude(Enums::Attitude attitude) {
	attitudeTowardsPlayer = attitude;
}

/////////////////////////////PRIVATE/////////////////////////////////
void Npc::Move(float deltaTime) {
	std::vector<SpellActionBase*> curSpellbook = getSpellbook();
	for (size_t spellIndex = 0; spellIndex < curSpellbook.size(); spellIndex++) {

		/// \todo better AI to determine what spells to be cast.
		///
		/// As of now, we just check if the spell is within range,
		/// has enough mana and isn't on cooldown...
		bool castableSpell = true;

		if (dynamic_cast<GeneralBuffSpell*>(curSpellbook[spellIndex]) == NULL) {
			castableSpell = false;
		}

		if (dynamic_cast<Spell*>(curSpellbook[spellIndex]) != NULL) {
			if (curSpellbook[spellIndex]->getSpellCost() > getCurrentMana()) {
				/// can't cast. not enough mana.
				castableSpell = false;
			}
		}

		if (curSpellbook[spellIndex]->getEffectType() != Enums::EffectType::SelfAffectingSpell && getTarget() != NULL) {
			uint16_t distance = sqrt(pow((getXPos() + getWidth() / 2) - (getTarget()->getXPos() + getTarget()->getWidth() / 2), 2) + pow((getYPos() + getHeight() / 2) - (getTarget()->getYPos() + getTarget()->getHeight() / 2), 2));
			if (curSpellbook[spellIndex]->isInRange(distance) == false) {
				/// can't cast, not in range.
				castableSpell = false;
			}
		}

		
		for (size_t curSpell = 0; curSpell < cooldownSpells.size(); curSpell++) {
			if (cooldownSpells[curSpell]->getID() == curSpellbook[spellIndex]->getID()){
				if (cooldownSpells[curSpell]->m_timer.getElapsedTimeMilli() > cooldownSpells[curSpell]->getCooldown() * 1000u) {
					/// can't cast, spell has a cooldown on it.
					castableSpell = false;
				}
			}
		}

		if (castableSpell == true) {

			SpellActionBase *curAction = NULL;

			Enums::EffectType effectType = curSpellbook[spellIndex]->getEffectType();

			if (effectType == Enums::EffectType::SingleTargetSpell && getTarget() != NULL) {

				curAction = curSpellbook[spellIndex]->cast(this, getTarget(), false);

			} else if (effectType == Enums::EffectType::SelfAffectingSpell) {
				curAction = curSpellbook[spellIndex]->cast(this, this, false);
			}

			if (curAction != NULL) {
				castSpell(dynamic_cast<SpellActionBase*>(curAction));
			}
		}
	}

	if (mayDoAnythingAffectingSpellActionWithoutAborting() && chasingPlayer == true) {
		std::vector<SpellActionBase*> curSpellbook = getSpellbook();
		for (size_t spellIndex = 0; spellIndex < curSpellbook.size(); spellIndex++) {

			/// \todo better AI to determine what spells to be cast.
			///
			/// As of now, we just check if the spell is within range,
			/// has enough mana and isn't on cooldown...
			bool castableSpell = true;

			if (dynamic_cast<Action*>(curSpellbook[spellIndex]) != NULL) {
				if (curSpellbook[spellIndex]->getSpellCost() > getCurrentFatigue()) {
					/// can't cast. cost more fatigue than we can afford.
					castableSpell = false;
				}
			} else if (dynamic_cast<Spell*>(curSpellbook[spellIndex]) != NULL) {
				if (curSpellbook[spellIndex]->getSpellCost() > getCurrentMana()) {
					/// can't cast. not enough mana.
					castableSpell = false;
				}
			}
			
			if (curSpellbook[spellIndex]->getEffectType() != Enums::EffectType::SelfAffectingSpell && getTarget() != NULL) {
				uint16_t distance = sqrt(pow((getXPos() + getWidth() / 2) - (getTarget()->getXPos() + getTarget()->getWidth() / 2), 2) + pow((getYPos() + getHeight() / 2) - (getTarget()->getYPos() + getTarget()->getHeight() / 2), 2));
				if (curSpellbook[spellIndex]->isInRange(distance) == false) {
					/// can't cast, not in range.
					castableSpell = false;
				}
			}

			for (size_t curSpell = 0; curSpell < cooldownSpells.size(); curSpell++) {
				if (cooldownSpells[curSpell]->getID() == curSpellbook[spellIndex]->getID()) {
					if (cooldownSpells[curSpell]->m_timer.getElapsedTimeMilli() > cooldownSpells[curSpell]->getCooldown() * 1000u) {
						/// can't cast, spell has a cooldown on it.
						castableSpell = false;
					}
				}
			}

			if (castableSpell == true) {
				SpellActionBase *curAction = NULL;
				
				Enums::EffectType effectType = curSpellbook[spellIndex]->getEffectType();

				if (effectType == Enums::EffectType::SingleTargetSpell && getTarget() != NULL) {
					curAction = curSpellbook[spellIndex]->cast(this, getTarget(), false);
				} else if (effectType == Enums::EffectType::SelfAffectingSpell) {
					curAction = curSpellbook[spellIndex]->cast(this, this, false);
				}

				if (curAction != NULL) {
					castSpell(dynamic_cast<SpellActionBase*>(curAction));
				}
			}
		}
	}
	
	Player *player = &Player::Get();
	double distance = sqrt(pow((getXPos() + getWidth() / 2) - (player->getXPos() + player->getWidth() / 2), 2) + pow((getYPos() + getHeight() / 2) - (player->getYPos() + player->getHeight() / 2), 2));
	// if player is inside agro range of NPC, we set NPC to attack mode.
	if (distance < 200 && getAttitude() == Enums::Attitude::HOSTILE && (player->isInvisible() == false || canSeeInvisible() == true) && (player->isSneaking() == false || canSeeSneaking() == true)) {
		chasingPlayer = alive;
	} else if (distance < 80 && getAttitude() == Enums::Attitude::HOSTILE && player->isSneaking() == true) { // do this if player is sneaking and NPC is near the character.
		chasingPlayer = alive;
	}

	if (!m_stopped && !chasingPlayer) {
		if (activeDirection != Enums::Direction::STOP || (wander_points_left > 0)) {
			wander_points_left--;
			m_canWander = (pow(x_pos - x_spawn_pos, 2) + pow(y_pos - y_spawn_pos, 2)) < getWanderRadiusSq();

			if (!m_canWander) {
				WanderDirection = GetOppositeDirection(lastActiveDirection);
				while ((pow(x_pos - x_spawn_pos, 2) + pow(y_pos - y_spawn_pos, 2)) >= getWanderRadiusSq()) {
					Move(deltaTime, WanderDirection);
				}

				activeDirection = Enums::Direction::STOP;
				m_handleAnimation = false;
				wander_points_left = 0;
				currentFrame = 0;

			}else if (wander_points_left == 0) {
				activeDirection = Enums::Direction::STOP;
				wander_points_left = m_handleAnimation ? wander_points_left + 1 : 0;
				m_handleAnimation = m_smoothOut;
			}

			wander_lastframe = Globals::clock.getElapsedTimeMilli();

		}else {
			
			wander_thisframe = Globals::clock.getElapsedTimeMilli();
			if (m_canWander = (wander_thisframe - wander_lastframe) > (wander_every_seconds * 1000)) {

				//wander_points_left = 200;
				wander_points_left = RNG::randomSizeT(10, 59);  // how far will the NPC wander?

				WanderDirection = WanderDirection != lastActiveDirection ? WanderDirection : GetDirectionRNG();
				activeDirection = WanderDirection;
				lastActiveDirection = activeDirection;

				m_handleAnimation = m_smoothOut;

				//wander_every_seconds = 1;
				wander_every_seconds = RNG::randomSizeT(3, 6);
			}
		}

		if (m_canWander) {
			Move(deltaTime, lastActiveDirection);
		}
	}

	if (chasingPlayer) {
		chasePlayer(player);
		activeDirection = GetDirection();
		lastActiveDirection = activeDirection;

		Move(deltaTime, lastActiveDirection);
		if (waypoints.size() > 0) {
			std::array<int, 2> nextWP = waypoints.back();
			if (getXPos() == nextWP[0] && getYPos() == nextWP[1]) {
				waypoints.pop_back();
			}
		}
	}
		
}

void Npc::Move(float deltaTime, Enums::Direction& direction) {
	
	if (isStunned() == true || isMesmerized() == true) {
		return;
	}

	if (isFeared() == false) {
		hasChoosenFearDirection = false;
	}

	if (!mayDoAnythingAffectingSpellActionWithoutAborting()) {
		if (!mayDoAnythingAffectingSpellActionWithAborting()) {
			return;
		}
	}

	if (direction != Enums::Direction::STOP && isChanneling() == true) {
		removeSpellsWithCharacterState(Enums::CharacterStates::Channeling);
	}


	/// if we are feared (fleeing) we run at a random direction. Only choose a direction once for each fear effect.
	if (isFeared() == true) {
		if (hasChoosenFearDirection == false) {
			fearDirection = GetDirectionRNG();
			hasChoosenFearDirection = true;
		}
		direction = fearDirection;
	}

	float movePerStep = 0.01f;
	if (direction == Enums::Direction::NW || direction == Enums::Direction::NE || direction == Enums::Direction::SW || direction == Enums::Direction::SE)
		movePerStep = 0.014f;
	
	m_elapsedTime += deltaTime;
	while (m_elapsedTime > movePerStep) {
		
		m_elapsedTime -= movePerStep;
		Character::Move(direction);
	}
}

void Npc::Animate(float deltaTime) {
	if (activeDirection != Enums::Direction::STOP || (m_handleAnimation || m_waitForAnimation)) {
		const TileSet& tileSet = m_characterType->m_moveTileSets.at({ curActivity, lastActiveDirection });
		unsigned short numActivityTextures = m_characterType->m_numMoveTexturesPerDirection.at(curActivity);

		m_animationTime = m_animationTime >= numActivityTextures ? 0.0f : m_animationTime + deltaTime * 12;
		currentFrame = static_cast<unsigned short>(floor(m_animationTime));
		if (++currentFrame > numActivityTextures - 1) {
			currentFrame = curActivity == Enums::ActivityType::Dying ? numActivityTextures - 1 : 0;
			m_handleAnimation = activeDirection != Enums::Direction::STOP && !chasingPlayer;
			m_waitForAnimation = false;
			activeDirection = curActivity == Enums::ActivityType::Dying ? Enums::Direction::STOP : activeDirection;

		}	
		rect = &tileSet.getAllTiles()[currentFrame].textureRect;
	}
}

void Npc::processInput() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyDown(Keyboard::KEY_E) && !m_waitForAnimation) {
		curActivity = Enums::ActivityType::Dying;		
		currentFrame = 0;
		m_animationTime = 0.0f;
		m_waitForAnimation = true;
		m_stopped = true;
		activeDirection = Enums::Direction::STOP;
		wander_points_left = 0;
		
	}

	if (keyboard.keyDown(Keyboard::KEY_R) && !m_waitForAnimation) {
		curActivity = Enums::ActivityType::Attacking;
		currentFrame = 0;
		m_animationTime = 0.0f;
		m_waitForAnimation = true;
		m_stopped = true;
		activeDirection = Enums::Direction::STOP;
		wander_points_left = 0;
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		curActivity = Enums::ActivityType::Walking;
		activeDirection = GetDirectionRNG();
		currentFrame = 0;
		m_stopped = false;

		//wander_points_left = RNG::randomSizeT(10, 59);
		wander_points_left = 59;
	}
}

void Npc::ProcessInput() {
	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		// get and iterate through the NPCs
		std::vector<Npc*> zoneNPCs = GetNPCs();
		for (unsigned int x = 0; x < zoneNPCs.size(); x++) {
			Npc* curNPC = zoneNPCs[x];
			// is the mouse over a NPC and no AoE spell is being prepared?
			if (curNPC->CheckMouseOver(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY())) {
				// is the NPC friendly?
				if (!curNPC->getAttitude() == Enums::Attitude::FRIENDLY) {
					// set a target if the player has none
					if (!Player::Get().hasTarget(curNPC)) {
						Player::Get().setTarget(curNPC, curNPC->getAttitude());
					}
					else {
						Player::Get().setTarget(NULL);
					}
					break;
				}
			}
		}
	}
}

unsigned short Npc::getWanderRadius() const {
	return wander_radius;
}

unsigned short Npc::getWanderRadiusSq() const {
	return wander_radius * wander_radius;
}

void Npc::addOnDieEventHandler(CallIndirection* eventHandler) {
	onDieEventHandlers.push_back(eventHandler);

	auto it = std::find(onDieEventHandlers.begin(), onDieEventHandlers.end(), eventHandler);

	if (onDieEventHandlers.end() == it) {
		onDieEventHandlers.emplace_back(eventHandler);
	}
}

void Npc::removeOnDieEventHandler(CallIndirection* eventHandler) {
	auto it = std::find(onDieEventHandlers.begin(), onDieEventHandlers.end(), eventHandler);
	if (it != onDieEventHandlers.end()) { onDieEventHandlers.erase(it); }
}

bool Npc::hasOnDieEventHandler() const {

	if (onDieEventHandlers.size() > 0) {
		return true;
	}else {
		return false;
	}
}

bool Npc::canSeeInvisible() const {
	for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell]->getCharacterState().first == Enums::CharacterStates::SeeInvisible) {
			return true;
		}
	}
	return false;
}

bool Npc::canSeeSneaking() const {
	for (size_t activeSpell = 0; activeSpell < activeSpells.size(); activeSpell++) {
		if (activeSpells[activeSpell]->getCharacterState().first == Enums::CharacterStates::SeeSneaking) {
			return true;
		}
	}
	return false;

}

std::string Npc::getLuaEditorSaveText() const {
	std::ostringstream oss;

	oss << "DawnInterface.addMobSpawnPoint( \"" << m_characterTypeStr << "\", "
		<< x_spawn_pos << ", "
		<< y_spawn_pos << ", "
		<< seconds_to_respawn << ", "
		<< do_respawn << ", "
		<< "Enums." << Character::AttitudeToString(attitudeTowardsPlayer) << " );" << std::endl;

	return oss.str();
}

std::string Npc::getLuaSaveText() const {
	std::ostringstream oss;
	std::string objectName = "curNPC";
	oss << "local " << objectName << " = DawnInterface.addMobSpawnPoint( \"" << m_characterTypeStr << "\", " << "\"" << name << "\", "
		<< x_spawn_pos << ", "
		<< y_spawn_pos << ", "
		<< seconds_to_respawn << ", "
		<< do_respawn << ", "
		<< "Enums." << Character::AttitudeToString(attitudeTowardsPlayer) << " );" << std::endl;
	// add onDieEventHandlers for this npc
	for (size_t curOnDieHandlerNr = 0; curOnDieHandlerNr<onDieEventHandlers.size(); ++curOnDieHandlerNr) {
		LuaCallIndirection *luaHandler = dynamic_cast<LuaCallIndirection*>(onDieEventHandlers[curOnDieHandlerNr]);
		if (luaHandler != NULL) {
			// a real LuaCallIndirection
			oss << objectName << ":addOnDieEventHandler( " << DawnInterface::getItemReferenceRestore("",luaHandler) << " );" << std::endl;
		}
		
	}
	return oss.str();
}


std::vector<Npc*>& Npc::GetNPCs() {
	return ZoneManager::Get().getCurrentZone()->getNPCs();
}

void Npc::DrawActiveSpells() {
	
	std::vector<Npc*>& NPCs = GetNPCs();
	for (unsigned int x = 0; x < NPCs.size(); x++) {
		Npc *NPC = NPCs[x];
		// draw the spell effects for our NPCs

		const std::vector<SpellActionBase*>& activeSpellActions = NPC->getActiveSpells();
		for (size_t curActiveSpellNr = 0; curActiveSpellNr < activeSpellActions.size(); ++curActiveSpellNr) {
			if (!activeSpellActions[curActiveSpellNr]->isEffectComplete()) {
	
				activeSpellActions[curActiveSpellNr]->draw();
			}
		}
	}
}