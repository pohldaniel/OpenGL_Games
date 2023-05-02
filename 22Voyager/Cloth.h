#pragma once

#include <vector>

#include "engine/Shader.h"
#include "engine/Camera.h"
#include "engine/Texture.h"

#include "ClothParticle.h"
#include "Constraint.h"

class Cloth {

public:
	Cloth();
	~Cloth();

	void Configure(float w, float h, int totalParticlesW, int totalParticlesH);
	void Draw(const Camera& cam);
	void Update();
	void AddForce(const Vector3f& dir);
	void WindForce(const Vector3f& dir);

	void SetPos(const Vector3f& pos) { m_position = pos; }
	Shader* GetShaderComponent() { return m_shader; }

private:

	struct Vert {
		Vector3f m_pos;
		Vector2f m_uv;
		Vector3f m_norm;
	};

	int m_numParticlesWidth, m_numParticlesHeight;
	std::vector<ClothParticle> m_particles;
	std::vector<Constraint> m_constraints;
	Shader* m_shader;
	Vector3f m_position;

	// Private functions
	int GetParticleIndex(int x, int y) { return y * m_numParticlesWidth + x; }
	ClothParticle* GetParticle(int x, int y) { return &m_particles[GetParticleIndex(x, y)]; }
	void CreateConstraint(ClothParticle* p1, ClothParticle* p2) { m_constraints.push_back(Constraint(p1, p2)); }
	Vector3f CalculateTriNormal(ClothParticle* p1, ClothParticle* p2, ClothParticle* p3);
	void AddWindForce(ClothParticle* p1, ClothParticle* p2, ClothParticle* p3, const Vector3f& windDir);
	void AddTriangle(ClothParticle* p1, const Vector2f& uv, std::vector<Vert>& vertexData);
};