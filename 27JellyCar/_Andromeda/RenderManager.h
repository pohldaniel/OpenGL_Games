#pragma once
#include <string>

#include "GraphicsEnums.h"
#include "Shader2.h"
#include "VertexArrayObject.h"
#include "FrameBufferObject.h"

#ifdef ANDROMEDA_GL3
#define RGBA8(r,g,b,a) ((((a)&0xFF)<<24) | (((b)&0xFF)<<16) | (((g)&0xFF)<<8) | (((r)&0xFF)<<0))
#endif

class TextureManager;

class RenderManager{

protected:

	static RenderManager* _renderManager;

	int _width;
	int _height;

	bool _fullScreen;

public:

	static RenderManager* Instance();

	RenderManager();

	friend class TextureManager;

public:

	size_t GetVertexSize(VertexType vertexType);
	void DeleteVertexData(void* vertices, VertexType vertexType);

	//window size
	int GetWidth();
	int GetHeight();

	bool IsFullScreen();

	void SetWindowSize(int width, int height);
	void SetFullScreen(bool state);

public:

	virtual ~RenderManager();

	//initialize render manager - init everything that is needed to start 
	virtual void Init() = 0;

	//end rendering and clean everything
	virtual void Finish() = 0;

	//clear screen
	virtual void ClearScreen() = 0;

	//set clear screen color
	virtual void SetClearColor(unsigned int color) = 0;

	//start and end of every frame
	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

	//swap buffers after everything is on screen
	virtual void SwapBuffers() = 0;

	//textures
	virtual void CreateTexture(Texture2* image) = 0;
	virtual void CreateEmptyTexture(Texture2* image) = 0;
	virtual void DestroyTexture(Texture2* image) = 0;
	virtual void UseTexture(Texture2* image) = 0;
	virtual void UpdateTexture(Texture2* image, void *data) = 0;

	//vertices
	virtual void* CreateVertices(VertexType vertexType, int size) = 0;
	virtual VertexArrayObject* CreateVertexArrayObject(VertexType vertexType, VertexBufferDrawType drawType) = 0;

	//framebuffer
	virtual FrameBufferObject* CreateFrameBufferObject(int width, int height) = 0;

	//shaders
	virtual Shader2* CreateShader() = 0;
	virtual std::string GetVertexShaderName() = 0;
	virtual std::string GetFragmnetShaderName() = 0;

	//depth
	virtual void SetDepth(bool state) = 0;

	//test
	virtual void SaveDisplayBuffer(std::string fileName) = 0;
};