#pragma once

#include "Texture2.h"

class RenderManager;

class FrameBufferObject {
protected:

	Texture2* _texture;

	int _width;
	int _height;

public:

	virtual ~FrameBufferObject();

	virtual void Bind() = 0;
	virtual void UnBind() = 0;

	Texture2* GetTexture();

	int GetWidth();
	int GetHeight();
};