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

	struct DirectionalLight {
		float direction[3];
		float padding;
		float color[4];
	};

	struct PointLight {
		float position[3];
		float padding;
		float color[4];
		uint32_t active;
		uint32_t pad1;      
		uint32_t pad2;
		uint32_t pad3;
	};

public:

	Isometric(StateMachine& machine);
	~Isometric();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnPostDraw();
	void OnDrawShadow(const WGPURenderPassEncoder& renderPassEncoder);
	void OnDrawEmission(const WGPURenderPassEncoder& renderPassEncoder);
	void OnDrawScene(const WGPURenderPassEncoder& renderPassEncoder);
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
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsBlur();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsEmission();
	std::vector<WGPUBindGroup> OnBindGroupsPlayerEmission();
	std::vector<WGPUBindGroup> OnBindGroupsGunEmission();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsFloorEmission();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsComposite();

	std::vector<WGPUBindGroup> OnBindGroupsPlayer();
	std::vector<WGPUBindGroup> OnBindGroupsGun();
	std::vector<WGPUBindGroup> OnBindGroupsFloor();
	std::vector<WGPUBindGroup> OnBindGroupsBullet();
	std::vector<WGPUBindGroup> OnBindGroupsShadow();
	std::vector<WGPUBindGroup> OnBindGroupsFloorEmission();

	WGPUBindGroup createBindGroupBillboard();
	WGPUBindGroup createBindGroupMuzzle();
	WGPUBindGroup createBindGroupWiggly();
	WGPUBindGroup createBindGroupComposite();
	WGPUBindGroup createBindGroupBlurH();
	WGPUBindGroup createBindGroupBlurV();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	bool getWorldPosition(int xPos, int yPos, const Vector3f& planeNormal, Vector3f& outIntersection);
	float getLookAtYRotation(const Vector3f& objectPos, const Vector3f& targetPos);
	CollisionEntity* createNewBulletToPool();
	void spawnBillboard(const Vector3f& position);
	void resetMuzzle();
	void updateBillboards(float dt);
	void updateMuzzle(float dt, float x, float y, float z);

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_isDeath = false;
	bool m_debugCollision = false;
	bool m_wantResize = false;

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
	WgpBuffer m_pointLightBuffer, m_directionalLightBuffer;
	
	WgpModel m_wgpPlayer, m_wgpFloor, m_wgpEnemy, m_wgpBullet;
	WgpTexture m_wgpFloorD, m_wgpFloorN, m_wgpFloorS, m_wgpEnemyD, m_wgpEnemyN, m_wgpEnemyS, m_wgpBulletTexture, m_sprite, m_muzzle, m_wgpTextureShadow;

	WgpTexture m_wgpPlayerD, m_wgpPlayerN, m_wgpPlayerS, m_wgpPlayerE, m_wgpGunD, m_wgpGunN, m_wgpGunS, m_wgpGunE;

	WgpTexture m_wgpEmissionTarget, m_wgpEmissionDepth;
	WgpTexture m_wgpSceneTarget, m_wgpSceneDepth;
	WgpTexture m_wgpBlurTempTarget;
	WgpTexture m_wgpBlurFinalTarget;

	WGPUBindGroup m_bindGroupBillboard, m_bindGroupMuzzle, m_bindGroupComposite, m_bindGroupBlurH, m_bindGroupBlurV;
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
	int m_spreadAmount = 20;

	EnemySpawner m_enemySpawner;

	SoundEffect m_fire, m_ding;
	std::vector<CollisionEntity*> m_entities;
	Player* m_playerEntity;
	std::vector<Enemy*> m_enemies;
	std::vector<Matrix4f> m_cpuInstanceBuffer;
	std::vector<SpriteInstance> m_activeBillboards;
	SpriteInstance m_muzzleInstance;
	Matrix4f m_lightProjection, m_lightView;
	Vector3f m_lightDir;
	std::vector<float> m_muzzleFlashSpritesAge;

	static WGPUBindGroup CreateBindGroupShadow(const WgpBuffer& uniformBuffer, const WgpBuffer& wigglyBuffer, const WgpBuffer& storageBuffer);
};