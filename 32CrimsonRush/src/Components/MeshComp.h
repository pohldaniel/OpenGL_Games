#pragma once

#include <cstring>

struct MeshComp{
	char modelName[64];

	MeshComp(const char* name){
		memset(this->modelName, '\0', 64);
		strcpy_s(this->modelName, name);
	}
};