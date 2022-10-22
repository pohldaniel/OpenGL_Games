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

	WanderDirection = Enums::Direction::STOP;
	activeDirection = Enums::Direction::STOP;
	lastActiveDirection = activeDirection;
	curActivity = Enums::ActivityType::Walking;

	direction_texture = 0;
	attitudeTowardsPlayer = Enums::Attitude::NEUTRAL;
	chasingPlayer = false;
	//setTarget(NULL);
	markedAsDeleted = false;
	lastPathCalculated = 0;
	wander_points_left = 0;
	rect = &m_characterType.m_moveTileSets.at({ getCurActivity(), GetDirectionRNG() }).getAllTiles()[0].textureRect;
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
		processInput();
		lastActiveDirection = activeDirection != Enums::Direction::STOP ? activeDirection : lastActiveDirection;
		Move(deltaTime);
		Animate(deltaTime);
	}
}

Enums::Direction Npc::GetDirectionRNG() {
	return static_cast<Enums::Direction>(RNG::randomSizeT(1, 8));	
}

void Npc::Move(float deltaTime) {
		
	if (!m_stopped) {
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
}

void Npc::Move(float deltaTime, Enums::Direction direction) {
	
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
		const TileSet& tileSet = m_characterType.m_moveTileSets.at({ curActivity, lastActiveDirection });
		unsigned short numActivityTextures = getNumActivityTextures(curActivity);

		m_animationTime = m_animationTime >= numActivityTextures ? 0.0f : m_animationTime + deltaTime * 12;
		currentFrame = static_cast<unsigned short>(floor(m_animationTime));
		if (++currentFrame > numActivityTextures - 1) {
			currentFrame = curActivity == Enums::ActivityType::Dying ? numActivityTextures - 1 : 0;
			m_handleAnimation = activeDirection != Enums::Direction::STOP;
			m_waitForAnimation = false;

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

void Npc::markAsDeleted() {
	markedAsDeleted = true;
}

bool Npc::isMarkedAsDeletable() const {
	return markedAsDeleted;
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

			//waypoints = aStar(Point(getXPos(), getYPos()), Point(player->getXPos() + player->getWidth() / 2, player->getYPos() + player->getHeight() / 2), getWidth(), getHeight());
			if (waypoints.size() > 0) {
				waypoints.pop_back();
				lastPathCalculated = Globals::clock.getElapsedTimeMilli();
			}
		}
	}
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

Enums::Attitude Npc::getAttitude() const {
	return this->attitudeTowardsPlayer;
}