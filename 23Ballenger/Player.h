#pragma once

#include "engine/input/Mouse.h"
#include "engine/input/Keyboard.h"
#include "engine/Camera.h"
#include "RenderableObject.h"

#include "CharacterController.h"

#define RADIUS   0.5f

class Terrain;

class Player : public RenderableObject {

public:

	Player(Camera& camera);
	~Player();
	void init(const Terrain& terrain);
	void draw(const Camera& camera) override;
	void update(const float dt) override;

	void setPosition(const Vector3f &position) override;
	void setPosition(const float x, const float y, const float z) override;
	void resetOrientation();
	Vector3f& getInitialPosition();
	CharacterController* getCharacterController();
	bool isMoving();
	void setEnvMap(unsigned int map);

private:

	Camera& m_camera;
	CharacterController* m_characterController;
	Vector3f m_pos;
	bool m_move;
	unsigned int m_map;
};