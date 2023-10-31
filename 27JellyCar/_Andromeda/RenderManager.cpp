#include "RenderManager.h"
#include "RenderManagerGL3.h"
#include "VertexTypes.h"

RenderManager* RenderManager::_renderManager = 0;

RenderManager* RenderManager::Instance()
{
	if (_renderManager == 0)
	{
#ifdef ANDROMEDA_GL3
		{
			_renderManager = new RenderManagerGL3();
		}
#endif

#ifdef ANDROMEDA_SWITCH
		{
			_renderManager = new RenderManagerGL3();
		}
#endif

#ifdef ANDROMEDA_VITA
		{
			_renderManager = new RenderManagerGxm();
		}
#endif
	}

	return _renderManager;
}

RenderManager::RenderManager()
{
	_fullScreen = false;

	_width = -1;
	_height = -1;
}

RenderManager::~RenderManager()
{

}

size_t RenderManager::GetVertexSize(VertexType vertexType)
{
	switch (vertexType)
	{
	case Simple:
		return sizeof(SimpleVertex);
		break;
	case Color:
		return sizeof(ColorVertex);
		break;
	case Textured:
		return sizeof(TextureVertex);
		break;
	case TextureNormal:
		return sizeof(TextureNormalVertex);
		break;
	case TextureColor:
		return sizeof(TextureColorVertex);
		break;
	case ColorNormal:
		return sizeof(ColorNormalVertex);
		break;
	case TextureColorNormal:
		return sizeof(TextureColorNormalVertex);
		break;
	case NormalColorWeighJoint:
		return sizeof(NormalColorWeighJointVertex);
		break;
	case NormalTextureWeighJoint:
		return sizeof(NormalTextureWeighJointVertex);
		break;
	default: return 0;
	}

	return 0;
}

void RenderManager::DeleteVertexData(void* vertices, VertexType vertexType)
{
	//switch (vertexType)
	//{
	//case Simple:
	//    delete[] (SimpleVertex*)vertices;
	//    break;
	//case Color:
	//    delete[](ColorVertex);
	//    break;
	//case Textured:
	//    delete[](TextureVertex);
	//    break;
	//case TextureNormal:
	//    delete[](TextureNormalVertex);
	//    break;
	//case TextureColor:
	//    delete[](TextureColorVertex);
	//    break;
	//case ColorNormal:
	//    delete[](ColorNormalVertex);
	//    break;
	//case TextureColorNormal:
	//    delete[](TextureColorNormalVertex);
	//    break;
	//}
}

int RenderManager::GetWidth()
{
	return _width;
}

int RenderManager::GetHeight()
{
	return _height;
}

bool RenderManager::IsFullScreen()
{
	return _fullScreen;
}

void RenderManager::SetWindowSize(int width, int height)
{
	_width = width;
	_height = height;
}

void RenderManager::SetFullScreen(bool state)
{
	_fullScreen = state;
}