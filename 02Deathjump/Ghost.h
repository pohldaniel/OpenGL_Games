#pragma once
#include "engine/animation/Animator2D.h"
#include "engine/Spritesheet.h"
#include "engine/Clock.h"
#include "engine/Random.h"

#include "Entity.h"
#include "ParticleEmitter.h"
#include "Light.h"

class Ghost : public Entity {
public:
	Ghost(const float& dt, const float& fdt);
	~Ghost();

	void fixedUpdate() override;
	void update() override {};	
	void render() override {};
	void render(float deltaTime);
	void update(Collider obj);

	bool m_isAlive = true;
	bool m_blowUp = false;
	Animator2D m_animator;
	ParticleEmitter* m_emitter;
	bool m_left = false;

private:

	Quad *m_quad;
	Shader *m_shaderArray;
	Spritesheet *m_spriteSheet;
	Vector2f m_direction;
	Vector2f m_sizeGhost = Vector2f(64.0f, 64.0f) * 2.75f;
	Vector4f m_blendColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

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