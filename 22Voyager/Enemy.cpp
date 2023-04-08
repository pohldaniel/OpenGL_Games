#include <cmath>
#include "Enemy.h"
#include "Utils.h"
#include "Player.h"

Enemy::Enemy(CameraVo& cam) :
	m_pos(Utils::GetInstance().RandomNumBetweenTwo(50.0f, 450.0f), 0.0f, Utils::GetInstance().RandomNumBetweenTwo(0.0f, 450.0f)),
	m_maximumSpeed(15.0f),
	m_maximumDroneSpeed(100.0f),
	m_velocity(glm::vec3(1.0f, 1.0f, 1.0f)),
	m_camera(cam),
	m_health(100),
	m_blastRadius(0.01f),
	m_distance(0.0f),
	m_shootDuration(0.0f),
	m_evadeDurationCounter(0.0f),
	m_damageTakenDuration(0.0f),
	m_attackDamage(10.0f),
	m_currLifeTimer(0.0f),
	m_dead(false),
	m_takingDamage(false),
	m_evade(false),
	m_evadeRight(false),
	m_fire(false),
	m_droneStatus(true),
	m_damageToken(true),
	m_canRespawn(true),
	m_dronePos(m_pos)
{
	
}

Enemy::~Enemy()
{}

void Enemy::Draw(short int enemyId, short int enemyDroneId)
{
	if (!m_dead)
	{
		// Activate enemy's shader program
		/*Renderer::GetInstance().GetComponent(enemyId).GetShaderComponent().ActivateProgram();

		// Check if the enemy is taking damage, if so, make the enemy flash red
		if (m_takingDamage)
			Renderer::GetInstance().GetComponent(enemyId).GetShaderComponent().SetBool("damaged", true);
		else
			Renderer::GetInstance().GetComponent(enemyId).GetShaderComponent().SetBool("damaged", false);

		// Update the enemy's transform and particle system every frame and draw the enemy
		Renderer::GetInstance().GetComponent(enemyId).SetTransform(m_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		Renderer::GetInstance().GetComponent(enemyId).Draw(m_camera, glm::vec3(0.0f, 0.0f, 0.0f), false, Player::GetInstance().GetSpotLight());

		

		// Check if a small drone has been fired by the enemy
		if (m_droneActive)
		{
			// Update the small drone's transform per frame
			Renderer::GetInstance().GetComponent(enemyDroneId).SetTransform(m_dronePos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f));
			Renderer::GetInstance().GetComponent(enemyDroneId).Draw(m_camera);

			// Check if the player is colliding with the drone
			if (Physics::GetInstance().PointInSphere(m_camera, m_dronePos, 2.0f))
			{
				// Inflict damage on player
				Physics::GetInstance().OnPlayerHit(m_attackDamage);
			}
		}*/
	}
	else
	{
		m_currLifeTimer = 0.0f;
		Respawn();
	}
}

void Enemy::DrawShockwave(short int enemyDroneBlastId)
{
	// Check if the smart drone has reached its desired destination where it is about to explode
	if (m_droneSelfDestruct)
	{
		// Increase the explosion radius
		m_blastRadius += 5.0f * m_deltaTime;

		if (m_blastRadius < 7.0f)
		{
			// Update explosion blast 
			//Renderer::GetInstance().GetComponent(enemyDroneBlastId).SetTransform(m_oldPlayerPos, glm::vec3(m_blastRadius * 20, m_blastRadius * 20, m_blastRadius * 20), glm::vec3(m_blastRadius));
			//Renderer::GetInstance().GetComponent(enemyDroneBlastId).Draw(m_camera);

			// Check if enemy can damage and player is caught within the growing blast
			if (m_damageToken && Physics::GetInstance().PointInSphere(m_camera, m_oldPlayerPos, (m_blastRadius * 4)))
			{
				// Inflict damage on player
				Physics::GetInstance().OnPlayerHit(m_attackDamage);

				// Take away the enemy's damage token (so that player does not continuously get hit while within the blast) 
				m_damageToken = false;
			}
		}
		else
		{
			// Restart blast properties
			m_droneSelfDestruct = false;
			m_blastRadius = 0.01f;

			// Restore the enemy's damage token
			m_damageToken = true;
		}
	}
}

void Enemy::Update(CameraVo& cam, float dt)
{
	
}

void Enemy::ReduceHealth(int amount) {
	m_health -= amount;
	m_takingDamage = true;

	if (m_health <= 0) {
		m_dead = true;

		
	}
}

// Function that finds the distance between two vectors
inline float Enemy::CalcDistance(glm::vec3& enemyPos, glm::vec3& playerPos)
{
	return sqrt(powf(enemyPos.x - playerPos.x, 2.0f) + powf(enemyPos.y - playerPos.y, 2.0f) + powf(enemyPos.z - playerPos.z, 2.0f));
}

// Function that finds the distance between two vectors without taking the y-axis into consideration (XZ plane only)
inline float Enemy::CalcDistanceNoHeight(glm::vec3& enemyPos, glm::vec3& playerPos)
{
	return sqrt(powf(enemyPos.x - playerPos.x, 2.0f) + powf(enemyPos.z - playerPos.z, 2.0f));
}

void Enemy::SetRespawnStatus(bool canRespawn)
{
	m_canRespawn = canRespawn;
}

void Enemy::Seek(CameraVo& target, const float dt)
{
	glm::vec3 desiredVelocity = target.GetCameraPos() - m_pos;
	desiredVelocity = glm::normalize(desiredVelocity);
	desiredVelocity *= m_maximumSpeed;

	glm::vec3 steering = desiredVelocity - m_velocity;
	steering = glm::clamp(steering, -m_maximumSpeed, m_maximumSpeed);

	m_pos += steering * dt;
}

void Enemy::Flee(CameraVo& target, const float dt)
{
	glm::vec3 desiredVelocity = target.GetCameraPos() - m_pos;
	desiredVelocity = glm::normalize(desiredVelocity);
	desiredVelocity *= m_maximumSpeed;

	glm::vec3 steering = desiredVelocity - m_velocity;
	steering = glm::clamp(steering, -m_maximumSpeed, m_maximumSpeed);

	m_pos -= steering * dt;
}

void Enemy::Respawn()
{
	if (m_canRespawn)
	{
		m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
		m_dronePos = m_pos;
		m_respawnTimer += 1.0f * m_deltaTime;

		if (m_respawnTimer >= 15.0f)
		{
			// Restart some properties
			m_respawnTimer = 0.0f;
			m_currLifeTimer = 0.0f;
			m_blastRadius = 0.01f;
			m_shootDuration = 0.0f;
			m_evadeDurationCounter = 0.0f; 
			m_damageTakenDuration = 0.0f;
			m_takingDamage = false;
			m_dead = false;
			m_droneSelfDestruct = false;
			m_blastRadius = 0.01f;
			m_health = 100;

			// Set new spawn position
			m_pos = glm::vec3(Utils::GetInstance().RandomNumBetweenTwo(50.0f, 520.0f), 0.0f, Utils::GetInstance().RandomNumBetweenTwo(0.0f, 650.0f));
		}
	}
}