#pragma once

#include <vector>

#include "Character.h"

class Npc : public CCharacter{

public:

	Npc(int _x_spawn_pos, int _y_spawn_pos, int _NPC_id, int _seconds_to_respawn, int _do_respawn);
	~Npc();

private:

	//Attitude::Attitude attitudeTowardsPlayer;
	//std::vector<sLootTable> lootTable;
	//std::vector<CallIndirection*> onDieEventHandlers;
	//std::vector<Point> waypoints;
	bool markedAsDeleted;
	bool chasingPlayer;
	uint32_t lastPathCalculated;
};