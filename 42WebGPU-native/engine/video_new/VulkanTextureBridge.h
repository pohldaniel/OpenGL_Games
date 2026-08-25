#pragma once

extern "C" {
#include <libavutil/hwcontext_vulkan.h >
}

#include "IVideoTextureBridge.h"

class VulkanTextureBridge : public IVideoTextureBridge {

public:

	VulkanTextureBridge(int width, int height) : m_width(width), m_height(height) {
		initWebGPUEntities();
	}
	~VulkanTextureBridge() override { release(); }
	void updateTexture(AVFrame* frame) override;
	void initWebGPUEntities();
	void clearCache() override {}
	void release() override {
		if (m_textureViewY) { wgpuTextureViewRelease(m_textureViewY); m_textureViewY = nullptr; }
		if (m_textureViewUV) { wgpuTextureViewRelease(m_textureViewUV); m_textureViewUV = nullptr; }
		if (m_videoTexture) { wgpuTextureRelease(m_videoTexture); m_videoTexture = nullptr; }
		if (m_yTexture) { wgpuTextureRelease(m_videoTexture); m_yTexture = nullptr; }
		if (m_uvTexture) { wgpuTextureRelease(m_videoTexture); m_uvTexture = nullptr; }
		if (m_sharedTextureMemory) { wgpuSharedTextureMemoryRelease(m_sharedTextureMemory); m_sharedTextureMemory = nullptr; }
	}
	void init_export_texture_vulkan(AVHWDeviceContext* vulkanDevCtx, int width, int height);
private:
	int m_width;
	int m_height;
};