#pragma once

#include <vector>
#include <array>
#include <engine/Vector.h>
#include <Utils/SolidIO.h>
#include "Enums.h"

class Animator {

public:

	Animator(Utils::Skeleton& skeleton);
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
	void FootLand(Utils::bodypart whichfoot, float opacity);

	Utils::AnimationFrame& currentFrame();
	Utils::AnimationFrame& targetFrame();

	Utils::Skeleton& skeleton;

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
	void Reverse() {}
};

class Model {

public:

	Model();
	~Model();
	void draw();
	void update(float dt);
	void resetPose();
	void bindPose();

	short m_muscleNum, m_vertexNum;
	Matrix4f* m_skinMatrices;
	Utils::Skeleton m_skeleton;
	
	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;
	std::vector<std::array<float, 4>> m_weights;
	std::vector<std::array<unsigned int, 4>> m_boneIds;
	std::vector<Utils::SolidIO::Vertex> m_vertexBufferMap;

	unsigned int m_vao = 0u;
	unsigned int m_vbo[3] = { 0u };
	unsigned int m_drawCount;

	Animator animator;
	bool m_bindpose;
};