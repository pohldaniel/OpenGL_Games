#include <GL/glew.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <engine/Batchrenderer.h>
#include <engine/Spritesheet.h>

#include "Overworld.h"
#include "Globals.h"
#include "Application.h"

Overworld::Overworld(const Camera& camera, const bool _initDebug) : Zone(camera, _initDebug),
	m_pointCount(0),
	m_pointBatchPtr(nullptr),
	m_pointBatch(nullptr),
	m_vao(0u),
	m_vbo(0u) {

	if (_initDebug)
		initDebug();

	m_mainRenderTarget.create(Application::Width, Application::Height);
	m_mainRenderTarget.attachTexture2D(AttachmentTex::RGBA);
	m_mainRenderTarget.attachTexture2D(AttachmentTex::DEPTH24);
}

Overworld::~Overworld() {

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
}

void Overworld::draw() {

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
		for (const CollisionRect& rect : m_collisionRects) {
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

void Overworld::loadZone(const std::string path, const std::string currentTileset) {

	m_cellsMain.reserve(600);
	m_collisionRects.reserve(700);
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
		if (layer->getName() == "main" || layer->getName() == "top") {
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
				m_collisionRects.push_back({ object.getPosition().x , object.getPosition().y - object.getAABB().height + 0.3f * object.getAABB().height, object.getAABB().width, object.getAABB().height * 0.4f, false });
			}
		}
	}
}

void Overworld::resize() {
	Zone::resize();
	m_mainRenderTarget.resize(Application::Width, Application::Height);
}

void Overworld::initDebug() {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * MAX_POINTS, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);

	m_pointBatch = new Vector3f[MAX_POINTS];
	m_pointBatchPtr = m_pointBatch;
}

void Overworld::updatePoints() {
	for (auto& cell : m_visibleCellsMain) {
		*m_pointBatchPtr = { cell.centerX - camera.getPositionX(), m_mapHeight - cell.centerY - camera.getPositionY(), 0.0f };
		m_pointBatchPtr++;
		m_pointCount++;
	}
}