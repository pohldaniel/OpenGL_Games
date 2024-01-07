#pragma once
#include <engine/Vector.h>
#include <cstring>

struct MeshComp{
	char modelName[64];
	Vector4f color;
	MeshComp(const char* name, const Vector4f& color = Vector4f(0.0f, 0.0f, 0.0f, 0.0f)) : color(color) {
		memset(this->modelName, '\0', 64);
		strcpy_s(this->modelName, name);
	}
};