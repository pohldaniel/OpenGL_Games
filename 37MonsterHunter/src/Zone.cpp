#include <GL/glew.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <engine/TileSet.h>
#include <engine/Batchrenderer.h>

#include <Entities/Player.h>
#include <Entities/Character.h>
#include <States/StateMachine.h>
#include <States/MonsterHunter.h>
#include "Zone.h"
#include "Dialog.h"
#include "Application.h"
#include "Globals.h"

std::unordered_map<std::string, TileSetData> Zone::TileSets;
std::random_device Zone::RandomDevice;
std::mt19937 Zone::MersenTwist(RandomDevice());
std::uniform_int_distribution<int> Zone::Distribution(800, 2500);

Zone::Zone(const Camera& camera) :
	camera(camera),
	m_pointCount(0u),
	pointBatch(nullptr),
	pointBatchPtr(nullptr),
	m_spritesheet(0u),
	m_vao(0u),
	m_vbo(0u),
	m_screeBorder(0.0f),
	m_useCulling(true),
	m_drawScreenBorder(false),
	m_drawCenter(false),
	m_debugCollision(false),
	m_borderDirty(true),
	m_elapsedTime(0.0f),
	m_currentFrame(0),
	m_frameCount(4),
	m_waterOffset(242),
	m_coastOffset(246),	
	m_mapHeight(0.0f),
	m_alpha(1.0f),
	m_fade(m_alpha),
	m_biomeIndex(-1){
	initDebug();
	m_fade.setTransitionSpeed(2.353f);
	m_mainRenderTarget.create(Application::Width, Application::Height);
	m_mainRenderTarget.attachTexture2D(AttachmentTex::RGBA);
	m_mainRenderTarget.attachTexture2D(AttachmentTex::DEPTH24);
}

Zone::~Zone() {
	delete[] pointBatch;
	pointBatch = nullptr;
	pointBatchPtr = nullptr;

	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0u;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0u;
	}

	for (auto& layer : m_layers) {
		for (int i = 0; i < m_cols; i++) {			
			delete[] layer[i];
		}
		delete[] layer;
		layer = nullptr;
	}
}


void Zone::update(float dt) {
	culling();
	m_biomeIndex = -1;
	int index = CheckMonsterCollision(m_spriteEntities.back().get(), m_biomes);
	if (index >= 0) {
		if (!m_monsterEncounter.isActivated()) {
			m_monsterEncounter.start(Distribution(MersenTwist), false);
			m_monsterEncounter.setOnTimerEnd([&m_monsterEncounter = m_monsterEncounter, &m_biomes = m_biomes, &index = index, &m_biomeIndex = m_biomeIndex]() {
				m_biomeIndex = index;
			});
		}
	}else {	
		m_monsterEncounter.stop();
	}

	m_monsterEncounter.update(dt);
	m_elapsedTime += 6.0f * dt;
	m_currentFrame = static_cast<int>(std::floor(m_elapsedTime));
	if (m_currentFrame > m_frameCount - 1) {
		m_currentFrame = 0;
		m_elapsedTime -= static_cast<float>(m_frameCount);
	}

	for (AnimatedCell& animatedCell : m_visibleCellsAnimated) {
		animatedCell.currentFrame = m_currentFrame;
	}
	m_fade.update(dt);
	std::sort(m_visibleCellsMain.begin(), m_visibleCellsMain.end(), [&](const CellShadow& cell1, const CellShadow& cell2) {return cell1.centerY < cell2.centerY; });
}

void Zone::draw() {

	m_mainRenderTarget.bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Spritesheet::Bind(m_spritesheet);
	const std::vector<TextureRect>& rects = m_tileSet.getTextureRects();
	for (const AnimatedCell& animatedCell : m_visibleCellsAnimated) {
		const TextureRect& rect = rects[animatedCell.currentFrame + animatedCell.startFrame];
		Batchrenderer::Get().addQuadAA(Vector4f(animatedCell.posX - camera.getPositionX(), m_mapHeight - 64.0f - animatedCell.posY - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
	}

	for (const Cell& cell : m_visibleCellsBackground) {
		const TextureRect& rect = rects[cell.currentFrame];
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - camera.getPositionX(), m_mapHeight - 64.0f - cell.posY - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
	}

	for (const CellShadow& cell : m_visibleCellsMain) {
		if (cell.hasShadow) {
			const TextureRect& rect = rects[m_playerOffset + 16];
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX + 40.0f - camera.getPositionX(), m_mapHeight - cell.posY - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
		}

		const TextureRect& rect = rects[cell.currentFrame];
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - camera.getPositionX(), m_mapHeight - cell.posY - camera.getPositionY(), cell.width, cell.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
		if (cell.isNoticed) {
			const TextureRect& rect = rects[m_playerOffset + 17];
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX + 10.0f - camera.getPositionX(), m_mapHeight + 128.0f - cell.posY - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
		}
	}

	Batchrenderer::Get().drawBuffer();
	if (m_useCulling && m_drawScreenBorder)
		drawCullingRect();

	if (m_drawCenter) {
		updatePoints();
		glBindVertexArray(m_vao);
		GLsizeiptr size = (uint8_t*)pointBatchPtr - (uint8_t*)pointBatch;
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, pointBatch);

		auto shader = Globals::shaderManager.getAssetPointer("color");
		shader->use();
		shader->loadMatrix("u_projection", camera.getOrthographicMatrix());

		glDrawArrays(GL_POINTS, 0, m_pointCount);
		shader->unuse();

		pointBatchPtr = pointBatch;
		m_pointCount = 0;
	}

	if (m_debugCollision) {
		const TextureRect& textureRect = rects[m_playerOffset + 18];
		for (const Rect& rect : m_collisionRects) {
			Batchrenderer::Get().addQuadAA(Vector4f(rect.posX - camera.getPositionX(), m_mapHeight - (rect.posY + rect.height) - camera.getPositionY(), rect.width, rect.height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), textureRect.frame);
		}

		for (const Biome& biome : m_biomes) {
			Batchrenderer::Get().addQuadAA(Vector4f(biome.rect.posX - camera.getPositionX(), m_mapHeight - (biome.rect.posY + biome.rect.height) - camera.getPositionY(), biome.rect.width, biome.rect.height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 1.0f, 0.0f, 1.0f), textureRect.frame);
		}

		for (const Transition& transition : m_transitions) {
			Batchrenderer::Get().addQuadAA(Vector4f(transition.collisionRect.posX - camera.getPositionX(), m_mapHeight - (transition.collisionRect.posY + transition.collisionRect.height) - camera.getPositionY(), transition.collisionRect.width, transition.collisionRect.height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(0.0f, 1.0f, 0.0f, 1.0f), textureRect.frame);
		}

		const CellShadow& player = m_cellsMain[m_playerIndex];
		Batchrenderer::Get().addQuadAA(Vector4f(player.posX + 32.0f - camera.getPositionX(), m_mapHeight - (player.posY - 30.0f) - camera.getPositionY(), 128.0f - 64.0f, player.height - 60.0f), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 0.0f, 0.0f, 1.0f), textureRect.frame);

		Batchrenderer::Get().drawBuffer();
	}

	m_mainRenderTarget.unbind();

	m_mainRenderTarget.bindColorTexture(0u, 0u);
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::IDENTITY);	
	shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, m_alpha));
	Globals::shapeManager.get("quad").drawRaw();
	shader->unuse();
}

void Zone::loadZone(const std::string path, const std::string currentTileset, const std::string position) {

	for (auto& layer : m_layers) {
		for (int i = 0; i < m_cols; i++) {
			delete[] layer[i];
		}
		delete[] layer;
		layer = nullptr;
	}

	m_layers.clear();
	m_layers.shrink_to_fit();

	m_cellsAnimated.clear();
	m_cellsAnimated.shrink_to_fit();

	m_cellsBackground.clear();
	m_cellsBackground.shrink_to_fit();

	m_cellsMain.clear();
	m_cellsMain.shrink_to_fit();

	m_visibleCellsAnimated.clear();
	m_visibleCellsAnimated.shrink_to_fit();

	m_visibleCellsBackground.clear();
	m_visibleCellsBackground.shrink_to_fit();

	m_visibleCellsMain.clear();
	m_visibleCellsMain.shrink_to_fit();

	m_collisionRects.clear();
	m_collisionRects.shrink_to_fit();

	//m_monsterRects.clear();
	//m_monsterRects.shrink_to_fit();

	m_biomes.clear();
	m_biomes.shrink_to_fit();

	m_spriteEntities.clear();
	m_spriteEntities.shrink_to_fit();

	m_characters.clear();
	m_characters.shrink_to_fit();

	m_transitions.clear();
	m_transitions.shrink_to_fit();

	//https://stackoverflow.com/questions/24697063/how-to-make-pointer-reference-on-element-in-vector
	m_cellsMain.reserve(600);
	m_collisionRects.reserve(700);
	//m_monsterRects.reserve(308);
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
		if (layer->getName() == "Terrain") {
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
						m_cellsBackground.push_back({ static_cast<float>(x) * m_tileWidth, static_cast<float>(y) * m_tileHeight, m_tileWidth, m_tileHeight, m_layers.back()[y][x].first, static_cast<float>(x) * m_tileWidth + 0.5f * m_tileWidth, static_cast<float>(y) * m_tileHeight + 0.5f * m_tileHeight, false, false });
						m_layers.back()[y][x].second = static_cast<unsigned int>(m_cellsBackground.size() - 1);
					}
				}
			}
		}

		if (layer->getName() == "Objects") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				if (object.getName() == "top") {
					m_cellsMain.push_back(CellShadow(object.getPosition().x, object.getPosition().y, object.getAABB().width, object.getAABB().height, static_cast<int>(object.getTileID() - 1u), object.getPosition().x + 0.5f * object.getAABB().width, object.getPosition().y - 0.5f * object.getAABB().height + m_mapHeight, false, false, false, false));
				}else {
					m_cellsMain.push_back(CellShadow(object.getPosition().x, object.getPosition().y, object.getAABB().width, object.getAABB().height, static_cast<int>(object.getTileID() - 1u), object.getPosition().x + 0.5f * object.getAABB().width, object.getPosition().y - 0.5f * object.getAABB().height, false, false, false, false));
					m_collisionRects.push_back({ object.getPosition().x , object.getPosition().y - object.getAABB().height + 0.3f *object.getAABB().height,  object.getAABB().width, object.getAABB().height * 0.4f });
				}
			}
		}

		if (layer->getName() == "Monsters") {

			std::random_device rd;  // a seed source for the random number engine
			std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
			std::uniform_int_distribution<> distrib(-3, 3);

			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				if (object.getProperties()[0].getStringValue() == "sand") {
					m_cellsMain.push_back(CellShadow(object.getPosition().x, object.getPosition().y, 64.0f, 64.0f, static_cast<int>(object.getTileID() - 1u) , object.getPosition().x + 0.5f * object.getAABB().width, (object.getPosition().y - 0.5f * object.getAABB().height) - m_mapHeight, false, false, false, false));
				}else {
					m_cellsMain.push_back(CellShadow(object.getPosition().x, object.getPosition().y, 64.0f, 64.0f, static_cast<int>(object.getTileID() - 1u) , object.getPosition().x + 0.5f * object.getAABB().width, (object.getPosition().y - 0.5f * object.getAABB().height) - 40.0f, false, false, false, false));
				}
				//m_biomes.push_back({ object.getProperties()[0].getStringValue(), { object.getPosition().x, object.getPosition().y - 64.0f, 64.0f, 64.0f } });
				m_biomes.push_back(Biome());
				m_biomes.back().biome = object.getProperties()[0].getStringValue();
				m_biomes.back().rect = { object.getPosition().x, object.getPosition().y - 64.0f, 64.0f, 64.0f };

				std::stringstream ss(object.getProperties()[2].getStringValue());				
				while (ss.good()){
					std::string substr;
					getline(ss, substr, ',');
					m_biomes.back().monsters.push_back({ substr, static_cast<unsigned int>(object.getProperties()[1].getIntValue()) + distrib(gen) , 0.0f, 0.0f, 0.0f, false});
				}	
			}
		}

		if (layer->getName() == "Entities") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			std::string playerDirection;
			for (auto& object : objectLayer->getObjects()) {			
				if (object.getName() == "Player" && object.getProperties()[1].getStringValue() == position) {
					m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 128.0f, 128.0f, m_playerOffset, object.getPosition().x ,  object.getPosition().y - 64.0f, false, false, false, true));
					m_playerIndex = m_cellsMain.size() - 1;
					playerDirection = object.getProperties()[0].getStringValue();
				}

				if (object.getName() == "Character") {
					if (object.getProperties()[3].getStringValue() == "straw") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 128.0f, 128.0f, m_charachterOffsets["straw"], object.getPosition().x, object.getPosition().y - 64.0f, false, false, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back(), m_collisionRects.back()));
						m_spriteEntities.back()->setViewDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));
						m_spriteEntities.back()->setMovingSpeed(250.0f);

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
						m_characters.back().get().setCollisionRectIndex(m_collisionRects.size() - 1);
					}else if (object.getProperties()[3].getStringValue() == "blond") {



						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 128.0f, 128.0f, m_charachterOffsets["blond"], object.getPosition().x,  object.getPosition().y - 64.0f, false, false, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back(), m_collisionRects.back()));
						m_spriteEntities.back()->setViewDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));
						m_spriteEntities.back()->setMovingSpeed(250.0f);

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
						m_characters.back().get().setCollisionRectIndex(m_collisionRects.size() - 1);
					}else if (object.getProperties()[3].getStringValue() == "hat_girl") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 128.0f, 128.0f, m_charachterOffsets["hat_girl"], object.getPosition().x,  object.getPosition().y - 64.0f, false, false, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back(), m_collisionRects.back()));
						m_spriteEntities.back()->setViewDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));
						m_spriteEntities.back()->setMovingSpeed(250.0f);

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
						m_characters.back().get().setCollisionRectIndex(m_collisionRects.size() - 1);
					}else if (object.getProperties()[3].getStringValue() == "young_guy") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 128.0f, 128.0f, m_charachterOffsets["young_guy"], object.getPosition().x ,  object.getPosition().y - 64.0f, false, false, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back(), m_collisionRects.back()));
						m_spriteEntities.back()->setViewDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));
						m_spriteEntities.back()->setMovingSpeed(250.0f);

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
						m_characters.back().get().setCollisionRectIndex(m_collisionRects.size() - 1);
					}else if (object.getProperties()[3].getStringValue() == "young_girl") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 128.0f, 128.0f, m_charachterOffsets["young_girl"], object.getPosition().x, object.getPosition().y - 64.0f, false, false, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back(), m_collisionRects.back()));
						m_spriteEntities.back()->setViewDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));
						m_spriteEntities.back()->setMovingSpeed(250.0f);

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
						m_characters.back().get().setCollisionRectIndex(m_collisionRects.size() - 1);
					}else if (object.getProperties()[3].getStringValue() == "purple_girl") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 128.0f, 128.0f, m_charachterOffsets["purple_girl"], object.getPosition().x, object.getPosition().y - 64.0f, false, false, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back(), m_collisionRects.back()));
						m_spriteEntities.back()->setViewDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));
						m_spriteEntities.back()->setMovingSpeed(250.0f);

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
						m_characters.back().get().setCollisionRectIndex(m_collisionRects.size() - 1);
					}else if (object.getProperties()[3].getStringValue() == "water_boss") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 128.0f, 128.0f, m_charachterOffsets["water_boss"], object.getPosition().x, object.getPosition().y - 64.0f, false, false, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back(), m_collisionRects.back()));
						m_spriteEntities.back()->setViewDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));
						m_spriteEntities.back()->setMovingSpeed(250.0f);

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
						m_characters.back().get().setCollisionRectIndex(m_collisionRects.size() - 1);
					}else if (object.getProperties()[3].getStringValue() == "grass_boss") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 128.0f, 128.0f, m_charachterOffsets["grass_boss"], object.getPosition().x, object.getPosition().y - 64.0f, false, false, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back(), m_collisionRects.back()));
						m_spriteEntities.back()->setViewDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));
						m_spriteEntities.back()->setMovingSpeed(250.0f);

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
						m_characters.back().get().setCollisionRectIndex(m_collisionRects.size() - 1);
					}else if (object.getProperties()[3].getStringValue() == "fire_boss") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 128.0f, 128.0f, m_charachterOffsets["fire_boss"], object.getPosition().x, object.getPosition().y - 64.0f, false, false, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back(), m_collisionRects.back()));
						m_spriteEntities.back()->setViewDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));
						m_spriteEntities.back()->setMovingSpeed(250.0f);

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
						m_characters.back().get().setCollisionRectIndex(m_collisionRects.size() - 1);
					}else if (object.getProperties()[3].getStringValue() == "Nurse") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 128.0f, 128.0f, m_charachterOffsets["Nurse"], object.getPosition().x, object.getPosition().y - 64.0f, false, false, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back(), m_collisionRects.back()));
						m_spriteEntities.back()->setViewDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));
						m_spriteEntities.back()->setMovingSpeed(250.0f);

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
						m_characters.back().get().setCollisionRectIndex(m_collisionRects.size() - 1);
					}
				}
			}

			//IMPORTANT: the Collisions has to be loaded before the Entities
			m_spriteEntities.push_back(std::make_unique<Player>(m_cellsMain[m_playerIndex], const_cast<Camera&>(camera), getCollisionRects()));
			m_spriteEntities.back()->setViewDirection(SpriteEntity::GetDirection(playerDirection));
		}

		if (layer->getName() == "Water") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				for (float x = object.getPosition().x; x < object.getPosition().x + object.getAABB().width; x = x + 64.0f) {
					for (float y = object.getPosition().y; y < object.getPosition().y + object.getAABB().height; y = y + 64.0f) {
						m_cellsAnimated.push_back({ x, y, 0, m_waterOffset });
					}
				}
			}
		}
		
		if (layer->getName() == "Coast") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {

				for (auto& property : object.getProperties()) {

					if (property.getName() == "terrain" && property.getStringValue() == "grass") {
						if (object.getProperties()[0].getStringValue() == "topleft") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset });
						}

						if (object.getProperties()[0].getStringValue() == "left") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 4 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomleft") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 8 });
						}

						if (object.getProperties()[0].getStringValue() == "top") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 12 });
						}

						if (object.getProperties()[0].getStringValue() == "bottom") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 20 });
						}

						if (object.getProperties()[0].getStringValue() == "topright") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 24 });
						}

						if (object.getProperties()[0].getStringValue() == "right") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 28 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomright") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 32 });
						}
					}

					if (property.getName() == "terrain" && property.getStringValue() == "grass_i") {

						if (object.getProperties()[0].getStringValue() == "topleft") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 36 });
						}

						if (object.getProperties()[0].getStringValue() == "left") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 40 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomleft") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 44 });
						}

						if (object.getProperties()[0].getStringValue() == "top") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 48 });
						}

						if (object.getProperties()[0].getStringValue() == "bottom") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 56 });
						}

						if (object.getProperties()[0].getStringValue() == "topright") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 60 });
						}

						if (object.getProperties()[0].getStringValue() == "right") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 64 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomright") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 68 });
						}
					}

					if (property.getName() == "terrain" && property.getStringValue() == "sand_i") {

						if (object.getProperties()[0].getStringValue() == "topleft") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 72 });
						}

						if (object.getProperties()[0].getStringValue() == "left") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 76 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomleft") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 80 });
						}

						if (object.getProperties()[0].getStringValue() == "top") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 84 });
						}

						if (object.getProperties()[0].getStringValue() == "bottom") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 92 });
						}

						if (object.getProperties()[0].getStringValue() == "topright") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 96 });
						}

						if (object.getProperties()[0].getStringValue() == "right") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 100 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomright") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 104 });
						}
					}

					if (property.getName() == "terrain" && property.getStringValue() == "sand") {

						if (object.getProperties()[0].getStringValue() == "topleft") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 108 });
						}

						if (object.getProperties()[0].getStringValue() == "left") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 112 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomleft") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 116 });
						}

						if (object.getProperties()[0].getStringValue() == "top") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 120 });
						}

						if (object.getProperties()[0].getStringValue() == "bottom") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 128 });
						}

						if (object.getProperties()[0].getStringValue() == "topright") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 132 });
						}

						if (object.getProperties()[0].getStringValue() == "right") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 136 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomright") {
							m_cellsAnimated.push_back({ object.getPosition().x, object.getPosition().y, 0, m_coastOffset + 140 });
						}
					}
				}
			}
		}

		if (layer->getName() == "Collisions") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				m_collisionRects.push_back({ object.getPosition().x , object.getPosition().y,  object.getAABB().width, object.getAABB().height });
			}
		}

		if (layer->getName() == "Transition") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				m_transitions.push_back({ object.getProperties()[1].getStringValue(), object.getProperties()[0].getStringValue(),{ object.getPosition().x , object.getPosition().y,  object.getAABB().width, object.getAABB().height }});
			}
		}
	}

	for (Character& character : m_characters) {
		Trainer& trainer = DialogTree::Trainers[character.getCharacterId()];
		if (trainer.viewDirections.size() > 1) {
			character.setViewDirections(trainer.viewDirections);
		}
	}
}

Player& Zone::getPlayer() {
	return static_cast<Player&>(*m_spriteEntities.back().get());
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

	pointBatch = new Vector3f[MAX_POINTS];
	pointBatchPtr = pointBatch;
}

void Zone::updatePoints() {
	for (auto& cell : m_cellsMain) {
		*pointBatchPtr = { cell.centerX - camera.getPositionX(), m_mapHeight - cell.centerY - camera.getPositionY(), 0.0f };
		pointBatchPtr++;
		m_pointCount++;
	}
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

	m_visibleCellsAnimated.clear();
	for (AnimatedCell& animatedCell : m_cellsAnimated) {
		if (isRectOnScreen(animatedCell.posX, animatedCell.posY, 64.0f, 64.0f) || !m_useCulling) {
			m_visibleCellsAnimated.push_back(animatedCell);
		}
	}

	m_visibleCellsMain.clear();
	const std::vector<TextureRect>& rects = m_tileSet.getTextureRects();
	for (CellShadow& cell : m_cellsMain) {
		cell.visibile = false;
		const TextureRect& rect = rects[cell.currentFrame];
		if (isRectOnScreen(cell.posX, cell.posY - cell.height, cell.width, cell.height) || !m_useCulling) {
			cell.visibile = true;
			m_visibleCellsMain.push_back(cell);
		}
	}
}

void Zone::drawCullingRect() {
	const Vector3f& position = camera.getPosition();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-position[0], -position[1], 0.0f);

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(m_cullingVertices[0][0], m_cullingVertices[0][1], 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(m_cullingVertices[1][0], m_cullingVertices[1][1], 0.0f);
	glVertex3f(m_cullingVertices[2][0], m_cullingVertices[2][1], 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(m_cullingVertices[3][0], m_cullingVertices[3][1], 0.0f);

	glEnd();
	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Zone::setUseCulling(bool useCulling) {
	m_useCulling = useCulling;
}

void Zone::setScreeBorder(float screeBorder) {
	m_screeBorder = screeBorder;
}

void Zone::setDrawScreenBorder(bool drawScreenBorder) {
	m_drawScreenBorder = drawScreenBorder;
}

void Zone::setDrawCenter(bool drawCenter) {
	m_drawCenter = drawCenter;
}

void Zone::setDebugCollision(bool debugCollision) {
	m_debugCollision = debugCollision;
}

float Zone::getMapHeight() {
	return m_mapHeight;
}

void Zone::resize() {
	m_left = camera.getLeftOrthographic();
	m_right = camera.getRightOrthographic();
	m_bottom = camera.getBottomOrthographic();
	m_top = camera.getTopOrthographic();
	m_mainRenderTarget.resize(Application::Width, Application::Height);
}

const std::vector<Rect>& Zone::getCollisionRects() {
	return m_collisionRects;
}

const std::vector<std::unique_ptr<SpriteEntity>>& Zone::getSpriteEntities() {
	return m_spriteEntities;
}

const std::vector<std::reference_wrapper<Character>>& Zone::getCharacters() {
	return m_characters;
}

void Zone::setSpritesheet(const unsigned int& spritesheet) {
	m_spritesheet = spritesheet;
}

const std::vector<Transition>& Zone::getTransitions() {
	return m_transitions;
}

Fade& Zone::getFade(){
	return m_fade;
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
	m_playerOffset = m_tileSet.getTextureRects().size() - 19;
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

void Zone::setAlpha(float alpha) {
	m_alpha = alpha;
}

int Zone::CheckMonsterCollision(const SpriteEntity* entity, const std::vector<Biome>& biomes) {
	Rect entityRect = { entity->getCell().posX + 32.0f, entity->getCell().posY - (128.0f - 30.0f) , 128.0f - 64.0f, 128.0f - 60.0f };
	ViewDirection direction = entity->getViewDirection();
	int index = 0;
	for (const Biome& biome : biomes) {
		if (SpriteEntity::HasCollision(biome.rect.posX, biome.rect.posY, biome.rect.posX + biome.rect.width, biome.rect.posY + biome.rect.height, entityRect.posX, entityRect.posY, entityRect.posX + entityRect.width, entityRect.posY + entityRect.height) &&
			entity->getViewDirection() != ViewDirection::NONE) {
			return index;
		}	
		index++;
	}
	return -1;
}

const int Zone::getBiomeIndex() const {
	return m_biomeIndex;
}

const std::vector<Biome>& Zone::getBiomes() {
	return m_biomes;
}