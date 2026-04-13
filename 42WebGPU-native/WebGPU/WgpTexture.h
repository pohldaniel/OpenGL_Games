#pragma once
#include <string>
#include <filesystem>
#include <array>
#include <webgpu.h>

class FIBITMAP;
class WgpTexture {

	union UFloat {
		float flt;
		unsigned char c[4];
	};

public:

	WgpTexture();
	WgpTexture(WgpTexture const& rhs);
	WgpTexture(WgpTexture&& rhs) noexcept;
	~WgpTexture();

	void loadFromFile(const std::string& fileName, const bool flipVertical = false, const short alphaChannel = -1);
	void loadFromMemory(unsigned char* data, uint32_t size, const bool flipVertical = false, const short alphaChannel = -1);
	void loadHDRICubeFromFile(const std::string& fileName, const bool flipVertical = false, const bool halfBPP = false);
	void loadHDRIFromFile(const std::string& fileName, const bool flipVertical = false, const bool halfBPP = false);

	void createEmpty(uint32_t width, uint32_t height, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat);
	void cleanup();
	void markForDelete();

	const WGPUTexture& getTexture() const;
	const WGPUTextureView& getTextureView() const;

	const unsigned int getWidth() const;
	const unsigned int getHeight() const;
	const WGPUTextureFormat getFormat() const;

	static unsigned char* LoadFromFile(std::string fileName, const bool flipVertical = false, const short alphaChannel = -1);
	static unsigned char* LoadFromFile(std::string fileName, uint32_t& width, uint32_t& height, const bool flipVertical = false, const short alphaChannel = -1);
	static unsigned char* LoadFromMemory(unsigned char* data, uint32_t size, uint32_t& width, uint32_t& height, const bool flipVertical = false, const short alphaChannel = -1);
	
	static void Safe(const std::string& fileOut, unsigned char* bytes, uint32_t width, uint32_t height, uint32_t channels);
	static void Safe(const std::string& fileOut, unsigned char* bytes, uint32_t size);
	static void SafeHDRI(const std::string& fileOut, unsigned char* bytes, uint32_t width, uint32_t height, uint32_t channels);

private:
	
	static FIBITMAP* AddAlphaChannel(FIBITMAP* bitmap, const short alphaChannel = -1);
	static FIBITMAP* RemoveAlphaChannel(FIBITMAP* bitmap);
	static void FlipVertical(unsigned char* data, uint32_t width, uint32_t bytesPerChannel, uint32_t height);
	static void FlipHorizontal(unsigned char* data, uint32_t width, uint32_t bytesPerChannel, uint32_t height);
	static void Rotate90(unsigned char*& sourceInOut, uint32_t width, uint32_t bytesPerChannel, uint32_t height, bool ccw = false);

	static std::vector<unsigned char*> CrossToFaces(unsigned char* source, uint32_t width, uint32_t bytesPerChannel, uint32_t height);
	static void EquirectangularToCross(unsigned char*& sourceInOut, uint32_t width, uint32_t bytesPerChannel, uint32_t& height);
	static float Clamp(const float& n, const float& lower, const float& upper);
	static std::array<float, 3> OutImgToXYZ(int i, int j, int face, float edge);
	static float BytesToFloatLE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
	static float BytesToFloatBE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
	static uint32_t BitWidth(uint32_t m);
	static uint16_t Float32Tofloat16(float value);	
	static uint16_t* GetFloat16(float* data, uint32_t width, uint32_t height, uint32_t channels);
	

	template<typename component_t>
	static void WriteMipMaps(WGPUTexture& texture, WGPUExtent3D textureSize, uint32_t mipLevelCount, component_t* pixelData, uint32_t layer = 0u, const bool halfBPP = false);

	WGPUTexture m_texture;
	WGPUTextureFormat m_format;
	WGPUTextureView m_textureView;

	unsigned int m_width;
	unsigned int m_height;
	unsigned short m_channels;
	bool m_markForDelete;
};

template<typename component_t>
static void WgpTexture::WriteMipMaps(WGPUTexture& texture, WGPUExtent3D textureSize, uint32_t mipLevelCount, component_t* pixelData, uint32_t layer, const bool halfBPP) {
	uint32_t channels = 4u;

	WGPUTexelCopyTextureInfo destination = {};
	destination.texture = texture;
	destination.mipLevel = 0u;
	destination.origin = { 0u, 0u, layer };
	destination.aspect = WGPUTextureAspect_All;

	WGPUTexelCopyBufferLayout source = {};
	source.offset = 0u;

	// Create image data
	WGPUExtent3D mipLevelSize = textureSize;
	std::vector<component_t> previousLevelPixels;
	WGPUExtent3D previousMipLevelSize;
	for (uint32_t level = 0; level < mipLevelCount; ++level) {
		std::vector<component_t> pixels(channels * mipLevelSize.width * mipLevelSize.height);
		if (level == 0) {
			memcpy(pixels.data(), pixelData, pixels.size() * sizeof(component_t));
		}
		else {
			for (uint32_t i = 0; i < mipLevelSize.width; ++i) {
				for (uint32_t j = 0; j < mipLevelSize.height; ++j) {
					component_t* p = &pixels[channels * (j * mipLevelSize.width + i)];
					// Get the corresponding 4 pixels from the previous level
					component_t* p00 = &previousLevelPixels[channels * ((2 * j + 0) * previousMipLevelSize.width + (2 * i + 0))];
					component_t* p01 = &previousLevelPixels[channels * ((2 * j + 0) * previousMipLevelSize.width + (2 * i + 1))];
					component_t* p10 = &previousLevelPixels[channels * ((2 * j + 1) * previousMipLevelSize.width + (2 * i + 0))];
					component_t* p11 = &previousLevelPixels[channels * ((2 * j + 1) * previousMipLevelSize.width + (2 * i + 1))];
					// Average
					p[0] = (p00[0] + p01[0] + p10[0] + p11[0]) / (component_t)4;
					p[1] = (p00[1] + p01[1] + p10[1] + p11[1]) / (component_t)4;
					p[2] = (p00[2] + p01[2] + p10[2] + p11[2]) / (component_t)4;
					p[3] = (p00[3] + p01[3] + p10[3] + p11[3]) / (component_t)4;
				}
			}
		}

		// Upload data to the GPU texture
		destination.mipLevel = level;
		source.bytesPerRow = channels * mipLevelSize.width * sizeof(component_t);
		source.rowsPerImage = mipLevelSize.height;
		if (halfBPP) {
			source.bytesPerRow = channels * mipLevelSize.width * sizeof(uint16_t);
			uint16_t* float16 = GetFloat16(reinterpret_cast<float*>(pixels.data()), mipLevelSize.width, mipLevelSize.height, channels);
			wgpuQueueWriteTexture(wgpContext.queue, &destination, float16, mipLevelSize.width * mipLevelSize.height * channels * sizeof(uint16_t), &source, &mipLevelSize);
			free(float16);
		}
		else
			wgpuQueueWriteTexture(wgpContext.queue, &destination, pixels.data(), pixels.size() * sizeof(component_t), &source, &mipLevelSize);

		previousLevelPixels = std::move(pixels);
		previousMipLevelSize = mipLevelSize;
		mipLevelSize.width /= 2;
		mipLevelSize.height /= 2;
	}
}