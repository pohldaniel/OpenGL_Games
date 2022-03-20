#pragma once
#include "Entity.h"
#include "Animator.h"
#include "Spritesheet.h"
#include "ParticleEmitter.h"
#include "Clock.h"

class Fireball : public Entity {
public:
	Fireball(const float& dt, const float& fdt, float velocity, bool left);
	~Fireball();

	void fixedUpdate() override;
	void update() override;
	void render() const override;

	ParticleEmitter* m_emitter;

	bool isAlive() const;

private:
	friend class Player;

	Quad *m_quad;
	Quad *m_quadBlow;
	Shader *m_shader;
	Shader *m_shaderArray;
	Spritesheet *m_spriteSheet;
	std::unordered_map<std::string, unsigned int> m_sprites;

	Vector2f m_size = Vector2f(32.0f, 32.0f);
	std::unordered_map<std::string, Animator> m_Animations;

	bool m_left = true;
	bool m_isAlive = true;
	bool m_blowUp = false;

	const float m_velocity = 150.0f;

	const float& m_dt;
	const float& m_fdt;
	
	void move();
	void initCollider(Vector2f position);
	void animate();
	void initAnimations();
	void initSprites();

	const float m_positions[13] ={
		35.f,
		105.f,
		145.f,
		210.f,
		270.f,
		310.f,
		380.f,
		455.f,
		500.f,
		565.f,
		625.f,
		730.f,
		790.f,
	};
};