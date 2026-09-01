#pragma once
#include <vector>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AnimationController.h>
#include <engine/animation/AnimatedModel.h>
#include <engine/animation/Animation.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>
#include <engine/sound/OpenALPlayer.h>
#include <engine/sound/RtAudioPlayer.h>
#include <engine/sound/RtAudioAudioSystem.h>
#include <engine/sound/OpenALAudioSystem.h>
#include <engine/sound/AudioDecoder.h>
#include <engine/sound/OpenALEffect.h>

#include <States/StateMachine.h>
#include <Nuklear/NkJoystick.h>
#include <Nuklear/NkContext.h>

#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpData.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "bullet_store.h"

class Isometric : public State, public MouseEventListener, public KeyboardEventListener {

	struct Wiggly {
		Vector3f nosePos;
		float time;
	};

public:

	Isometric(StateMachine& machine);
	~Isometric();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
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

	std::vector<WGPUBindGroup> OnBindGroups();
	std::vector<WGPUBindGroup> OnBindGroupsFloor();
	std::vector<WGPUBindGroup> OnBindGroupsBullet();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	bool getWorldPosition(int xPos, int yPos, const Vector3f& planeNormal, Vector3f& outIntersection);
	float getLookAtYRotation(const Vector3f& objectPos, const Vector3f& targetPos);

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_isDeath = false;

	Camera m_camera;
	Uniforms m_uniforms;	
	TrackBall m_trackball;
	JoystickResult m_joystickResult;
	RotationButtonResult m_rotationButtonResult;
	Wiggly m_wiggly;

	AssimpModel m_enemy;
	AnimatedModel m_player;
	Shape m_floor, m_bullet;

	WgpBuffer m_uniformBuffer, m_instanceBuffer, m_wigglyBuffer, m_skinBuffer, m_rotationBuffer, m_offsetBuffer;
	WgpModel m_wgpPlayer, m_wgpFloor, m_wgpEnemy, m_wgpBullet;
	WgpTexture m_wgpFloorD, m_wgpEnemyD, m_wgpBulletTexture;
	BulletStore m_bulletStore;

	float prev_idleWeight = 0.0f;
	float prev_rightWeight = 0.0f;
	float prev_forwardWeight = 0.0f;
	float prev_backWeight = 0.0f;
	float prev_leftWeight = 0.0f;
	const float animTransitionTime = 0.2f;
	float deathTime = -1.0f;
	float aimTheta = 0.0f;
	float lastFireTime = 0.0f;
	//OpenALPlayer m_openALPlayer;
	//RtAudioPlayer m_rtAudioPlayer;
	std::unique_ptr<AudioSystem> m_audioSystem;
	AudioDecoder m_audioDecoder;
	OpenALEffect m_effectPlayer;

	static WGPUBindGroup CreateBindGroup(const WgpBuffer& uniformBuffer, const WgpBuffer& wigglyBuffer, const WgpTexture& texture);
};