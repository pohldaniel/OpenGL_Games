#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>

extern "C" {
#include <libavutil/hwcontext_d3d11va.h>
}

#include "IVideoDecoder.h"

typedef struct SharedTextureMemoryD3D11Texture2DDescriptor {
	WGPUChainedStruct chain;
	ID3D11Texture2D* texture;
} SharedTextureMemoryD3D11Texture2DDescriptor;

class D3D11Decoder : public IVideoDecoder {

public:

	D3D11Decoder();
	~D3D11Decoder();

	void updateTexture(AVFrame* frame) override;
	void configureContext(AVCodecContext* ctx, AVBufferRef* hwDeviceCtx) override;
	void init(int width, int height) override;

	void beginMemoryAccess() override;
	void endMemoryAccess() override;
	void setBindGroup(const WGPUBindGroup& bindgroup);

private:

	void initWebGPUEntities();

	ID3D11Device* m_d3d11_device = nullptr;
	ID3D11DeviceContext* m_d3d11_context = nullptr;
	ID3D11Texture2D* m_single_texture = nullptr;
};