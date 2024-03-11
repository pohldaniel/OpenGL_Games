#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AssimpAnimatedModel.h>
#include <engine/MeshObject/Shape.h>
#include <engine/scene/SceneNode.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <States/StateMachine.h>
#include <Windows.h>
#include <Utils/SolidIO.h>
#include "Skeleton.h"
#include "ModelNew.h"
#include "Enums.h"

class SolidState : public State, public MouseEventListener, public KeyboardEventListener {

public:

	SolidState(StateMachine& machine);
	~SolidState();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void renderUi();
	void updateModel();
	void doAnimations(float dt);
	bool isIdle();
	bool isFlip();
	bool wasFlip();
	int getIdle();
	bool wasIdle();
	bool wasLanding();
	bool isLanding();
	int getLanding();
	bool wasStop();
	bool isStop();
	int getStop();
	bool wasLandhard();
	bool isLandhard();
	int getLandhard();
	bool wasCrouch();
	bool isCrouch();
	int getCrouch();
	bool wasRun();
	bool isRun();
	int getRun();
	bool hasWeapon();
	bool isPlayerControlled();
	void FootLand(bodypart whichfoot, float opacity);

	Utils::AnimationFrame& currentFrame();
	Utils::AnimationFrame& targetFrame();

	

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;

	Utils::SolidIO solidConverter;
	Shape m_body;
	Skeleton m_skeleton;
	

	ModelNew modelNew;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBufferDraw;
	std::vector<Utils::SolidIO::Vertex> m_vertexBufferMap;

	int animCurrent;
	int animTarget;
	int howactive;
	float normalsupdatedelay;
	Utils::Animation tempanimation;
	unsigned id;
	Vector3f velocity;
	bool landhard;
	bool crouchkeydown;
	bool crouchtogglekeydown;
	float targetyaw;

	int frameCurrent;
	int frameTarget;
	float target;
	int numflipped;
	bool feint;
	bool lastfeint;
	int aitype;

	int escapednum;
	bool reversaltrain;
	int detail;
	bool onfire;
	float deathbleeding;
	Vector3f facing;
	float yaw;
	float transspeed;
	Vector3f currentoffset, targetoffset, offset;
	bool hasvictim;
	bool jumpkeydown;
	bool attackkeydown;
	int hostile;
	int weaponactive;
	int num_weapons;
	int weaponids[4];
	float targetrot;
	bool superruntoggle;
	float speed;
	float velspeed;
	float speedmult;
	float scale;
	float rot;
	float oldrot;

	int oldanimCurrent;
	int oldanimTarget;
	int oldframeCurrent;
	int oldframeTarget;
	bool calcrot;
	void Reverse(){}
};