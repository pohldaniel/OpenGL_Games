#include "Player.h"
#include "CharacterController_cs.h"
Player::Player(const float& dt, const float& fdt) : Entity(Category::Type::Player, dt, fdt) {

	m_size = Vector2f(26.0f, 26.0f);
	m_position = Vector2f(200.0f, 600.0f);

	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_quad = new Quad(true, 0.0f, 1.0f, 0.0f, 1.0f, m_size[0], m_size[1], 0.0f, 0.0f, 1.0f, 1.0f, 0, 0);
	m_quad->setFlipped(true);
	m_characterControllerCS = new CharacterControllerCS(dt, fdt);
	m_characterControllerCS->setUserPointer(reinterpret_cast<std::uintptr_t>(this));

	setPosition(m_position);
	initAnimations();
}

Player::~Player() {
	delete m_quad;
}

void Player::initAnimations() {
	m_textureAtlas = new unsigned int();
	m_currentFrame = new unsigned int();

	m_Animations["move"].create(Globals::spritesheetManager.getAssetPointer("marvin_move"), 0.08f, *m_textureAtlas, *m_currentFrame);
	m_Animations["jump"].create(Globals::spritesheetManager.getAssetPointer("marvin_jump"), 0.08f, *m_textureAtlas, *m_currentFrame);
	m_Animations["fall"].create(Globals::spritesheetManager.getAssetPointer("marvin_fall"), 0.08f, *m_textureAtlas, *m_currentFrame);
	m_Animations["fade"].create(Globals::spritesheetManager.getAssetPointer("marvin_fade"), 0.08f, *m_textureAtlas, *m_currentFrame);

	m_Animations["idle"].create(Globals::spritesheetManager.getAssetPointer("marvin_idle"), 0.08f, *m_textureAtlas, *m_currentFrame);

	updatePosition(Vector2f(m_characterControllerCS->m_position.x, m_characterControllerCS->m_position.y));
	setOrigin(Vector2f(13.0f, 13.0f));
}

void  Player::fixedUpdate() {
	if (!m_fade) {
		m_characterControllerCS->fixedUpdate();

		updatePosition(Vector2f(m_characterControllerCS->m_position.x, m_characterControllerCS->m_position.y));

		ViewEffect::get().setPosition(m_position);
	}
}

void  Player::update() {
	if (!m_fade) {
		if (m_characterControllerCS->m_velocity.x < 0) {
			m_quad->setFlipped(false);
		}
		else if (m_characterControllerCS->m_velocity.x > 0) {
			m_quad->setFlipped(true);
		}

		if (m_characterControllerCS->isGrounded()) { // NOT JUMPING
			if (m_characterControllerCS->m_velocity.x != 0) {

				m_Animations["move"].update(m_dt);
			}
			else {
				m_Animations["idle"].update(m_dt);
			}
		}
		else {
			if (m_characterControllerCS->m_velocity.y > 0) {
				m_Animations["jump"].update(m_dt);
			}if (m_characterControllerCS->m_velocity.y < 0) {
				m_Animations["fall"].update(m_dt);
			}
		}
	}else{
		m_Animations["fade"].update(m_dt);
	}
}

void Player::render() {
	

	//glEnable(GL_BLEND);	
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", Globals::projection * ViewEffect::get().getView() * m_transform);
	m_shaderArray->loadInt("u_layer", *m_currentFrame);
	m_quad->render(*m_textureAtlas, true);
	glUseProgram(0);
	//glDisable(GL_BLEND);
	m_characterControllerCS->render();
}

void Player::updatePosition(const Vector2f &position) {
	m_position = position;
	m_transform.translate((position[0] - m_origin[0]), (position[1] - m_origin[1]), 0.0f);	
}

void Player::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform.translate((position[0] - m_origin[0]), (position[1] - m_origin[1]), 0.0f);

	m_characterControllerCS->setPosition(position[0], position[1]);
}

void Player::setPosition(const float x, const float y) {
	m_position[0] = x;
	m_position[1] = y;

	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
	m_characterControllerCS->setPosition(m_position[0], m_position[1]);
}

void Player::setFade(bool fade) {
	m_fade = fade;
}