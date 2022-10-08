#include "Npc.h"
#include "TilesetManager.h"
#include "Constants.h"

Npc::Npc(const CharacterType& characterType, int _x_spawn_pos, int _y_spawn_pos, int _NPC_id, int _seconds_to_respawn, int _do_respawn) : Character(characterType) {
	alive = true;
	current_texture = 1; // this will be altered later on to draw what animation frame we want to draw.
	respawn_thisframe = 0.0f;
	respawn_lastframe = 0.0f; // helps us count when to respawn the NPC.
	wander_thisframe = 0.0f;
	wander_lastframe = 0.0f; // helping us decide when the mob will wander.
	wander_every_seconds = 3; // this mob wanders every 1 seconds.
	wandering = false;
	MovingDirection = Enums::Direction::STOP;

	direction_texture = 0;
	attitudeTowardsPlayer = Enums::Attitude::NEUTRAL;
	chasingPlayer = false;
	//setTarget(NULL);
	markedAsDeleted = false;
	lastPathCalculated = 0;
}

Npc::~Npc() {

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

void Npc::draw() {


	//if (m_updated) {
		Enums::ActivityType curActivity = getCurActivity();

		int drawX = getXPos();
		int drawY = getYPos();
		TextureManager::DrawTextureBatched(*rect, drawX, drawY, true, true);
	//}
}
 
void Npc::update(float deltaTime) {
	m_updated = TextureManager::IsRectOnScreen(getXPos(), rect->width, getYPos(), rect->height);
	
	if (m_updated) {
		
		Wander(deltaTime);
		Move(deltaTime);
		//Character::update(deltaTime);
	}
}

Enums::Direction Npc::GetDirection(){

	if (wandering) {
		return WanderDirection;
	}else {
		return MovingDirection;
	}
}

Enums::Direction Npc::GetDirectionRNG() {
	return static_cast<Enums::Direction>(RNG::randomSizeT(1, 8));	
}

void Npc::Wander(float deltaTime) {
	
	if (wandering) {
		Enums::ActivityType curActivity = Enums::ActivityType::Attacking;
		unsigned short numActivityTextures = getNumActivityTextures(curActivity);
		const TileSet& tileSet = m_characterType.m_moveTileSets.at({ curActivity, activeDirection });

		if (wander_points_left > 0) {
		
			m_canWander = (pow(x_pos - x_spawn_pos, 2) + pow(y_pos - y_spawn_pos, 2)) < getWanderRadiusSq();

			wander_points_left = m_canWander ? wander_points_left - 1 : 0;

			m_wanderTime = m_wanderTime >= numActivityTextures ? 0.0f : m_wanderTime + deltaTime * 12;
			index = static_cast<unsigned short>(floor(m_wanderTime));
			currentFrame = index % (numActivityTextures);
			index = index % (numActivityTextures + 1);

			if ((wander_points_left == 0 && (index + 1 < numActivityTextures + 1)  && m_canWander) && m_smoothOut) {
				wander_points_left++;					
			}
		}else {

			wander_lastframe = Globals::clock.getElapsedTimeMilli();
			wandering = false;
			WanderDirection = Enums::Direction::STOP;				
			m_wanderTime = m_smoothOut ? 0.0f : m_wanderTime;
			index = m_smoothOut ? 0 : index;
			
			//index = 0;
			//currentFrame = 0;
		}
	
		rect = &tileSet.getAllTiles()[currentFrame].textureRect;

	}else {

		wander_thisframe = Globals::clock.getElapsedTimeMilli();
		if ((wander_thisframe - wander_lastframe) > (wander_every_seconds * 1000)) {
			wandering = true;
			wander_points_left = RNG::randomSizeT(10, 59);  // how far will the NPC wander?

			WanderDirection = !m_canWander ? GetOppositeDirection(activeDirection) : GetDirectionRNG();
			activeDirection = WanderDirection;

			if (!m_canWander) {
				while ((pow(x_pos - x_spawn_pos, 2) + pow(y_pos - y_spawn_pos, 2)) >= getWanderRadiusSq()) {
					Move(deltaTime);
				}

				m_canWander = true;
			}

			wander_every_seconds = 1;		
		}
	}
}

void Npc::Move(float deltaTime) {

	if (isStunned() == true || isMesmerized() == true) {
		m_elapsedTime = 0.0f;
		return;
	}

	if (isFeared() == false) {
		hasChoosenFearDirection = false;
	}

	continuePreparing();
	if (!mayDoAnythingAffectingSpellActionWithoutAborting()) {
		if (!mayDoAnythingAffectingSpellActionWithAborting()) {
			m_elapsedTime = 0.0f;
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

	float movePerStep = 0.01f; // moves one step per movePerStep ms

							   // To balance moving diagonally boost, movePerStep = 10*sqrt(2)
	if (movingDirection == Enums::Direction::NW || movingDirection == Enums::Direction::NE || movingDirection == Enums::Direction::SW || movingDirection == Enums::Direction::SE)
		movePerStep = 0.014f;

	// recalculate the movementpoints based on our movementspeed (spells that affect this can be immobolizing spells, snares or movement enhancer
	m_elapsedTime += deltaTime;
	while (m_elapsedTime > movePerStep) {
		m_elapsedTime -= movePerStep;
		switch (movingDirection) {
		case Enums::Direction::NW:
			MoveLeft(1);
			MoveUp(1);
			break;
		case Enums::Direction::N:
			MoveUp(1);
			break;
		case Enums::Direction::NE:
			MoveRight(1);
			MoveUp(1);
			break;
		case Enums::Direction::W:
			MoveLeft(1);
			break;
		case Enums::Direction::E:
			MoveRight(1);
			break;
		case Enums::Direction::SW:
			MoveLeft(1);
			MoveDown(1);
			break;
		case Enums::Direction::S:
			MoveDown(1);
			break;
		case Enums::Direction::SE:
			MoveRight(1);
			MoveDown(1);
			break;
		default:
			break;
		}
	}
}

void Npc::markAsDeleted() {
	markedAsDeleted = true;
}

bool Npc::isMarkedAsDeletable() const {
	return markedAsDeleted;
}

std::string Npc::getLuaEditorSaveText() const {
	std::ostringstream oss;

	oss << "DawnInterface.addMobSpawnPoint( \"" << "Wolf" << "\", "
		<< x_spawn_pos << ", "
		<< y_spawn_pos << ", "
		<< seconds_to_respawn << ", "
		<< do_respawn << ", " 
		<<  "Enums." << Character::AttitudeToString(attitudeTowardsPlayer) << " );" << std::endl;

	return oss.str();
}
