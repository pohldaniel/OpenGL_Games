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
	MovingDirection = STOP;

	remainingMovePoints = 0;
	direction_texture = 0;
	//attitudeTowardsPlayer = Attitude::NEUTRAL;
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

void Npc::setAttitude(Attitude::Attitude attitude) {
	attitudeTowardsPlayer = attitude;
}

void Npc::draw() {
	ActivityType::ActivityType curActivity = getCurActivity();
	
	int drawX = getXPos();
	int drawY = getYPos();

	//TextureRect& rect = getTileSet(curActivity, GetDirectionTexture())->getAllTiles()[index]->textureRect;
	TextureManager::DrawTextureBatched(*rect, drawX, drawY, true);
}

void Npc::update(float deltaTime) {
	Character::update(deltaTime);
}

Direction Npc::GetDirection(){

	if (wandering) {
		return WanderDirection;
	}else {
		return MovingDirection;
	}
}

void Npc::Wander() {
	if (wandering) {
		if (wander_points_left > 0) {
			// checking if character is moving more than the wander_radius. if he does we'll stop him.
			// probably is some other function we could use here that doesnt require as much power... /arnestig
			if (sqrt((pow(x_pos - x_spawn_pos, 2) + pow(y_pos - y_spawn_pos, 2))) < getWanderRadius()) {
				wander_points_left--;

				
				if (wander_points_left == 0 && index != 0) {
					wander_points_left++;
				}
				
			}else {
				wander_lastframe = Globals::clock.getElapsedTimeMilli();
				wandering = false;
				WanderDirection = STOP;
			}
		}else {
			wander_lastframe = Globals::clock.getElapsedTimeMilli();
			wandering = false;
			WanderDirection = STOP;
		}
	}else {
		
		wander_thisframe = Globals::clock.getElapsedTimeMilli();
		if ((wander_thisframe - wander_lastframe) > (wander_every_seconds * 1000)) {
			wandering = true;
			wander_points_left = RNG::randomSizeT(10, 59);  // how far will the NPC wander?
			WanderDirection = static_cast<Direction>(RNG::randomSizeT(1, 8));  // random at which direction NPC will go.
			wander_every_seconds = RNG::randomSizeT(3, 6);
		}
	}
}

void Npc::Move() {
	Character::Move();
}