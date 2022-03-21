#pragma once
#include <unordered_map>
#include <vector>
#include "Constants.h"
#include "Entity.h"
#include "Wall.h"
#include "Ghost.h"
#include "Animator.h"
#include "Spritesheet.h"
#include "ParticleEmitter.h"

class Player : public Entity {
public:
	Player(const float& dt, const float& fdt);
	~Player();

	virtual void fixedUpdate();
	virtual void update();
	virtual void render();

	void resolveCollision(Entity* entity);
	void resolveCollision(std::vector<Wall>& walls);
	void resolveCollision(Ghost* entity, std::vector<Ghost*>& ghosts);

	bool isAlive() const;

	

private:

	void initAnimations();
	void initBody();
	void initCollider();
	void updateEmitters();
	void initEmitters();

	void animate();
	void updateTimer();
	void keepInBorders();
	void crouch();
	void move();
	void updateVelocity();

	Quad *m_quad;
	Shader *m_shaderArray;
	Spritesheet *m_spriteSheet;
	Vector2f m_velocity;
	Vector2f m_playerSize = Vector2f(96.0f * 2.0f, 84.0f * 2.0f);
	bool m_alive = true;

	const float m_movementSpeed = 420.0f;
	float m_torque = 0.85f;
	float m_gravity = 4605.0f;
	const float m_jumpVelocity = -1200.0f;
	bool m_grounded = true;
	bool m_crouching = false;
	bool m_grabbing = false;
	bool m_hit = true;
	bool m_movable = true;
	bool m_wasGrounded = false;
	unsigned m_currentHitTake = 10;

	std::unordered_map<std::string, Animator> m_Animations;	

	ParticleEmitter* m_emitter;
	ParticleEmitter* m_fallEmitter;

	Clock m_hitTimer;
};