#pragma once
#pragma once

#include <vector>
#include "engine/Camera.h"
#include "Enemy.h"

struct Ray {
	Vector3f pos;
	Vector3f dir;
};

class Physics {
public:
	~Physics();

	static Physics& GetInstance() {
		static Physics instance;
		return instance;
	}

	Physics(Physics const&) = delete;

	void operator=(Physics const&) = delete;

	//void ProcessInput(CameraVo& cam, float dt, std::vector<SDL_Event> events);
	void Update(const Camera& cam, float dt, std::vector<Enemy*>& enemies);

	void CastRay()					{ m_castRay = true; }
	Ray& GetRay()					{ return m_ray; }
	bool GetDebugRayCastDraw()		{ return m_debugRayCastDraw; }
	float GetGravity()				{ return m_gravity; }
	void OnEnemyHit(Enemy* enemy);
	void OnPlayerHit(float damage);
	bool PointInSphere(const Camera& cam, const Vector3f& other, float radius);

private:

	Physics();

	Ray m_ray;
	float m_mouseX, m_mouseY;
	bool m_debugRayCastDraw;
	bool m_collision;
	bool m_castRay;
	float m_gravity;

	// Private functions
	Ray CastRayFromMouse(const Camera& cam);
	Ray CastRayFromWeapon(const Camera& cam);
	void CheckRaySphereCollision(const Camera& cam, std::vector<Enemy*> enemies);
	bool RaySphere(const Camera& cam, const Vector3f& RayDirWorld, double SphereRadius, double x, double y, double z);
};