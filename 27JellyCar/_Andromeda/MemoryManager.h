#pragma once

#include "Texture2.h"

class MemoryManager{

protected:

	static MemoryManager *_memoryManager;
	MemoryManager();

public:

	static MemoryManager* Instance();

	virtual void AllocTexture(Texture2* texture) = 0;
	virtual void FreeTexture(Texture2* texture) = 0;
};