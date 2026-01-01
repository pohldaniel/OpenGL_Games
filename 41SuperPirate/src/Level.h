#pragma once

#include <memory>
#include <engine/Camera.h>
#include <engine/TileSet.h>
#include <Entities/Entity2D.h>

#include "DataStructs.h"
#include "Zone.h"

class Player;

class Level : public Zone {

public:

	Level(const Camera& camera);
	~Level();
	
	void draw() override;
	void update(float dt) override;
	void loadZone(const std::string path, const std::string currentTileset) override;

	const std::vector<Rect>& getStaticRects();
	const std::vector<DynamicRect>& getDynamicRects();
	const std::vector<std::unique_ptr<Entity2D>>& getEntities();
	float getMapHeight();
	static Player& GetPlayer();

private:

	std::vector<std::unique_ptr<Entity2D>> m_entities;
	std::vector<DynamicRect> m_dynamicRects;
	static std::unique_ptr<Player> s_player;
};