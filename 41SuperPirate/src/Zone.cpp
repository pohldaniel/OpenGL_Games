#include <GL/glew.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <engine/Batchrenderer.h>
#include <engine/Spritesheet.h>

#include "Zone.h"
#include "Globals.h"
#include "Application.h"

std::unordered_map<std::string, TileSetData> Zone::TileSets;

Zone::Zone(const Camera& camera, const bool _initDebug) :
	camera(camera),	
	m_useCulling(true),
	m_debugCollision(false),
	m_drawCenter(false),
	m_borderDirty(true),
	m_screeBorder(0.0f),
	m_bottom(FLT_MAX),
	m_top(FLT_MIN),
	m_left(FLT_MAX),
	m_right(FLT_MIN),	
	m_tileWidth(0.0f),
	m_tileHeight(0.0f),
	m_rows(0),
	m_cols(0),
	m_mapHeight(0.0f),
	m_spritesheet(0u)
	{

}

Zone::~Zone() {

	for (auto& layer : m_layers) {
		for (int i = 0; i < m_rows; i++) {
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
}

void Zone::update(float dt) {
	culling();
	std::sort(m_visibleCellsMain.begin(), m_visibleCellsMain.end(), [&](const Cell& cell1, const Cell& cell2) {return cell1.centerY < cell2.centerY; });
}

void Zone::loadTileSet(const TileSetData& tileSetData) {
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
		m_animationOffsets[offset.first] = static_cast<int>(offset.second);
	}

	for (auto& index : tileSetData.indices) {
		m_tileSet.removeTextureRect(index);
	}

	m_tileSet.loadTileSetGpu();
	m_spritesheet = m_tileSet.getAtlas();
}

void Zone::resize() {
	updateBorder();
}

void Zone::updateBorder() {
	if (m_borderDirty) {
		m_left = camera.getLeftOrthographic();
		m_right = camera.getRightOrthographic();
		m_bottom = camera.getBottomOrthographic();
		m_top = camera.getTopOrthographic();
		m_borderDirty = false;
	}
}

int Zone::posYToRow(float y, float cellHeight, int min, int max, int shift) {
	return Math::Clamp(static_cast<int>(std::roundf(y / cellHeight)) + shift, min, max);
}

int Zone::posXToCol(float x, float cellWidth, int min, int max, int shift) {
	return Math::Clamp(static_cast<int>(std::roundf(x / cellWidth)) + shift, min, max);
}

bool Zone::isRectOnScreen(float posX, float posY, float width, float height) {
	if (posX + width < m_cullingVertices[0][0] || posX > m_cullingVertices[2][0] || m_mapHeight - posY < m_cullingVertices[0][1] || m_mapHeight - (posY + height) > m_cullingVertices[2][1]) {
		return false;
	}
	return true;
}

void Zone::culling() {
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

void Zone::setDrawCenter(bool drawCenter) {
	m_drawCenter = drawCenter;
}

void Zone::setUseCulling(bool useCulling) {
	m_useCulling = useCulling;
}

void Zone::setDebugCollision(bool debugCollision) {
	m_debugCollision = debugCollision;
}

void Zone::LoadTileSetData(const std::string& path) {
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
				Zone::TileSets[tileset->name.GetString()].pathSizes.push_back({ tuple->name.GetString(),tuple->value.GetFloat() });
			}
		}

		if (tileset->value.HasMember("offsets")) {
			for (rapidjson::Value::ConstValueIterator tuples = tileset->value["offsets"].GetArray().Begin(); tuples != tileset->value["offsets"].GetArray().End(); ++tuples) {

				for (rapidjson::Value::ConstMemberIterator tuple = tuples->MemberBegin(); tuple != tuples->MemberEnd(); ++tuple) {
					Zone::TileSets[tileset->name.GetString()].offsets.push_back({ tuple->name.GetString(), tuple->value.GetUint() });
				}
			}
		}

		if (tileset->value.HasMember("remove")) {
			for (rapidjson::Value::ConstValueIterator index = tileset->value["remove"].GetArray().Begin(); index != tileset->value["remove"].GetArray().End(); ++index) {
				Zone::TileSets[tileset->name.GetString()].indices.push_back(index->GetUint());
			}
		}

		if (tileset->value.HasMember("animation_offset")) {
			for (rapidjson::Value::ConstValueIterator tuples = tileset->value["animation_offset"].GetArray().Begin(); tuples != tileset->value["animation_offset"].GetArray().End(); ++tuples) {

				for (rapidjson::Value::ConstMemberIterator tuple = tuples->MemberBegin(); tuple != tuples->MemberEnd(); ++tuple) {
					TileSets[tileset->name.GetString()].offsets.push_back({ tuple->name.GetString(), tuple->value.GetUint() });
				}
			}
		}
	}
}