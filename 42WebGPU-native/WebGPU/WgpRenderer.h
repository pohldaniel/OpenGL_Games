#pragma once

#include <webgpu.h>

class WgpTexture;
struct WgpRenderer {
	static void Draw(const WgpTexture& texture, std::function<void(const WGPURenderPassEncoder& commandBuffer, uint32_t layer, uint32_t mip)> OnDraw = NULL);
	static void DrawDepth(const WgpTexture& texture, std::function<void(const WGPURenderPassEncoder& commandBuffer)> OnDraw = NULL);
};