#include <GL/glew.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <engine/Batchrenderer.h>
#include <engine/Spritesheet.h>
#include <Entities/SpriteEntity.h>
#include <Entities/Player.h>
#include <Entities/Enemy.h>

#include "Level.h"

std::unique_ptr<Player> Level::s_player = nullptr;

Level::Level(const Camera& camera) : Zone(camera), 
	m_playerIndex(SIZE_MAX) {
}

Level::~Level() {
	Player* player = s_player.release();
	delete player;
	Level::s_player = nullptr;
}

void Level::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Spritesheet::Bind(m_spritesheet);
	const std::vector<TextureRect>& rects = m_tileSet.getTextureRects();

	if (!m_debugCollision) {
		for (const Cell& cell : m_visibleCellsBackground) {
			const TextureRect& rect = rects[cell.tileID];
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX + cell.offsetX - camera.getPositionX(), m_mapHeight - 64.0f - (cell.posY + cell.offsetY) - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
		}

		for (const Cell& cell : m_visibleCellsMain) {
			const TextureRect& rect = rects[cell.tileID];
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX + cell.offsetX - camera.getPositionX(), m_mapHeight - (cell.posY + cell.offsetY) - camera.getPositionY(), rect.width, rect.height), Vector4f(cell.flipped ? rect.textureOffsetX + rect.textureWidth : rect.textureOffsetX, rect.textureOffsetY, cell.flipped ? -rect.textureWidth : rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
		}
		Batchrenderer::Get().drawBuffer();
	}

	if (m_debugCollision) {
		const TextureRect& textureRect = rects.back();
		for (const Rect& rect : m_collisionRects) {
			Batchrenderer::Get().addQuadAA(Vector4f(rect.posX - camera.getPositionX(), m_mapHeight - (rect.posY + rect.height) - camera.getPositionY(), rect.width, rect.height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), rect.hasCollision ? Vector4f(1.0f, 1.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f), textureRect.frame);
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
	s_player->update(dt);
	Zone::update(dt);
}

void Level::loadZone(const std::string path, const std::string currentTileset) {

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
			m_layers.back() = new std::pair<int, unsigned int>* [m_rows];
			for (int y = 0; y < m_rows; ++y)
				m_layers.back()[y] = new std::pair<int, unsigned int>[m_cols];

			const auto& tileIDs = tileLayer->getTiles();
			for (auto y = 0u; y < mapSize.y; ++y) {
				for (auto x = 0u; x < mapSize.x; ++x) {
					auto idx = y * mapSize.x + x;
					m_layers.back()[y][x].first = (tileIDs[idx].ID - 1);
					if (m_layers.back()[y][x].first != -1) {						
						m_cellsBackground.push_back({ m_layers.back()[y][x].first, static_cast<float>(x) * m_tileWidth, static_cast<float>(y) * m_tileHeight, m_tileWidth, m_tileHeight, static_cast<float>(x) * m_tileWidth + 0.5f * m_tileWidth, static_cast<float>(y) * m_tileHeight + 0.5f * m_tileHeight, 0.0f, 0.0f, false, false });
						m_layers.back()[y][x].second = static_cast<unsigned int>(m_cellsBackground.size() - 1);

						if (layer->getName() == "Terrain") {
							m_collisionRects.push_back({ static_cast<float>(x) * m_tileWidth, static_cast<float>(y) * m_tileHeight,  m_tileWidth, m_tileHeight, false });
						}
					}
				}
			}
		}

		if (layer->getName() == "Enemies") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				if (object.getName() == "shell") {
					//m_cellsMain.push_back({ static_cast<int>(object.getTileID() - 1u), object.getPosition().x, object.getPosition().y, object.getAABB().width, object.getAABB().height, object.getPosition().x + 0.5f * object.getAABB().width, object.getPosition().y, false, false });					
					m_cellsMain.push_back({ m_animationOffsets["shell_attack"], object.getPosition().x, object.getPosition().y, object.getAABB().width, object.getAABB().height, object.getPosition().x + 0.5f * object.getAABB().width, object.getPosition().y, 0.0f, 0.0f, false, false });
					m_collisionRects.push_back({ object.getPosition().x, object.getPosition().y - object.getAABB().height, object.getAABB().width, object.getAABB().height });
					m_spriteEntities.push_back(std::make_unique<Enemy>(m_cellsMain.back(), 0.0f, 6));
					m_spriteEntities.back()->setMovingSpeed(250.0f);
				}
			}
		}

		if (layer->getName() == "Objects") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				if (object.getName() == "player") {
					//m_cellsMain.push_back({ static_cast<int>(object.getTileID() - 1u), object.getPosition().x + 48.0f, object.getPosition().y + 28.0f, object.getAABB().width, object.getAABB().height, object.getPosition().x + 0.5f * object.getAABB().width, object.getPosition().y, false, false });
					m_cellsMain.push_back({ m_animationOffsets["player_idle"], object.getPosition().x + 48.0f, object.getPosition().y + 28.0f, object.getAABB().width, object.getAABB().height, object.getPosition().x + 0.5f * object.getAABB().width, object.getPosition().y, -33.0f, + 18.0f, false, false });
					m_playerIndex = m_cellsMain.size() - 1;
				}				
			}
			
			m_spriteEntities.push_back(std::make_unique<Player>(m_cellsMain[m_playerIndex], const_cast<Camera&>(camera), getCollisionRects(), 0.0f, 5));

			s_player = std::make_unique<Player>(m_cellsMain[m_playerIndex], const_cast<Camera&>(camera), getCollisionRects(), 0.0f, 5);
			s_player->setMovingSpeed(200.0f);
			s_player->setMapHeight(m_mapHeight);
		}		
	}
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

Player& Level::GetPlayer() {
	return static_cast<Player&>(*s_player.get());
}