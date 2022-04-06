#include "Fireball.h"

Fireball::Fireball(const float& dt, const float& fdt, float velocity, bool left) : Entity(dt, fdt), m_velocity(velocity) {
	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_shader = Globals::shaderManager.getAssetPointer("quad");

	m_quad = new Quad(true, 0.0f, 1.0f, 0.0f, 1.0f, m_fireballSize[0], m_fireballSize[1]);
	m_quadBlow = new Quad(true, 0.0f, 2.0f, 0.0f, 2.0f, m_blowSize[0], m_blowSize[1]);

	m_left = left;	
	initSprites();
	initCollider(Vector2f(left ? -100.f : WIDTH + 100.f, m_positions[Random::RandInt(0, 12)]));	
	initAnimations();
	initEmitter();
	initLight();
}

Fireball::~Fireball() {
	delete m_quad;
	delete m_quadBlow;
	delete m_light;
	delete m_emitter;
}

void Fireball::fixedUpdate() {
	if (!m_blowUp)
		move();
}

void Fireball::update() {	
	updateLight();
	updateEmitter();
	animate();

	if (m_collider.position[0] > WIDTH + 300.0f || m_collider.position[0] < -300.0f)
		m_isAlive = false;
}

bool Fireball::isAlive() const {
	return m_isAlive;
}

void Fireball::move() {
	int multi = (m_left ? 1 : -1);

	m_collider.position += Vector2f(m_velocity * multi * m_fdt, 0.0f);
	setPosition(m_collider.position);
}

void Fireball::updateLight() {
	m_light->setPosition(m_collider.position);
}

void Fireball::initLight() {
	m_light = new Light(m_collider.position, 290.0f);
	m_light->setOrigin(m_collider.size * 1.75f);
}

void Fireball::animate() {
	if (m_blowUp) {
		if (m_Animations["blow_up"].getCurrentFrame() == m_Animations["blow_up"].getFrameCount() - 1) {
			m_isAlive = false;
			return;
		}
		setPosition(m_collider.position);
		m_Animations["blow_up"].update(m_dt);
	}
}

void Fireball::initAnimations() {
	m_textureAtlas = new unsigned int();
	m_currentFrame = new unsigned int();
	m_Animations["blow_up"].create(Globals::spritesheetManager.getAssetPointer("blow_up"), 0.05f, *m_textureAtlas, *m_currentFrame);
}

void Fireball::initSprites() {
	m_sprites["fireball"] = Globals::textureManager.get("fireball").getTexture();		
	setSize(Vector2f(m_quad->getScale()[0] * m_fireballSize[0], m_quad->getScale()[1] * m_fireballSize[1]));
	m_quad->setFlipped(m_left);	
	originBlow = Vector2f(m_blowSize[0] * m_quadBlow->getScale()[0], -m_blowSize[1] * m_quadBlow->getScale()[1]) * 0.5;
}

void Fireball::initCollider(Vector2f position) {
	const Vector2f size = Vector2f(64.f, 64.f);
	m_collider.size = size;
	m_collider.position = position;
	
	setPosition(m_collider.position);
	setOrigin(0.0f, -m_size[1]);
}

void Fireball::updateEmitter() {
	if (!m_blowUp)
		m_emitter->addParticles();
	m_emitter->update(m_dt);
	m_emitter->setPosition(m_collider.position + m_size * 0.5f);
}

void Fireball::initEmitter() {
	m_emitter = new ParticleEmitter(Vector4f(1.0f, 1.0f, 0.0f, 1.0f), Vector4f(1.0f, 0.0f, 0.0f, 0.0f), 20);
	m_emitter->setDirection(Vector2f(m_left ? -1 : 1, 0.0f));
	m_emitter->setLifeTimeRange(0.8f, 2.0f);
	m_emitter->setSpeed(4.1f);
	m_emitter->setPosition(m_collider.position + m_size * 0.5f);
}

void Fireball::render(float deltaTime) {
	glEnable(GL_BLEND);
	m_emitter->render();
	

	if (!m_blowUp) {
		glUseProgram(m_shader->m_program);
		m_shader->loadMatrix("u_transform", m_transform * Globals::projection);
		m_quad->render(m_sprites.at("fireball"));
		glUseProgram(0);
	}else {
		m_blowTrans.translate((m_collider.position[0] - originBlow[0]), (HEIGHT - m_collider.position[1] + originBlow[1]), 0.0f);
		glUseProgram(m_shaderArray->m_program);
		m_shaderArray->loadMatrix("u_transform", m_blowTrans * Globals::projection);
		m_shaderArray->loadInt("u_layer", *m_currentFrame);
		m_shaderArray->loadVector("u_blendColor", Vector4f(1.0f, 1.0f, 1.0f, 0.6f));
		m_quadBlow->render(*m_textureAtlas, true);
		glUseProgram(0);
	}

	glUseProgram(m_light->getShader().m_program);
	m_light->getShader().loadVector("u_color", Vector4f(0.75, 0.42, 0.28, 0.72));
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

	glVertex3f(xpos, HEIGHT - ypos, 0.0f);
	glVertex3f(xpos, HEIGHT - (ypos + h), 0.0f);
	glVertex3f(xpos + w, HEIGHT - (ypos + h), 0.0f);
	glVertex3f(xpos + w, HEIGHT - ypos, 0.0f);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	#endif
}
