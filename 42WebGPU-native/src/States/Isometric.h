#pragma once

#include <vector>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AnimationController.h>
#include <engine/animation/AnimatedModel.h>
#include <engine/animation/Animation.h>
#include <engine/scene/SceneNode.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <engine/sound/AudioDecoder.h>
#include <engine/sound/SoundEffect.h>

#include <States/StateMachine.h>
#include <Nuklear/NkJoystick.h>
#include <Nuklear/NkContext.h>

#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpData.h>

#include <Physics/Physics.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "bullet_store.h"
#include "enemy_spawner.h"

struct BulletCollisionCallback : public btCollisionWorld::ContactResultCallback {
	bool m_hasCollided = false;
	btCollisionObject* m_hitTarget = nullptr;
	virtual bool needsCollision(btBroadphaseProxy* proxy) const override {

		auto* targetObj = static_cast<btCollisionObject*>(proxy->m_clientObject);
		if (!targetObj) 
			return false;

		if (targetObj->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
			return false;

		return (Physics::collisiontypes::ENEMY & proxy->m_collisionFilterGroup) && (Physics::collisiontypes::SPHERE & proxy->m_collisionFilterMask);
	}

	virtual btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
		const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override
	{
		m_hasCollided = true;
		m_hitTarget = const_cast<btCollisionObject*>(colObj1Wrap->getCollisionObject());
		return 0;
	}
};

struct BulletCollisionPlayerCallback : public btCollisionWorld::ContactResultCallback {
	bool m_hasCollided = false;
	btCollisionObject* m_hitTarget = nullptr;
	virtual bool needsCollision(btBroadphaseProxy* proxy) const override {

		auto* targetObj = static_cast<btCollisionObject*>(proxy->m_clientObject);
		if (!targetObj)
			return false;

		if (targetObj->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
			return false;

		return (Physics::collisiontypes::ENEMY & proxy->m_collisionFilterGroup) && (Physics::collisiontypes::CHARACTER & proxy->m_collisionFilterMask);
	}

	virtual btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
		const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override
	{
		m_hasCollided = true;
		m_hitTarget = const_cast<btCollisionObject*>(colObj1Wrap->getCollisionObject());
		return 0;
	}
};

class CollisionEntity;
class Enemy;
class Player;
class Isometric : public State, public MouseEventListener, public KeyboardEventListener {

	struct Wiggly {
		Vector3f nosePos;
		float time;
	};

	struct SpriteInstance {
		float position[3];
		float age;
		float scale[2];
		float currentFrame;
		uint32_t padding2;
	};

	struct FrameInfo {
		uint32_t colRow[2];
		float frameSize[2];
	};

public:

	Isometric(StateMachine& machine);
	~Isometric();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnDrawShadow(const WGPURenderPassEncoder& renderPassEncoder);
	void OnFillBuffer(nk_context& nkCntxt);

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsFloor();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsWiggly();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsBullet();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsBillboard();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsShadow();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsWigglyShadow();

	std::vector<WGPUBindGroup> OnBindGroups();
	std::vector<WGPUBindGroup> OnBindGroupsFloor();
	std::vector<WGPUBindGroup> OnBindGroupsBullet();
	std::vector<WGPUBindGroup> OnBindGroupsShadow();
	WGPUBindGroup createBindGroupBillboard();
	WGPUBindGroup createBindGroupMuzzle();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	bool getWorldPosition(int xPos, int yPos, const Vector3f& planeNormal, Vector3f& outIntersection);
	float getLookAtYRotation(const Vector3f& objectPos, const Vector3f& targetPos);
	CollisionEntity* createNewBulletToPool();
	void spawnBillboard(const Vector3f& position);
	void spawnMuzzle();
	void updateBillboards(float dt);
	void updateMuzzle(float dt);

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_isDeath = false;
	bool m_debugCollision = false;

	Camera m_camera;
	Uniforms m_uniforms;	
	TrackBall m_trackball;
	JoystickResult m_joystickResult;
	RotationButtonResult m_rotationButtonResult;
	Wiggly m_wiggly;

	AssimpModel m_enemy;
	AnimatedModel m_player;
	Shape m_floor, m_bullet;

	WgpBuffer m_uniformBuffer, m_infoBufferBillboard, m_infoBufferMuzzle, m_storageBuffer, m_wigglyBuffer, m_skinBuffer, m_rotationBuffer, m_offsetBuffer, m_spriteBuffer, m_muzzleBuffer;
	WgpModel m_wgpPlayer, m_wgpFloor, m_wgpEnemy, m_wgpBullet;
	WgpTexture m_wgpFloorD, m_wgpEnemyD, m_wgpBulletTexture, m_sprite, m_muzzle, m_wgpTextureShadow;
	WGPUBindGroup m_bindGroupBillboard, m_bindGroupMuzzle;
	BulletStore m_bulletStore;
	SceneNode* m_scene;

	float prev_idleWeight = 0.0f;
	float prev_rightWeight = 0.0f;
	float prev_forwardWeight = 0.0f;
	float prev_backWeight = 0.0f;
	float prev_leftWeight = 0.0f;
	const float animTransitionTime = 0.2f;
	float deathTime = -1.0f;
	float aimTheta = 0.0f;
	float lastFireTime = 0.0f;
	size_t m_targetPoolSize;

	EnemySpawner m_enemySpawner;

	SoundEffect m_fire, m_ding;
	std::vector<CollisionEntity*> m_entities;
	Player* m_playerEnitity;
	std::vector<Enemy*> m_enemies;
	std::vector<Matrix4f> m_cpuInstanceBuffer;
	std::vector<SpriteInstance> m_activeBillboards;
	std::vector<SpriteInstance> m_activeMuzzle;
	Matrix4f m_lightProjection, m_lightView;
	Vector3f m_lightDir;

	static WGPUBindGroup CreateBindGroup(const WgpBuffer& uniformBuffer, const WgpBuffer& wigglyBuffer, const WgpTexture& texture, const WgpBuffer& storageBuffer);
	static WGPUBindGroup CreateBindGroupShadow(const WgpBuffer& uniformBuffer, const WgpBuffer& wigglyBuffer, const WgpBuffer& storageBuffer);
};