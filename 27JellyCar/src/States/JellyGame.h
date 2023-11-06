#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Framebuffer.h>
#include <States/StateMachine.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "Levels/LevelManager.h"
#include "JellyPhysics/World.h"

#include "_Andromeda/Sprite.h"

class JellyGame : public State, public KeyboardEventListener {

public:

	JellyGame(StateMachine& machine, std::string scene);
	~JellyGame();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

	void renderScene();

private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput();
	void UpdateTransformMeter(float dt);

	unsigned int backWidth = 0, backHeight = 0;
	unsigned int columns = 0, rows = 0;


	LevelManager* _levelManager;
	std::vector<LevelSoftBody*> _gameBodies;
	World* _world;
	Car* _car;

	glm::mat4 _projection;
	glm::mat4 _jellyProjection;

	Vector2 _levelTarget;
	glm::vec4 _screenBounds;

	std::string m_scene;

	std::string _levelName;
	std::string _sceneFile;

	Vector2 _wholeMapSize;
	Vector2 _wholeMapPosition;
	AABB _worldLimits;
	AABB _mapLimits;
	float _levelLine;
	int carBreakCount;

	bool _newTimeRecord;
	bool _newJumpRecord;

	bool _fastCar;
	bool _slowCar;
	bool _showMap;
	float _dt;
	float _time;

	//hit sound
	float _hitTime;
	float _chassisHit;

	//jumping
	bool _isJumping;
	bool _inTheAir;
	float _jumpStartPosition;
	float _bestJumpLenght;

	//transformeter
	float				mTransformMeter;
	float				mTransformMeterRechargeSpeed;
	int					mTransformMeterGrowDir;
	int					mTransformTex;

	//checkpoint
	bool _checkpoint;
	glm::vec2 _checkpointPosition;

	//ballon and tire
	bool _haveBallon;
	bool _haveTire;

	bool _ballonActive;
	bool _tireActive;

	bool _ballonPressed;
	bool _tirePressed;

	float _ballonTime;
	float _tireTime;

	AABB _ballonAABB;
	AABB _tireAABB;

	Shader2* _shader;
	ShaderManager* _shaderManager;

	Sprite* _backSprite;
	Sprite* _targetSprite;
	Sprite* _tireSprite;
	Sprite* _ballonSprite;
	Sprite* _tireSpriteBack;
	Sprite* _ballonSpriteBack;
	Sprite* _transformMeter;

	Framebuffer m_mainRT;
};