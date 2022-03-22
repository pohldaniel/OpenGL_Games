#pragma once
#include "Entity.h"
#include "Animator.h"
#include "Spritesheet.h"
#include "ParticleEmitter.h"
#include "Light.h"


class Fireball : public Entity {
public:
	Fireball(const float& dt, const float& fdt, float velocity, bool left);
	~Fireball();

	void fixedUpdate() override;
	void update() override;
	void render() override {};
	void render(float deltaTime);

	ParticleEmitter* m_emitter;

	bool isAlive() const;

private:
	friend class Player;

	Quad *m_quad;
	Quad *m_quadBlow;
	Shader *m_shader;
	Shader *m_shaderArray;
	
	Vector2f m_fireballSize = Vector2f(64.0f, 64.0f);
	Vector2f m_blowSize = Vector2f(512.0f * 0.3f, 512.0f * 0.3f);
	Matrix4f m_blowTrans = Matrix4f::IDENTITY;
	Vector2f originBlow;

	std::unordered_map<std::string, Animator> m_Animations;
	std::unordered_map<std::string, unsigned int> m_sprites;
	Light* m_light;

	bool m_left = true;
	bool m_isAlive = true;
	bool m_blowUp = false;
	const float m_velocity = 150.0f;

	
	void move();
	void initCollider(Vector2f position);
	void animate();
	void initAnimations();
	void initSprites();
	void updateEmitter();
	void initEmitter();
	void updateLight();
	void initLight();

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