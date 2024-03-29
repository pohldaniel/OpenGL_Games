#pragma once

#include <engine/Vector.h>
#include <engine/Object.h>
#include <engine/BoundingBox.h>

#include "Random.h"
#include "Interpolators.h"

class ParticleSystem {

public:

	class Particle{

	public:
		Particle(ParticleSystem* system, const Vector3f& pos, const Vector3f& dir);
		void updateParameters(float lrp);
		void update(float dt);

		Vector3f pos;
		Vector3f dir;

		float elapsed_sec;

		ParticleSystem *system;

		float lifetime_sec;
		float lifetime_sec_inv;

		float speed;
		float size;
		Vector3f color;
		float alpha;

		float distance_to_camera;

		bool canBeRemoved;
	};

	ParticleSystem();
	~ParticleSystem();

	void update(float dt);
	void setLifetime(float v_sec);
	float getLifetime() const;
	float getLifetime_inv() const;
	void setRateOverTime(float v);
	Particle *emmitParticle();
	void emmitStart();
	void computeAABB();
	void prewarmStart();
	void sortPositions(const Vector3f& cameraPos, const Vector3f& cameraDirection);

	float lifetime_sec;
	float lifetime_sec_inv;

	float rateOverTime;
	float rateOverTime_inv;
	float rateElapsed_sec;

	Utils::Random32 random32;
	Utils::MathRandomExt<Utils::Random32> mathRandom;

	std::vector<Particle> particles;

	bool loop;
	float elapsed_sec;
	float duration_sec;

	Utils::LinearInterpolator<float> Speed;
	Utils::LinearInterpolator<float> Size;
	Utils::LinearInterpolator<Vector3f> Color;
	Utils::LinearInterpolator<float> Alpha;

	Vector4f textureColor;
	bool soft;

	Object boxEmmiter;
	Vector3f emissionDir;
	Vector3f emissionPos;
	BoundingBox aabb;
	Vector3f aabb_center;
};