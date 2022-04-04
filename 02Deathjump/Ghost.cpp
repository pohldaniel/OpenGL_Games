#include "Ghost.h"
#include "Random.h"

Ghost::Ghost(const float& dt, const float& fdt) : Entity(dt, fdt) {

	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_quad = new Quad(true, 0.0f, 1.0f, 0.0f, 1.0f, m_sizeGhost[0], m_sizeGhost[1], 1.0f, 1.0f, 0, 0);

	initBody();
	initCollider();
	initAnimations();
	initEmitter();
	initLight();
}

Ghost::~Ghost() {
	delete m_quad;
	delete m_emitter;
	delete m_light;
}

void Ghost::fixedUpdate() {
	const float velocity = 165.0f;

	m_collider.position += m_direction * velocity * m_fdt;
	m_collider.position[1] += 80.0f * (sinf(4 * m_clock.getElapsedTimeSec())) * m_fdt;
	setPosition(m_collider.position);
}

void Ghost::update(Collider obj) {
	updateLight();

	m_emitter->update(m_dt);
	m_emitter->setPosition(m_collider.position + m_size * 0.5f);

	if (m_blowUp) {		
		m_emitter->addParticles();
		m_animator.update(m_dt);

		if (m_animator.getCurrentFrame() == m_animator.getFrameCount() - 1)
			m_isAlive = false;

		return;
	}

	m_lifeTime -= m_dt;

	if (m_lifeTime < 2.0f) {
		float life = m_lifeTime / 2.0f;
		float alpha = lerp(0.0f, 1.0f, life);

		if (alpha <= 0.08f)
			m_isAlive = false;

		if (alpha < 0.55f)
			m_lightVal = alpha;

		m_blendColor = Vector4f(1.0f, 1.0f, 1.0f, alpha);
	}
	obj.position[1] += 35.0f;

	Vector2f diff = obj.position - m_collider.position;

	float magnitude = sqrt(diff[0] * diff[0] + diff[1] * diff[1]);
	m_direction = diff / magnitude;
	m_left = obj.position[0] > m_collider.position[0];
	m_animator.update(m_dt);
	m_quad->setFlipped(m_left);

	setOrigin(Vector2f(m_left ? m_collider.size[0] * 0.55f : m_collider.size[0] * 0.8f, m_collider.size[1] * 0.9 - m_size[1]));
}

float Ghost::lerp(const float& x, const float& y, const float& t) {
	return x * (1.f - t) + y * t;
}

void Ghost::updateLight() {
	m_light->setPosition(m_collider.position);
}

void Ghost::initLight() {
	m_light = new Light(m_collider.position, 290.0f);
	m_light->setOrigin(Vector2f(m_collider.size[0] * 1.5f, m_collider.size[1] * 1.8f));
}

void Ghost::initEmitter() {
	m_emitter = new ParticleEmitter(Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Vector4f(0.0f, 0.0f, 0.0f, 0.0f), 25);
	m_emitter->setLifeTimeRange(0.8f, 2.0f);
	m_emitter->setSpeed(4.1f);
	m_emitter->setPosition(m_collider.position + m_size * 0.5f);
}

void Ghost::initAnimations() {
	m_textureAtlas = new unsigned int();
	m_currentFrame = new unsigned int();
	m_animator.create(Globals::spritesheetManager.getAssetPointer("ghost"), 0.1f, *m_textureAtlas, *m_currentFrame);
}

void Ghost::initBody() {
	setSize(Vector2f(m_quad->getScale()[0] * m_sizeGhost[0], m_quad->getScale()[1] * m_sizeGhost[1]));	
}

void Ghost::initCollider() {
	float posX = Random::RandInt(0, 1) ? -100.0f : WIDTH + 100.0f;
	float posY = Random::RandInt(90, HEIGHT - 90);

	m_collider.position = Vector2f(posX, posY);
	m_collider.size = Vector2f(75.0f, 60.0f);
	setPosition(Vector2f(posX, posY));
}

void Ghost::render(float deltaTime){
	glEnable(GL_BLEND);
	if (!m_blowUp) {
		glUseProgram(m_shaderArray->m_program);
		m_shaderArray->loadMatrix("u_transform", m_transform * Globals::projection);
		m_shaderArray->loadInt("u_layer", *m_currentFrame);
		m_shaderArray->loadVector("u_blendColor", m_blendColor);
		m_quad->render(*m_textureAtlas, true);
		glUseProgram(0);
	}	
	m_emitter->render();	
	glUseProgram(m_light->getShader().m_program);
	m_light->getShader().loadVector("u_color", Vector4f(0.9, 0.9, 0.9, m_lightVal));
	m_light->getShader().loadFloat("u_time", deltaTime);
	m_light->render();
	glUseProgram(0);
	glDisable(GL_BLEND);

	//Debug colider
	#if DEBUGCOLLISION
	Matrix4f transProj = Globals::projection.transpose();
	glMatrixMode(GL_PROJECTION);										
	glLoadIdentity();														
	glLoadMatrixf(&transProj[0][0]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glColor3f(1, 1, 0); 

	float xpos = m_collider.position[0];
	float ypos = m_collider.position[1];
	float w = m_collider.size[0];
	float h = m_collider.size[1];

	glVertex3f(xpos,     HEIGHT - ypos,     0.0f);
	glVertex3f(xpos,     HEIGHT - (ypos + h), 0.0f);
	glVertex3f(xpos + w, HEIGHT - (ypos + h), 0.0f);
	glVertex3f(xpos + w, HEIGHT - ypos,     0.0f);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	#endif
}