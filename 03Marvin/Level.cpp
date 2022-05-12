#include "Level.h"

Level::Level(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_shaderArray = Globals::shaderManager.getAssetPointer("level");
	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("base");
	m_quadBackground = new Quad(false, -1.0f, 1.0f, -1.0f, 1.0f);
	m_sprites["background"] = Globals::textureManager.get("background").getTexture();

	loadFile("Resources/Maps/easypeasy.json");

	for (const TileLayer& layer : m_layers) {
		for (const Tile& tile : layer.tiles) {
			addTile(tile, m_vertices, m_indexBuffer);
		}
		if (layer.collisionLayer) {
			for (unsigned int count = 0; count < layer.height * layer.width; count++) {
				m_bitVector.push_back(false);
			}

			bool tracing = true;

			while (tracing) {
				tracing = contourTrace(layer.tilesCol, m_bitVector, layer);
			}
		}
	}

	short stride = 5, offset = 3;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_vboMap);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	//map indices
	glBindBuffer(GL_ARRAY_BUFFER, m_vboMap);
	glBufferData(GL_ARRAY_BUFFER, m_indexMap.size() * sizeof(m_indexMap[0]), &m_indexMap[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 1 * sizeof(unsigned int), 0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Level::loadFile(const std::string &_file) {
	nlohmann::json model;
	std::ifstream file(_file);
	file >> model;
	file.close();
	loadLayers(model);
	loadObjects(model);
}

void Level::loadLayers(nlohmann::json &map) {

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
		}
	}
}

void Level::loadObjects(nlohmann::json &map) {
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
				m_phyObjects.push_back(createPhysicsBody(m_objects.back()));
				
			}
		}
	}
}

b2Body* Level::createPhysicsBody(Object &object) {

	GameObject *newObject;

	if (object.type.compare("Exit") == 0) {
		newObject = new GameObject(Category::Type::Exit);
	}else if (object.type.compare("Water") == 0) {
		newObject = new GameObject(Category::Type::Seeker);
	}else if (object.type.compare("Gem") == 0) {
		newObject = new GameObject(Category::Type::Gem);
	}

	b2Vec2 position = b2Vec2(15.0f + object.position[0] * (30.0f / 70.0f), 905.0f - object.position[1] * (30.0f / 70.0f));
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	//bodyDef.allowSleep = false;
	//bodyDef.awake = true;
	bodyDef.position = position;
	b2Body *objectBody = Globals::world->CreateBody(&bodyDef);

	b2PolygonShape boundingBox;
	boundingBox.SetAsBox(15.0f, 2.0f);

	b2FixtureDef objectFixture;
	objectFixture.shape = &boundingBox;
	objectFixture.friction = 0.0f;
	objectFixture.density = 0.0f;
	objectFixture.userData.pointer = 3;
	objectFixture.filter.categoryBits = 1;
	objectFixture.filter.maskBits = 1;
	//objectFixture;
	objectFixture.isSensor = true;
	objectBody->CreateFixture(&objectFixture);

	//objectBody->SetUserData(newObject);
	objectBody->GetUserData().pointer = reinterpret_cast<std::uintptr_t>(newObject);

	return objectBody;
}

void Level::loadTileLayer(nlohmann::json &layer_in, TileLayer &layer_out, bool reverse) {
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

void Level::addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
	Vector2f pos = tile.position;
	float w = tile.size[0];
	float h = tile.size[1];

	vertices.push_back(pos[0]); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
	vertices.push_back(pos[0]); vertices.push_back(pos[1] + h); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(1.0f);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1] + h); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(1.0f);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);

	m_indexMap.push_back(tile.gid); m_indexMap.push_back(tile.gid); m_indexMap.push_back(tile.gid); m_indexMap.push_back(tile.gid);

	unsigned int currentOffset = (vertices.size() / 5) - 4;

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 1);
	indices.push_back(currentOffset + 2);

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 2);
	indices.push_back(currentOffset + 3);
}

bool Level::contourTrace(const std::vector<Tile> &grid, std::vector<bool> &visited, const TileLayer& layer) {
	for (int row = 0; row < layer.height; ++row) {
		for (int col = 0; col < layer.width; ++col) {
			if (!visited[row * layer.width + col] && grid[row * layer.width + col].gid) {

				if ((row > 0) && (!grid[(row - 1) * layer.width + col].gid)) {
					contourTraceComponent(col, row, col, row - 1, grid, visited, layer);
					return true;
				}else if ((row < layer.height - 1) && (!grid[(row + 1) * layer.width + col].gid)) {
					contourTraceComponent(col, row, col, row + 1, grid, visited, layer);
					return true;
				}else if ((col > 0) && (!grid[row * layer.width + (col - 1)].gid)) {
					contourTraceComponent(col, row, col - 1, row, grid, visited, layer);
					return true;
				}else if ((col < layer.width - 1) && (!grid[row * layer.width + (col + 1)].gid)) {
					contourTraceComponent(col, row, col + 1, row, grid, visited, layer);
					return true;
				}
			}
		}
	}
	return false;
}

void Level::contourTraceComponent(int startX, int startY, int startBacktrackX, int startBacktrackY, const std::vector<Tile> &grid, std::vector<bool> &visited, const TileLayer& layer) {
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
			/*if ((!grid[currentY * m_layer.width + currentX].gid) && (i % 2 == 1) && (numVisited > 2)) {
				std::cout << "not connected" << std::endl;
				addChainVertex(
					boundaryX,
					boundaryY,
					neighborhoodCoords[i - 1].first,
					neighborhoodCoords[i - 1].second,
					chainVertices);
			}*/
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

void Level::createStaticBody(std::vector<Vector2f> &chainVertices) {
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

	if (numVertices == 2) {
		vertices.push_back(b2Vec2(chainVertices[0][0] * 30.0f, chainVertices[0][1] * 30.0f));
		vertices.push_back(b2Vec2(chainVertices[1][0] * 30.0f, chainVertices[1][1] * 30.0f));

		vertices.push_back(b2Vec2(chainVertices[1][0] * 30.0f, (chainVertices[1][1] - 1) * 30.0f));
		vertices.push_back(b2Vec2((chainVertices[1][0] - 1) * 30.0f, (chainVertices[1][1] - 1) * 30.0f));
	}else {
		for (int i = numVertices - 1; i >= 0; i--) {
			vertices.push_back(b2Vec2(chainVertices[i][0] * 30.0f, chainVertices[i][1] * 30.0f));
		}
	}

	b2ChainShape chain;
	//chain.CreateChain(&vertices[0], numVertices, vertices[0], vertices[numVertices - 1]);
	chain.CreateLoop(&vertices[0], vertices.size());

	b2Fixture *contourFixture = platform->CreateFixture(&chain, 0);
	contourFixture->SetFriction(1.f);
	m_contours.push_back(platform);
}

void Level::addChainVertex(int startX, int startY, int previousX, int previousY, std::vector<Vector2f> &chainVertices, const TileLayer& layer) {

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

std::vector<std::pair<int, int>> Level::getMooreNeighborhood(int startX, int startY, const TileLayer& layer) {

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

int Level::getMooreIndex(int boundaryX, int boundaryY, int neighborX, int neighborY) {

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

void Level::render() {
	
	glUseProgram(m_shader->m_program);
	//m_shader->loadMatrix("u_transform", m_transBackground);
	m_quadBackground->render(m_sprites["background"]);
	glUseProgram(0);

	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", ViewEffect::get().getView() *  Globals::projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_spriteSheet->getAtlas());
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glUseProgram(0);


	#if DEBUGCOLLISION
	int index = 0;
	Matrix4f transProj = Globals::projection.transpose();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&transProj[0][0]);

	Matrix4f transView = ViewEffect::get().getView();
	transView = transView.transpose();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&transView[0][0]);
	
	for (const auto& b : m_contours) {
		for (b2Fixture *fixture = b->GetFixtureList(); fixture; fixture = fixture->GetNext()) {

			//We know it's a chain shape
			b2ChainShape *chain = static_cast<b2ChainShape*>(fixture->GetShape());

			for (int i = 0; i < chain->GetChildCount(); ++i) {
				b2EdgeShape edge;
				chain->GetChildEdge(&edge, i);
				b2Vec2 v1 = edge.m_vertex1;
				b2Vec2 v2 = edge.m_vertex2;
				
				glBegin(GL_LINES);
				glColor3f(1, 1, 1);
				glVertex3f(v1.x, v1.y, 0.0f);
				glVertex3f(v2.x, v2.y, 0.0f);
				glEnd();
			}
		}
	}
	for (const auto& body : m_phyObjects) {
		
		b2Vec2 position = body->GetPosition();
		b2PolygonShape *boxShape = static_cast<b2PolygonShape*>(body->GetFixtureList()->GetShape());

		b2Vec2 v1 = boxShape->m_vertices[0];
		b2Vec2 v2 = boxShape->m_vertices[1];
		b2Vec2 v3 = boxShape->m_vertices[2];
		b2Vec2 v4 = boxShape->m_vertices[3];
		//std::cout << position.x << "  " << position.y << std::endl;
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_QUADS);
		glColor3f(1, 0, 0);

		float xpos = position.x + v1.x;
		float ypos = position.y + v1.y;
		float w = v2.x - v1.x;
		float h = v4.y - v1.y;

		glVertex3f(xpos, ypos, 0.0f);
		glVertex3f(xpos, (ypos + h), 0.0f);
		glVertex3f(xpos + w, (ypos + h), 0.0f);
		glVertex3f(xpos + w, ypos, 0.0f);
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	#endif
}

void Level::update() {
}

void Level::fixedUpdate() {

}