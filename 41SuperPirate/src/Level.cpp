#include <GL/glew.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <engine/Batchrenderer.h>
#include <engine/Spritesheet.h>
#include <Entities/SpriteEntity.h>
#include <Entities/Player.h>

#include "Level.h"

Level::Level(const Camera& camera) : Zone(camera), 
	m_playerIndex(SIZE_MAX) {

}

Level::~Level() {

}

void Level::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Spritesheet::Bind(m_spritesheet);
	const std::vector<TextureRect>& rects = m_tileSet.getTextureRects();

	if (!m_debugCollision) {
		for (const Cell& cell : m_visibleCellsBackground) {
			const TextureRect& rect = rects[cell.tileID];
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - camera.getPositionX(), m_mapHeight - 64.0f - cell.posY - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
		}

		for (const Cell& cell : m_visibleCellsMain) {
			const TextureRect& rect = rects[cell.tileID];
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - camera.getPositionX(), m_mapHeight - cell.posY - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
		}
		Batchrenderer::Get().drawBuffer();
	}

	if (m_debugCollision) {
		const TextureRect& textureRect = rects.back();
		for (const Rect& rect : m_collisionRects) {
			Batchrenderer::Get().addQuadAA(Vector4f(rect.posX - camera.getPositionX(), m_mapHeight - (rect.posY + rect.height) - camera.getPositionY(), rect.width, rect.height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), textureRect.frame);
		}

		const Cell& player = m_cellsMain[m_playerIndex];
		Batchrenderer::Get().addQuadAA(Vector4f(player.posX  - camera.getPositionX(), m_mapHeight - (player.posY + camera.getPositionY()), 48.0f, 56.0f), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 0.0f, 0.0f, 1.0f), textureRect.frame);
		Batchrenderer::Get().drawBuffer();
	}
}

void Level::update(float dt) {
	for (auto&& spriteEntity : getSpriteEntities()) {		
		spriteEntity->update(dt);
	}

	Zone::update(dt);
}

void Level::loadZone(const std::string path, const std::string currentTileset) {

	for (auto& layer : m_layers) {
		for (int i = 0; i < m_cols; i++) {
			delete[] layer[i];
		}
		delete[] layer;
		layer = nullptr;
	}

	m_layers.clear();
	m_layers.shrink_to_fit();

	m_cellsBackground.clear();
	m_cellsBackground.shrink_to_fit();

	m_cellsMain.clear();
	m_cellsMain.shrink_to_fit();

	m_cellsMain.reserve(6000);
	m_collisionRects.reserve(7000);
	m_currentTileset = currentTileset;

	loadTileSet(Zone::TileSets[m_currentTileset]);
	tmx::Map map;
	map.load(path);

	const tmx::Vector2u& mapSize = map.getTileCount();
	m_mapHeight = static_cast<float>(mapSize.y * map.getTileSize().y);
	m_tileHeight = static_cast<float>(map.getTileSize().y);
	m_tileWidth = static_cast<float>(map.getTileSize().x);
	m_cols = mapSize.x;
	m_rows = mapSize.y;

	const std::vector<std::unique_ptr<tmx::Layer>>& layers = map.getLayers();
	for (auto& layer : layers) {
		if (layer->getName() == "Terrain" || layer->getName() == "Platforms" || layer->getName() == "FG" || layer->getName() == "BG") {
			const tmx::TileLayer* tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());
			if (!tileLayer)
				continue;
			m_layers.resize(m_layers.size() + 1);
			m_layers.back() = new std::pair<int, unsigned int>* [mapSize.y];
			for (int y = 0; y < mapSize.y; ++y)
				m_layers.back()[y] = new std::pair<int, unsigned int>[mapSize.x];

			const auto& tileIDs = tileLayer->getTiles();
			for (auto y = 0u; y < mapSize.y; ++y) {
				for (auto x = 0u; x < mapSize.x; ++x) {
					auto idx = y * mapSize.x + x;
					m_layers.back()[y][x].first = (tileIDs[idx].ID - 1);
					if (m_layers.back()[y][x].first != -1) {						
						m_cellsBackground.push_back({ m_layers.back()[y][x].first, static_cast<float>(x) * m_tileWidth, static_cast<float>(y) * m_tileHeight, m_tileWidth, m_tileHeight, static_cast<float>(x) * m_tileWidth + 0.5f * m_tileWidth, static_cast<float>(y) * m_tileHeight + 0.5f * m_tileHeight, false, false });
						m_layers.back()[y][x].second = static_cast<unsigned int>(m_cellsBackground.size() - 1);

						if (layer->getName() == "Terrain") {
							m_collisionRects.push_back({ static_cast<float>(x) * m_tileWidth, static_cast<float>(y) * m_tileHeight,  m_tileWidth, m_tileHeight });
						}
					}
				}
			}
		}

		if (layer->getName() == "Objects") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				if (object.getName() == "player") {
					m_cellsMain.push_back({ static_cast<int>(object.getTileID() - 1u), object.getPosition().x + 48.0f, object.getPosition().y + 28.0f, object.getAABB().width, object.getAABB().height, object.getPosition().x + 0.5f * object.getAABB().width, object.getPosition().y, false, false });
					m_playerIndex = m_cellsMain.size() - 1;
				}
			}

			m_spriteEntities.push_back(std::make_unique<Player>(m_cellsMain[m_playerIndex], const_cast<Camera&>(camera), getCollisionRects()));
			m_spriteEntities.back()->setViewDirection(SpriteEntity::GetDirection("none"));
			m_spriteEntities.back()->setMovingSpeed(200.0f);
			getPlayer().setMapHeight(m_mapHeight);
		}		
	}
}

Player& Level::getPlayer() {
	return static_cast<Player&>(*m_spriteEntities.back().get());
}

const std::vector<Rect>& Level::getCollisionRects() {
	return m_collisionRects;
}

const std::vector<std::unique_ptr<SpriteEntity>>& Level::getSpriteEntities() {
	return m_spriteEntities;
}

float Level::getMapHeight() {
	return m_mapHeight;
}