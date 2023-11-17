#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Framebuffer.h>
#include <States/StateMachine.h>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "JellyPhysics/World.h"

class JellyGame : public State, public KeyboardEventListener, JellyPhysics::CollisionCallback {

	friend class JellyDialogPause;

public:

	JellyGame(StateMachine& machine, std::string sceneName);
	~JellyGame();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

	void renderScene();
	void spawnAtCheckpoint();

	bool collisionFilter(Body* bA, int bodyApm, Body* bodyB, int bodyBpm1, int bodyBpm2, Vector2 hitPt, float normalVel);
	Vector2f touchToScreen(Vector4f screenBound, Vector2f touch);

private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput();
	void UpdateTransformMeter(float dt);

	std::vector<LevelSoftBody*> m_gameBodies;
	World* m_world;
	Car* m_car;
	Vector4f m_screenBounds;
	std::string m_sceneName;
	float m_time;

	Vector2 m_levelTarget;
	std::string m_levelName;
	std::string m_sceneFile;
	bool m_newTimeRecord;
	bool m_newJumpRecord;
	float m_levelLine;
	int m_carBreakCount;

	//transformeter
	float m_transformMeter;
	float m_transformMeterRechargeSpeed;
	int m_transformMeterGrowDir;

	//checkpoint
	bool m_checkpoint;
	Vector2f m_checkpointPosition;


	Vector2 m_wholeMapSize;
	Vector2 m_wholeMapPosition;
	AABB m_worldLimits;
	AABB m_mapLimits;
	

	bool m_fastCar;
	bool m_slowCar;
	bool m_showMap;
	
	//hit sound
	float m_hitTime;
	float m_chassisHit;

	//jumping
	bool m_isJumping;
	bool m_inTheAir;
	float m_jumpStartPosition;
	float m_bestJumpLenght;

	//ballon and tire
	bool m_haveBallon;
	bool m_haveTire;

	bool m_ballonActive;
	bool m_tireActive;

	bool m_ballonPressed;
	bool m_tirePressed;

	float m_ballonTime;
	float m_tireTime;

	AABB m_ballonAABB;
	AABB m_tireAABB;

	Framebuffer m_mainRT;

	Matrix4f m_orthographic;
	Camera m_camera;
};