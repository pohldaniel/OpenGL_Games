#pragma once

#include "engine/Shader.h"
#include "VolumeBuffer.h"

// class to represent a simple 3D simulation
class FluidSystem {
public:
	FluidSystem(int width, int height, int depth);
	~FluidSystem();

	void reset();
	void step(float timestep);
	void splat();

	VolumeBuffer *getStateBuffer() { return m_state[m_current]; }
	Shader* m_splat;
	Shader* m_copy;
	Shader* m_waves;
	Shader* m_step;
	Shader* m_clear;

private:
	float frand();

	int m_width, m_height, m_depth;

	VolumeBuffer *m_state[2];
	VolumeBuffer *m_prevState;
	int m_current;
};
