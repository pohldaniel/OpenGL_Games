#pragma once
#include <string>
#include <webgpu.h>

class WgpTexture {

public:

	WgpTexture(const WGPUDevice& device, const WGPUQueue& queue);
	WgpTexture(WgpTexture const& rhs);
	WgpTexture(WgpTexture&& rhs) noexcept;

	void loadFromFile(std::string fileName, const bool flipVertical = false);

private:

	WGPUTexture m_texture;

	unsigned int m_width;
	unsigned int m_height;
	unsigned short m_channels;
	unsigned char* m_data;

	const WGPUDevice& device;
	const WGPUQueue& queue;

	static void FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
};