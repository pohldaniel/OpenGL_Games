#pragma once
#define NOMINMAX
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

#include "engine/Texture.h"
#include "engine/Extension.h"
#include "engine/Batchrenderer.h"



struct sTexture {
	unsigned int texture;
	float x1, x2, y1, y2;
	int height, width, textureOffsetX, textureOffsetY;
	bool contains_collision_box;
	std::string textureFile;
	unsigned int frame;
	sTexture() {
		texture = 0;
		x1 = 0.0f;
		x2 = 0.0f;
		y1 = 0.0f;
		y2 = 0.0f;
		height = 0;
		width = 0;
		contains_collision_box = false;
		textureFile = "";
		textureOffsetX = 0;
		textureOffsetY = 0;
		frame = 0;
	}
};



struct TextureAtlas {
	

	unsigned char* buffer;
	size_t width;
	size_t height;
	size_t curX;
	size_t curY;
	size_t maxY;
	GLuint texID;
	GLuint m_texture;
	size_t curTextures;
	size_t usedSpace;
	size_t currentFrame;

	TextureAtlas(){
		width = 1024;
		height = 1024;
		curX = 0;
		curY = 0;
		maxY = 0;
		curTextures = 0;
		currentFrame = 0;
		usedSpace = 0;
		buffer = new unsigned char[width * height * 4];
		memset(buffer, 0, width*height * 4);
		glGenTextures(1, &texID);

		glGenTextures(1, &m_texture);
	}

	size_t getWidth() const{
		return width;
	}

	size_t getHeight() const{
		return height;
	}

	void addTexture(sTexture& stexture, char *texture, size_t w, size_t h){

		if (width - curX < w){
			curX = 0;
			curY = maxY;
		}

		if (height - curY < h){
			finishFrame();
		}

		for (size_t row = 0; row<h; ++row){
			memcpy(buffer + (((curY + row)*width + curX) * 4), texture + (w*row * 4), 4 * w);
		}

		stexture.x1 = static_cast<float>(curX) / static_cast<float>(width);
		stexture.x2 = static_cast<float>(curX + w) / static_cast<float>(width);
		stexture.y1 = static_cast<float>(curY) / static_cast<float>(height);
		stexture.y2 = static_cast<float>(curY + h) / static_cast<float>(height);
		stexture.width = w;
		stexture.height = h;
		stexture.texture = texID;
		stexture.frame = currentFrame;
		curX += w;
		maxY = (std::max)(maxY, curY + h);
		++curTextures;
	}

	void finishFrame(){
		if (curX == 0 && curY == 0) return;
		//Texture::Safe("atlas_" + std::to_string(currentFrame) + ".png", texID, width, height, 4, GL_RGBA);
		currentFrame++;

		unsigned int fbo = 0;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

		unsigned int texture_new;
		glGenTextures(1, &texture_new);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture_new);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, currentFrame, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		for (int layer = 0; layer < currentFrame - 1; ++layer) {
			glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0, layer);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, 0, 0, width, height);
		}
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, currentFrame - 1, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		/*glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
		glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, currentFrame - 1, 0, 0, width, height);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);*/

		glBindTexture(GL_TEXTURE_2D_ARRAY, texture_new);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

		glDeleteTextures(1, &m_texture);
		m_texture = texture_new;
		
		memset(buffer, 0, width*height * 4);

		curX = 0;
		curY = 0;
		maxY = 0;
		curTextures = 0;
		usedSpace = 0;

		// Have OpenGL generate a texture object handle for us
		glGenTextures(1, &texID);

		
	}

	void safeAtlas() {
		Texture::Safe("atlas.png", texID, width, height, 4, GL_RGBA);
	}
};


class TextureCache{

public:
	TextureCache();
	~TextureCache();

	sTexture getTextureFromCache(std::string filename);

private:
	std::map< std::string, sTexture > textures;
};

class CTexture{
public:
	sTexture& getTexture(int index);
	std::vector<sTexture>& getTexture();
	void LoadIMG(std::string file, int texture_index, bool isOpenGLThreadInThreadedMode = false, int textureOffsetX = 0, int textureOffsetY = 0);
	void DrawTexture(int x, int y, int draw_id, float transparency = 1.0f, float red = 1.0f, float green = 1.0f, float blue = 1.0f, float x_scale = 1.0f, float y_scale = 1.0f);

	static bool isRectOnScreen(int left, int width, int bottom, int height);

	static TextureAtlas s_textureAtlas;

private:
	std::vector<sTexture> m_texture;

	
};