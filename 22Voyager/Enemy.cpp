#include <cmath>
#include "Enemy.h"
#include "Utils.h"
#include "Player.h"
#include "Constants.h"
#include "Terrain.h"

RenderableObject Enemy::Drone = RenderableObject("sphere", "unlit", "drone");
RenderableObject Enemy::Shockwave = RenderableObject("sphere", "unlit", "shockwave");

Enemy::Enemy(const Camera& cam) : RenderableObject("sphere", "enemy", "enemyTex"), 
	m_pos(Utils::GetInstance().RandomNumBetweenTwo(50.0f, 450.0f), 0.0f, Utils::GetInstance().RandomNumBetweenTwo(0.0f, 450.0f)),
	m_maximumSpeed(15.0f),
	m_maximumDroneSpeed(100.0f),
	m_velocity(Vector3f(1.0f, 1.0f, 1.0f)),
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
	m_damageToken(false),
	m_canRespawn(true),
	m_dronePos(m_pos) {

	Drone.setDrawFunction([&](const Camera& camera) {
		if (Drone.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(Drone.getShader());

		shader->use();

		shader->loadMatrix("projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("view", camera.getViewMatrix());
		shader->loadMatrix("model", Drone.getTransformationSP());
		shader->loadInt("meshTexture", 0);

		Globals::textureManager.get(Drone.getTexture()).bind(0);
		Globals::shapeManager.get(Drone.getShape()).drawRaw();

		shader->unuse();
	});

	Shockwave.setDrawFunction([&](const Camera& camera) {
		if (Shockwave.isDisabled()) return;

		auto shader = Globals::shaderManager.getAssetPointer(Shockwave.getShader());

		shader->use();

		shader->loadMatrix("projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("view", camera.getViewMatrix());
		shader->loadMatrix("model", Shockwave.getTransformationSOP());
		shader->loadInt("meshTexture", 0);

		Globals::textureManager.get(Shockwave.getTexture()).bind(0);
		Globals::shapeManager.get(Shockwave.getShape()).drawRaw();

		shader->unuse();
	});

	m_particleEffect.Init(20);
}

Enemy::~Enemy() {

}

void Enemy::draw(const Camera& camera) {
	if (!m_dead){
		setPosition(m_pos);
		auto shader = Globals::shaderManager.getAssetPointer(getShader());

		shader->use();

		shader->loadMatrix("projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("view", camera.getViewMatrix());
		shader->loadMatrix("model", getTransformationSP());
		shader->loadVector("lightPos", Vector3f(0.0f, 0.0f, 0.0f));
		shader->loadVector("viewPos", camera.getPosition());
		shader->loadInt("meshTexture", 0);

		shader->loadBool("damaged", m_takingDamage);

		if (m_spotlight != nullptr) {
			shader->loadVector("spotlight.position", m_spotlight->getPosition());
			shader->loadVector("spotlight.direction", m_spotlight->getDirection());
			shader->loadVector("spotlight.diffuse", m_spotlight->getDiffuse());
			shader->loadVector("spotlight.specular", m_spotlight->getSpecular());
			shader->loadFloat("spotlight.constant", m_spotlight->getConstant());
			shader->loadFloat("spotlight.linear", m_spotlight->getLinear());
			shader->loadFloat("spotlight.quadratic", m_spotlight->getQuadratic());
			shader->loadFloat("spotlight.cutOff", cosf(m_spotlight->getCutOff() * PI_ON_180));
			shader->loadFloat("spotlight.outerCutOff", cosf(m_spotlight->getOuterCutOff() * PI_ON_180));
		}
		Globals::textureManager.get("enemyTex").bind(0);
		Globals::shapeManager.get("sphere").drawRaw();

		shader->unuse();
	
		if (m_currLifeTimer >= 0.2f)
			m_particleEffect.Render(camera, m_deltaTime, Vector3f(m_pos[0] - 1.7f, m_pos[1] + 4.5f, m_pos[2] - 0.4f));

		// Check if a small drone has been fired by the enemy
		if (m_droneActive){
			
			Drone.setScale(0.25f, 0.25f, 0.25f);
			Drone.setPosition(m_dronePos);
			Drone.draw(camera);
			// Check if the player is colliding with the drone
			if (Physics::GetInstance().PointInSphere(camera, m_dronePos, 2.0f)){
				// Inflict damage on player
				Physics::GetInstance().OnPlayerHit(m_attackDamage);
			}
		}
	} else {
		m_currLifeTimer = 0.0f;
		Respawn();
	}
}

void Enemy::DrawShockwave() {
	// Check if the smart drone has reached its desired destination where it is about to explode
	if (m_droneSelfDestruct) {
		// Increase the explosion radius
		m_blastRadius += 5.0f * m_deltaTime;

		if (m_blastRadius < 7.0f) {
			// Update explosion blast 
			Shockwave.setScale(m_blastRadius);
			Shockwave.setOrientation(m_blastRadius * 20, m_blastRadius * 20, m_blastRadius * 20);
			Shockwave.setPosition(m_oldPlayerPos);
			Shockwave.draw(m_camera);

			// Check if enemy can damage and player is caught within the growing blast
			if (m_damageToken && Physics::GetInstance().PointInSphere(m_camera, m_oldPlayerPos, (m_blastRadius * 4))) {
				// Inflict damage on player
				Physics::GetInstance().OnPlayerHit(m_attackDamage);

				// Take away the enemy's damage token (so that player does not continuously get hit while within the blast) 
				m_damageToken = false;
			}
		} else {
			// Restart blast properties
			m_droneSelfDestruct = false;
			m_blastRadius = 0.01f;

			// Restore the enemy's damage token
			m_damageToken = true;
		}
	}
}

void Enemy::update(const Terrain& terrain, const Camera& cam, const float dt) {

	if (!m_dead){
		m_deltaTime = dt;

		m_pos[1] = terrain.getHeightOfTerrain(m_pos[0], m_pos[2]) + 5.0f;
		m_distance = CalcDistance(m_pos, cam.getPosition());
		m_currLifeTimer += 0.1f * dt;

		// Check if the player is getting too close, if so then run away, if not then move towards player
		if (m_distance < 75.0f) {
			Flee(cam, dt);
		} else{
			Seek(cam, dt);
		}

		// Check if enemy is being hit
		if (m_takingDamage) {
			// Create damage taken duration window to simulate enemy panick behaviour
			m_damageTakenDuration += 0.1f * dt;

			// Check if the damage taken duration window has exceeded a small threshold
			if (m_damageTakenDuration > 0.02f) {
				// Stop panicking
				m_takingDamage = false;
				m_damageTakenDuration = 0.0f;
			} else {
				// Generate random number (1 - 100)
				int dashSidewaysChance = Utils::GetInstance().RandomNumBetween1And100();

				// Check the chance for evade (3% chance)
				if (dashSidewaysChance > 97) {
					// Mark the next goal of the enemy (to evade) 
					m_evade = true;

					// Generate random side the enemy should evade towards
					int dashSide = Utils::GetInstance().RandomNumBetween1And100();

					// 50/50 chance to evade in either sides
					if (dashSide > 50)
						m_evadeRight = false;
					else
						m_evadeRight = true;

					// Stop panicking
					m_takingDamage = false;
					m_damageTakenDuration = 0.0f;
				}
			}
		}

		if (m_evade) {
			if (!m_evadeRight) {
				m_pos[0] -= (m_maximumSpeed * 5) * dt;
			} else {
				m_pos[0] += (m_maximumSpeed * 5) * dt;
			}

			m_evadeDurationCounter += 0.1f * dt;

			if (m_evadeDurationCounter > 0.07f) {
				m_evade = false;
				m_evadeRight = false;
				m_evadeDurationCounter = 0.0f;
			}
		}

		if (m_shootDuration > 1.0f) {
			m_shootDuration = 0.0f;

			// Check if the enemy unit is not firing already
			if (!m_fire)
				m_fire = true;
		}else {
			m_shootDuration += Utils::GetInstance().RandomNumBetweenTwo(0.1f, 0.5f) * dt;
		}

		if (m_fire) {
			m_dronePos[1] = terrain.getHeightOfTerrain(m_dronePos[0], m_dronePos[2]) + 10.0f;
			Fire(cam, terrain, dt);
		}

	}else {
		m_pos[1] = -999.0f;
		m_dronePos[1] = -999.0f;
	}
}

void Enemy::ReduceHealth(int amount) {
	m_health -= amount;
	m_takingDamage = true;
	if (m_health <= 0) {
		m_dead = true;

		// Player one of the monster dead sounds
		if (Utils::GetInstance().RandomNumBetweenTwo(1.0f, 2.0f) > 1.5f)
			Globals::soundManager.get("enemy").playOverlayed("res/Audio/EnemyDead.wav");
		else
			Globals::soundManager.get("enemy").playOverlayed("res/Audio/EnemyDead2.wav");
	}
}

// Function that finds the distance between two vectors
inline float Enemy::CalcDistance(const Vector3f& enemyPos, const Vector3f& playerPos) {
	return sqrt(powf(enemyPos[0] - playerPos[0], 2.0f) + powf(enemyPos[1] - playerPos[1], 2.0f) + powf(enemyPos[2] - playerPos[2], 2.0f));
}

// Function that finds the distance between two vectors without taking the y-axis into consideration (XZ plane only)
inline float Enemy::CalcDistanceNoHeight(const Vector3f& enemyPos, const Vector3f& playerPos) {
	return sqrt(powf(enemyPos[0] - playerPos[0], 2.0f) + powf(enemyPos[2] - playerPos[2], 2.0f));
}

void Enemy::SetRespawnStatus(bool canRespawn) {
	m_canRespawn = canRespawn;
}

void Enemy::Seek(const Camera& target, const float dt) {

	Vector3f desiredVelocity = target.getPosition() - m_pos;
	desiredVelocity = Vector3f::Normalize(desiredVelocity);
	desiredVelocity *= m_maximumSpeed;

	Vector3f steering = desiredVelocity - m_velocity;
	steering = Vector3f::Clamp(steering, -m_maximumSpeed, m_maximumSpeed);

	m_pos += steering * dt;
}

void Enemy::Flee(const Camera& target, const float dt) {

	Vector3f desiredVelocity = target.getPosition() - m_pos;
	desiredVelocity = Vector3f::Normalize(desiredVelocity);
	desiredVelocity *= m_maximumSpeed;

	Vector3f steering = desiredVelocity - m_velocity;
	steering = Vector3f::Clamp(steering, -m_maximumSpeed, m_maximumSpeed);

	m_pos -= steering * dt;
}

void Enemy::Fire(const Camera& target, const Terrain& terrain, const float dt) {
	// Check if the drone has just been fired
	if (m_droneStatus && !m_droneSelfDestruct){
		// Set the drone's position equal to the big sphere enemy position
		m_dronePos = m_pos;

		// Store the old player's position and set the drone active to true for rendering purposes
		m_oldPlayerPos = target.getPosition();
		m_droneActive = true;

		// Set drone's status to false so that the player position is only retrieved once
		m_droneStatus = false;
	}

	// Travel to the old player position 
	Vector3f desiredVelocity = m_oldPlayerPos - m_dronePos;
	desiredVelocity = Vector3f::Normalize(desiredVelocity);
	desiredVelocity *= m_maximumDroneSpeed;

	Vector3f steering = desiredVelocity - m_velocity;
	steering = Vector3f::Clamp(steering, -m_maximumDroneSpeed, m_maximumDroneSpeed);

	m_dronePos += steering * dt;

	// Check if the drone reached the old player's position
	float dist = CalcDistanceNoHeight(m_dronePos, m_oldPlayerPos);

	if (dist <= 3){
		// Drone self-destruct 
		m_droneSelfDestruct = true;

		// Recycle the drone for future use
		m_fire = false;
		m_droneStatus = true;
		m_dronePos = m_pos;
		m_dronePos[1] = -999.0f;
	}
}


void Enemy::Respawn() {

	if (m_canRespawn) {

		m_pos = Vector3f(0.0f, 0.0f, 0.0f);
		m_dronePos = m_pos;
		m_respawnTimer += 1.0f * m_deltaTime;

		if (m_respawnTimer >= 15.0f) {
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
			m_pos = Vector3f(Utils::GetInstance().RandomNumBetweenTwo(50.0f, 520.0f), 0.0f, Utils::GetInstance().RandomNumBetweenTwo(0.0f, 650.0f));
		}
	}
}