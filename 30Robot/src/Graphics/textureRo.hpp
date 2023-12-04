#pragma once

#include <string>

class TextureRo {
private:
	unsigned int m_rendererID;
	std::string m_filePath;
	unsigned char* m_localBuffer;
	int m_width, m_height, m_bpp;

public:
	TextureRo(const std::string& path = "textures/missing.png");
	~TextureRo();
 
	void bind(unsigned int slot = 0) const;
	void unbind() const;

	unsigned int getID() const { return m_rendererID; }
	inline int getWidth() const { return m_width; }
	inline int getHeight() const { return m_height; }
};
