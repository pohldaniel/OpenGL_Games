#include "Player.h"
//#include "Renderer.h"
#include "Utils.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include <iostream>

Player::Player() :
	m_jumpHeight(50.0f),
	m_upwardSpeed(0.0f),
	m_health(100),
	m_isInAir(false),
	m_swapped(false),
	m_sniperAiming(false),
	m_toggleFlashlight(false),
	m_dead(false)
{}

Player::~Player()
{
	delete m_assaultRifle;
	m_assaultRifle = nullptr;

	delete m_sniperRifle;
	m_sniperRifle = nullptr;

	delete m_spotLight;
	m_spotLight = nullptr;
}

void Player::Init(CameraVo& cam, glm::vec3 initialPosition)
{
	cam.GetCameraPos() = initialPosition;

	m_assaultRifle = new Weapon();
	m_assaultRifle->Init("res/Models3D/Sci-fi_AssaultRifle/AssaultRifle.dae", cam, "res/Shaders/SingleModelLoader.vs", "res/Shaders/SingleModelLoader.fs");
	m_assaultRifle->Configure(35, 0.12f, 1.0f, 35);

	m_sniperRifle = new Weapon();
	m_sniperRifle->Init("res/Models3D/Sci-fi_SniperRifle/SniperRifle.obj", cam, "res/Shaders/SingleModelLoader.vs", "res/Shaders/SingleModelLoader.fs");
	m_sniperRifle->Configure(7, 1.0f, 1.5f, 100);

	m_currWeapon = m_assaultRifle;
	m_usingAR = true;

	m_spotLight = new SpotLight();
	m_spotLight->Configure(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.027f, 0.0028f, 22.5f, 25.0f);
}

void Player::Update(CameraVo& cam, Terrain& terrain, float dt) {
	if (m_health <= 0)
	{
		m_dead = true;
		StopMoving();

		return;
	}

	if (!IsPlayerWalking())
	{
		m_walking = false;
		m_sprinting = false;
	}

	ProcessInput(cam);
#pragma region // CAMERA_MOVEMENT
	if (m_bCamMovements[CAM_FORWARD])
		cam.MoveForward(dt);
	if (m_bCamMovements[CAM_BACKWARD])
		cam.MoveBackward(dt);
	if (m_bCamMovements[CAM_LEFT])
		cam.StrafeLeft(dt);
	if (m_bCamMovements[CAM_RIGHT])
		cam.StrafeRight(dt);
	if (m_bCamMovements[CAM_RISE])
		cam.Rise(dt);
	if (m_bCamMovements[CAM_FALL])
		cam.Fall(dt);

	// Ensure player stays within terrain boundaries
	if (cam.GetCameraPos().z < 41.0f)
		cam.GetCameraPos().z = 41.0f;
	if (cam.GetCameraPos().z > 730.0f)
		cam.GetCameraPos().z = 730.0f;
	if (cam.GetCameraPos().x < 41.0f)
		cam.GetCameraPos().x = 41.0f;
	if (cam.GetCameraPos().x > 730.0f)
		cam.GetCameraPos().x = 730.0f;
#pragma endregion

	// Update current weapon
	m_currWeapon->Update(cam, dt);

	// Check if player is swapping weapons
	if (m_swapping)
	{
		// Check if the current weapon is an assault rifle, if so switch to sniper rifle
		if (!m_usingAR)
		{
			// Play swap animation
			m_currWeapon->GetAnimComponent().PlaySwapTwoWeapons(m_assaultRifle->GetModel(), m_sniperRifle->GetModel(), cam, dt, m_swapped);

			// Swap the weapons
			m_sniperRifle->GetAnimComponent().SetWeaponZOffset(-4.0f);
			m_sniperRifle->GetAnimComponent().SetSprintWeaponZOffset(-2.5f);
			m_currWeapon = m_sniperRifle;
			m_sniperEquipped = true;

			// Check if the weapons swapping animation has finished
			if (m_swapped)
			{
				// End swapping status
				m_swapping = false;
			}
		}
		else if (m_usingAR)
		{
			m_currWeapon->GetAnimComponent().PlaySwapTwoWeapons(m_sniperRifle->GetModel(), m_assaultRifle->GetModel(), cam, dt, m_swapped);
			m_assaultRifle->GetAnimComponent().SetWeaponZOffset(-2.5f);
			m_assaultRifle->GetAnimComponent().SetSprintWeaponZOffset(-2.5f);
			m_currWeapon = m_assaultRifle;
			m_sniperEquipped = false;

			if (m_swapped)
			{
				m_swapping = false;
			}
		}
	}

	// Check if the player is not firing, reloading or swapping weapons
	if (!m_firing && !m_reloading && !m_swapping)
	{
		// Proceed to playing one of the following animations: Walking - Sprinting - Idle
		if (m_walking)
		{
			m_currWeapon->GetAnimComponent().PlayWalkFPS(m_currWeapon->GetModel(), cam, dt);
		}
		else if (m_sprinting)
		{
			m_currWeapon->GetAnimComponent().PlaySprintFPS(m_currWeapon->GetModel(), cam, dt);
		}
		else
		{
			m_currWeapon->GetAnimComponent().PlayIdleFPS(m_currWeapon->GetModel(), cam, dt);
		}
	}

	// Check if player is firing
	if (m_firing && !m_reloading && !m_swapping)
	{
		m_currWeapon->Fire(m_currWeapon->GetModel(), cam, dt, m_firing, m_reloading);
	}

	// Check if player is reloading
	if (m_reloading && !m_swapping)
	{
		m_currWeapon->Reload(m_currWeapon->GetModel(), cam, dt, m_reloading);
	}

	

	// Check if player is jumping
	if (m_jumping){
		m_upwardSpeed += Physics::GetInstance().GetGravity() * dt;
		cam.GetCameraPos().y += m_upwardSpeed * dt;

		if (cam.GetCameraPos().y < terrain.GetHeightOfTerrain(cam.GetCameraPos().x, cam.GetCameraPos().z) + 10.0f){
			m_upwardSpeed = 0.0f;
			cam.GetCameraPos().y = terrain.GetHeightOfTerrain(cam.GetCameraPos().x, cam.GetCameraPos().z) + 10.0f;
			m_jumping = false;
		}

	} else{
		cam.GetCameraPos().y = terrain.GetHeightOfTerrain(cam.GetCameraPos().x, cam.GetCameraPos().z) + 10.0f;
		m_isInAir = false;
	}

	// Check if player is using a flashlight 
	if (m_toggleFlashlight)
	{
		m_spotLight->SetPosition(glm::vec3(cam.GetCameraPos().x, cam.GetCameraPos().y, cam.GetCameraPos().z));
		m_spotLight->SetDirection(glm::vec3(cam.GetCameraForward().x, cam.GetCameraForward().y, cam.GetCameraForward().z));
	}
}

// Function that checks if the camera (FPS character) is moving 
bool Player::IsPlayerWalking()
{
	return m_bCamMovements[CAM_FORWARD] || m_bCamMovements[CAM_BACKWARD] || m_bCamMovements[CAM_LEFT] || m_bCamMovements[CAM_RIGHT];
}

void Player::Switch()
{
	if (m_usingAR)
	{
		m_usingAR = false;
	}
	else
	{
		m_usingAR = true;
	}

	m_swapping = true;
	m_swapped = false;
}

void Player::Respawn(CameraVo& cam)
{
	m_toggleFlashlight = false;
	m_dead = false;
	m_health = 100;
	cam.GetCameraPos() = glm::vec3(256.0f, 0.0f, 300.0f);
	m_assaultRifle->RestartWeapon();
	m_sniperRifle->RestartWeapon();
}

void Player::StopMoving()
{
	m_bCamMovements[CAM_FORWARD] = false;
	m_bCamMovements[CAM_BACKWARD] = false;
	m_bCamMovements[CAM_LEFT] = false;
	m_bCamMovements[CAM_RIGHT] = false;
}

void Player::ProcessInput(CameraVo& cam) {

	m_bCamMovements[CAM_FORWARD] = false;
	m_bCamMovements[CAM_BACKWARD] = false;
	m_bCamMovements[CAM_LEFT] = false;
	m_bCamMovements[CAM_RIGHT] = false;

	m_firing = false;
	m_sniperAiming = false;

	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		m_bCamMovements[CAM_FORWARD] = true;
		m_walking = true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		m_bCamMovements[CAM_BACKWARD] = true;
		m_walking = true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		m_bCamMovements[CAM_LEFT] = true;
		m_walking = true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		m_bCamMovements[CAM_RIGHT] = true;
		m_walking = true;
	}

	if (keyboard.keyDown(Keyboard::KEY_LALT)) {
		if (!m_isInAir) {
			m_jumping = true;
			m_isInAir = true;
			m_upwardSpeed = m_jumpHeight;
		}
	}

	if (keyboard.keyDown(Keyboard::KEY_LSHIFT)) {
		if (IsPlayerWalking()) {
			cam.SetCameraSpeed(30.0f);
			m_sprinting = true;
		}

		m_walking = false;
	}

	if (keyboard.keyPressed(Keyboard::KEY_Q)) {
		if (!m_reloading){
			Switch();
		}
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::BUTTON_LEFT)) {
		m_firing = true;
	}

	if (mouse.buttonDown(Mouse::BUTTON_RIGHT)) {
		if (!m_usingAR) {
			// Set sniper aiming to true
			m_sniperAiming = true;
		}
	}
}