#pragma once
#include <unordered_map>
#include <vector>
#include "Constants.h"
#include "Entity.h"
#include "Wall.h"
#include "Animator.h"
#include "Spritesheet.h"


class Player : public Entity {
public:
	Player(const float& dt, const float& fdt);
	~Player();

	virtual void FixedUpdate();
	virtual void Update();
	virtual void draw() ;

	void ResolveCollision(Entity* entity);
	void ResolveCollision(std::vector<Wall>& walls);

	bool IsAlive() const;

	//unsigned int m_frame;
	Quad *m_quad;
	Quad *m_quadFlipped;
	Shader *m_shaderArray;
	Spritesheet *m_spriteSheet;

private:
	Vector2f m_velocity;

	bool m_alive = true;

	const float m_movementSpeed = 420.0f;
	float m_torque = 0.85f;
	const float m_gravity = 4605.0f;
	const float m_jumpVelocity = -1200.0f;

	bool m_grounded = true;
	bool m_crouching = false;
	bool m_grabbing = false;

	bool m_hit = true;
	bool m_movable = true;

	bool m_wasGrounded = false;

	unsigned m_currentHitTake = 10;

	const Vector2f m_playerSize = Vector2f(96, 84);
	std::unordered_map<std::string, Animator> m_Animations;
	

	unsigned int *m_textureAtlas;
	unsigned int *m_currentFrame;


	//sf::Clock m_hitTimer;

	void UpdateTimer();

	void Animate();

	void KeepInBorders();

	void Crouch();

	void Move();
	void UpdateVelocity();

	void InitAnimations();
	void InitBody();
	void InitCollider();

	
	const float xScale = 96 / (float)(WIDTH);
	const float yScale = 84 / (float)(HEIGHT);
};