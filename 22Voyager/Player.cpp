#include "Player.h"
//#include "Renderer.h"
#include "Utils.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include <iostream>
#include "Application.h"
#include "Constants.h"

Player::Player() :
	m_jumpHeight(50.0f),
	m_upwardSpeed(0.0f),
	m_health(100),
	m_isInAir(false),
	m_swapped(false),
	m_sniperAiming(false),
	m_toggleFlashlight(false),
	m_dead(false){
}

Player::~Player() {
	delete m_assaultRifle;
	m_assaultRifle = nullptr;

	delete m_sniperRifle;
	m_sniperRifle = nullptr;

	delete m_spotLight;
	m_spotLight = nullptr;
}

void Player::Init() {

	m_assaultRifle = new Weapon();
	m_assaultRifle->init("res/Models3D/Sci-fi_AssaultRifle/AssaultRifle.dae", Globals::shaderManager.getAssetPointer("weapon"));
	m_assaultRifle->configure(35, 0.12f, 1.0f, 35);
	m_assaultRifle->m_materialIndex = 0;

	m_sniperRifle = new Weapon();
	m_sniperRifle->init("res/Models3D/Sci-fi_SniperRifle/SniperRifle.obj", Globals::shaderManager.getAssetPointer("weapon"));
	m_sniperRifle->configure(7, 1.0f, 1.5f, 100);
	m_sniperRifle->m_materialIndex = 1;

	m_currWeapon = m_assaultRifle;
	m_usingAR = true;

	m_spotLight = new SpotLight();
	m_spotLight->configure(Vector3f(5.0f, 5.0f, 5.0f), Vector3f(1.0f, 1.0f, 1.0f), 1.0f, 0.027f, 0.0028f, 22.5f, 25.0f);

	m_camera = Camera();
	m_camera.perspective(80.0f, (float)Application::Width / (float)Application::Height, 1.0f, 1500.0f);
	m_camera.lookAt(Vector3f(256.0f, 0.0f, 300.0f), Vector3f(256.0f, 0.0f, 300.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.orthographic(-static_cast<float>(Application::Width / 2), static_cast<float>(Application::Width / 2), -static_cast<float>(Application::Height / 2), static_cast<float>(Application::Height / 2), -1.0f, 1.0f);
	//m_camera.orthographic(0.0f, static_cast<float>(Application::Width ), 0.0f, static_cast<float>(Application::Height ), -1.0f, 1.0f);
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
		m_camera.perspective(30.0f, (float)Application::Width / (float)Application::Height, 1.0f, 1500.0f);
	} else if (m_sniperAiming) {
		m_camera.setMovingSpeed(0.5f);
		m_camera.setRotationSpeed(0.05f);
		m_camera.perspective(30.0f, (float)Application::Width / (float)Application::Height, 1.0f, 1500.0f);
	} else if (m_sprinting) {
		m_camera.setMovingSpeed(2.0f);
		m_camera.setRotationSpeed(0.1f);
		m_camera.perspective(80.0f, (float)Application::Width / (float)Application::Height, 1.0f, 1500.0f);
	} else {
		m_camera.setMovingSpeed(1.0f);
		m_camera.setRotationSpeed(0.1f);
		m_camera.perspective(80.0f, (float)Application::Width / (float)Application::Height, 1.0f, 1500.0f);
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
	m_currWeapon->update(dt);

	// Check if player is jumping
	if (m_jumping){
		m_upwardSpeed += Physics::GetInstance().GetGravity() * dt;

		m_camera.moveY(m_upwardSpeed * dt);

		if (m_camera.getPositionY() < terrain.getHeightOfTerrain(m_camera.getPositionX(), m_camera.getPositionZ()) + 10.0f) {
			m_upwardSpeed = 0.0f;
			m_camera.setPositionY(terrain.getHeightOfTerrain(m_camera.getPositionX(), m_camera.getPositionZ()) + 10.0f);
			m_jumping = false;
		}

	} else{		
		m_camera.setPositionY(terrain.getHeightOfTerrain(m_camera.getPositionX(), m_camera.getPositionZ()) + 10.0f);
		m_isInAir = false;
	}

	// Check if player is using a flashlight 
	if (m_toggleFlashlight){
		m_spotLight->setPosition(m_camera.getPosition());
		m_spotLight->setDirection(m_camera.getViewDirection());
	}
}

void Player::Animate(float dt) {
	// Check if player is swapping weapons
	if (m_swapping) {
		// Check if the current weapon is an assault rifle, if so switch to sniper rifle
		if (!m_usingAR) {
			// Play swap animation
			m_currWeapon->getAnimComponent().PlaySwapTwoWeapons(m_assaultRifle, m_sniperRifle, m_camera, dt, m_swapped);

			// Swap the weapons
			m_sniperRifle->getAnimComponent().SetWeaponZOffset(-4.0f);
			m_sniperRifle->getAnimComponent().SetSprintWeaponZOffset(-2.5f);
			m_currWeapon = m_sniperRifle;
			m_sniperEquipped = true;

			// Check if the weapons swapping animation has finished
			if (m_swapped) {
				// End swapping status
				m_swapping = false;
			}
		} else if (m_usingAR) {
			m_currWeapon->getAnimComponent().PlaySwapTwoWeapons(m_sniperRifle, m_assaultRifle, m_camera, dt, m_swapped);
			m_assaultRifle->getAnimComponent().SetWeaponZOffset(-2.5f);
			m_assaultRifle->getAnimComponent().SetSprintWeaponZOffset(-2.5f);
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
		if (m_sprinting) {
			m_currWeapon->getAnimComponent().PlaySprintFPS(m_currWeapon, m_camera, dt);

		}else if (m_move) {
			m_currWeapon->getAnimComponent().PlayWalkFPS(m_currWeapon, m_camera, dt);

		} else {
			m_currWeapon->getAnimComponent().PlayIdleFPS(m_currWeapon, m_camera, dt);

		}
	}

	// Check if player is firing
	if (m_firing && !m_reloading && !m_swapping) {
		m_currWeapon->fire(m_currWeapon, m_camera, dt, m_firing, m_reloading);
	}

	// Check if player is reloading
	if (m_reloading && !m_swapping) {
		m_currWeapon->reload(m_currWeapon, m_camera, dt, m_reloading);
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
	m_assaultRifle->restartWeapon();
	m_sniperRifle->restartWeapon();
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