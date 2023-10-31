#include "FrameBufferObject.h"

FrameBufferObject::~FrameBufferObject(){

}

Texture2* FrameBufferObject::GetTexture(){
	return _texture;
}

int FrameBufferObject::GetWidth(){
	return _width;
}

int FrameBufferObject::GetHeight(){
	return _height;
}