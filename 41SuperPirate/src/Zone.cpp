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
	m_pointCount(0),
	m_pointBatchPtr(nullptr),
	m_pointBatch(nullptr),
	m_vao(0u),
	m_vbo(0u){

	if(_initDebug)
		initDebug();

	m_mainRenderTarget.create(Application::Width, Application::Height);
	m_mainRenderTarget.attachTexture2D(AttachmentTex::RGBA);
	m_mainRenderTarget.attachTexture2D(AttachmentTex::DEPTH24);
}

Zone::~Zone() {
	if (m_pointBatch) {
		delete[] m_pointBatch;
		m_pointBatch = nullptr;
		m_pointBatchPtr = nullptr;
	}

	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0u;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0u;
	}

	for (auto& layer : m_layers) {
		for (int i = 0; i < m_rows; i++) {
			delete[] layer[i];
		}
		delete[] layer;
		layer = nullptr;
	}
}

void Zone::draw() {

	m_mainRenderTarget.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Spritesheet::Bind(m_spritesheet);
	const std::vector<TextureRect>& rects = m_tileSet.getTextureRects();

	for (const Cell& cell : m_visibleCellsBackground) {
		const TextureRect& rect = rects[cell.tileID];
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - camera.getPositionX(), m_mapHeight - 64.0f - cell.posY - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
	}

	for (const Cell& cell : m_visibleCellsMain) {
		const TextureRect& rect = rects[cell.tileID];
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - camera.getPositionX(), m_mapHeight - cell.posY - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
	}

	Batchrenderer::Get().drawBuffer();
	if (m_drawCenter && m_vao) {
		updatePoints();
		glBindVertexArray(m_vao);
		GLsizeiptr size = (uint8_t*)m_pointBatchPtr - (uint8_t*)m_pointBatch;
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, m_pointBatch);

		auto shader = Globals::shaderManager.getAssetPointer("color");
		shader->use();
		shader->loadMatrix("u_projection", camera.getOrthographicMatrix());

		glDrawArrays(GL_POINTS, 0, m_pointCount);
		shader->unuse();

		m_pointBatchPtr = m_pointBatch;
		m_pointCount = 0;
	}

	if (m_debugCollision) {
		const TextureRect& textureRect = rects.back();
		for (const Rect& rect : m_collisionRects) {
			Batchrenderer::Get().addQuadAA(Vector4f(rect.posX - camera.getPositionX(), m_mapHeight - (rect.posY + rect.height) - camera.getPositionY(), rect.width, rect.height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), textureRect.frame);
		}
		Batchrenderer::Get().drawBuffer();
	}

	m_mainRenderTarget.unbind();

	m_mainRenderTarget.bindColorTexture(0u, 0u);
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Globals::shapeManager.get("quad").drawRaw();
	shader->unuse();
}

void Zone::update(float dt) {
	culling();
	std::sort(m_visibleCellsMain.begin(), m_visibleCellsMain.end(), [&](const Cell& cell1, const Cell& cell2) {return cell1.centerY < cell2.centerY; });
}

void Zone::loadTileSet(const std::vector<std::pair<std::string, float>>& pathSizes, const std::vector<std::pair<std::string, unsigned int>>& offsets) {
	m_tileSet.cleanup();
	TextureAtlasCreator::Get().init(2048u, 2048u);
	for (auto& pathSize : pathSizes) {
		if (pathSize.second > 0.0f) {
			m_tileSet.loadTileSetCpu(pathSize.first, false, pathSize.second, pathSize.second, true, false);
		}else {
			m_tileSet.loadTileCpu(pathSize.first, false, true, false);
		}
	}

	for (auto& offset : offsets) {
		m_charachterOffsets[offset.first] = offset.second;
	}

	m_tileSet.loadTileSetGpu();
	m_spritesheet = m_tileSet.getAtlas();
}

void Zone::loadZone(const std::string path, const std::string currentTileset) {

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

	m_cellsMain.reserve(600);
	m_collisionRects.reserve(700);
	m_currentTileset = currentTileset;

	loadTileSet(Zone::TileSets[m_currentTileset].pathSizes, Zone::TileSets[m_currentTileset].offsets);
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
		if (layer->getName() == "main" || layer->getName() == "top") {
			const tmx::TileLayer* tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());

			if (!tileLayer)
				continue;
			m_layers.resize(m_layers.size() + 1);
			m_layers.back() = new std::pair<int, unsigned int>*[mapSize.y];
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
					}
				}
			}
		}

		if (layer->getName() == "Objects") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				m_cellsMain.push_back({ static_cast<int>(object.getTileID() - 1u), object.getPosition().x, object.getPosition().y, object.getAABB().width, object.getAABB().height, object.getPosition().x + 0.5f * object.getAABB().width, object.getPosition().y, false, false });
				m_collisionRects.push_back({ object.getPosition().x , object.getPosition().y - object.getAABB().height + 0.3f *object.getAABB().height,  object.getAABB().width, object.getAABB().height * 0.4f });
			}
		}
	}
}

void Zone::loadTileSetData(const std::string& path) {
	std::ifstream file("res/tilesets.json", std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << "res/tilesets.json" << std::endl;
	}

	rapidjson::IStreamWrapper streamWrapper(file);
	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);

	for (rapidjson::Value::ConstMemberIterator tileset = doc.MemberBegin(); tileset != doc.MemberEnd(); ++tileset) {
		for (rapidjson::Value::ConstValueIterator tuples = tileset->value["paths"].GetArray().Begin(); tuples != tileset->value["paths"].GetArray().End(); ++tuples) {
			for (rapidjson::Value::ConstMemberIterator tuple = tuples->MemberBegin(); tuple != tuples->MemberEnd(); ++tuple) {
				TileSets[tileset->name.GetString()].pathSizes.push_back({ tuple->name.GetString(),tuple->value.GetFloat() });
			}
		}

		if (tileset->value.HasMember("offsets")) {
			for (rapidjson::Value::ConstValueIterator tuples = tileset->value["offsets"].GetArray().Begin(); tuples != tileset->value["offsets"].GetArray().End(); ++tuples) {

				for (rapidjson::Value::ConstMemberIterator tuple = tuples->MemberBegin(); tuple != tuples->MemberEnd(); ++tuple) {
					TileSets[tileset->name.GetString()].offsets.push_back({ tuple->name.GetString(), tuple->value.GetUint() });
				}
			}
		}
	}
}

void Zone::resize() {
	updateBorder();
	m_mainRenderTarget.resize(Application::Width, Application::Height);
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

void Zone::initDebug() {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(PointVertex) * MAX_POINTS, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PointVertex), (void*)offsetof(PointVertex, position));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);

	m_pointBatch = new Vector3f[MAX_POINTS];
	m_pointBatchPtr = m_pointBatch;
}

void Zone::updatePoints() {
	for (auto& cell : m_visibleCellsMain) {
		*m_pointBatchPtr = { cell.centerX - camera.getPositionX(), m_mapHeight - cell.centerY - camera.getPositionY(), 0.0f };
		m_pointBatchPtr++;
		m_pointCount++;
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