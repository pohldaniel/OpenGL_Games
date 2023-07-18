#pragma once
#include "engine/input/Mouse.h"
#include "engine/input/Keyboard.h"
#include "engine/Camera.h"
#include "RenderableObject.h"

#include "CharacterController.h"

#define RADIUS   0.5f
#define CAMERA_SMOOTHING_SPEED  0.85f

class Terrain;

class Player : public RenderableObject {

public:

	Player(Camera& camera);
	~Player();
	void init();
	void draw(const Camera& camera) override;
	void update(const float dt) override;
	void fixedUpdate(const float fdt);

	void setPosition(const Vector3f &position) override;
	void setPosition(const float x, const float y, const float z) override;
	void setColor(const Vector4f &color);

	void resetOrientation();
	CharacterController* getCharacterController();
	btCollisionObject* getContactObject();
	bool isMoving();
	void setFade(bool fade);


private:

	
	CharacterController* m_characterController;
	bool m_move;
	Vector4f m_color;
	btCollisionObject* m_collisionObject;
	bool m_fade;
	float m_prevFraction;
	bool m_useGravity;

	Camera& camera;
};