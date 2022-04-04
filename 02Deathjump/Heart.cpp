#include "Heart.h"
#include "Random.h"

Heart::Heart(const float& dt, const float& fdt) : Entity(dt, fdt){
	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_quad = new Quad(true, 0.0f, 1.0f, 0.0f, 1.0f, m_heartSize[0], m_heartSize[1], 1.0f, 1.0f, 0, 0);
	
	initBody();
	initAnimations();
	initLight();
}

Heart::~Heart() {
	delete m_light;
}

void Heart::fixedUpdate() {
	constexpr float gravity = 300.0f;

	if (!m_grounded)
		m_collider.position[1] += gravity * m_fdt;

	setPosition(m_collider.position);
}

void Heart::initAnimations() {
	m_textureAtlas = new unsigned int();
	m_currentFrame = new unsigned int();
	m_animation.create(Globals::spritesheetManager.getAssetPointer("heart"), 0.1f, *m_textureAtlas, *m_currentFrame);
}

void Heart::initBody() {
	int pos = Random::RandInt(32, WIDTH - 32);

	setSize(Vector2f(m_quad->getScale()[0] * m_heartSize[0], m_quad->getScale()[0] * m_heartSize[1]));

	const Vector2f size = Vector2f(32.f, 25.f);

	m_collider.size = size;
	m_collider.position = Vector2f(pos, -100.0f);

	setPosition(m_collider.position);
	setOrigin(Vector2f((m_size[0] - size[0]) * 0.5f, -(size[1] + 7.0f)));
}

void Heart::initLight() {
	m_light = new Light(m_collider.position, 180.0f);
	m_light->setOrigin(Vector2f(m_collider.size[0] * 2.3f, m_collider.size[1] * 3.0f));
}

void Heart::updateLight() {
	m_light->setPosition(m_collider.position);
}

void Heart::update() {
	m_animation.update(m_dt);
	updateLight();

	if (m_realDeSpawnClock.getElapsedTimeSec() > m_deSpawnTime * 3.f)
		isAlive = false;


	if (m_deSpawnClock.getElapsedTimeSec() > m_deSpawnTime) {
		m_disappearAnimationTime -= 0.203f;
		m_disappear = true;
		m_deSpawnClock.restart();
	}

	if (m_disappear) {
		animateDisappear();
	}
}

void Heart::resolveCollision(std::vector<Wall>& walls) {
	for (auto& o : walls) {
		Vector2f MTV;

		if (!getCollider().checkCollision(o.getCollider(), MTV))
			continue;

		m_collider.position += MTV;
		m_grounded = true;
		break;
	}
}

void Heart::animateDisappear() {
	if (m_disappearClock.getElapsedTimeSec() > m_disappearAnimationTime) {
		m_currentColor++;
		if (m_currentColor > 4)
			m_currentColor = 0;
		m_disappearClock.restart();
	}
	m_blendColor = m_colors[m_currentColor];
}

void Heart::render(float deltaTime) {
	glEnable(GL_BLEND);
	glUseProgram(m_light->getShader().m_program);
	m_light->getShader().loadVector("u_color", Vector4f(0.75, 0.20, 0.70, 0.8));
	m_light->getShader().loadFloat("u_time", deltaTime);
	m_light->render();
	glUseProgram(0);

	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", m_transform * Globals::projection);
	m_shaderArray->loadInt("u_layer", *m_currentFrame);
	m_shaderArray->loadVector("u_blendColor", m_blendColor);
	m_quad->render(*m_textureAtlas, true);
	glUseProgram(0);	
	glDisable(GL_BLEND);

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