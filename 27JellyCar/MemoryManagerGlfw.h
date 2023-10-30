#pragma once

#include "MemoryManager.h"

class MemoryManagerGlfw : public MemoryManager {

public:

	void AllocTexture(Texture2* texture);
	void FreeTexture(Texture2* texture);
};