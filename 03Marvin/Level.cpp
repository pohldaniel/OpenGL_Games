#include "Level.h"

Level::Level(){
	m_shaderArray = Globals::shaderManager.getAssetPointer("level");
	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("base");

	loadFile("Resources/Maps/easypeasy.json");

	for (const Tile& tile : m_layer.tiles) {
		addTile(tile, m_vertices, m_indexBuffer);
	}

	for (unsigned int count = 0; count < m_layer.height * m_layer.width; count++) {
		m_bitVector.push_back(false);
	}

	bool tracing = true;

	while (tracing) {
		tracing = contourTrace(m_layer.tilesCol, m_bitVector);
	}

	/*for (unsigned int row = 0; row < m_layer.height; row++) {
		for (unsigned int col = 0; col < m_layer.width; col++) {
			std::cout << m_layer.tilesCol[row * m_layer.width + col].gid << "  ";
		}
		std::cout << std::endl;
	}

	for (unsigned int row = 0; row < m_layer.height; row++) {
		for (unsigned int col = 0; col < m_layer.width; col++) {
			std::cout << m_bitVector[row * m_layer.width + col] << "  ";
		}
		std::cout << std::endl;
	}*/

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
	glBufferData(GL_ARRAY_BUFFER, m_map.size() * sizeof(m_map[0]), &m_map[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 1 * sizeof(unsigned int), 0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


bool Level::contourTrace(const std::vector<Tile> &grid, std::vector<bool> &visited) {
	for (int row = 0; row < m_layer.height; ++row) {
		for (int col = 0; col < m_layer.width; ++col) {
			if (!visited[row * m_layer.width + col] && grid[row * m_layer.width + col].gid) {

				if ((row > 0) && (!grid[(row - 1) * m_layer.width + col].gid)) {
					contourTraceComponent(col, row, col, row - 1, grid, visited);
					return true;
				}else if ((row < m_layer.height - 1) && (!grid[(row + 1) * m_layer.width + col].gid)) {
					contourTraceComponent(col, row, col, row + 1, grid, visited);
					return true;
				}else if ((col > 0) && (!grid[row * m_layer.width + (col - 1)].gid)) {
					contourTraceComponent(col, row, col - 1, row, grid, visited);
					return true;
				}else if ((col < m_layer.width - 1) && (!grid[row * m_layer.width + (col + 1)].gid)) {
					contourTraceComponent(col, row, col + 1, row, grid, visited);
					return true;
				}
			}
		}
	}
	return false;
}

void Level::contourTraceComponent(int startX, int startY, int startBacktrackX, int startBacktrackY, const std::vector<Tile> &grid, std::vector<bool> &visited) {
	visited[startY * m_layer.width + startX] = true;
	std::vector<Vector2f> chainVertices;
	

	//Set up our initial chain edge    
	auto neighborhoodCoords = getMooreNeighborhood(startX, startY);
	int i = getMooreIndex(startX, startY, startBacktrackX, startBacktrackY);
	int cClockwiseIndex = (i == 0 ? 6 : i - 2);

	addChainVertex(
		startX,
		startY,
		neighborhoodCoords[cClockwiseIndex].first,
		neighborhoodCoords[cClockwiseIndex].second,
		chainVertices);
	addChainVertex(
		startX,
		startY,
		neighborhoodCoords[i].first,
		neighborhoodCoords[i].second,
		chainVertices);

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

		auto neighborhoodCoords = getMooreNeighborhood(boundaryX, boundaryY);
		int i = getMooreIndex(boundaryX, boundaryY, currentX, currentY);
		int numVisited = 0;
		auto invalidPair = std::make_pair(-1, -1);

		while (!grid[currentY * m_layer.width + currentX].gid) {

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
				//createStaticBody(chainVertices);
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
		visited[currentY * m_layer.width + currentX] = true;
		boundaryX = currentX;
		boundaryY = currentY;
		currentX = backtrackX;
		currentY = backtrackY;
		addChainVertex(boundaryX, boundaryY, backtrackX, backtrackY, chainVertices);
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

	for (int i = numVertices - 1; i >= 0; i--) {
		vertices.push_back(b2Vec2(chainVertices[i][0] * 30.0f, chainVertices[i][1] * 30.0f));	
	}

	b2ChainShape chain;
	chain.CreateLoop(&vertices[0], numVertices);
	b2Fixture *contourFixture = platform->CreateFixture(&chain, 0);
	contourFixture->SetFriction(1.f);

	//m_chain.push_back(chain);
	m_contours.push_back(platform);
}

void Level::addChainVertex(int startX, int startY, int previousX, int previousY, std::vector<Vector2f> &chainVertices) {

	Vector2f v1;

	//Visited from below
	if (startY - previousY == -1) {
		v1 = Vector2f(
			static_cast<float>(startX),
			static_cast<float>(m_layer.height - (startY + 1)));
	}
	//Visited from above
	else if (startY - previousY == 1) {
		v1 = Vector2f(
			static_cast<float>(startX + 1),
			static_cast<float>(m_layer.height - (startY)));
	}
	//Visited from the left
	else if (startX - previousX == 1) {
		v1 = Vector2f(
			static_cast<float>(startX),
			static_cast<float>(m_layer.height - (startY)));
	}
	//Visited from the right
	else {
		v1 = Vector2f(
			static_cast<float>(startX + 1),
			static_cast<float>(m_layer.height - (startY + 1)));
	}
	chainVertices.push_back(v1);
}

std::vector<std::pair<int, int>> Level::getMooreNeighborhood(int startX, int startY) {

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

	exists = (startX < m_layer.width - 1 && startY > 0);
	neighborhood.push_back(
		std::make_pair(exists ? startX + 1 : -1, exists ? startY - 1 : -1));

	exists = (startX < m_layer.width - 1);
	neighborhood.push_back(
		std::make_pair(exists ? startX + 1 : -1, exists ? startY : -1));

	exists = (startX < m_layer.width - 1 && startY < m_layer.height - 1);
	neighborhood.push_back(
		std::make_pair(exists ? startX + 1 : -1, exists ? startY + 1 : -1));

	exists = (startY < m_layer.height - 1);
	neighborhood.push_back(
		std::make_pair(exists ? startX : -1, exists ? startY + 1 : -1));

	exists = (startX > 0 && startY < m_layer.height - 1);
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


void Level::loadFile(const std::string &_file) {
	nlohmann::json model;
	std::ifstream file(_file);
	file >> model;
	file.close();
	loadLayers(model);
}

void Level::loadLayers(nlohmann::json &map) {

	nlohmann::json layers = map["layers"];
	for (auto &layer : layers) {
		std::string type = layer["type"].get<std::string>();
		if (type.compare("tilelayer") == 0 && (layer["name"].get<std::string>().compare("Tilemap") == 0))
			loadTileLayer(layer, m_layer);
	}
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
		layer_out.tilesCol.push_back({ Vector2f(30.0f * posX, 30.0f * posY), Vector2f(30.0f, 30.0f), tile.get<unsigned int>()});

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

	m_map.push_back(tile.gid); m_map.push_back(tile.gid); m_map.push_back(tile.gid); m_map.push_back(tile.gid);

	unsigned int currentOffset = (vertices.size() / 5) - 4;

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 1);
	indices.push_back(currentOffset + 2);

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 2);
	indices.push_back(currentOffset + 3);
}

void Level::render() {
	
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", Globals::projection);
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
	#endif
}