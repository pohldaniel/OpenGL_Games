#include "Player.h"
//#include "Renderer.h"
#include "Utils.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include <iostream>
#include "Application.h"

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

void Player::Init() {

	m_assaultRifle = new Weapon();
	m_assaultRifle->Init("res/Models3D/Sci-fi_AssaultRifle/AssaultRifle.dae", "res/Shaders/SingleModelLoader.vs", "res/Shaders/SingleModelLoader.fs");
	m_assaultRifle->Configure(35, 0.12f, 1.0f, 35);

	m_sniperRifle = new Weapon();
	m_sniperRifle->Init("res/Models3D/Sci-fi_SniperRifle/SniperRifle.obj", "res/Shaders/SingleModelLoader.vs", "res/Shaders/SingleModelLoader.fs");
	m_sniperRifle->Configure(7, 1.0f, 1.5f, 100);

	m_currWeapon = m_assaultRifle;
	m_usingAR = true;

	m_spotLight = new SpotLight();
	m_spotLight->Configure(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.027f, 0.0028f, 22.5f, 25.0f);

	m_camera = Camera();
	m_camera.perspective(80.0f, (float)Application::Width / (float)Application::Height, 1.0f, 1000.0f);
	m_camera.lookAt(Vector3f(256.0f, 0.0f, 300.0f), Vector3f(256.0f, 0.0f, 300.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
}

void Player::Update(Terrain& terrain, float dt) {

	if (m_health <= 0) {
		m_dead = true;
		return;
	}

	ProcessInput();

	if (m_sniperAiming && m_sprinting) {
		m_camera.setMovingSpeed(0.2f);
		m_camera.setRotationSpeed(0.05f);
	} else if (m_sniperAiming) {
		m_camera.setMovingSpeed(0.5f);
		m_camera.setRotationSpeed(0.05f);
	} else if (m_sprinting) {
		m_camera.setMovingSpeed(2.0f);
		m_camera.setRotationSpeed(0.1f);
	} else {
		m_camera.setMovingSpeed(1.0f);
		m_camera.setRotationSpeed(0.1f);
	}

	if (m_move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotateSmoothly(dx, dy, 0.0f);
		}

		if (m_move) {
			m_camera.move(directrion * 20.0f * dt);
		}
	}

	// Ensure player stays within terrain boundaries
	if (m_camera.getPositionZ() < 41.0f)
		m_camera.setPositionZ(41.0f);
	if (m_camera.getPositionZ() > 730.0f)
		m_camera.setPositionZ(730.0f);
	if (m_camera.getPositionX() < 41.0f)
		m_camera.setPositionX(41.0f);
	if (m_camera.getPositionX() > 730.0f)
		m_camera.setPositionX(730.0f);

	
	
	// Update current weapon
	m_currWeapon->Update(dt);

	// Check if player is jumping
	if (m_jumping){
		m_upwardSpeed += Physics::GetInstance().GetGravity() * dt;

		m_camera.moveY(m_upwardSpeed * dt);

		if (m_camera.getPositionY() < terrain.GetHeightOfTerrain(m_camera.getPositionX(), m_camera.getPositionZ()) + 10.0f) {
			m_upwardSpeed = 0.0f;
			m_camera.setPositionY(terrain.GetHeightOfTerrain(m_camera.getPositionX(), m_camera.getPositionZ()) + 10.0f);
			m_jumping = false;
		}

	} else{		
		m_camera.setPositionY(terrain.GetHeightOfTerrain(m_camera.getPositionX(), m_camera.getPositionZ()) + 10.0f);
		m_isInAir = false;
	}

	// Check if player is using a flashlight 
	if (m_toggleFlashlight){
		m_spotLight->SetPosition(glm::vec3(m_camera.getPositionX(), m_camera.getPositionY(), m_camera.getPositionZ()));
		m_spotLight->SetDirection(glm::vec3(m_camera.getViewDirectionX(), m_camera.getViewDirectionY(), m_camera.getViewDirectionZ()));
	}
}

void Player::Animate(float dt) {
	// Check if player is swapping weapons
	if (m_swapping) {
		// Check if the current weapon is an assault rifle, if so switch to sniper rifle
		if (!m_usingAR) {
			// Play swap animation
			m_currWeapon->GetAnimComponent().PlaySwapTwoWeapons(m_assaultRifle->GetModel(), m_sniperRifle->GetModel(), m_camera, dt, m_swapped);

			// Swap the weapons
			m_sniperRifle->GetAnimComponent().SetWeaponZOffset(-4.0f);
			m_sniperRifle->GetAnimComponent().SetSprintWeaponZOffset(-2.5f);
			m_currWeapon = m_sniperRifle;
			m_sniperEquipped = true;

			// Check if the weapons swapping animation has finished
			if (m_swapped) {
				// End swapping status
				m_swapping = false;
			}
		} else if (m_usingAR) {
			m_currWeapon->GetAnimComponent().PlaySwapTwoWeapons(m_sniperRifle->GetModel(), m_assaultRifle->GetModel(), m_camera, dt, m_swapped);
			m_assaultRifle->GetAnimComponent().SetWeaponZOffset(-2.5f);
			m_assaultRifle->GetAnimComponent().SetSprintWeaponZOffset(-2.5f);
			m_currWeapon = m_assaultRifle;
			m_sniperEquipped = false;

			if (m_swapped) {
				m_swapping = false;
			}
		}
	}

	// Check if the player is not firing, reloading or swapping weapons
	if (!m_firing && !m_reloading && !m_swapping) {
		// Proceed to playing one of the following animations: Walking - Sprinting - Idle
		if (m_move) {
			m_currWeapon->GetAnimComponent().PlayWalkFPS(m_currWeapon->GetModel(), m_camera, dt);

		} else if (m_sprinting) {
			m_currWeapon->GetAnimComponent().PlaySprintFPS(m_currWeapon->GetModel(), m_camera, dt);

		} else {
			m_currWeapon->GetAnimComponent().PlayIdleFPS(m_currWeapon->GetModel(), m_camera, dt);

		}
	}

	// Check if player is firing
	if (m_firing && !m_reloading && !m_swapping) {
		m_currWeapon->Fire(m_currWeapon->GetModel(), m_camera, dt, m_firing, m_reloading);
	}

	// Check if player is reloading
	if (m_reloading && !m_swapping) {
		m_currWeapon->Reload(m_currWeapon->GetModel(), m_camera, dt, m_reloading);
	}

}

void Player::Switch() {
	m_usingAR = !m_usingAR;
	m_swapping = true;
	m_swapped = false;
}

void Player::Respawn() {
	m_toggleFlashlight = false;
	m_dead = false;
	m_health = 100;
	m_camera.setPosition(256.0f, 0.0f, 300.0f);
	m_assaultRifle->RestartWeapon();
	m_sniperRifle->RestartWeapon();
}

void Player::ProcessInput() {

	Keyboard &keyboard = Keyboard::instance();

	m_firing = false;
	m_sniperAiming = false;
	m_sprinting = false;
	m_move = false;

	directrion.set(0.0f, 0.0f, 0.0f);

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
		m_move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		m_move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f, 0.0f, 0.0f);
		m_move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		m_move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
		m_move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
		m_move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_LALT)) {
		if (!m_isInAir) {
			m_jumping = true;
			m_isInAir = true;
			m_upwardSpeed = m_jumpHeight;
		}
	}

	if (keyboard.keyDown(Keyboard::KEY_LSHIFT)) {
		if (m_move) {
			m_sprinting = true;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_Q)) {
		if (!m_reloading){
			Switch();
		}
	}

	Mouse &mouse = Mouse::instance();
	dx = 0.0f;
	dy = 0.0f;

	if (mouse.buttonDown(Mouse::BUTTON_LEFT)) {
		m_firing = true;
	}

	if (mouse.buttonDown(Mouse::BUTTON_RIGHT)) {
		if (!m_usingAR) {
			// Set sniper aiming to true
			m_sniperAiming = true;
		}
	}

	dx = mouse.xPosRelative();
	dy = mouse.yPosRelative();
}