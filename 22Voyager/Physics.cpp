#include "Physics.h"
#include "Player.h"
#include "Constants.h"

Physics::Physics() : m_mouseX(0.0f), m_mouseY(0.0f), m_gravity(-90.0f), m_castRay(false), m_collision(false) {

}

Physics::~Physics() { 

}

Ray Physics::CastRayFromWeapon(const Camera& cam) {
	Ray ray;
	ray.pos = cam.getPosition();
	ray.dir = cam.getViewDirection();
	return ray;
}

bool Physics::RaySphere(const Camera& cam, const Vector3f& RayDirWorld, double SphereRadius, double x, double y, double z) {

	// work out components of quadratic
	Vector3f v = Vector3f(x, y, z) - cam.getPosition();
	long double a = Vector3f::Dot(RayDirWorld, RayDirWorld);
	long double b = 2.0 * Vector3f::Dot(v, RayDirWorld);
	long double c = Vector3f::Dot(v, v) - SphereRadius * SphereRadius;
	long double b_squared_minus_4ac = b * b + (-4.0) * a * c;

	if (b_squared_minus_4ac == 0) {
		// One real root 
		return true;
	} else if (b_squared_minus_4ac > 0) {
		// Two real roots 
		long double x1 = (-b - sqrt(b_squared_minus_4ac)) / (2.0 * a);
		long double x2 = (-b + sqrt(b_squared_minus_4ac)) / (2.0 * a);

		if (x1 >= 0.0 || x2 >= 0.0)
			return true;
		if (x1 < 0.0 || x2 >= 0.0)
			return true;
	}

	// No real roots
	return false;
}

void Physics::CheckRaySphereCollision(const Camera& cam, std::vector<Enemy*> enemies) {

	for (auto i = enemies.begin(); i != enemies.end(); ++i) {
		m_collision = RaySphere(cam, m_ray.dir, 3.0f, (*i)->GetPos()[0], (*i)->GetPos()[1], (*i)->GetPos()[2]);

		// Check if the ray is colliding with the sphere
		if (m_collision){
			// Pass in the value the iterator is pointing to to the OnEnemyHit() function
			OnEnemyHit((*i));
		}
	}
}

inline void Physics::OnEnemyHit(Enemy* enemy) {
	Globals::soundManager.get("enemy").play("res/Audio/EnemyHit.wav");
	enemy->ReduceHealth(Player::GetInstance().GetCurrWeapon().getDamage());
}

 void Physics::OnPlayerHit(float damage) {
	 Globals::soundManager.get("player").play("res/Audio/Hurt.wav");
	Player::GetInstance().ReduceHealth(damage);
}

bool Physics::PointInSphere(const Camera& camera, const Vector3f& other, float radius) {
	// Calculate distance between player and center of circle
	float distanceSq = std::pow(camera.getPositionX() - other[0], 2) + std::pow(camera.getPositionY() - other[1], 2) + std::pow(camera.getPositionZ() - other[2], 2);

	// Check if the player is within the radius (if radius is bigger than point is inside circle) 
	if (distanceSq < (radius * radius)) {
		return true;
	}

	return false;
}

void Physics::Update(const Camera& cam, float dt, std::vector<Enemy*>& enemies) {
	//ProcessInput(cam, dt, events);

	// Check if ray casting is true
	if (m_castRay){
		// Cast ray and check for collision
		m_castRay = false;
		m_debugRayCastDraw = true;
		m_ray = CastRayFromWeapon(cam);
		CheckRaySphereCollision(cam, enemies);
	}
}