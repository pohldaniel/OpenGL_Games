#include "Ghost.h"
#include "Random.h"

Ghost::Ghost(const float& dt, const float& fdt) : Entity(dt, fdt) {

	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_quad = new Quad(true, 1.0f, -1.0f, m_size[0], m_size[1]);

	initBody();
	initCollider();
	initAnimations();
	//initEmitters();
}

Ghost::~Ghost() {

}

void Ghost::fixedUpdate() {
	const float velocity = 165.0f;

	m_collider.position += m_direction * velocity * i_fdt;
	m_collider.position[1] += 80.0f * (sinf(4 * m_clock.getElapsedTimeSec())) * i_fdt;
	setPosition(m_collider.position);

	//m_sprite->setPosition(i_collider.position);

	//LOG(i_collider.position.x, '\n');
}

void Ghost::update(Collider obj) {
	//UpdateLight();

	//m_emitter->Update(i_dt);
	//m_emitter->SetPosition(i_collider.position);

	if (m_blowUp) {
		m_animator.update(i_dt);

		if (m_animator.getCurrentFrame() == m_animator.getFrameCount() - 1)
			m_isAlive = false;

		return;
	}else
		//m_emitter->AddParticles();

	m_lifeTime -= i_dt;

	if (m_lifeTime < 2.0f) {
		float life = m_lifeTime / 2.0f;
		float alpha = lerp(0.0f, 1.0f, life);

		if (alpha <= 0.08f)
			m_isAlive = false;

		if (alpha < 0.55f)
			m_lightVal = alpha;

		//m_sprite->setColor(sf::Color(255, 255, 255, alpha * 255));
	}
	obj.position[1] += 35.0f;

	Vector2f diff = obj.position - m_collider.position;

	float magnitude = sqrt(diff[0] * diff[0] + diff[1] * diff[1]);

	m_direction = diff / magnitude;

	m_left = obj.position[0] > m_collider.position[0];

	m_animator.update(i_dt);
	m_quad->setFlipped(m_left);

	//m_sprite->setScale(m_left ? sf::Vector2f(-2.75f, 2.75f) : sf::Vector2f(2.75f, 2.75f));
}

float Ghost::lerp(const float& x, const float& y, const float& t) {
	return x * (1.f - t) + y * t;
}


void Ghost::initAnimations() {
	m_textureAtlas = new unsigned int();
	m_currentFrame = new unsigned int();
	m_animator.create(Globals::spritesheetManager.getAssetPointer("ghost"), 0.1f, *m_textureAtlas, *m_currentFrame);
}

void Ghost::initBody() {
	setPosition(Vector2f(WIDTH, HEIGHT) / 2.0f);
	setSize(Vector2f(m_quad->getScale()[0] * m_size[0], m_quad->getScale()[0] * m_size[1]));
	setOrigin(Vector2f(m_size[0] * 0.5f, m_size[1] / 1.315f));
}

void Ghost::initCollider() {
	float posX = Random::RandInt(0, 1) ? -100.0f : WIDTH + 100.0f;
	float posY = Random::RandInt(90, HEIGHT - 90);

	m_collider.position = Vector2f(posX, posY);
	m_collider.size = Vector2f(10.0f, 13.0f) * 2.75f;
}

void Ghost::render() const {
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", m_transform * Globals::projection);
	m_shaderArray->loadInt("u_layer", *m_currentFrame);
	m_quad->render(*m_textureAtlas, true);
	glUseProgram(0);
}