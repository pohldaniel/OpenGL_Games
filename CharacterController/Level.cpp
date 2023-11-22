#include "Level.h"

Level::Level(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	
	m_movingPlatform = new MovingPlatform(dt, fdt);

	loadFile("Resources/Maps/easypeasy.json");

	for (unsigned int count = 0; count < m_layer.height * m_layer.width; count++) {
		m_bitVector.push_back(false);
	}

	bool tracing = true;

	while (tracing) {
		tracing = contourTrace(m_layer.tilesCol, m_bitVector);
	}

	b2BodyDef slopeBodyDef;
	slopeBodyDef.type = b2_staticBody; 
	slopeBodyDef.position.Set(slopePosition.x, slopePosition.y);
	slopeBodyDef.angle = slopeAngel * PI / 180;
	//blockBodyDef.userData = block;
	slopeBody = Globals::world->CreateBody(&slopeBodyDef);

	// Create block shape
	b2PolygonShape slopShape;
	slopShape.SetAsBox(200.0f, 50.0f);
	// Create shape definition and add to body
	b2FixtureDef slopShapeDef;
	slopShapeDef.shape = &slopShape;
	slopShapeDef.density = 10.0;
	slopShapeDef.friction = 0.0;
	slopShapeDef.restitution = 0.1f;
	slopeBody->CreateFixture(&slopShapeDef);

	b2BodyDef slopeBodyDef2;
	slopeBodyDef2.type = b2_staticBody;
	slopeBodyDef2.position.Set(slopePosition2.x, slopePosition2.y);
	slopeBodyDef2.angle = (slopeAngel + 90.0f) * PI / 180;
	//blockBodyDef.userData = block;
	slopeBody2 = Globals::world->CreateBody(&slopeBodyDef2);

	b2PolygonShape slopShape2;
	slopShape2.SetAsBox(200.0f, 50.0f);
	// Create shape definition and add to body
	b2FixtureDef slopShapeDef2;
	slopShapeDef2.shape = &slopShape2;
	slopShapeDef2.density = 10.0;
	slopShapeDef2.friction = 0.0;
	slopShapeDef2.restitution = 0.1f;
	slopeBody2->CreateFixture(&slopShapeDef2);


	b2BodyDef platformBodyDef;
	platformBodyDef.type = b2_kinematicBody;
	platformBodyDef.position.Set(platformPosition.x, platformPosition.y);
	platformBodyDef.angle = 0.0f * PI / 180;
	//blockBodyDef.userData = block;
	platformBody = Globals::world->CreateBody(&platformBodyDef);

	// Create block shape
	b2PolygonShape platformShape;
	platformShape.SetAsBox(200.0f, 5.0f);
	// Create shape definition and add to body
	b2FixtureDef platformShapeDef;
	platformShapeDef.shape = &platformShape;
	platformShapeDef.density = 10.0;
	platformShapeDef.friction = 0.0;
	platformShapeDef.restitution = 0.1f;
	platformShapeDef.userData.pointer = 3;
	platformBody->CreateFixture(&platformShapeDef);

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

void Level::render() {
	
	#if DEBUGCOLLISION
	int index = 0;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&Globals::projection[0][0]);

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

	Matrix4f rot;
	rot.rotate(Vector3f(0.0f, 0.0f, 1.0f), slopeAngel);
	rot.transpose();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(slopePosition.x, slopePosition.y, 0.0);
	glRotatef(slopeAngel, 0.0f, 0.0f, 1.0f); // rotate the robot on its y-axis
	glTranslatef(-slopePosition.x, -slopePosition.y, 0.0);
	//glTranslatef(xpos, ypos, 0.0f);
	//glLoadMatrixf(&rot[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	b2Vec2 position = slopeBody->GetPosition();
	b2PolygonShape *boxShape = static_cast<b2PolygonShape*>(slopeBody->GetFixtureList()->GetShape());

	b2Vec2 v1 = boxShape->m_vertices[0];
	b2Vec2 v2 = boxShape->m_vertices[1];
	b2Vec2 v3 = boxShape->m_vertices[2];
	b2Vec2 v4 = boxShape->m_vertices[3];

	glBegin(GL_QUADS);
	glColor3f(1, 1, 0);

	//left bottom corner
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
	glPopMatrix();
	glLoadIdentity();


	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(slopePosition2.x, slopePosition2.y, 0.0);
	glRotatef(slopeAngel + 90.0f, 0.0f, 0.0f, 1.0f); // rotate the robot on its y-axis
	glTranslatef(-slopePosition2.x, -slopePosition2.y, 0.0);
	//glTranslatef(xpos, ypos, 0.0f);
	//glLoadMatrixf(&rot[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	b2Vec2 position2 = slopeBody2->GetPosition();
	b2PolygonShape *boxShape2 = static_cast<b2PolygonShape*>(slopeBody2->GetFixtureList()->GetShape());

	b2Vec2 v12 = boxShape2->m_vertices[0];
	b2Vec2 v22 = boxShape2->m_vertices[1];
	b2Vec2 v32 = boxShape2->m_vertices[2];
	b2Vec2 v42 = boxShape2->m_vertices[3];

	glBegin(GL_QUADS);
	glColor3f(1, 1, 0);

	//left bottom corner
	float xpos2 = position2.x + v12.x;
	float ypos2 = position2.y + v12.y;
	float w2 = v22.x - v12.x;
	float h2 = v42.y - v12.y;

	glVertex3f(xpos2, ypos2, 0.0f);
	glVertex3f(xpos2, (ypos2 + h2), 0.0f);
	glVertex3f(xpos2 + w2, (ypos2 + h2), 0.0f);
	glVertex3f(xpos2 + w2, ypos2, 0.0f);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	b2Vec2 position3 = platformBody->GetPosition();
	b2PolygonShape *boxShape3 = static_cast<b2PolygonShape*>(platformBody->GetFixtureList()->GetShape());

	b2Vec2 v13 = boxShape3->m_vertices[0];
	b2Vec2 v23 = boxShape3->m_vertices[1];
	b2Vec2 v33 = boxShape3->m_vertices[2];
	b2Vec2 v43 = boxShape3->m_vertices[3];

	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);

	//left bottom corner
	float xpos3 = position3.x + v13.x;
	float ypos3 = position3.y + v13.y;
	float w3 = v23.x - v13.x;
	float h3 = v43.y - v13.y;

	glVertex3f(xpos3, ypos3, 0.0f);
	glVertex3f(xpos3, (ypos3 + h3), 0.0f);
	glVertex3f(xpos3 + w3, (ypos3 + h3), 0.0f);
	glVertex3f(xpos3 + w3, ypos3, 0.0f);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	m_movingPlatform->render();

	#endif
}

void Level::update() {	}

void Level::fixedUpdate() {
	float posX = platformBody->GetTransform().p.x;
	if (right & posX > 300.0f) {
		velX = -1.0f * m_speed;
		right = false;
	}

	if (!right & posX < 50.0f) {
		velX = 1.0f * m_speed;
		right = true;
	}

	platformBody->SetLinearVelocity(b2Vec2(velX, 0.0f));

	m_movingPlatform->fixedUpdate();
}