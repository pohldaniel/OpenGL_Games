#include "Player.h"
#include "Camera.h"
#include "Extern.h"

//#include "Key Check.hpp"

Player::Player(const float& dt, const float& fdt) : Entity(dt, fdt){
	
	InitAnimations();
	
	InitBody();
	InitCollider();
	
	m_shaderArray = new Shader("shader/quad_array.vs", "shader/quad_array.fs");
	m_quad = new Quad(true, xScale * 2.0f, yScale * 2.0f, 1.0, 1.0);

}

Player::~Player() {
	delete m_textureAtlas;
	delete m_currentFrame;
}

void Player::FixedUpdate() {
	Move();
}

void Player::Update() {
	Animate();
	UpdateVelocity();
	KeepInBorders();
	UpdateTimer();
}

void Player::ResolveCollision(Entity* entity) {
	Vector2f MTV;

	if (!GetCollider().CheckCollision(entity->GetCollider(), MTV) || m_hit)
		return;
	Camera::Get().Shake(1.75f);

	m_hit = true;
	m_grabbing = false;
	m_movable = false;
	m_torque = 0.925f;

	//m_Animations["takedamage"].SetFrame(0);

	m_velocity[1] = -950.0f;
	int multi = 1;
	m_velocity[0] = multi * 1200.0f;

	//reinterpret_cast<sf::Sprite*>(i_drawable)->setScale(sf::Vector2f(2.0f * -multi, 2.0f));
}

void Player::ResolveCollision(std::vector<Wall>& walls) {
	Vector2f MTV;

	m_grounded = false;

	//auto body = reinterpret_cast<sf::Sprite*>(i_drawable);

	for (auto& o : walls) {
		if (!GetCollider().CheckCollision(o.GetCollider(), MTV))
			continue;

		if (MTV[1] < 0.0f && m_velocity[1] >= 0.0f) {
			m_grounded = true;
			m_velocity[1] = 0.0f;
			Crouch();
		}
		else if (MTV[1] > 0.0f && m_velocity[1] < 0.0f) {
			m_velocity[1] = 0.0f;
		}

		i_collider.position += MTV;
		//body->setPosition(i_collider.position);

		if (MTV[0] != 0.0f && m_velocity[1] > 0.0f && m_movable) {
			bool diffY = i_collider.position[1] - i_collider.size[1] / 2.0f > o.GetCollider().GetBody().position[1] - o.GetCollider().GetBody().size[1];

			if (!diffY)
				return;

			m_grabbing = true;

			m_velocity[1] = 0.0f;
			m_velocity[0] = 0.0f;

			//bool diffX = body->getPosition()[0] > o.GetCollider().GetBody().position[0] + o.GetCollider().GetBody().size[0] / 2.0f;

			//const int mulit = (diffX) ? -1 : 1;

			//body->setScale(Vector2f(2.0f * mulit, 2.0f));
			i_collider.position[1] += -(i_collider.position[1] - i_collider.size[1] / 2.0f - o.GetCollider().GetBody().position[1] + o.GetCollider().GetBody().size[1] / 2.0f);
		}

		MTV = Vector2f();
	}

	if (m_velocity[1] > 0.0f)
		m_wasGrounded = false;
}

bool Player::IsAlive() const {
	return m_alive;
}

void Player::UpdateTimer() {
	if (m_hit) {
		if (m_currentHitTake > 9) {
			m_currentHitTake = 0;
			m_hit = false;
			return;
		}

		unsigned alpha[2] =
		{
			60,
			255,
		};

		//auto body = reinterpret_cast<sf::Sprite*>(i_drawable);

		//sf::Color c = body->getColor();

		//body->setColor(sf::Color(c.r, c.g, c.b, alpha[(!(m_currentHitTake % 2) ? 0 : 1)]));

		/*if (m_hitTimer.getElapsedTime().asSeconds() > 0.175f) {
			m_currentHitTake++;
			m_hitTimer.restart();
		}*/
	}
}

void Player::Animate() {

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

void Player::KeepInBorders() {
	if (i_collider.position[0] < 0.0f)
		i_collider.position[0] = 0.0f;
	else if (i_collider.position[0] > WIDTH)
		i_collider.position[0] = WIDTH;
}

void Player::Crouch() {
	bool wasCrouching = m_crouching;
	// yeah i know
	m_crouching =  m_grounded && m_movable;

	if (m_crouching) {
		m_velocity[0] = 0.0f;
		i_collider.size[1] = 40.0f;

		if (!wasCrouching)
			i_collider.position[1] += 20.0f;

		//reinterpret_cast<sf::Sprite*>(i_drawable)->setOrigin(sf::Vector2f(m_playerSize[0] / 2, m_playerSize[1] / 1.135f));
	}else {
		i_collider.size[1] = 80.0f;
		if (wasCrouching)
			i_collider.position[1] -= 20.0f;
	}
}

void Player::Move() {
	//if (!m_grabbing)
		//m_velocity[1] += m_gravity * i_fdt;
	m_velocity[0] *= m_torque;
	//i_collider.position += m_velocity * i_fdt;

	float vel = abs(m_velocity[0]);
	m_velocity[0] = (vel < 8.0f ? 0.0f : m_velocity[0]);

	/*auto body = reinterpret_cast<sf::Sprite*>(i_drawable);

	body->setOrigin(Vector2f(m_playerSize[0] / 2, m_playerSize[1] / 1.315f));

	if (m_crouching)
		body->setOrigin(Vector2f(m_playerSize[0] / 2, m_playerSize[1] / 1.135f));

	body->setPosition((m_grabbing ? Vector2f(i_collider.position[0] + 5.0f * body->getScale()[0] / 2.0f, i_collider.position[1]) : i_collider.position));*/
}

void Player::UpdateVelocity() {
	if (m_crouching || !m_movable)
		return;



	if ((m_grounded && Globals::CONTROLLS & Globals::KEY_W) ){
		//m_grounded = false;
		//m_grabbing = false;
		m_velocity[1] = m_jumpVelocity;
		m_Animations["jump"].setCurrentFrame(0);

		std::cout << m_velocity[1] << std::endl;
	}

	/*if (m_grabbing && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		m_grabbing = false;*/

	if (m_grabbing)
		return;

	if (Globals::CONTROLLS & Globals::KEY_A) {
		m_velocity[0] = -m_movementSpeed;
		m_quad->setFlipped(true);
		//reinterpret_cast<sf::Sprite*>(i_drawable)->setScale(sf::Vector2f(-2.0f, 2.0f));
	}

	if (Globals::CONTROLLS & Globals::KEY_D) {
		m_velocity[0] = m_movementSpeed;
		m_quad->setFlipped(false);
		//reinterpret_cast<sf::Sprite*>(i_drawable)->setScale(sf::Vector2f(2.0f, 2.0f));
	}

}

void Player::InitAnimations() {
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

void Player::InitBody() {
	/*i_drawable = new sf::Sprite(*i_texture);

	auto sprite = reinterpret_cast<sf::Sprite*>(i_drawable);

	sprite->setOrigin(sf::Vector2f(m_playerSize[0] / 2, m_playerSize[1] / 1.315f));
	sprite->setScale(sf::Vector2f(2.f, 2.f));*/
}

void Player::InitCollider() {
	/*const sf::Vector2f size = sf::Vector2f(40.0f, 80.0f);

	i_collider.position = sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT) / 2.0f;
	i_collider.size = size;*/
}

void Player::draw() {	
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_shaderArray->loadInt("u_layer", *m_currentFrame);
	m_quad->render(*m_textureAtlas, true);
	glUseProgram(0);

}
