#pragma once

#include <vector>

#include "Character.h"


class Npc : public Character {

	friend class Editor;
public:

	Npc(const CharacterType& characterType, int _x_spawn_pos, int _y_spawn_pos, int _NPC_id, int _seconds_to_respawn, int _do_respawn);

	
	~Npc();
	void draw() override;
	void update(float deltaTime) override;
	
	int getWidth() const override;;
	int getHeight() const override;;
	unsigned short getNumActivityTextures(Enums::ActivityType activity) override;

	Enums::Direction GetDirectionRNG();
	
	void setSpawnInfo(int _x_spawn_pos, int _y_spawn_pos, int _seconds_to_respawn, int _do_respawn);
	void setAttitude(Enums::Attitude attitude);
	void markAsDeleted();
	bool isMarkedAsDeletable() const;
	std::string getLuaEditorSaveText() const;
	const CharacterType& getCharacterType();
	Enums::Attitude getAttitude() const;
	void chasePlayer(Character *player);
	void setWanderRadius(unsigned short newWanderRadius);
	Enums::Direction GetOppositeDirection(Enums::Direction direction);

private:
	unsigned short getWanderRadius() const;
	unsigned short getWanderRadiusSq() const;
	//Attitude::Attitude attitudeTowardsPlayer;
	//std::vector<sLootTable> lootTable;
	//std::vector<CallIndirection*> onDieEventHandlers;
	//std::vector<Point> waypoints;
	bool markedAsDeleted;
	bool chasingPlayer;
	uint32_t lastPathCalculated;
	Enums::Attitude attitudeTowardsPlayer;

	bool m_updated = false;

	const CharacterType& m_characterType;

	void Move(float deltaTime);
	void Move(float deltaTime, Enums::Direction direction);
	void Animate(float deltaTime);
	void processInput();

	bool m_canWander = true;
	bool m_smoothOut = false;
	bool m_stopped = false;
	bool m_handleAnimation = false;

	Enums::Direction WanderDirection;
	std::vector<std::array<int, 2>> waypoints;

	unsigned short wander_radius;

	float wander_thisframe, wander_lastframe;
	float respawn_thisframe, respawn_lastframe;
	int wander_every_seconds, wander_points_left;
	bool do_respawn;
	int x_spawn_pos, y_spawn_pos;
	int NPC_id;
	int seconds_to_respawn;
	bool wandering, moving, in_combat;
};