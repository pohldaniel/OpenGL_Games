#pragma once

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/hwcontext_vulkan.h >
}
#include <WebGPU/WgpContext.h>

#include "IVideoDecoder.h"

class VulkanDecoder : public IVideoDecoder {

public:

	VulkanDecoder();
	~VulkanDecoder() override;

	void configureContext(AVCodecContext* ctx, AVBufferRef* hwDeviceCtx) override;
	void init(int width, int height) override;
	void updateTexture(AVFrame* frame) override;
	
private:

	void initWebGPUEntities();

	AVFrame* m_cpuFrame = nullptr;
	WGPUBuffer m_stagingBuffers[2] = { nullptr, nullptr };
	uint32_t m_currentFrameIndex = 0;
	uint32_t m_yBufferSize = 0;
	uint32_t m_uvBufferSize = 0;
};