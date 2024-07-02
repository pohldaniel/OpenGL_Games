#include <GL/glew.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <engine/TileSet.h>
#include <engine/Batchrenderer.h>

#include <Entities/Player.h>
#include <Entities/Character.h>
#include <States/StateMachine.h>
#include "Zone.h"
#include "Globals.h"

Zone::Zone(const Camera& camera) : 
	camera(camera), 
	m_pointCount(0u), 
	pointBatch(nullptr), 
	pointBatchPtr(nullptr), 
	m_vao(0u), 
	m_vbo(0u), 
	m_screeBorder(0.0f), 
	m_useCulling(true), 
	m_drawScreenBorder(false),
	m_drawCenter(false),
	m_debugCollision(false),
	m_borderDirty(true),
	elapsedTime(0.0f), 
	currentFrame(0), 
	frameCount(4){
	
	
	loadZone("res/tmx/data/maps/world.tmx", 588);
	initDebug();
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
		layer = NULL;
	}
}


void Zone::update(float dt) {
	culling();

	elapsedTime += 6.0f * dt;
	currentFrame = static_cast <int>(std::floor(elapsedTime));
	if (currentFrame > frameCount - 1) {
		currentFrame = 0;
		elapsedTime -= static_cast <float>(frameCount);
	}

	for (AnimatedCell& animatedCell : m_visibleCellsAni) {
		animatedCell.currentFrame = currentFrame;
	}

	std::sort(m_visibleCellsMain.begin(), m_visibleCellsMain.end(), [&](const CellShadow& cell1, const CellShadow& cell2) {return cell1.centerY < cell2.centerY; });
}

void Zone::draw() {
	const std::vector<TextureRect>& rects = TileSetManager::Get().getTileSet("world").getTextureRects();
	
	for (auto& animatedCell : m_visibleCellsAni) {
		const TextureRect& rect = rects[animatedCell.currentFrame + animatedCell.startFrame];
		Batchrenderer::Get().addQuadAA(Vector4f(animatedCell.posX - camera.getPositionX(), m_mapHeight - 64.0f - animatedCell.posY - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
	}

	for (auto& cell : m_visibleCellsBG) {
		const TextureRect& rect = rects[cell.currentFrame];
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - camera.getPositionX(), m_mapHeight - 64.0f - cell.posY - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
	}

	for (const CellShadow& cell : m_visibleCellsMain) {
		if (cell.hasShadow) {
			const TextureRect& rect = rects[534];
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX + 40.0f - camera.getPositionX(), m_mapHeight - cell.posY - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
		}

		const TextureRect& rect = rects[cell.currentFrame];
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - camera.getPositionX(), m_mapHeight - cell.posY - camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
		
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
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		const TextureRect& textureRect = rects[535];
		for (const Rect& rect : m_collisionRects) {
			Batchrenderer::Get().addQuadAA(Vector4f(rect.posX - camera.getPositionX(), m_mapHeight - (rect.posY + rect.height) - camera.getPositionY(), rect.width, rect.height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), textureRect.frame);
		}

		const CellShadow& player = m_cellsMain[m_playerIndex];
		Batchrenderer::Get().addQuadAA(Vector4f(player.posX + 32.0f - camera.getPositionX(), m_mapHeight - (player.posY - 30.0f) - camera.getPositionY(), 128.0f - 64.0f, player.height - 60.0f), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 0.0f, 0.0f, 1.0f), textureRect.frame);

		Batchrenderer::Get().drawBuffer();
		//glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
	}
}

void Zone::loadZone(const std::string& path, size_t capacity) {
	//https://stackoverflow.com/questions/24697063/how-to-make-pointer-reference-on-element-in-vector
	m_cellsMain.reserve(capacity);
	
	tmx::Map map;
	map.load(path);

	const tmx::Vector2u& mapSize = map.getTileCount();
	m_mapHeight = static_cast<float>(mapSize.y * map.getTileSize().y);
	m_tileHeight = static_cast<float>(map.getTileSize().y);
	m_tileWidth = static_cast<float>(map.getTileSize().x);
	m_cols = mapSize.y;
	m_rows = mapSize.x;

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
					m_layers.back()[y][x].first = tileIDs[idx].ID - 1;
					if (m_layers.back()[y][x].first != -1) {
						m_cellsBackground.push_back({ static_cast<float>(x) * m_tileWidth, static_cast<float>(y) * m_tileHeight, m_layers.back()[y][x].first, static_cast<float>(x) * m_tileWidth + 0.5f * m_tileWidth, static_cast<float>(y) * m_tileHeight + 0.5f * m_tileHeight, m_tileHeight, false });
						m_layers.back()[y][x].second = static_cast<unsigned int>(m_cellsBackground.size() - 1);
					}
				}
			}
		}

		if (layer->getName() == "Objects") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				if (object.getName() == "top") {
					m_cellsMain.push_back(CellShadow(object.getPosition().x, object.getPosition().y, static_cast<int>(object.getTileID() - 1u), object.getPosition().x + 0.5f * object.getAABB().width, object.getPosition().y - 0.5f * object.getAABB().height + m_mapHeight, object.getAABB().height, false, false));
				}else {
					m_cellsMain.push_back(CellShadow(object.getPosition().x, object.getPosition().y, static_cast<int>(object.getTileID() - 1u), object.getPosition().x + 0.5f * object.getAABB().width, object.getPosition().y - 0.5f * object.getAABB().height, object.getAABB().height, false, false));
					m_collisionRects.push_back({ object.getPosition().x , object.getPosition().y - object.getAABB().height + 0.3f *object.getAABB().height,  object.getAABB().width, object.getAABB().height * 0.4f });
				}
			}
		}

		if (layer->getName() == "Monsters") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				if (object.getProperties()[0].getStringValue() == "sand") {
					m_cellsMain.push_back(CellShadow(object.getPosition().x, object.getPosition().y, static_cast<int>(object.getTileID() - 1u) , object.getPosition().x + 0.5f * object.getAABB().width, (object.getPosition().y - 0.5f * object.getAABB().height) - m_mapHeight, object.getAABB().height, false, false));
				}else {
					m_cellsMain.push_back(CellShadow(object.getPosition().x, object.getPosition().y, static_cast<int>(object.getTileID() - 1u) , object.getPosition().x + 0.5f * object.getAABB().width, (object.getPosition().y - 0.5f * object.getAABB().height) - 40.0f, object.getAABB().height, false, false));
				}
			}
		}

		if (layer->getName() == "Entities") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				if (object.getName() == "Player" && object.getProperties()[1].getStringValue() == "house") {
					m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 536, object.getPosition().x ,  object.getPosition().y - 64.0f, 128.0f, false, true));					
					m_playerIndex = m_cellsMain.size() - 1;
				}

				if (object.getName() == "Character") {
					if (object.getProperties()[3].getStringValue() == "straw") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 632, object.getPosition().x, object.getPosition().y - 64.0f, 128.0f, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back()));
						m_spriteEntities.back()->setDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
					}else if (object.getProperties()[3].getStringValue() == "blond") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 552, object.getPosition().x,  object.getPosition().y - 64.0f, 128.0f, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back()));
						m_spriteEntities.back()->setDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
					}else if (object.getProperties()[3].getStringValue() == "hat_girl") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 600, object.getPosition().x,  object.getPosition().y - 64.0f, 128.0f, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back()));
						m_spriteEntities.back()->setDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
					}else if (object.getProperties()[3].getStringValue() == "young_guy") {
						m_cellsMain.push_back(CellShadow(object.getPosition().x - 64.0f, object.getPosition().y, 680, object.getPosition().x ,  object.getPosition().y - 64.0f, 128.0f, false, true));
						m_collisionRects.push_back({ (object.getPosition().x - 64.0f) + 32.0f, (object.getPosition().y) - (128.0f - 30.0f), 128.0f - 64.0f, 128.0f - 60.0f });
						m_spriteEntities.push_back(std::make_unique<Character>(m_cellsMain.back()));
						m_spriteEntities.back()->setDirection(SpriteEntity::GetDirection(object.getProperties()[2].getStringValue()));

						m_characters.push_back(reinterpret_cast<Character&>(*m_spriteEntities.back().get()));
						m_characters.back().get().setCharacterId(object.getProperties()[1].getStringValue());
						m_characters.back().get().setRadius(std::stof(object.getProperties()[4].getStringValue()));
					}
				}
			}

			//IMPORTANT: the Collisions has to be loaded before the Entities
			m_spriteEntities.push_back(std::make_unique<Player>(m_cellsMain[m_playerIndex], const_cast<Camera&>(camera), getCollisionRects()));
		}

		if (layer->getName() == "Water") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				for (float x = object.getPosition().x; x < object.getPosition().x + object.getAABB().width; x = x + 64.0f) {
					for (float y = object.getPosition().y; y < object.getPosition().y + object.getAABB().height; y = y + 64.0f) {
						m_animatedCells.push_back({ x, y, 0, 242 });
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
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 246 });
						}

						if (object.getProperties()[0].getStringValue() == "left") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 250 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 254 });
						}

						if (object.getProperties()[0].getStringValue() == "top") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 258 });
						}

						if (object.getProperties()[0].getStringValue() == "bottom") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 266 });
						}

						if (object.getProperties()[0].getStringValue() == "topright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 270 });
						}

						if (object.getProperties()[0].getStringValue() == "right") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 274 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 278 });
						}
					}

					if (property.getName() == "terrain" && property.getStringValue() == "grass_i") {

						if (object.getProperties()[0].getStringValue() == "topleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 282 });
						}

						if (object.getProperties()[0].getStringValue() == "left") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 286 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 290 });
						}

						if (object.getProperties()[0].getStringValue() == "top") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 294 });
						}

						if (object.getProperties()[0].getStringValue() == "bottom") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 301 });
						}

						if (object.getProperties()[0].getStringValue() == "topright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 306 });
						}

						if (object.getProperties()[0].getStringValue() == "right") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 310 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 314 });
						}
					}

					if (property.getName() == "terrain" && property.getStringValue() == "sand_i") {

						if (object.getProperties()[0].getStringValue() == "topleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 318 });
						}

						if (object.getProperties()[0].getStringValue() == "left") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 322 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 326 });
						}

						if (object.getProperties()[0].getStringValue() == "top") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 330 });
						}

						if (object.getProperties()[0].getStringValue() == "bottom") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 338 });
						}

						if (object.getProperties()[0].getStringValue() == "topright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 342 });
						}

						if (object.getProperties()[0].getStringValue() == "right") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 346 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 350 });
						}
					}

					if (property.getName() == "terrain" && property.getStringValue() == "sand") {

						if (object.getProperties()[0].getStringValue() == "topleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 354 });
						}

						if (object.getProperties()[0].getStringValue() == "left") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 358 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 362 });
						}

						if (object.getProperties()[0].getStringValue() == "top") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 366 });
						}

						if (object.getProperties()[0].getStringValue() == "bottom") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 374 });
						}

						if (object.getProperties()[0].getStringValue() == "topright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 378 });
						}

						if (object.getProperties()[0].getStringValue() == "right") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 382 });
						}

						if (object.getProperties()[0].getStringValue() == "bottomright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 386 });
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
	}
}

const Player& Zone::getPlayer() {
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

	m_visibleCellsBG.clear();

	for (int j = 0; j < m_layers.size(); j++) {
		for (int y = rowMin; y < rowMax; y++) {
			for (int x = colMin; x < colMax; x++) {
				if (m_layers[j][y][x].first != -1) {
					m_visibleCellsBG.push_back(m_cellsBackground[m_layers[j][y][x].second]);
				}

			}
		}
	}

	m_visibleCellsAni.clear();
	for (AnimatedCell& animatedCell : m_animatedCells) {
		if (isRectOnScreen(animatedCell.posX, animatedCell.posY, 64.0f, 64.0f) || !m_useCulling) {
			m_visibleCellsAni.push_back(animatedCell);
		}
	}

	m_visibleCellsMain.clear();
	const std::vector<TextureRect>& rects = TileSetManager::Get().getTileSet("world").getTextureRects();
	for (CellShadow& cell : m_cellsMain) {
		cell.visibile = false;
		const TextureRect& rect = rects[cell.currentFrame];
		if (isRectOnScreen(cell.posX, cell.posY - rect.height, rect.width, rect.height) || !m_useCulling) {
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