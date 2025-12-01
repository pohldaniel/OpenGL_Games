#pragma once

#include <memory>
#include <engine/Camera.h>
#include <engine/TileSet.h>

#include "DataStructs.h"
#include "Zone.h"

class SpriteEntity;
class Player;
class Enemy;

class Level : public Zone {

public:

	Level(const Camera& camera);
	~Level();
	
	void draw() override;
	void update(float dt) override;
	void loadZone(const std::string path, const std::string currentTileset) override;

	const std::vector<Rect>& getCollisionRects();
	const std::vector<std::unique_ptr<SpriteEntity>>& getSpriteEntities();
	float getMapHeight();
	static Player& GetPlayer();

private:

	size_t m_playerIndex;
	std::vector<std::unique_ptr<SpriteEntity>> m_spriteEntities;
	static std::unique_ptr<Player> s_player;
};