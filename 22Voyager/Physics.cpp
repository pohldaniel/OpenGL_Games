#include "Physics.h"
#include "Player.h"

Physics::Physics() :
	m_mouseX(0.0f),
	m_mouseY(0.0f),
	m_gravity(-90.0f),
	m_castRay(false),
	m_collision(false)
{}

Physics::~Physics()
{}

Ray Physics::CastRayFromMouse(Camera& cam) {
	// screen space (viewport coordinates)
	float x = 1.0f;
	float y = 1.0f;
	//float x = (2.0f * m_mouseX) / Renderer::GetInstance().GetWindowWidth() - 1.0f;
	//float y = 1.0f - (2.0f * m_mouseY) / Renderer::GetInstance().GetWindowHeight();
	float z = 1.0f;

	// normalised device space
	/*glm::vec3 mouseNdcCoords = glm::vec3(x, y, z);
	glm::vec4 mouseClipCoords = glm::vec4(mouseNdcCoords.x, mouseNdcCoords.y, -1.0f, 1.0f);
	glm::mat4 invProjMat = glm::inverse(cam.GetProjectionMatrix());
	glm::vec4 eyeCoords = invProjMat * mouseClipCoords;
	eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
	glm::mat4 invViewMat = glm::inverse(cam.GetViewMatrix());
	glm::vec4 rayWorld = invViewMat * eyeCoords;
	glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));*/

	Ray ray;
	//ray.pos = cam.GetCameraPos();
	//ray.dir = rayDirection;

	return ray;
}

Ray Physics::CastRayFromWeapon(Camera& cam) {
	Ray ray;
	//ray.pos = cam.GetCameraPos();
	//ray.dir = cam.GetCameraForward();
	return ray;
}

bool Physics::RaySphere(Camera& cam, glm::vec3 RayDirWorld, double SphereRadius, double x, double y, double z) {

	// work out components of quadratic
	glm::vec3 v = glm::vec3(x, y, z);
	long double a = glm::dot(RayDirWorld, RayDirWorld);
	long double b = 2.0 * glm::dot(v, RayDirWorld);
	long double c = glm::dot(v, v) - SphereRadius * SphereRadius;
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

void Physics::CheckRaySphereCollision(Camera& cam, std::vector<Enemy*> enemies) {

	for (auto i = enemies.begin(); i != enemies.end(); ++i) {
		m_collision = RaySphere(cam, m_ray.dir, 3.0f, (*i)->GetPos().x, (*i)->GetPos().y, (*i)->GetPos().z);

		// Check if the ray is colliding with the sphere
		if (m_collision)
		{
			// Pass in the value the iterator is pointing to to the OnEnemyHit() function
			OnEnemyHit((*i));
		}
	}
}

inline void Physics::OnEnemyHit(Enemy* enemy) {
	enemy->ReduceHealth(Player::GetInstance().GetCurrWeapon().GetDamage());
}

 void Physics::OnPlayerHit(float damage) {
	Player::GetInstance().ReduceHealth(damage);
}

bool Physics::PointInSphere(Camera& camera, glm::vec3& other, float radius) {
	// Calculate distance between player and center of circle
	float distanceSq = std::pow(camera.getPositionX() - other.x, 2) + std::pow(camera.getPositionY() - other.y, 2) + std::pow(camera.getPositionZ() - other.z, 2);

	// Check if the player is within the radius (if radius is bigger than point is inside circle) 
	if (distanceSq < (radius * radius)) {
		return true;
	}

	return false;
}

void Physics::Update(Camera& cam, float dt) {
	//ProcessInput(cam, dt, events);

	// Check if ray casting is true
	if (m_castRay){
		// Cast ray and check for collision
		m_castRay = false;
		m_debugRayCastDraw = true;
		m_ray = CastRayFromWeapon(cam);
		//CheckRaySphereCollision(cam, enemies);
	}
}