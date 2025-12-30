#pragma once

#include <memory>
#include <engine/Camera.h>
#include <engine/TileSet.h>

#include "DataStructs.h"
#include "Zone.h"

class EntityNew;
class Player;
class Enemy;

class Level : public Zone {

public:

	Level(const Camera& camera);
	~Level();
	
	void draw() override;
	void update(float dt) override;
	void loadZone(const std::string path, const std::string currentTileset) override;

	const std::vector<Rect>& getStaticRects();
	const std::vector<CollisionRect>& getDynamicRects();
	const std::vector<std::unique_ptr<EntityNew>>& getEntities();
	float getMapHeight();
	static Player& GetPlayer();

private:

	std::vector<std::unique_ptr<EntityNew>> m_entities;
	static std::unique_ptr<Player> s_player;
};