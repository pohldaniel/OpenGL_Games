#pragma once

#include "RenderManager.h"

class RenderManagerGL3 : public RenderManager {

private:

	float clearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	unsigned int _lastTextureId = 0;

public:

	RenderManagerGL3();



	void Init();
	void Finish();

	void ClearScreen();
	void SetClearColor(unsigned color);

	void StartFrame();
	void EndFrame();

	void SwapBuffers();

	void CreateTexture(Texture2* image);
	void CreateEmptyTexture(Texture2* image);
	void UseTexture(Texture2* image);
	void DestroyTexture(Texture2* image);
	void UpdateTexture(Texture2* image, void *data);

	void* CreateVertices(VertexType vertexType, int size);
	VertexArrayObject* CreateVertexArrayObject(VertexType vertexType, VertexBufferDrawType drawType);

	FrameBufferObject* CreateFrameBufferObject(int width, int height);

	Shader2* CreateShader();
	std::string GetVertexShaderName();
	std::string GetFragmnetShaderName();

	void SetDepth(bool state);

	void SaveDisplayBuffer(std::string fileName);
};