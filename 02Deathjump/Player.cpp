#include "Player.h"
#include "Fireball.h"

Player::Player(const float& dt, const float& fdt) : Entity(dt, fdt){		
	
	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	//m_quad = new Quad(true, 1.0f, -1.0f, m_playerSize[0], m_playerSize[1]);
	m_quad = new Quad(true, 0.0f, 1.0f, 0.0f, 1.0f, m_playerSize[0], m_playerSize[1], 1.0f, 1.0f, 0, 0);

	initBody();
	initCollider();
	initAnimations();
	initEmitters();
}

Player::~Player() {
	delete m_quad;
	delete m_emitter;
	delete m_fallEmitter;
}

void Player::fixedUpdate() {
	move();
}

void Player::update() {
	animate();
	updateVelocity();
	updateEmitters();
	keepInBorders();
	updateTimer();
}

void Player::resolveCollision(Entity* entity) {
	Vector2f MTV;

	if (!getCollider().checkCollision(entity->getCollider()) || m_hit)
		return;
		
	auto fireball = reinterpret_cast<Fireball*>(entity);
	fireball->m_blowUp = true;
	fireball->m_emitter->clear();
	fireball->m_emitter->setDirection(Vector2f(0, 0));
	fireball->m_emitter->setSpeed(5.4f);
	fireball->m_emitter->setParticleMax(80);
	fireball->m_emitter->setSize(25);
	fireball->m_emitter->setLifeTimeRange(1.5f, 3.2f);
	fireball->m_emitter->addParticles();

	//Camera::Get().Shake(1.75f);

	m_hit = true;
	m_grabbing = false;
	m_movable = false;
	m_torque = 0.925f;
	m_velocity[1] = -950.0f;
	int multi = (reinterpret_cast<Fireball*>(entity)->m_left ? 1 : -1);
	m_velocity[0] = multi * 1200.0f;
}

void Player::resolveCollision(Ghost* entity, std::vector<Ghost*>& ghosts) {
	Vector2f MTV;
	
	if (!getCollider().checkCollision(entity->getCollider(), MTV) || m_hit || entity->m_blowUp)
		return;

	//m_ghost.setVolume(Extern::sound_volume);
	//m_ghost.play();

	if (MTV[1] < 0.0f && m_velocity[1] != 0.0f) {
		// i know do not scream
		entity->m_blowUp = true;
		entity->m_emitter->clear();
		entity->m_emitter->setDirection(Vector2f(0.0f, 0.0));
		entity->m_emitter->setSpeed(5.4f);
		entity->m_emitter->setParticleMax(80);
		entity->m_emitter->setSize(25);
		entity->m_emitter->setLifeTimeRange(1.5f, 3.2f);
		entity->m_emitter->addParticles();

		entity->m_animator.setCurrentFrame(0);
		entity->m_animator.setUpdateTime(0.1f);

		//MusicController::Get().GetSound("ghost").play();
		
		m_velocity[1] = m_jumpVelocity + 400.0f;
		m_Animations["jump"].setCurrentFrame(0);

		//MusicController::Get().GetSound("player_jump").play();
		return;
	}


	// i know do not scream
	for (auto& g : ghosts) {
		g->m_blowUp = true;
		g->m_emitter->clear();
		g->m_emitter->setDirection(Vector2f(0.0f, 0.0));
		g->m_emitter->setSpeed(5.4f);
		g->m_emitter->setParticleMax(80);
		g->m_emitter->setSize(25);
		g->m_emitter->setLifeTimeRange(1.5f, 3.2f);
		g->m_emitter->addParticles();

		g->m_animator.setCurrentFrame(0);
		g->m_animator.setUpdateTime(0.1f);
	}

	//m_healthBar.Change(true);

	//MusicController::Get().GetSound("blow_up").play();
	//MusicController::Get().GetSound("ghost").play();
	//Camera::Get().Shake(1.75f);

	m_hit = true;
	m_grabbing = false;
	m_movable = false;
	m_torque = 0.925f;

	//m_Animations["takedamage"].SetFrame(0);

	m_velocity[1] = -950.0f;
	int multi = entity->m_left ? 1 : -1;
	m_velocity[0] = multi * 1200.0f;

	//reinterpret_cast<sf::Sprite*>(i_drawable)->setScale(sf::Vector2f(2.0f * -multi, 2.0f));
}

void Player::resolveCollision(std::vector<Wall>& walls) {
	
	Vector2f MTV = m_collider.position;
	m_grounded = false;

	for (auto& o : walls) {
		if (!getCollider().checkCollision(o.getCollider(), MTV))
			continue;
		
		if (MTV[1] < 0.0f && m_velocity[1] >= 0.0f) {
			m_grounded = true;
			m_velocity[1] = 0.0f;
			crouch();
		}else if (MTV[1] > 0.0f && m_velocity[1] < 0.0f) {
			m_velocity[1] = 0.0f;
		}

		m_collider.position += MTV;

		if (MTV[0] != 0.0f && m_velocity[1] > 0.0f && m_movable) {
			bool diffY = abs(o.getCollider().getBody().position[1] - m_collider.position[1]) < 5.0f;

			if (!diffY)
				return;

			m_grabbing = true;
			m_velocity[1] = 0.0f;
			m_velocity[0] = 0.0f;

			m_collider.position[1] = o.getCollider().getBody().position[1];
		}
		setPosition(m_collider.position);
	}

	if (m_velocity[1] > 0.0f)
		m_wasGrounded = false;
}

void Player::updateTimer(){
	if (m_hit) {
		if (m_currentHitTake > 9) {
			m_currentHitTake = 0;
			m_hit = false;
			return;
		}

		unsigned alpha[2] = {60, 255};

		/*auto body = reinterpret_cast<sf::Sprite*>(i_drawable);

		Vector4f c = body->getColor();

		body->setColor(Vector4f(c[0], c[1], c[2], alpha[(!(m_currentHitTake % 2) ? 0 : 1)]));*/
		if (m_hitTimer.getElapsedTimeSec() > 0.175f) {		
			m_currentHitTake++;
			m_hitTimer.restart();
		}
	}
}

bool Player::isAlive() const {
	return m_alive;
}

void Player::animate() {

	if (!m_movable) {
		m_Animations["takedamage"].update(i_dt);
		if (m_Animations["takedamage"].getCurrentFrame() == m_Animations["takedamage"].getFrameCount() - 1) {
			m_Animations["takedamage"].setCurrentFrame(0);
			m_movable = true;
			m_torque = 0.85f;
		}
		return;
	}

	if (m_grabbing) {
		m_Animations["grab"].update(i_dt);
		return;
	}

	if (m_crouching) {
		m_Animations["crouch"].update(i_dt);
		return;
	}

	if (m_velocity[1] == 0.0f) { // NOT JUMPING
		if (m_velocity[0] < 0) {
			m_Animations["move"].update(i_dt);
		}else if (m_velocity[0] > 0) {
			m_Animations["move"].update(i_dt);
		}else if (m_grounded && m_velocity[0] == 0) {
			m_Animations["idle"].update(i_dt);
		}
	}else {
		if (m_velocity[1] < 0) {
			if (m_Animations["jump"].getCurrentFrame() != m_Animations["jump"].getFrameCount() - 1)
				m_Animations["jump"].update(i_dt);
		}if (m_velocity[1] > 0) {
			m_Animations["fall"].update(i_dt);
		}
	}
}

void Player::keepInBorders() {
	if (m_collider.position[0] < 0.0f)
		m_collider.position[0] = 0.0f;
	else if (m_collider.position[0] > WIDTH - m_collider.size[0])
		m_collider.position[0] = WIDTH - m_collider.size[0];
}

void Player::crouch() {
	bool wasCrouching = m_crouching;
	m_crouching = Globals::CONTROLLS & Globals::KEY_S && m_grounded && m_movable;

	if (m_crouching) {
		m_velocity[0] = 0.0f;
		m_collider.size[1] = 40.0f;

		if (!wasCrouching) {
			m_collider.position[1] += 40.0f;
			setOrigin(m_origin[0], m_origin[1] + 40.0f);			
		}
		
	}else {
		m_collider.size[1] = 80.0f;
		if (wasCrouching) {
			m_collider.position[1] -= 40.0f;
			setOrigin(m_origin[0], m_origin[1] - 40.0f);			
		}
	}
}

void Player::move() {
	if (!m_grabbing)
		m_velocity[1] += m_gravity * i_fdt;
	m_velocity[0] *= m_torque;
	m_collider.position += m_velocity * i_fdt;

	float vel = abs(m_velocity[0]);
	m_velocity[0] = (vel < 8.0f ? 0.0f : m_velocity[0]);

	setPosition(m_grabbing ? Vector2f(m_collider.position[0] + 5.0f * 2.0f * 0.5f, m_collider.position[1]) : m_collider.position);
}

void Player::updateVelocity() {
	if (m_crouching || !m_movable)
		return;

	if ((m_grounded && Globals::CONTROLLS & Globals::KEY_W) || (m_grabbing && Globals::CONTROLLS & Globals::KEY_W)){
		m_grounded = false;
		m_grabbing = false;
		m_velocity[1] = m_jumpVelocity;
		m_Animations["jump"].setCurrentFrame(0);
	}

	if (m_grabbing && (Globals::CONTROLLS & Globals::KEY_S)) {
		m_grabbing = false;
	}

	if (m_grabbing)
		return;

	if (Globals::CONTROLLS & Globals::KEY_A) {
		m_velocity[0] = -m_movementSpeed;
		m_quad->setFlipped(true);
	}

	if (Globals::CONTROLLS & Globals::KEY_D) {
		m_velocity[0] = m_movementSpeed;
		m_quad->setFlipped(false);
	}
}

void Player::updateEmitters() {
	m_emitter->setDirection(Vector2f((m_velocity[0] < 0.0f ? 1.0f : -1.0f), 0.0f));
	m_emitter->setPosition(m_collider.position + m_collider.size * 0.25f);
	if (m_velocity[0] != 0.0f)
		m_emitter->addParticles();
	m_emitter->update(i_dt);

	m_fallEmitter->setPosition(Vector2f(m_collider.position[0] +  m_collider.size[1] * 0.25f, m_collider.position[1] + m_collider.size[1] * 0.7f));
	if (!m_wasGrounded && m_grounded) {
		m_fallEmitter->addParticles();
		m_wasGrounded = true;
	}
	m_fallEmitter->update(i_dt);
}

void Player::initAnimations() {
	//todo multiple time load texture
	m_textureAtlas = new unsigned int();
	m_currentFrame = new unsigned int();
	
	m_Animations["move"].create("res/textures/player.png", 96, 84, 3, 7, 0.08f, *m_textureAtlas, *m_currentFrame);
	m_Animations["jump"].create("res/textures/player.png", 96, 84, 4, 1, 0.125f, *m_textureAtlas, *m_currentFrame);
	m_Animations["fall"].create("res/textures/player.png", 96, 84, 6, 0, 0.1f, *m_textureAtlas, *m_currentFrame);
	m_Animations["crouch"].create("res/textures/player.png", 96, 84, 9, 5, 0.06f, *m_textureAtlas, *m_currentFrame);
	m_Animations["grab"].create("res/textures/player.png", 96, 84, 15, 0, 0.1f, *m_textureAtlas, *m_currentFrame);
	m_Animations["takedamage"].create("res/textures/player.png", 96, 84, 17, 5, 0.08f, *m_textureAtlas, *m_currentFrame);

	m_Animations["idle"].create("res/textures/player.png", 96, 84, 1, 6, 0.08f, *m_textureAtlas, *m_currentFrame);
}

void Player::initBody() {	
	setSize(Vector2f(m_quad->getScale()[0] * m_playerSize[0], m_quad->getScale()[0] * m_playerSize[1]));
}

void Player::initCollider() {
	const Vector2f size = Vector2f(40.0f, 80.0f);
	m_collider.position = Vector2f(WIDTH - 40.0f, HEIGHT) * 0.5f;
	m_collider.size = size;

	setPosition(m_collider.position);
	setOrigin(Vector2f((m_size[0] - size[0]) * 0.5f, -size[1]));
}

void Player::initEmitters() {
	m_emitter = new ParticleEmitter(Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 0.0f), 15);
	m_emitter->setLifeTimeRange(0.5f, 2.0f);
	m_emitter->setSpeed(1.5f);

	m_fallEmitter = new ParticleEmitter(Vector4f(0.31f, 0.18f, 0.1f, 1.0f), Vector4f(0.08f, 0.19f, 0.071, 0.0f), 13);
	m_fallEmitter->setLifeTimeRange(0.5f, 1.5f);
	m_fallEmitter->setDirection(Vector2f(0, -1));
	m_fallEmitter->setSpread(4.59f);
	m_fallEmitter->setSpeed(1.1f);
	m_fallEmitter->setParticleMax(35);
}

void Player::render(){	
	glEnable(GL_BLEND);
	m_emitter->render();
	m_fallEmitter->render();
	glDisable(GL_BLEND);
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", m_transform * Globals::projection);
	m_shaderArray->loadInt("u_layer", *m_currentFrame);
	m_quad->render(*m_textureAtlas, true);
	glUseProgram(0);

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
