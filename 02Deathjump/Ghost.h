#pragma once
#include "Entity.h"
#include "Animator.h"
#include "Spritesheet.h"
#include "ParticleEmitter.h"
#include "Light.h"
#include "Clock.h"

class Ghost : public Entity {
public:
	Ghost(const float& dt, const float& fdt);
	~Ghost();

	void fixedUpdate() override;
	void update() override {};	
	void render() override {};
	void render(float deltaTime);
	void update(Collider obj);

	// i should die for that too ;)
	bool m_isAlive = true;
	bool m_blowUp = false;
	Animator m_animator;
	ParticleEmitter* m_emitter;
	bool m_left = false;
private:
	//sf::Sprite* m_sprite;

	Quad *m_quad;
	Shader *m_shaderArray;
	Spritesheet *m_spriteSheet;
	Vector2f m_direction;
	Vector2f m_sizeGhost = Vector2f(64.0f, 64.0f) * 2.75f;

	Clock m_clock;
	Light* m_light;

	float m_lifeTime = 12.0f;
	float m_lightVal = 0.55f;
	float lerp(const float& x, const float& y, const float& t);



	void initCollider();
	void initAnimations();
	void initBody();
	void initEmitter();
	void updateLight();
	void initLight();
};