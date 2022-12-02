#pragma once

#include <vector>

#include "Callindirection.h"
#include "Character.h"


class Npc : public Character {

	friend class Editor;
public:

	Npc(int _x_spawn_pos, int _y_spawn_pos, int _NPC_id, int _seconds_to_respawn, int _do_respawn);
	~Npc();

	void draw() override;
	void update(float deltaTime) override;	
	void setCharacterType(std::string characterType) override;
	bool canBeDamaged() const override;

	bool CheckMouseOver(int _x_pos, int _y_pos);
	void chasePlayer(Character *player);
	Enums::Direction GetDirectionRNG();	
	Enums::Direction GetOppositeDirection(Enums::Direction direction);
	Enums::Attitude getAttitude() const;
	void markAsDeleted();
	bool isMarkedAsDeletable() const;
	
	////////////////LUA///////////////////
	void setWanderRadius(unsigned short newWanderRadius);
	void setSpawnInfo(int _x_spawn_pos, int _y_spawn_pos, int _seconds_to_respawn, int _do_respawn);
	void setAttitude(Enums::Attitude attitude);
	std::string getLuaEditorSaveText() const;
	void addOnDieEventHandler(CallIndirection *eventHandler);
	bool hasOnDieEventHandler() const;

private:

	void Move(float deltaTime);
	void Move(float deltaTime, Enums::Direction direction);
	void Animate(float deltaTime);
	void processInput();

	unsigned short getWanderRadius() const;
	unsigned short getWanderRadiusSq() const;

	Enums::Attitude attitudeTowardsPlayer;
	Enums::Direction WanderDirection;
	unsigned int lastPathCalculated;
	
	bool m_updated = false;
	bool m_canWander = true;
	bool m_smoothOut = false;
	bool m_stopped = false;
	bool m_handleAnimation = false;
	bool markedAsDeleted;
	bool chasingPlayer;

	unsigned short wander_radius;

	float wander_thisframe, wander_lastframe;
	float respawn_thisframe, respawn_lastframe;
	int wander_every_seconds, wander_points_left;
	bool do_respawn;
	int x_spawn_pos, y_spawn_pos;
	int NPC_id;
	int seconds_to_respawn;
	bool wandering, moving, in_combat;
	
	std::vector<std::array<int, 2>> waypoints;
	//std::vector<sLootTable> lootTable;
	std::vector<CallIndirection*> onDieEventHandlers;
	//std::vector<Point> waypoints;
};