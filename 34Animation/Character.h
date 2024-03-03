#pragma once

#include <engine/input/Keyboard.h>
#include <engine/Camera.h>

#include <Animation/AnimationController.h>

class Character {

	const float INAIR_THRESHOLD_TIME = 0.1f;
	const float MOVE_SPEED = 0.2f;

public:

	Character();

	void draw(const Camera& camera);
	void update(const float dt);
	void fixedUpdate(float fdt);

	AnimatedModel m_model;
	AnimationController* m_animationController;

	bool m_onGround;
	bool m_okToJump;
	bool m_isJumping;
	bool m_jumpStarted;
	float m_inAirTimer;
	Vector3f m_curMoveDir;
};