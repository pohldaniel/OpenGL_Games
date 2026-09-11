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

#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5

class Cubes : public State, public MouseEventListener, public KeyboardEventListener {
	
	struct GPUInstanceData {
		Matrix4f modelMatrix;
		Vector4f color;
	};

	Vector4f colors[4] = { 
                           {244.0f / 256.0f, 194.0f / 256.0f, 13.0f / 256., 1.0f},
                           {219.0f / 256.0f, 50.0f / 256.0f, 54.0f / 256., 1.0f},
                           {72.0f / 256.0f, 133.0f / 256.0f, 237.0f / 256., 1.0f},
                           { 60.0f / 256.0f,  186.0f / 256.0f,84.0f / 256.0f, 1.0f }
	};

public:

	Cubes(StateMachine& machine);
	~Cubes();

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
	std::vector<WGPUBindGroup> OnBindGroups();
	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	void shootCube(unsigned int posX, unsigned int posY);

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_debugPhysic = false;

	Camera m_camera;
	Uniforms m_uniforms;	
	TrackBall m_trackball;
	SceneNode* m_scene;
	Shape m_cube;
	std::vector<GPUInstanceData> m_cpuInstanceBuffer;

	WgpBuffer m_uniformBuffer, m_storageBuffer;
	WgpModel m_wgpCube;

	WGPUBindGroup m_bindGroup;

	std::vector<SceneNode*> m_children;
};