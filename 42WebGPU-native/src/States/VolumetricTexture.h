#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpData.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpTexture.h>

struct VolumeBuffer {
	WGPUTexture texture = nullptr;
	WGPUTextureView view = nullptr;
	uint32_t width, height, depth;

	VolumeBuffer(WGPUDevice device, uint32_t w, uint32_t h, uint32_t d)
		: width(w), height(h), depth(d) {

		WGPUTextureDescriptor desc = {};
		desc.dimension = WGPUTextureDimension_3D;
		desc.size = { w, h, d };
		desc.mipLevelCount = 1;
		desc.sampleCount = 1;
		desc.format = WGPUTextureFormat_RGBA16Float;
		desc.usage = WGPUTextureUsage_StorageBinding |
			WGPUTextureUsage_TextureBinding |
			WGPUTextureUsage_CopySrc |
			WGPUTextureUsage_CopyDst;

		texture = wgpuDeviceCreateTexture(device, &desc);

		WGPUTextureViewDescriptor viewDesc = {};
		viewDesc.format = desc.format;
		viewDesc.dimension = WGPUTextureViewDimension_3D;
		viewDesc.baseMipLevel = 0;
		viewDesc.mipLevelCount = 1;
		viewDesc.baseArrayLayer = 0;
		viewDesc.arrayLayerCount = 1;

		view = wgpuTextureCreateView(texture, &viewDesc);
	}

	~VolumeBuffer() {
		if (view) {
			wgpuTextureViewRelease(view);
			view = nullptr;
		}
		if (texture) {
			wgpuTextureDestroy(texture);
			wgpuTextureRelease(texture);
			texture = nullptr;
		}
	}
};

class VolumetricTexture : public State, public MouseEventListener, public KeyboardEventListener {

	struct RayMarchUniforms {
		Matrix4f projection;
		Matrix4f modelView;
		Matrix4f invModelView;
		Vector3f boxMin;
		float density;
		Vector3f boxMax;
		float brightness;
		int32_t steps;
		float threshold;
		float pad[2];
	};

	struct SplatUniforms {
		Vector3f center;
		float radius;
		Vector4f color;
	};

	struct WaveUniforms {
		Vector3f voxelSize;
		float damping;
	};

public:

	VolumetricTexture(StateMachine& machine);
	~VolumetricTexture();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;
	void resize(int deltaW, int deltaH) override;

private:

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void splat();

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_pendingSplat = false;

	Camera m_camera;
	Uniforms m_uniforms;
	Shape m_cube;
	TrackBall m_trackball;

	WgpBuffer m_uniformBuffer, m_splatBuffer, m_waveBuffer, m_rayMarchBuffer;
	WgpModel m_wgpCube;

	uint32_t m_width = 128, m_height = 128, m_depth = 128;

	VolumeBuffer* m_state[2];
	VolumeBuffer* m_prevState;
	int m_current;

	WGPUBindGroup m_bgSplat[2], m_bgWaves[2], m_bgRayMarch[2];
};