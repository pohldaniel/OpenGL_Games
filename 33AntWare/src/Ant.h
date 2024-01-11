#pragma once

#include "ObjSequence.h"

class Ant : public ObjSequence {

public:
    
	Ant();

	void die();
	void start();
	void update(float deltaTime);
	void drawRaw();

private:

	void animate(float deltaTime);
	float speed = 8.0f;
	float detectionRaduis = 25.0f;
	float animTime = 0.0f;
	float walkcycleLength = 1.0f;
	int index = 0;
	int baseIndex = 0;
	
};
