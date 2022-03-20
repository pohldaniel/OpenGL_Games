#include "Fireball.h"
#include "Random.h"

Fireball::Fireball(const float& dt, const float& fdt, float velocity, bool left) : Entity(dt, fdt), m_velocity(velocity), m_dt(dt), m_fdt(fdt) {
	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_quad = new Quad(true, 1.0f, -1.0f, m_size[0], m_size[1]);
	m_quadBlow = new Quad(true, 1.0f, -1.0f, 512.f * 0.30f, 512.f * 0.30f);

	m_left = left;
	initSprites();
	initCollider(Vector2f(left ? -100.f : WIDTH + 100.f, m_positions[Random::RandInt(0, 12)]));
	initAnimations();
	//initEmitters();
}

Fireball::~Fireball() {

}

void Fireball::fixedUpdate() {
	if (!m_blowUp)
		move();
}

void Fireball::update() {	
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

/*void Fireball::UpdateLight() {
	m_light->SetPosition(i_collider.position);
}

void Fireball::InitLight() {
	m_light = new Light(i_collider.position, 290.0f);
}*/

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
	{
		/*m_fireballSprite.setTexture(*i_texture);
		m_fireballSprite.setOrigin(sf::Vector2f(m_size.x, m_size.y) / 2.0f);
		m_fireballSprite.setScale(sf::Vector2f(m_left ? 2.f : -2.f, 2.f));
		m_fireballSprite.setPosition(i_collider.position);*/

		m_sprites["fireball"] = Globals::textureManager.get("fireball").getTexture();
		setSize(Vector2f(m_quad->getScale()[0] * m_size[0], m_quad->getScale()[0] * m_size[1]));
		setOrigin(Vector2f(m_size[0], m_size[1]) * 0.5f);
		m_quad->setFlipped(m_left);
		setPosition(m_collider.position);
	}

	/*{
		Vector2f size = Vector2f(512.f, 512.f);

		m_blowUpSprite.setTexture(*m_blowTexture);
		m_blowUpSprite.setTextureRect(sf::IntRect(0, 0, size.x, size.y));
		m_blowUpSprite.setOrigin(size / 2.f);
		m_blowUpSprite.setScale(sf::Vector2f(0.30f, 0.30f));
	}*/

	
}

void Fireball::initCollider(Vector2f position) {
	const Vector2f size = Vector2f(32.f, 32.f);
	m_collider.size = size;
	m_collider.position = position;
}

void Fireball::render() const{	
	if (!m_blowUp) {		
		glUseProgram(m_shader->m_program);
		m_shader->loadMatrix("u_transform", m_transform * Globals::projection);
		m_quad->render(m_sprites.at("fireball"));
		glUseProgram(0);		
	}else {		
		glUseProgram(m_shaderArray->m_program);
		m_shaderArray->loadMatrix("u_transform", m_transform * Globals::projection);
		m_shaderArray->loadInt("u_layer", *m_currentFrame);
		m_quadBlow->render(*m_textureAtlas, true);
		glUseProgram(0);		
	}	
}