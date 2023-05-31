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
	CharacterController* getCharacterController();

private:

	Camera& m_camera;
	CharacterController* m_characterController;
};