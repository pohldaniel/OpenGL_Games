#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpData.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpTexture.h>

struct BloomMip {
	Vector2f size;
	uint32_t width;
	uint32_t height;
	WGPUTextureView renderTargetView;
	WGPUTextureView sampledView;
};

class BloomFBO {
public:
	bool Init(WGPUDevice device, unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength);
	void Cleanup();

	WGPUTexture m_texture = nullptr;
	std::vector<BloomMip> mMipChain;
	bool mInit = false;
};

class HDRBloom : public State, public MouseEventListener, public KeyboardEventListener {
	struct Instance {
		Matrix4f model;
		Vector4f color;
	};
	struct DownsampleUniforms {
		Vector2f srcResolution;
		uint32_t mipLevel;
		uint32_t padding;
	};

	struct UpsampleUniforms {
		float filterRadius;
		float padding[3];
	};
public:

	HDRBloom(StateMachine& machine);
	~HDRBloom();

	void fixedUpdate() override;
	void update() override;
	void render() override;

	void resize(int deltaW, int deltaH) override;
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;
	void OnReEnter(unsigned int prevState) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsDown();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsUp();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsComposite();

	WGPUBindGroup createBindGroup(const WgpTexture& texture);
	//WGPUBindGroup createBindGroupDown();
	//WGPUBindGroup createBindGroupUp();
	WGPUBindGroup createBindGroupComposite();
	
	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	
	void RenderDownsamples(const WGPUCommandEncoder& commandEncoder);
	void RenderUpsamples(const WGPUCommandEncoder& commandEncoder, float filterRadius);
	void CreateBloomBindGroups(WGPUDevice device, WGPUBindGroupLayout layout, WGPUSampler sampler, WGPUTextureView sceneColorView);
	void UpdateDownsampleUniformBuffer(WGPUQueue queue, size_t mipIndex);
	void UpdateUpsampleUniformBuffer(WGPUQueue queue, float filterRadius);

	bool m_initUi = true;
	bool m_drawUi = false;
	long m_state = 0;
	float m_RightStickX = 0.0f;
	float m_RightStickY = 0.0f;
	float m_LeftStickX = 0.0f;
	float m_LeftStickY = 0.0f;

	Camera m_camera;
	Uniforms m_uniforms;
	Shape m_cube;

	WgpBuffer m_uniformBuffer, m_storageBuffer, m_downSampleBuffer, m_upSampleBuffer;
	WgpModel m_wgpCube;
	WgpTexture m_wgpSceneTarget, m_wgpSceneDepth;
	WgpTexture wgpTextureEmpty, wgpTextureWood, wgpTextureContainer;


	WGPUBindGroup m_bindGroupComposite, m_bindGroupDown, m_bindGroupUp;

	std::vector<Instance> m_instances;
	std::vector<Vector3f> m_lightPositions;
	std::vector<Vector3f> m_lightColors;

	BloomFBO m_bloomFBO;
	std::vector<WGPUBindGroup> m_downsampleBindGroups;
	std::vector<WGPUBindGroup> m_upsampleBindGroups;
	bool mKarisAverageOnDownsample = true;
};