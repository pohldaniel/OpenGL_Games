#pragma once

#include <vector>

#include "Callindirection.h"
#include "Character.h"

class Item;

struct LootTable{
	Item* item;
	double dropChance;
	LootTable(Item* item_, double dropChance_) {
		item = item_;
		dropChance = dropChance_;
	}
};

class Npc : public Character {

	friend class Editor;

public:

	Npc();
	~Npc();

	void draw() override;
	void update(float deltaTime) override;	
	void setCharacterType(std::string characterType) override;
	void Die() override;
	bool canBeDamaged() const override;
	void Damage(int amount, bool criticalHit) override;;
	void dropItems();
	void onDie();
	void Respawn();

	bool CheckMouseOver(int _x_pos, int _y_pos);
	void chasePlayer(Character *player);
	Enums::Direction GetDirectionRNG();	
	Enums::Direction GetDirection();
	Enums::Direction GetOppositeDirection(Enums::Direction direction);
	Enums::Attitude getAttitude() const;
	void markAsDeleted();
	bool isMarkedAsDeletable() const;
	bool canSeeInvisible() const;
	bool canSeeSneaking() const;

	////////////////LUA///////////////////
	void setWanderRadius(unsigned short newWanderRadius);
	void setSpawnInfo(int _x_spawn_pos, int _y_spawn_pos, int _seconds_to_respawn, int _do_respawn);
	void setAttitude(Enums::Attitude attitude);
	std::string getLuaEditorSaveText() const;
	std::string getLuaSaveText() const;
	void addOnDieEventHandler(CallIndirection *eventHandler);
	bool hasOnDieEventHandler() const;
	void addItemToLootTable(Item* item, double dropChance);
	void setCoinDrop(unsigned int minCoinDrop, unsigned int maxCoinDrop, double dropChance);

	std::vector<LootTable> lootTable;
	unsigned int minCoinDrop;
	unsigned int maxCoinDrop;
	double coinDropChance;

	bool hasDrawnDyingOnce = false;

	bool hasChoosenDyingDirection = false;
	float dyingTransparency = 1.0f;

	static void DrawActiveSpells();
	static std::vector<Npc*>& GetNPCs();
	static void ProcessInput();

private:

	void Move(float deltaTime);
	void Move(float deltaTime, Enums::Direction& direction);
	void Animate(float deltaTime);
	void processInput();

	unsigned short getWanderRadius() const;
	unsigned short getWanderRadiusSq() const;

	Enums::Direction getDirectionTowardsWaypointAt(int x_pos, int y_pos) const;
	Enums::Direction getDirectionTowards(int x_pos, int y_pos) const;

	Enums::Attitude attitudeTowardsPlayer;
	Enums::Direction WanderDirection, MovingDirection;
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
	float dyingStartFrame, reduceDyingTranspFrame;
	bool do_respawn;
	int x_spawn_pos, y_spawn_pos;
	int NPC_id;
	int seconds_to_respawn;
	bool wandering, moving, in_combat;
	std::string m_characterTypeStr;
	std::vector<std::array<int, 2>> waypoints;
	//std::vector<sLootTable> lootTable;
	std::vector<CallIndirection*> onDieEventHandlers;
	//std::vector<Point> waypoints;
};