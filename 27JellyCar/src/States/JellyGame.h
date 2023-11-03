#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <States/StateMachine.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "Levels/LevelManager.h"
#include "JellyPhysics/World.h"

class JellyGame : public State, public KeyboardEventListener {

public:

	JellyGame(StateMachine& machine, std::string scene);
	~JellyGame();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput();

	unsigned int backWidth = 0, backHeight = 0;
	unsigned int columns = 0, rows = 0;

	std::vector<SkinInfo> _carSkins;
	LevelManager* _levelManager;
	std::vector<LevelSoftBody*> _gameBodies;
	World* _world;
	Car* _car;

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
};