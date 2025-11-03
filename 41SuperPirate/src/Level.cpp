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

std::unordered_map<std::string, TileSetData> Level::TileSets;

Level::Level(const Camera& camera) : 
	camera(camera),
	m_useCulling(false),
	m_debugCollision(false),
	m_borderDirty(true),
	m_screeBorder(0.0f) {

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

	culling();
	std::sort(m_visibleCellsMain.begin(), m_visibleCellsMain.end(), [&](const Cell& cell1, const Cell& cell2) {return cell1.centerY < cell2.centerY; });
}

void Level::loadTileSet(const TileSetData& tileSetData) {
	m_tileSet.cleanup();
	TextureAtlasCreator::Get().init(2048u, 2048u);
	for (auto& pathSize : tileSetData.pathSizes) {
		if (pathSize.second > 0.0f) {
			m_tileSet.loadTileSetCpu(pathSize.first, false, pathSize.second, pathSize.second, true, false);
		}else {
			m_tileSet.loadTileCpu(pathSize.first, false, true, false);
		}
	}

	for (auto& offset : tileSetData.offsets) {
		m_charachterOffsets[offset.first] = offset.second;
	}

	for (auto& index : tileSetData.indices) {
		m_tileSet.removeTextureRect(index);
	}

	m_tileSet.loadTileSetGpu();
	m_spritesheet = m_tileSet.getAtlas();

	//Spritesheet::Safe("atlas_level", m_spritesheet);
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

	loadTileSet(Level::TileSets[m_currentTileset]);
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

void Level::loadTileSetData(const std::string& path) {
	std::ifstream file(path, std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << path << std::endl;
	}

	rapidjson::IStreamWrapper streamWrapper(file);
	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);

	for (rapidjson::Value::ConstMemberIterator tileset = doc.MemberBegin(); tileset != doc.MemberEnd(); ++tileset) {
		for (rapidjson::Value::ConstValueIterator tuples = tileset->value["paths"].GetArray().Begin(); tuples != tileset->value["paths"].GetArray().End(); ++tuples) {
			for (rapidjson::Value::ConstMemberIterator tuple = tuples->MemberBegin(); tuple != tuples->MemberEnd(); ++tuple) {
				Level::TileSets[tileset->name.GetString()].pathSizes.push_back({ tuple->name.GetString(),tuple->value.GetFloat() });
			}
		}

		if (tileset->value.HasMember("offsets")) {
			for (rapidjson::Value::ConstValueIterator tuples = tileset->value["offsets"].GetArray().Begin(); tuples != tileset->value["offsets"].GetArray().End(); ++tuples) {
				for (rapidjson::Value::ConstMemberIterator tuple = tuples->MemberBegin(); tuple != tuples->MemberEnd(); ++tuple) {
					Level::TileSets[tileset->name.GetString()].offsets.push_back({ tuple->name.GetString(), tuple->value.GetUint() });
				}
			}
		}

		if (tileset->value.HasMember("remove")) {
			for (rapidjson::Value::ConstValueIterator index = tileset->value["remove"].GetArray().Begin(); index != tileset->value["remove"].GetArray().End(); ++index) {
				Level::TileSets[tileset->name.GetString()].indices.push_back(index->GetUint());
			}
		}
	}
}

void Level::resize() {
	updateBorder();
}

void Level::updateBorder() {
	if (m_borderDirty) {
		m_left = camera.getLeftOrthographic();
		m_right = camera.getRightOrthographic();
		m_bottom = camera.getBottomOrthographic();
		m_top = camera.getTopOrthographic();
		m_borderDirty = false;
	}
}

int Level::posYToRow(float y, float cellHeight, int min, int max, int shift) {
	return Math::Clamp(static_cast<int>(std::roundf(y / cellHeight)) + shift, min, max);
}

int Level::posXToCol(float x, float cellWidth, int min, int max, int shift) {
	return Math::Clamp(static_cast<int>(std::roundf(x / cellWidth)) + shift, min, max);
}

bool Level::isRectOnScreen(float posX, float posY, float width, float height) {
	if (posX + width < m_cullingVertices[0][0] || posX > m_cullingVertices[2][0] || m_mapHeight - posY < m_cullingVertices[0][1] || m_mapHeight - (posY + height) > m_cullingVertices[2][1]) {
		return false;
	}
	return true;
}

void Level::culling() {
	updateBorder();
	const Vector3f& position = camera.getPosition();

	m_cullingVertices[0] = Vector2f(m_left + m_screeBorder + position[0], m_bottom + m_screeBorder + position[1]);
	m_cullingVertices[1] = Vector2f(m_left + m_screeBorder + position[0], m_top - m_screeBorder + position[1]);
	m_cullingVertices[2] = Vector2f(m_right - m_screeBorder + position[0], m_top - m_screeBorder + position[1]);
	m_cullingVertices[3] = Vector2f(m_right - m_screeBorder + position[0], m_bottom + m_screeBorder + position[1]);

	int colMin = m_useCulling ? posXToCol(m_cullingVertices[0][0], m_tileWidth, 0, m_cols, -1) : 0;
	int colMax = m_useCulling ? posXToCol(m_cullingVertices[2][0], m_tileWidth, 0, m_cols, 1) : m_cols;
	int rowMin = m_useCulling ? m_rows - posYToRow(m_cullingVertices[2][1], m_tileHeight, 0, m_rows, 1) : 0;
	int rowMax = m_useCulling ? m_rows - posYToRow(m_cullingVertices[0][1], m_tileHeight, 0, m_rows, -1) : m_rows;

	m_visibleCellsBackground.clear();
	for (int j = 0; j < m_layers.size(); j++) {
		for (int y = rowMin; y < rowMax; y++) {
			for (int x = colMin; x < colMax; x++) {
				if (m_layers[j][y][x].first != -1) {
					m_visibleCellsBackground.push_back(m_cellsBackground[m_layers[j][y][x].second]);
				}

			}
		}
	}

	m_visibleCellsMain.clear();
	const std::vector<TextureRect>& rects = m_tileSet.getTextureRects();
	for (Cell& cell : m_cellsMain) {
		cell.visibile = false;
		const TextureRect& rect = rects[cell.tileID];
		if (isRectOnScreen(cell.posX, cell.posY - cell.height, cell.width, cell.height) || !m_useCulling) {
			cell.visibile = true;
			m_visibleCellsMain.push_back(cell);
		}
	}
}

void Level::setUseCulling(bool useCulling) {
	m_useCulling = useCulling;
}

void Level::setDebugCollision(bool debugCollision) {
	m_debugCollision = debugCollision;
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