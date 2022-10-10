#pragma once

#include <vector>

#include "Character.h"


class Npc : public Character {

public:

	Npc(const CharacterType& characterType, int _x_spawn_pos, int _y_spawn_pos, int _NPC_id, int _seconds_to_respawn, int _do_respawn);

	
	~Npc();
	void draw() override;
	void update(float deltaTime) override;
	
	int getWidth() const override;;
	int getHeight() const override;;
	unsigned short getNumActivityTextures(Enums::ActivityType activity) override;

	Enums::Direction GetDirection();
	Enums::Direction GetDirectionRNG();
	void Wander(float deltaTime);

	void setSpawnInfo(int _x_spawn_pos, int _y_spawn_pos, int _seconds_to_respawn, int _do_respawn);
	void setAttitude(Enums::Attitude attitude);
	void markAsDeleted();
	bool isMarkedAsDeletable() const;
	std::string getLuaEditorSaveText() const;
	const CharacterType& getCharacterType();
private:

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
};