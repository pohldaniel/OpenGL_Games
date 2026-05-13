#pragma once

#include <webgpu.h>

class WgpTexture;
class WgpBuffer;

struct WgpRenderer {
	static void Draw(const WgpTexture& texture, std::function<void(const WGPURenderPassEncoder& commandBuffer, uint32_t layer, uint32_t mip)> OnDraw = NULL);
	static void DrawDepth(const WgpTexture& texture, std::function<void(const WGPURenderPassEncoder& commandBuffer)> OnDraw = NULL);
	static void Dispatch(const WgpTexture& texture, const WgpBuffer& probability, const WgpBuffer& bufferA, const WgpBuffer& bufferB);
};