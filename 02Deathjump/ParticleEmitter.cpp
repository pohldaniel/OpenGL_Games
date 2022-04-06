#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter() {
	m_batchrenderer = new Batchrenderer();
	m_batchrenderer->init();

	m_batchrenderer->resetStats();
	
	//m_batchrenderer->beginBatch2();
	//m_batchrenderer->addQuad(Vector2f(100.0f, 0.0f), Vector2f(16, 9), Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	//m_batchrenderer->endBatch2();
}

ParticleEmitter::ParticleEmitter(const Vector4f& birthColor, const Vector4f& deathColor, unsigned size) : m_size(size){
	
	m_batchrenderer = new Batchrenderer();	
	m_batchrenderer->init();

	m_birthColor = birthColor;
	m_deathColor = deathColor;
}

ParticleEmitter::~ParticleEmitter() {
	delete m_batchrenderer;
}

void ParticleEmitter::update(const float& dt) {
	m_elapsedTime += dt;
	while (m_elapsedTime >= m_updateTime) {
		m_elapsedTime -= m_updateTime;
		updateParticles();
	}
}

void ParticleEmitter::addParticles() {

	for (int i = m_particles.size(); i < m_max; i++) {
		float randX = Random::RandFloat(-m_spread, m_spread);
		float randY = Random::RandFloat(-m_spread, m_spread);

		if (m_direction == Vector2f()) {
			randX = Random::RandInt(0, 1) ? Random::RandFloat(-1.15f, -0.15f) : Random::RandFloat(0.15f, 1.15f);
			randY = Random::RandInt(0, 1) ? Random::RandFloat(-1.15f, -0.15f) : Random::RandFloat(0.15f, 1.15f);
		}

		Vector2f dir = Vector2f(
			m_direction[0] == 0.0f ? randX : m_direction[0] * Random::RandFloat(0.90f, 1.12f),
			m_direction[1] == 0.0f ? randY : m_direction[1] * Random::RandFloat(0.90f, 1.12f)
		);

		m_particles.push_back(Particle(Random::RandFloat(m_lifeTimeMin, m_lifeTimeMax), dir));
		addQuad(m_position);
		setQuadRotation(i, Random::RandInt(0, 180));
	}
}


void ParticleEmitter::addQuad(const Vector2f& pos) {
	std::array<Vertex, 4> vertices;

	vertices[0].posTex = Vector4f(pos[0]         , (float)HEIGHT -  pos[1]          , 0.0f, 0.0f);
	vertices[1].posTex = Vector4f(pos[0] + m_size, (float)HEIGHT -  pos[1]          , 1.0f, 0.0f);
	vertices[2].posTex = Vector4f(pos[0] + m_size, (float)HEIGHT - (pos[1] + m_size), 1.0f, 1.0f);
	vertices[3].posTex = Vector4f(pos[0]         , (float)HEIGHT - (pos[1] + m_size), 0.0f, 1.0f);

	for (auto& v : vertices) {
		v.color = m_birthColor;
		m_vertices.push_back(v);
	}
}

void ParticleEmitter::updateParticles() {
	for (int i = 0; i < m_particles.size(); i++) {
		auto& particle = m_particles[i];

		particle.lifeTime -= 0.1f;

		if (particle.lifeTime <= 0.0f) {
			deleteQuad(i);
			continue;
		}

		float life = particle.lifeTime / particle.constLifeTime;

		Vector4f color = lerp(m_deathColor, m_birthColor, life);
		setQuadColor(i, color);
		float scale = lerp((96.0f * m_size) / 100.0f, m_size, life);

		setQuadScale(i, scale / m_size);

		moveQuad(i, particle.direction);
	}
}

void ParticleEmitter::deleteQuad(unsigned index) {
	m_particles.erase(m_particles.begin() + index);

	index *= 4;
	for (int i = index; i < index + 4; i++)
		m_vertices.erase(m_vertices.begin() + index);
}

void ParticleEmitter::setQuadColor(unsigned index, const Vector4f& color) {
	index *= 4;

	for (int i = index; i < index + 4; i++)
		m_vertices[i].color = color;
}

void ParticleEmitter::setQuadScale(unsigned index, const float& scale) {
	index *= 4;

	float sizeX = (m_vertices[index].posTex[0] - m_vertices[index + 1].posTex[0]) / 2.0f;
	float sizeY = (m_vertices[index].posTex[1] - m_vertices[index + 3].posTex[1]) / 2.0f;

	float centX = m_vertices[index].posTex[0] - sizeX;
	float centY = m_vertices[index].posTex[1] - sizeY;

	for (int i = index; i < index + 4; i++) {
		scalePoint(scale, centX, centY, m_vertices[i].posTex[0], m_vertices[i].posTex[1]);
	}	
}

void ParticleEmitter::setQuadRotation(unsigned index, const float& angle) {
	index *= 4;

	float sizeX = (m_vertices[index].posTex[0] - m_vertices[index + 1].posTex[0]) / 2.0f;
	float sizeY = (m_vertices[index].posTex[1] - m_vertices[index + 3].posTex[1]) / 2.0f;

	float centX = m_vertices[index].posTex[0] - sizeX;
	float centY = m_vertices[index].posTex[1] - sizeY;

	for (int i = index; i < index + 4; i++) {
		rotatePoint(angle, centX, centY, m_vertices[i].posTex[0], m_vertices[i].posTex[1]);
	}
}

void ParticleEmitter::moveQuad(unsigned index, const Vector2f& dir) {
	index *= 4;

	const Vector2f velocity = Vector2f(m_movementSpeed * dir[0], m_movementSpeed * dir[1]);
	for (int i = index; i < index + 4; i++) {
		m_vertices[i].posTex[0] += velocity[0];
		m_vertices[i].posTex[1] += velocity[1];
	}
}

void ParticleEmitter::render() {
	m_batchrenderer->resetStats();
	m_batchrenderer->beginBatch();

	for (int i = 0; i < m_particles.size(); i++) {
		for (int j = i * 4; j < i * 4 + 4; j++) {
			m_batchrenderer->addVertex(Vector2f(m_vertices[j].posTex[0], m_vertices[j].posTex[1]), m_vertices[j].color);
		}		
	}

	m_batchrenderer->endBatch();
	m_batchrenderer->flush();
}


/*void ParticleEmitter::render() {
	m_batchrenderer->resetStats();
	m_batchrenderer->beginBatch();

	float sizeX = WIDTH / 100.0f;
	float sizeY = HEIGHT / 100.0f;
	float padding = 3;

	for (int x = 0; x < 100; x++) {
		for (int y = 0; y < 100; y++) {
			float xPos = (x * sizeX) + (padding * x);
			float yPos = (y * sizeY) + (padding * y);

			float w = WIDTH / 100.0f;
			float h = HEIGHT / 100.0f;

			m_batchrenderer->addQuad(Vector2f(xPos, yPos), Vector2f(w, h), Vector4f((xPos / (float)WIDTH), (yPos / (float)WIDTH), 0.0f, 1.0f));
		}
	}

	m_batchrenderer->endBatch();
	m_batchrenderer->flush();
}*/

void ParticleEmitter::render2() {
	m_batchrenderer->resetStats2();
	m_batchrenderer->beginBatch2();

	float sizeX = WIDTH / 100.0f;
	float sizeY = HEIGHT / 100.0f;
	float padding = 3;

	for (int x = 0; x < 100; x++) {
		for (int y = 0; y < 10; y++) {
			float xPos = (x * sizeX) + (padding * x);
			float yPos = (y * sizeY) + (padding * y);

			float w = WIDTH / 100.0f;
			float h = HEIGHT / 100.0f;

			std::array<Vertex, 4> vertices;

			vertices[0].posTex = Vector4f(xPos, (float)HEIGHT - yPos, 0.0f, 0.0f);
			vertices[1].posTex = Vector4f(xPos + w, (float)HEIGHT - yPos, 1.0f, 0.0f);
			vertices[2].posTex = Vector4f(xPos + w, (float)HEIGHT - (yPos + h), 1.0f, 1.0f);
			vertices[3].posTex = Vector4f(xPos, (float)HEIGHT - (yPos + h), 0.0f, 1.0f);

			for (auto& v : vertices) {
				v.color = Vector4f((xPos / (float)WIDTH), (yPos / (float)WIDTH), 0.0f, 1.0f);
				m_vertices.push_back(v);
			}
		}
	}

	m_batchrenderer->addQuad(m_vertices);
	m_batchrenderer->endBatch2();
	m_batchrenderer->flush2();
	m_vertices.resize(0);
}

void ParticleEmitter::setPosition(const Vector2f& pos) {
	m_position = pos;
}

void ParticleEmitter::setDirection(const Vector2f& dir) {
	m_direction = dir;
}

void ParticleEmitter::setLifeTimeRange(const float& min, const float& max) {
	m_lifeTimeMin = min;
	m_lifeTimeMax = max;
}

void ParticleEmitter::setSpeed(const float& speed) {
	m_movementSpeed = speed;
}
void ParticleEmitter::setBirthColor(Vector4f& color) {
	m_birthColor = color;
}
void ParticleEmitter::setDeathColor(Vector4f& color) {
	m_deathColor = color;
}

void ParticleEmitter::setParticleMax(unsigned max) {
	m_max = max;
}

void ParticleEmitter::setSpread(float spraed) {
	m_spread = spraed;
}

void ParticleEmitter::setSize(unsigned size) {
	m_size = size;
}

void ParticleEmitter::clear() {
	m_particles.clear();
	//m_vertices.clear();
}

Vector4f ParticleEmitter::lerp(const Vector4f& x, const Vector4f& y, const float& t) {
	return ((1.f - t) * x) + (t * y);
}

float ParticleEmitter::lerp(const float& x, const float& y, const float& t) {
	return x * (1.f - t) + y * t;
}


void ParticleEmitter::rotatePoint(float angle, float cx, float cy, float& x, float& y) {
	float s = sin(angle);
	float c = cos(angle);

	// translate point back to origin:
	x -= cx;
	y -= cy;

	// rotate point
	float xnew = x * c - y * s;
	float ynew = x * s + y * c;

	// translate point back:
	x = xnew + cx;
	y = ynew + cy;
}

void ParticleEmitter::scalePoint(float scale, float cx, float cy, float& x, float& y) {
	x -= cx;
	y -= cy;

	float xnew = x * scale;
	float ynew = y * scale;

	x = xnew + cx;
	y = ynew + cy;
}