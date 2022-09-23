#include "Npc.h"

Npc::Npc(int _x_spawn_pos, int _y_spawn_pos, int _NPC_id, int _seconds_to_respawn, int _do_respawn) : CCharacter() {
	alive = true;
	current_texture = 1; // this will be altered later on to draw what animation frame we want to draw.
	respawn_thisframe = 0.0f;
	respawn_lastframe = 0.0f; // helps us count when to respawn the NPC.
	wander_thisframe = 0.0f;
	wander_lastframe = 0.0f; // helping us decide when the mob will wander.
	wander_every_seconds = 3; // this mob wanders every 1 seconds.
	wandering = false;
	//MovingDirection = STOP;

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
	TextureRect& rect = getTileSet(curActivity)->getAllTiles()[4]->textureRect;

	TextureManager::DrawTextureBatched(rect, drawX, drawY, true);
}