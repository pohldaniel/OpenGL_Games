#pragma once
#include "engine/animation/Animator.h"
#include "engine/Clock.h"
#include "engine/Random.h"

#include "Constants.h"
#include "Entity.h"
#include "Wall.h"
#include "Light.h"

class Heart : public Entity {
public:
	Heart(const float& dt, const float& fdt);
	~Heart();

	virtual void fixedUpdate() override;
	virtual void update() override;
	void render() override {};
	void render(float deltaTime);

	void resolveCollision(std::vector<Wall>& walls);

	bool isAlive = true;
private:

	Quad *m_quad;
	Shader *m_shaderArray;
	Vector2f m_heartSize = Vector2f(32.0f, 32.0f);

	bool m_grounded = false;

	Light* m_light;

	Animator m_animation;

	Clock m_deSpawnClock;
	Clock m_realDeSpawnClock;
	Clock m_disappearClock;

	const float m_deSpawnTime = 3.0f;
	float m_disappearAnimationTime = 0.5f;

	bool m_disappear = false;

	unsigned m_currentColor = 0;

	const Vector4f m_colors[4] ={
		Vector4f(1.0f, 1.0f, 1.0f, 80.0f / 255.0f),
		Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4f(1.0f, 1.0f, 1.0f, 80.0f / 255.0f),
		Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
	};
	Vector4f m_blendColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

	void updateLight();
	void initLight();
	void initAnimations();
	void initBody();
	void animateDisappear();
};