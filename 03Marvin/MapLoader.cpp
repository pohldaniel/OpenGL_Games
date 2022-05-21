#include "MapLoader.h"

MapLoader MapLoader::s_instance;

MapLoader& MapLoader::get() {
	return s_instance;
}

void MapLoader::setDelta(const float& dt, const float& fdt) {
	m_dt = &dt;
	m_fdt = &fdt;
}

void MapLoader::loadLevel(const std::string &_file) {


	for (const auto& b : m_contours) {	
		Globals::world->DestroyBody(b);
	}

	for (const auto& b : m_phyObjects) {
		Globals::world->DestroyBody(b);
	}

	m_vertices.clear();
	m_indexBuffer.clear();
	m_indexMap.clear();
	m_contours.clear();
	m_phyObjects.clear();
	m_renObjects.clear();
	m_entities2.clear();

	m_objects.clear();
	m_entities.clear();
	m_layers.clear();
	m_bitVector.clear();

	nlohmann::json model;
	std::ifstream file(_file);
	file >> model;
	file.close();
	loadLayers(model);
	loadObjects(model);

	for (const TileLayer& layer : m_layers) {
		for (const Tile& tile : layer.tiles) {
			addTile(tile, m_vertices, m_indexBuffer, m_indexMap);

		}if (layer.collisionLayer) {
			for (unsigned int count = 0; count < layer.height * layer.width; count++) {
				m_bitVector.push_back(false);
			}

			bool tracing = true;

			while (tracing) {
				tracing = contourTrace(layer.tilesCol, m_bitVector, layer);
			}
		}
	}
}

void MapLoader::loadLayers(nlohmann::json &map) {
	m_width = map["width"].get<unsigned int>();
	m_height = map["height"].get<unsigned int>();

	nlohmann::json layers = map["layers"];
	for (auto &layer : layers) {
		std::string type = layer["type"].get<std::string>();
		if (type.compare("tilelayer") == 0 && (layer["name"].get<std::string>().compare("Tilemap") == 0)) {
			TileLayer _layer = TileLayer();
			_layer.collisionLayer = true;
			loadTileLayer(layer, _layer);
			m_layers.push_back(_layer);
		}else if (type.compare("tilelayer") == 0 && (layer["name"].get<std::string>().compare("Background2") == 0)) {
			TileLayer _layer = TileLayer();
			_layer.collisionLayer = false;
			loadTileLayer(layer, _layer);
			m_layers.push_back(_layer);
		}else if (type.compare("tilelayer") == 0 && (layer["name"].get<std::string>().compare("Background") == 0)) {
			TileLayer _layer = TileLayer();
			_layer.collisionLayer = false;
			loadTileLayer(layer, _layer);
			m_layers.push_back(_layer);
		}else if (type.compare("tilelayer") == 0 && (layer["name"].get<std::string>().compare("Platforms") == 0)) {			
			TileLayer _layer = TileLayer();
			_layer.collisionLayer = true;
			loadTileLayer(layer, _layer);
			m_layers.push_back(_layer);
		}
	}
}

void MapLoader::loadObjects(nlohmann::json &map) {

	nlohmann::json layers = map["layers"];
	for (auto &layer : layers) {
		if (layer["name"].get<std::string>().compare("Objects") == 0) {
			nlohmann::json data = layer["objects"];
			for (auto &object : data) {

				m_objects.push_back({
					object["type"].get<std::string>(),
					object["name"].get<std::string>(),
					Vector2f(object["x"].get<unsigned int>(), object["y"].get<unsigned int>()),
					object["x"].get<unsigned int>(),
					object["y"].get<unsigned int>()
				});
				createPhysicsBody(m_objects.back());
			}
		}else if (layer["name"].get<std::string>().compare("Spawns") == 0) {
			nlohmann::json data = layer["objects"];
			for (auto &object : data) {

				if (object["type"].get<std::string>().compare("Player") == 0) {
					m_playerPosition = Vector2f(15.0f + object["x"].get<unsigned int>() * (30.0f / 70.0f), (m_width * m_height) - ( 17.0f + object["y"].get<unsigned int>() * (30.0f / 70.0f)));
				}else {
					m_entities.push_back({
						object["type"].get<std::string>(),
						object["name"].get<std::string>(),
						Vector2f(object["x"].get<unsigned int>(), object["y"].get<unsigned int>()),
						object["x"].get<unsigned int>(),
						object["y"].get<unsigned int>()
					});
					createEntity(m_entities.back());
				}
			}
		}
	}
}

void MapLoader::createPhysicsBody(JSONObject &object) {
	Object *newObject;
	b2FixtureDef objectFixture;
	b2PolygonShape boundingBox;
	b2Vec2 position;
	b2BodyDef bodyDef;

	if (object.type.compare("Exit") == 0) {
		objectFixture.filter.categoryBits = Category::Type::Exit;
		objectFixture.filter.maskBits = Category::Type::Player;

		position = b2Vec2(15.0f + object.position[0] * (30.0f / 70.0f), (m_width * m_height) + 3.0f - object.position[1] * (30.0f / 70.0f));
		boundingBox.SetAsBox(15.0f, 2.0f);

		bodyDef.type = b2_kinematicBody;
		bodyDef.position = position;

		newObject = new Object(Category::Type::Exit);

	}else if (object.type.compare("Water") == 0) {
		objectFixture.filter.categoryBits = Category::Type::Seeker;
		objectFixture.filter.maskBits = Category::Type::Player;

		position = b2Vec2(15.0f + object.position[0] * (30.0f / 70.0f), (m_width * m_height) + 5.0f - object.position[1] * (30.0f / 70.0f));
		boundingBox.SetAsBox(15.0f, 9.0f);

		bodyDef.type = b2_kinematicBody;
		bodyDef.position = position;

		newObject = new Object(Category::Type::Seeker);

	}else if (object.type.compare("Gem") == 0) {
		objectFixture.filter.categoryBits = Category::Type::Gem;
		objectFixture.filter.maskBits = Category::Type::Player;

		position = b2Vec2(15.0f + object.position[0] * (30.0f / 70.0f), (m_width * m_height) + 5.0f - object.position[1] * (30.0f / 70.0f));
		boundingBox.SetAsBox(8.0f, 5.0f);

		bodyDef.type = b2_kinematicBody;
		bodyDef.position = position;

		newObject = new RenderableObject(Category::Type::Gem);
		newObject->setPosition(position.x, position.y);
		newObject->setSize(30.0f, 30.0f);
		newObject->setOrigin(newObject->getSize() * 0.5);
		m_renObjects.push_back(dynamic_cast<RenderableObject*>(newObject));
	}

	b2Body *objectBody = Globals::world->CreateBody(&bodyDef);

	objectFixture.shape = &boundingBox;
	objectFixture.friction = 0.0f;
	objectFixture.density = 0.0f;
	objectFixture.userData.pointer = 3;

	objectBody->CreateFixture(&objectFixture);
	objectBody->GetUserData().pointer = reinterpret_cast<std::uintptr_t>(newObject);

	m_phyObjects.push_back(objectBody);
}

void MapLoader::createEntity(JSONObject &object) {
	Object *newObject;
	b2FixtureDef objectFixture;
	b2PolygonShape boundingBox;
	boundingBox.SetAsBox(15.0f, 15.0f);
	b2Vec2 position = b2Vec2(15.0f + object.position[0] * (30.0f / 70.0f), (m_width * m_height) - (15.0f + object.position[1] * (30.0f / 70.0f)));
	b2BodyDef bodyDef;
	bodyDef.position = position;
	bodyDef.type = b2_kinematicBody;

	if (object.type.compare("Barnacle") == 0) {
		objectFixture.filter.categoryBits = Category::Type::Enemy;
		objectFixture.filter.maskBits = Category::Type::Player;

		newObject = new Barnacle(Category::Type::Enemy, *m_dt, *m_fdt);
		newObject->setPosition(position.x, position.y);
		newObject->setSize(30.0f, 30.0f);
		newObject->setOrigin(newObject->getSize() * 0.5);
		m_entities2.push_back(dynamic_cast<Barnacle*>(newObject));
	}else if (object.type.compare("Bee") == 0) {
		objectFixture.filter.categoryBits = Category::Type::Enemy;
		objectFixture.filter.maskBits = Category::Type::Player;

		newObject = new Bee(Category::Type::Enemy, *m_dt, *m_fdt);
		newObject->setPosition(position.x, position.y);
		newObject->setSize(30.0f, 30.0f);
		newObject->setOrigin(newObject->getSize() * 0.5);
		m_entities2.push_back(dynamic_cast<Bee*>(newObject));
	}else if (object.type.compare("Slime") == 0) {
		objectFixture.filter.categoryBits = Category::Type::Enemy;
		objectFixture.filter.maskBits = Category::Type::Player;

		newObject = new Slime(Category::Type::Enemy, *m_dt, *m_fdt);
		newObject->setPosition(position.x, position.y);
		newObject->setSize(30.0f, 30.0f);
		newObject->setOrigin(newObject->getSize() * 0.5);
		m_entities2.push_back(dynamic_cast<Slime*>(newObject));
	}else if (object.type.compare("GrassBlock") == 0) {
		objectFixture.filter.categoryBits = Category::Type::Enemy;
		objectFixture.filter.maskBits = Category::Type::Player;

		newObject = new GrassBlock(Category::Type::Enemy, *m_dt, *m_fdt);
		newObject->setPosition(position.x, position.y);
		newObject->setSize(30.0f, 30.0f);
		newObject->setOrigin(newObject->getSize() * 0.5);
		m_entities2.push_back(dynamic_cast<GrassBlock*>(newObject));
	}else if (object.type.compare("SnakeSlime") == 0) {
		objectFixture.filter.categoryBits = Category::Type::Enemy;
		objectFixture.filter.maskBits = Category::Type::Player;

		newObject = new SnakeSlime(Category::Type::Enemy, *m_dt, *m_fdt);
		newObject->setPosition(position.x, position.y);
		newObject->setSize(30.0f, 30.0f);
		newObject->setOrigin(newObject->getSize() * 0.5);
		m_entities2.push_back(dynamic_cast<SnakeSlime*>(newObject));
	}

	b2Body *objectBody = Globals::world->CreateBody(&bodyDef);

	objectFixture.shape = &boundingBox;
	objectFixture.friction = 0.0f;
	objectFixture.density = 0.0f;
	objectFixture.userData.pointer = 3;

	objectBody->CreateFixture(&objectFixture);
	objectBody->GetUserData().pointer = reinterpret_cast<std::uintptr_t>(newObject);

	m_phyObjects.push_back(objectBody);
}

void MapLoader::loadTileLayer(nlohmann::json &layer_in, TileLayer &layer_out, bool reverse) {
	layer_out.name = layer_in["name"].get<std::string>();
	layer_out.visible = layer_in["visible"].get<bool>();
	layer_out.opacity = layer_in["opacity"].get<float>();
	layer_out.width = layer_in["width"].get<unsigned int>();
	layer_out.height = layer_in["height"].get<unsigned int>();

	nlohmann::json data = layer_in["data"];

	unsigned short count = 0;
	unsigned short posX = 0;
	unsigned short posY = reverse ? layer_out.height - 1 : 0;

	for (auto &tile : data) {
		if (count % layer_out.width == 0 && count > 0) {
			if (reverse) posY--; else posY++;
			posX = 0;
		}
		if (tile.get<unsigned int>() != 0) {
			layer_out.tiles.push_back({ Vector2f(30.0f * posX, 30.0f * posY), Vector2f(30.0f, 30.0f), tile.get<unsigned int>() - 1 });
		}
		layer_out.tilesCol.push_back({ Vector2f(30.0f * posX, 30.0f * posY), Vector2f(30.0f, 30.0f), tile.get<unsigned int>() });

		posX++;
		count++;
	}
}

void MapLoader::addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<unsigned int>& mapIndices) {
	Vector2f pos = tile.position;
	float w = tile.size[0];
	float h = tile.size[1];

	vertices.push_back(pos[0]); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
	vertices.push_back(pos[0]); vertices.push_back(pos[1] + h); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(1.0f);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1] + h); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(1.0f);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);

	mapIndices.push_back(tile.gid); mapIndices.push_back(tile.gid); mapIndices.push_back(tile.gid); mapIndices.push_back(tile.gid);

	unsigned int currentOffset = (vertices.size() / 5) - 4;

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 1);
	indices.push_back(currentOffset + 2);

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 2);
	indices.push_back(currentOffset + 3);
}

bool MapLoader::contourTrace(const std::vector<Tile> &grid, std::vector<bool> &visited, const TileLayer& layer) {
	for (int row = 0; row < layer.height; ++row) {
		for (int col = 0; col < layer.width; ++col) {
			if (!visited[row * layer.width + col] && grid[row * layer.width + col].gid) {

				if ((row > 0) && (!grid[(row - 1) * layer.width + col].gid)) {
					contourTraceComponent(col, row, col, row - 1, grid, visited, layer);
					return true;
				}
				else if ((row < layer.height - 1) && (!grid[(row + 1) * layer.width + col].gid)) {
					contourTraceComponent(col, row, col, row + 1, grid, visited, layer);
					return true;
				}
				else if ((col > 0) && (!grid[row * layer.width + (col - 1)].gid)) {
					contourTraceComponent(col, row, col - 1, row, grid, visited, layer);
					return true;
				}
				else if ((col < layer.width - 1) && (!grid[row * layer.width + (col + 1)].gid)) {
					contourTraceComponent(col, row, col + 1, row, grid, visited, layer);
					return true;
				}
			}
		}
	}
	return false;
}

void MapLoader::contourTraceComponent(int startX, int startY, int startBacktrackX, int startBacktrackY, const std::vector<Tile> &grid, std::vector<bool> &visited, const TileLayer& layer) {
	visited[startY * layer.width + startX] = true;
	std::vector<Vector2f> chainVertices;


	//Set up our initial chain edge    
	auto neighborhoodCoords = getMooreNeighborhood(startX, startY, layer);
	int i = getMooreIndex(startX, startY, startBacktrackX, startBacktrackY);
	int cClockwiseIndex = (i == 0 ? 6 : i - 2);

	addChainVertex(
		startX,
		startY,
		neighborhoodCoords[cClockwiseIndex].first,
		neighborhoodCoords[cClockwiseIndex].second,
		chainVertices, layer);
	addChainVertex(
		startX,
		startY,
		neighborhoodCoords[i].first,
		neighborhoodCoords[i].second,
		chainVertices, layer);

	//Begin Moore-Neighbor Tracing
	int boundaryX = startX;
	int boundaryY = startY;
	int currentX = startBacktrackX;
	int currentY = startBacktrackY;
	int backtrackX = -1;
	int backtrackY = -1;
	int numStartVisits = 0;

	//Using Jacob's stopping criterion along with 
	//the start cell being visited a maximum of 3 times
	while ((!((boundaryX == startX) && (boundaryY == startY) &&
		(backtrackX == startBacktrackX) && (backtrackY == startBacktrackY))) &&
		(numStartVisits < 3)) {

		if (boundaryX == startX && boundaryY == startY) {
			if (numStartVisits > 0) {
				startBacktrackX = backtrackX;
				startBacktrackY = backtrackY;
			}
			++numStartVisits;
		}

		auto neighborhoodCoords = getMooreNeighborhood(boundaryX, boundaryY, layer);
		int i = getMooreIndex(boundaryX, boundaryY, currentX, currentY);
		int numVisited = 0;
		auto invalidPair = std::make_pair(-1, -1);

		while (!grid[currentY * layer.width + currentX].gid) {

			backtrackX = currentX;
			backtrackY = currentY;
			currentX = neighborhoodCoords[i].first;
			currentY = neighborhoodCoords[i].second;
			++numVisited;

			//Handle case when the map is not closed
			//I.E. we go off the map contour tracing
			if (neighborhoodCoords[i] == invalidPair) {
				createStaticBody(chainVertices);
				return;
			}

			i = (i == 7 ? 0 : i + 1);

			//Handle single tile
			if (numVisited == 9) {
				createStaticBody(chainVertices);
				return;
			}

			//Handle edges not adjacent to other tiles
			if ((!grid[currentY * layer.width + currentX].gid) && (i % 2 == 1) && (numVisited > 2)) {

				addChainVertex(
				boundaryX,
				boundaryY,
				neighborhoodCoords[i - 1].first,
				neighborhoodCoords[i - 1].second,
				chainVertices, 
				layer);
			}
		}
		visited[currentY * layer.width + currentX] = true;
		boundaryX = currentX;
		boundaryY = currentY;
		currentX = backtrackX;
		currentY = backtrackY;
		addChainVertex(boundaryX, boundaryY, backtrackX, backtrackY, chainVertices, layer);
	}

	createStaticBody(chainVertices);
}

void MapLoader::createStaticBody(std::vector<Vector2f> &chainVertices) {
	auto chainLoop = std::find(chainVertices.begin() + 1, chainVertices.end(), chainVertices[0]);
	if (chainLoop != chainVertices.end())
		chainVertices.erase(chainLoop + 1, chainVertices.end());
	int numVertices = chainVertices.size();

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.allowSleep = true;
	bodyDef.awake = true;
	b2Body *platform = Globals::world->CreateBody(&bodyDef);

	std::vector<b2Vec2> vertices;
	b2ChainShape chain;
	if (numVertices == 2) {
		vertices.push_back(b2Vec2(chainVertices[0][0] * 30.0f, chainVertices[0][1] * 30.0f));
		vertices.push_back(b2Vec2(chainVertices[1][0] * 30.0f, chainVertices[1][1] * 30.0f));

		vertices.push_back(b2Vec2(chainVertices[1][0] * 30.0f, (chainVertices[1][1] - 1) * 30.0f));
		vertices.push_back(b2Vec2((chainVertices[1][0] - 1) * 30.0f, (chainVertices[1][1] - 1) * 30.0f));
		chain.CreateLoop(&vertices[0], vertices.size());
	}
	else {
		for (int i = numVertices - 1; i >= 0; i--) {
			vertices.push_back(b2Vec2(chainVertices[i][0] * 30.0f, chainVertices[i][1] * 30.0f));
		}
		chain.CreateLoop(&vertices[0], vertices.size() - 1);
	}


	//chain.CreateChain(&vertices[0], numVertices, vertices[0], vertices[numVertices - 1]);


	b2Fixture *contourFixture = platform->CreateFixture(&chain, 0);
	contourFixture->SetFriction(1.f);
	m_contours.push_back(platform);
}

void MapLoader::addChainVertex(int startX, int startY, int previousX, int previousY, std::vector<Vector2f> &chainVertices, const TileLayer& layer) {

	Vector2f v1;

	//Visited from below
	if (startY - previousY == -1) {
		v1 = Vector2f(
			static_cast<float>(startX),
			static_cast<float>(layer.height - (startY + 1)));
	}
	//Visited from above
	else if (startY - previousY == 1) {
		v1 = Vector2f(
			static_cast<float>(startX + 1),
			static_cast<float>(layer.height - (startY)));
	}
	//Visited from the left
	else if (startX - previousX == 1) {
		v1 = Vector2f(
			static_cast<float>(startX),
			static_cast<float>(layer.height - (startY)));
	}
	//Visited from the right
	else {
		v1 = Vector2f(
			static_cast<float>(startX + 1),
			static_cast<float>(layer.height - (startY + 1)));
	}
	chainVertices.push_back(v1);
}

std::vector<std::pair<int, int>> MapLoader::getMooreNeighborhood(int startX, int startY, const TileLayer& layer) {

	std::vector<std::pair<int, int>> neighborhood;

	//Start from the left-tile and go clockwise
	//Values of -1 indicate border cases where the cell is outside the grid
	bool exists = (startX > 0);
	neighborhood.push_back(
		std::make_pair(exists ? startX - 1 : -1, exists ? startY : -1));

	exists = (startX > 0 && startY > 0);
	neighborhood.push_back(
		std::make_pair(exists ? startX - 1 : -1, exists ? startY - 1 : -1));

	exists = (startY > 0);
	neighborhood.push_back(
		std::make_pair(exists ? startX : -1, exists ? startY - 1 : -1));

	exists = (startX < layer.width - 1 && startY > 0);
	neighborhood.push_back(
		std::make_pair(exists ? startX + 1 : -1, exists ? startY - 1 : -1));

	exists = (startX < layer.width - 1);
	neighborhood.push_back(
		std::make_pair(exists ? startX + 1 : -1, exists ? startY : -1));

	exists = (startX < layer.width - 1 && startY < layer.height - 1);
	neighborhood.push_back(
		std::make_pair(exists ? startX + 1 : -1, exists ? startY + 1 : -1));

	exists = (startY < layer.height - 1);
	neighborhood.push_back(
		std::make_pair(exists ? startX : -1, exists ? startY + 1 : -1));

	exists = (startX > 0 && startY < layer.height - 1);
	neighborhood.push_back(
		std::make_pair(exists ? startX - 1 : -1, exists ? startY + 1 : -1));
	return neighborhood;
}

int MapLoader::getMooreIndex(int boundaryX, int boundaryY, int neighborX, int neighborY) {

	if ((neighborX == boundaryX - 1) && (neighborY == boundaryY))
		return 0;
	if ((neighborX == boundaryX - 1) && (neighborY == boundaryY - 1))
		return 1;
	if ((neighborX == boundaryX) && (neighborY == boundaryY - 1))
		return 2;
	if ((neighborX == boundaryX + 1) && (neighborY == boundaryY - 1))
		return 3;
	if ((neighborX == boundaryX + 1) && (neighborY == boundaryY))
		return 4;
	if ((neighborX == boundaryX + 1) && (neighborY == boundaryY + 1))
		return 5;
	if ((neighborX == boundaryX) && (neighborY == boundaryY + 1))
		return 6;
	if ((neighborX == boundaryX - 1) && (neighborY == boundaryY + 1))
		return 7;
	return -1;
}