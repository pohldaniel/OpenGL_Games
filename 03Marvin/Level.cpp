#include "Level.h"

Level::Level(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt), m_mapLoader(MapLoader::get()){
	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_shaderArray = Globals::shaderManager.getAssetPointer("level");
	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("base");
	m_quadBackground = new Quad(false, -1.0f, 1.0f, -1.0f, 1.0f);
	m_sprites["background"] = Globals::textureManager.get("background").getTexture();

	//m_mapLoader.loadLevel("res/Maps/easypeasy2.json");
	m_playerPosition = m_mapLoader.m_playerPosition;

	createBuffer();
}

void Level::createBuffer() {
	short stride = 5, offset = 3;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_vboMap);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_mapLoader.m_vertices.size() * sizeof(float), &m_mapLoader.m_vertices[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	//map indices
	glBindBuffer(GL_ARRAY_BUFFER, m_vboMap);
	glBufferData(GL_ARRAY_BUFFER, m_mapLoader.m_indexMap.size() * sizeof(m_mapLoader.m_indexMap[0]), &m_mapLoader.m_indexMap[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 1 * sizeof(unsigned int), 0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mapLoader.m_indexBuffer.size() * sizeof(unsigned int), &m_mapLoader.m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Level::render() {
	
	glUseProgram(m_shader->m_program);
	//m_shader->loadMatrix("u_transform", m_transBackground);
	m_quadBackground->render(m_sprites["background"]);
	glUseProgram(0);

	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", Globals::projection * ViewEffect::get().getView());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_spriteSheet->getAtlas());
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_mapLoader.m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glUseProgram(0);

	for (RenderableObject* object : m_mapLoader.m_renObjects) {
		if (!object->isDisabled()) {
			object->render();
		}
	}

	for (Entity* object : m_mapLoader.m_entities2) {
		if (!object->isDisabled()) {
			object->render();
		}
	}

	#if DEBUGCOLLISION
	int index = 0;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&Globals::projection[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&ViewEffect::get().getView()[0][0]);
	
	for (const auto& b : m_mapLoader.m_contours) {
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
	for (const auto& body : m_mapLoader.m_phyObjects) {
		
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
	//remove();

	for (Entity* object : m_mapLoader.m_entities2) {
		if (!object->isDisabled()) {
			object->update();
		}
	}

	if (Globals::CONTROLLS & Globals::KEY_Q) {
		reset();
	}
}

void Level::fixedUpdate() {

}

void Level::remove() {
	m_mapLoader.m_renObjects.erase(std::remove_if(m_mapLoader.m_renObjects.begin(), m_mapLoader.m_renObjects.end(), std::mem_fn(&RenderableObject::isDisabled)) , m_mapLoader.m_renObjects.end());
}

void Level::reset() {
	for (b2Body* object : m_mapLoader.m_phyObjects) {

		for (b2Fixture *fixture = object->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
			b2Filter filter = fixture->GetFilterData();
			filter.categoryBits &= ~Category::Type::None;
			fixture->SetFilterData(filter);
		}		
	}

	//std::transform(m_mapLoader.m_renObjects.begin(), m_mapLoader.m_renObjects.end(), m_mapLoader.m_renObjects.begin(), [](RenderableObject* c) -> RenderableObject* { c->setDisabled(false); return  c; });
	std::for_each(m_mapLoader.m_renObjects.begin(), m_mapLoader.m_renObjects.end(), [](RenderableObject* c) { c->setDisabled(false); });
}