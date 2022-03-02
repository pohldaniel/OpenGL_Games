#include "Player.h"

Player::Player(const float& dt, const float& fdt) : Entity(dt, fdt){	
	initAnimations();	
	initBody();
	initCollider();
	
	m_shaderArray = new Shader("shader/quad_array.vs", "shader/quad_array.fs");
	m_quad = new Quad(true, xScale * 2.0f, yScale * 2.0f, 1.0, 1.0);
}

Player::~Player() {
	delete m_textureAtlas;
	delete m_currentFrame;
}

void Player::fixedUpdate() {
	move();
}

void Player::update() {
	animate();
	updateVelocity();
	keepInBorders();
}

void Player::resolveCollision(Entity* entity) {
	Vector2f MTV;

	if (!GetCollider().CheckCollision(entity->GetCollider(), MTV) || m_hit)
		return;
	//Camera::Get().Shake(1.75f);

	m_hit = true;
	m_grabbing = false;
	m_movable = false;
	m_torque = 0.925f;
	m_velocity[1] = -950.0f;
	int multi = 1;
	m_velocity[0] = multi * 1200.0f;
}

void Player::resolveCollision(std::vector<Wall>& walls) {
	Vector2f MTV;
	m_grounded = false;

	for (auto& o : walls) {
		if (!GetCollider().CheckCollision(o.GetCollider(), MTV))
			continue;

		if (MTV[1] < 0.0f && m_velocity[1] >= 0.0f) {
			m_grounded = true;
			m_velocity[1] = 0.0f;
			crouch();

		}else if (MTV[1] > 0.0f && m_velocity[1] < 0.0f) {
			m_velocity[1] = 0.0f;
		}
	 
		i_collider.position += MTV;

		this->setPosition(i_collider.position);

		if (MTV[0] != 0.0f && m_velocity[1] > 0.0f && m_movable) {
			bool diffY = i_collider.position[1] - i_collider.size[1] / 2.0f > o.GetCollider().GetBody().position[1] - o.GetCollider().GetBody().size[1];

			if (!diffY)
				return;

			m_grabbing = true;
			m_velocity[1] = 0.0f;
			m_velocity[0] = 0.0f;

			bool diffX = this->getPosition()[0] > o.GetCollider().GetBody().position[0] + o.GetCollider().GetBody().size[0] / 2.0f;

			const int mulit = (diffX) ? -1 : 1;

			//body->setScale(Vector2f(2.0f * mulit, 2.0f));
			i_collider.position[1] += -(i_collider.position[1] - i_collider.size[1] / 2.0f - o.GetCollider().GetBody().position[1] + o.GetCollider().GetBody().size[1] / 2.0f);
		}

		MTV = Vector2f();
	}

	if (m_velocity[1] > 0.0f)
		m_wasGrounded = false;
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
		}else if (m_velocity[0] == 0) {
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
	if (i_collider.position[0] < 0.0f)
		i_collider.position[0] = 0.0f;
	else if (i_collider.position[0] > WIDTH)
		i_collider.position[0] = WIDTH;
}

void Player::crouch() {
	bool wasCrouching = m_crouching;
	m_crouching = Globals::CONTROLLS & Globals::KEY_S && m_grounded && m_movable;

	if (m_crouching) {
		m_velocity[0] = 0.0f;
		i_collider.size[1] = 40.0f;

		if (!wasCrouching)
			i_collider.position[1] += 20.0f;
	}else {
		i_collider.size[1] = 80.0f;
		if (wasCrouching)
			i_collider.position[1] -= 20.0f;
	}
}

void Player::move() {
	if (!m_grabbing)
		m_velocity[1] += m_gravity * i_fdt;
	m_velocity[0] *= m_torque;
	i_collider.position += m_velocity * i_fdt;

	float vel = abs(m_velocity[0]);
	m_velocity[0] = (vel < 8.0f ? 0.0f : m_velocity[0]);

	this->setPosition(m_grabbing ? Vector2f(i_collider.position[0] + 5.0f * 2.0f * 0.5f, i_collider.position[1]) : i_collider.position);
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

void Player::initAnimations() {
	//todo multiple time load texture
	m_textureAtlas = new unsigned int();
	m_currentFrame = new unsigned int();

	m_Animations["idle"].create("res/textures/player.png", 1, 6, 0.08f, *m_textureAtlas, *m_currentFrame);
	m_Animations["move"].create("res/textures/player.png", 3, 7, 0.08f, *m_textureAtlas, *m_currentFrame);
	m_Animations["jump"].create("res/textures/player.png", 4, 1, 0.125f, *m_textureAtlas, *m_currentFrame);
	m_Animations["fall"].create("res/textures/player.png", 6, 0, 0.1f, *m_textureAtlas, *m_currentFrame);
	m_Animations["crouch"].create("res/textures/player.png", 9, 5, 0.06f, *m_textureAtlas, *m_currentFrame);
	m_Animations["grab"].create("res/textures/player.png", 15, 0, 0.1f, *m_textureAtlas, *m_currentFrame);
	m_Animations["takedamage"].create("res/textures/player.png", 17, 5, 0.08f, *m_textureAtlas, *m_currentFrame);

}

void Player::initBody() {
	this->setSize(Vector2f(96.0f, 84.0f));
	this->setPosition(Vector2f(WIDTH, HEIGHT) / 2.0f);
}

void Player::initCollider() {
	const Vector2f size = Vector2f(40.0f, 80.0f);
	i_collider.position = Vector2f(WIDTH, HEIGHT) / 2.0f;
	i_collider.size = size;
}

void Player::render() const{	
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", m_transform);
	m_shaderArray->loadInt("u_layer", *m_currentFrame);
	m_quad->render(*m_textureAtlas, true);
	glUseProgram(0);

}
