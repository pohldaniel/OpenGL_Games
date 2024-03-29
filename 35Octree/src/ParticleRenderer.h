#pragma once

#include <vector>
#include <engine/Vector.h>

#include "ParticleSystem.h"



class ParticleRenderer{

	class NormalAttributes {
	public:
		Vector3f pos;
		Vector2f uv;
		Vector3f color;
		float size;
		float alpha;
	};

public:
	
	std::vector<NormalAttributes> normalVertex;
	ParticleRenderer();
	void draw(const ParticleSystem& particleSystem);

};