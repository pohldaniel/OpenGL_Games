#pragma once
#include "engine/Vector.h"

#include "Object.h"

class RenderableObject : Object {

public:
	RenderableObject(unsigned int category);
	virtual void render() = 0;

private:
	
};