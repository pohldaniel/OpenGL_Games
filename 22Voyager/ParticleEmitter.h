#pragma once

#include <vector>
#include "engine/Vector.h"
#include "engine/Camera.h"

#include "Particle.h"

class ParticleEmitter {

public:
	ParticleEmitter();
	~ParticleEmitter();

	void Init(int numberOfParticles);
	void Render(const Camera& camera, float dt, const Vector3f& origin = Vector3f(0.0f, 0.0f, 0.0f));

	std::vector<Particle>& GetParticle() { return m_particles; }
	std::vector<Vector3f>& GetPos() { return m_position; }

private:
	enum { VERTEX_VB, COLOR_VB, TOTAL_BUFFERS };

	unsigned int m_vao, m_vbo[TOTAL_BUFFERS], m_textureID;
	float m_numberOfParticles;

	std::vector<Particle> m_particles;
	std::vector<Vector3f> m_position;
};