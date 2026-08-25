#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>

extern "C" {
#include <libavutil/hwcontext_d3d11va.h>
}

#include "IVideoTextureBridge.h"

typedef struct SharedTextureMemoryD3D11Texture2DDescriptorNew {
	WGPUChainedStruct chain;
	ID3D11Texture2D* texture;
} SharedTextureMemoryD3D11Texture2DDescriptorNew;

class D3D11TextureBridge : public IVideoTextureBridge {

public:

	D3D11TextureBridge(int width, int height, ID3D11Device* d3d11_device, ID3D11DeviceContext* d3d11_context) : m_width(width), m_height(height), m_d3d11_device(d3d11_device), m_d3d11_context(d3d11_context){
		initWebGPUEntities();
	}
	~D3D11TextureBridge() override { release(); }
	void clearCache() override {}
	void updateTexture(AVFrame* frame) override;
	void initWebGPUEntities();

	void release() override {
		if (m_textureViewY) { wgpuTextureViewRelease(m_textureViewY); m_textureViewY = nullptr; }
		if (m_textureViewUV) { wgpuTextureViewRelease(m_textureViewUV); m_textureViewUV = nullptr; }
		if (m_videoTexture) { wgpuTextureRelease(m_videoTexture); m_videoTexture = nullptr; }
		if (m_yTexture) { wgpuTextureRelease(m_videoTexture); m_yTexture = nullptr; }
		if (m_uvTexture) { wgpuTextureRelease(m_videoTexture); m_uvTexture = nullptr; }
		if (m_sharedTextureMemory) { wgpuSharedTextureMemoryRelease(m_sharedTextureMemory); m_sharedTextureMemory = nullptr; }
	}

private:
	int m_width;
	int m_height;
	ID3D11Device* m_d3d11_device = nullptr;
	ID3D11DeviceContext* m_d3d11_context = nullptr;
	ID3D11Texture2D* m_single_texture = nullptr;
};