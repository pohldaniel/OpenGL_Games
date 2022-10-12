#include "Npc.h"
#include "TilesetManager.h"
#include "Constants.h"

Npc::Npc(const CharacterType& characterType, int _x_spawn_pos, int _y_spawn_pos, int _NPC_id, int _seconds_to_respawn, int _do_respawn) : m_characterType(characterType) {
	alive = true;
	current_texture = 1; // this will be altered later on to draw what animation frame we want to draw.
	respawn_thisframe = 0.0f;
	respawn_lastframe = 0.0f; // helps us count when to respawn the NPC.
	wander_thisframe = 0.0f;
	wander_lastframe = 0.0f; // helping us decide when the mob will wander.
	wander_every_seconds = 3; // this mob wanders every 1 seconds.
	wandering = false;
	MovingDirection = Enums::Direction::STOP;
	WanderDirection = GetDirectionRNG();
	activeDirection = WanderDirection;
	direction_texture = 0;
	attitudeTowardsPlayer = Enums::Attitude::NEUTRAL;
	chasingPlayer = false;
	//setTarget(NULL);
	markedAsDeleted = false;
	lastPathCalculated = 0;
	wander_points_left = 0;
	rect = &m_characterType.m_moveTileSets.at({ getCurActivity(), activeDirection }).getAllTiles()[0].textureRect;
}

Npc::~Npc() {}

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
		Move(deltaTime);
		Animate(deltaTime);
	}
}

Enums::Direction Npc::GetDirectionRNG() {
	return static_cast<Enums::Direction>(RNG::randomSizeT(1, 8));	
}

void Npc::Move(float deltaTime) {
	curActivity = Enums::ActivityType::Walking;

	if (wander_points_left > 0) {

		m_canWander = (pow(x_pos - x_spawn_pos, 2) + pow(y_pos - y_spawn_pos, 2)) < getWanderRadiusSq();
		
		if (!m_canWander) {				
			WanderDirection = GetOppositeDirection(activeDirection);
			while ((pow(x_pos - x_spawn_pos, 2) + pow(y_pos - y_spawn_pos, 2)) >= getWanderRadiusSq()) {
				Move(deltaTime, WanderDirection);
			}
			wander_points_left = -1;
		}

		wander_lastframe = Globals::clock.getElapsedTimeMilli();
	}else {
			
		wander_thisframe = Globals::clock.getElapsedTimeMilli();
		if (m_canWander = (wander_thisframe - wander_lastframe) > (wander_every_seconds * 1000)) {

			wander_points_left = RNG::randomSizeT(10, 59);  // how far will the NPC wander?

			WanderDirection = WanderDirection != activeDirection ? WanderDirection : GetDirectionRNG();
			activeDirection = WanderDirection;
			
			wander_every_seconds = 1;
			//wander_every_seconds = RNG::randomSizeT(3, 6);
		}
	}

	if (m_canWander) {
		Move(deltaTime, activeDirection);
	}
}

void Npc::Move(float deltaTime, Enums::Direction direction) {
	
	float movePerStep = 0.01f;
	if (direction == Enums::Direction::NW || direction == Enums::Direction::NE || direction == Enums::Direction::SW || direction == Enums::Direction::SE)
		movePerStep = 0.014f;

	// recalculate the movementpoints based on our movementspeed (spells that affect this can be immobolizing spells, snares or movement enhancer
	m_elapsedTime += deltaTime;
	while (m_elapsedTime > movePerStep) {
		m_elapsedTime -= movePerStep;
		Character::Move(direction);
	}
}

void Npc::Animate(float deltaTime) {
	const TileSet& tileSet = m_characterType.m_moveTileSets.at({ curActivity, activeDirection });
	unsigned short numActivityTextures = getNumActivityTextures(curActivity);

	if (wander_points_left > 0) {
		wander_points_left--;
		m_wanderTime = m_wanderTime >= numActivityTextures ? 0.0f : m_wanderTime + deltaTime * 12;
		index = static_cast<unsigned short>(floor(m_wanderTime));
		currentFrame = index % (numActivityTextures);
		index = index % (numActivityTextures + 1);

		if ((wander_points_left == 0 && (index + 1 < numActivityTextures + 1) && m_canWander) && m_smoothOut) {
			wander_points_left++;
		}
	}else {
		m_wanderTime = (m_smoothOut || !m_canWander) ? 0.0f : m_wanderTime;
		index = (m_smoothOut || !m_canWander) ? 0 : index;
	}

	rect = &tileSet.getAllTiles()[currentFrame].textureRect;
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

int Npc::getWidth() const {
	const TextureRect& rect = m_characterType.m_moveTileSets.at({ Enums::ActivityType::Walking, Enums::Direction::S }).getAllTiles()[0].textureRect;
	return useBoundingBox ? boundingBoxW : rect.width;
}

int Npc::getHeight() const {
	const TextureRect& rect = m_characterType.m_moveTileSets.at({ Enums::ActivityType::Walking, Enums::Direction::S }).getAllTiles()[0].textureRect;
	return useBoundingBox ? boundingBoxH : rect.height;
}

unsigned short Npc::getNumActivityTextures(Enums::ActivityType activity) {
	return m_characterType.m_numMoveTexturesPerDirection.at(activity);
}

const CharacterType& Npc::getCharacterType() {
	return m_characterType;
}