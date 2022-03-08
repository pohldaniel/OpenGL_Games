#pragma once
#include <array>
#include <vector>
#include "Vector.h"
#include "Batchrenderer.h"

struct Particle {

	Particle(float lifeTime, Vector2f& direction) : lifeTime(lifeTime), constLifeTime(lifeTime), direction(direction){ }

	float lifeTime = 0.0f;
	float constLifeTime = 0.0f;
	Vector2f direction;
};

class ParticleEmitter {
public:
	ParticleEmitter();
	ParticleEmitter(const Vector4f& birthColor, const Vector4f& deathColor, unsigned size);
	~ParticleEmitter();

	void Update(const float& dt);
	void AddParticles();

	void SetPosition(const Vector2f& pos);
	void SetDirection(const Vector2f& dir);

	void SetLifeTimeRange(const float& min, const float& max);

	void SetSpeed(const float& speed);

	void SetBirthColor(Vector4f& color);
	void SetDeathColor(Vector4f& color);

	void SetParticleMax(unsigned max);
	void SetSpread(float spred);

	void SetSize(unsigned size);

	void Clear();
	void render();
	void render2();
private:
	Vector4f m_birthColor;
	Vector4f m_deathColor;

	const float m_updateTime = 0.015f;
	float m_movementSpeed = 5.5f;
	float m_elapsedTime = 0.0f;

	unsigned m_size = 10;
	unsigned m_max = 50;

	float m_lifeTimeMin = 0.0f;
	float m_lifeTimeMax = 0.0f;

	float m_spread = 0.3f;

	Vector2f m_position = Vector2f(800.0f, 450.0f);
	Vector2f m_direction;

	std::vector<Vertex> m_vertices;
	std::vector<Particle> m_particles;

	Batchrenderer* m_batchrenderer;

	void UpdateParticles();

	void AddQuad(const Vector2f& pos);
	void DeleteQuad(unsigned index);
	void SetQuadColor(unsigned index, const Vector4f& color);
	void SetQuadScale(unsigned index, const float& scale);
	void SetQuadRotation(unsigned index, const float& angle);
	void MoveQuad(unsigned index, const Vector2f& dir);

	Vector4f Lerp(const Vector4f& x, const Vector4f& y, const float& t);
	float Lerp(const float& x, const float& y, const float& t);

	void rotatePoint(float angle, float cx, float cy, float& x, float& y);
	void scalePoint(float scale, float cx, float cy, float& x, float& y);
};