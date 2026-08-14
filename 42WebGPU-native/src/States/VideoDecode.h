#pragma once
#include <vector>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpData.h>

#include <engine/video/VideoDecoder.h>
#include <engine/video/MediaDecoder.h>
#include <engine/sound/RtAudioPlayer.h>
#include <engine/sound/OpenALPlayer.h>
#include <engine/sound/AudioSystem.h>
#include <engine/sound/OpenALAudioSystem.h>
#include <d3d12.h>

class VideoDecode : public State, public MouseEventListener, public KeyboardEventListener {

	struct CameraUniforms {
		Matrix4f viewMatrix;
		float fov = 1.0f;
		float aspect = 1.6f;
		float padding[2];
	};

public:

	VideoDecode(StateMachine& machine);
	~VideoDecode();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnPostDraw();

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts360();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts360HW();

	WGPUBindGroup createBindGroupRGBA();
	WGPUBindGroup createBindGroupRight360Packed();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	TrackBall m_trackball;
	
	WgpBuffer m_cameraBuffer;
	WgpTexture m_textureRGBA, m_texturePacked;
	WGPUBindGroup m_bindGroupRGBA = NULL, m_bindGroupPacked = NULL, m_bindGroupHw = NULL;

	std::unique_ptr<AudioSystem> m_audioSystem;

	MediaDecoder m_movieRGBA, m_movieHw, m_moviePacked;
	OpenALMovieStream m_openALStreamRGBA, m_openALStreamPacked;
	RtAudioPlayer m_rtAudioPlayer;

	std::vector<uint8_t> m_pixelBufferRGBA, m_pixelBufferPacked, m_pixelBufferHw, m_audioBufferRGBA, m_audioBufferPacked;
	bool m_isUserDraggingTimeline = false;
	bool m_hasActiveAccess = false;
};