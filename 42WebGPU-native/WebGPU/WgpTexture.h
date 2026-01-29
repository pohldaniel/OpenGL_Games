#pragma once
#include <string>
#include <filesystem>
#include <webgpu.h>

class WgpTexture {

	friend class WgpMesh;

public:

	WgpTexture();
	WgpTexture(WgpTexture const& rhs);
	WgpTexture(WgpTexture&& rhs) noexcept;
	~WgpTexture();

	void loadFromFile(std::string fileName, const bool flipVertical = false, short alphaChannel = -1);
	void copyToDestination(const WGPUTexture& destTesture);
	void cleanup();
	void markForDelete();

	static unsigned char* LoadFromFile(std::string fileName, const bool flipVertical = false, short alphaChannel = -1);
	static unsigned char* LoadFromFile(std::string fileName, uint32_t& width, uint32_t& height, const bool flipVertical = false, short alphaChannel = -1);
	
private:

	WGPUTexture m_texture;

	unsigned int m_width;
	unsigned int m_height;
	unsigned short m_channels;
	unsigned char* m_data;
	bool m_markForDelete;

	static void FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
};