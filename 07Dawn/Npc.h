#pragma once

#include <vector>

#include "Character.h"


class Npc : public Character{

public:

	Npc(const CharacterType& characterType, int _x_spawn_pos, int _y_spawn_pos, int _NPC_id, int _seconds_to_respawn, int _do_respawn);

	
	~Npc();
	void draw() override;
	void update(float deltaTime);

	Enums::Direction GetDirection() override;
	void Wander();
	void Move();

	void setSpawnInfo(int _x_spawn_pos, int _y_spawn_pos, int _seconds_to_respawn, int _do_respawn);
	void setAttitude(Enums::Attitude attitude);
	void markAsDeleted();
	bool isMarkedAsDeletable() const;
	std::string getLuaEditorSaveText() const;

	static std::string AttitudeToString(Enums::Attitude attitude);

private:

	//Attitude::Attitude attitudeTowardsPlayer;
	//std::vector<sLootTable> lootTable;
	//std::vector<CallIndirection*> onDieEventHandlers;
	//std::vector<Point> waypoints;
	bool markedAsDeleted;
	bool chasingPlayer;
	uint32_t lastPathCalculated;
	Enums::Attitude attitudeTowardsPlayer;
};