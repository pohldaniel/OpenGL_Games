#include <iostream>
#include <engine/input/Mouse.h>
#include <engine/input/Keyboard.h>

#include "Player.h"
#include "Application.h"
#include "HUD.h"
#include "Light.h"
#include "Globals.h"

Player::Player(Camera& camera, const MeshSequence& meshSequence, int meshIndex, const Vector2f& mapMinLimit, const Vector2f& mapMaxLimit) : Entity(meshSequence, meshIndex),
	camera(camera),
	mapMinLimit(mapMinLimit),
	mapMaxLimit(mapMaxLimit) {

	childrenEular.set(0.0f, 0.0f, 0.0f);
	childrenTranslation.set(0.0f, 0.0f, 0.0f);
	eularAngles.set(0.0f, 0.0f, 0.0f);
	setOrientation(eularAngles);
	camera.lookAt({ m_position[0], m_position[1], m_position[2] }, -eularAngles[0], -eularAngles[1], 180.0f);

	setIsStatic(false);
	setOrigin(0.0f, 0.0f, 0.0f);
	m_rigidbody.lockLinear(AXIS::y);
	m_rigidbody.lockAngular(AXIS::z);
	
	EventDispatcher::AddMouseListener(this);
}

Player::~Player() {
	EventDispatcher::RemoveMouseListener(this);
}

void Player::update(const float dt) {
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

	m_rigidbody.velocity = { 0.0f, 0.0f, 0.0f };
	m_rigidbody.angularVelocity = { 0.0f, 0.0f, 0.0f };

	if (!hasFallen) {

		if (keyboard.keyDown(Keyboard::KEY_W)) {
			m_rigidbody.velocity[2] -= 1;
		}

		if (keyboard.keyDown(Keyboard::KEY_S)) {
			m_rigidbody.velocity[2] += 1;
		}

		if (keyboard.keyDown(Keyboard::KEY_D)) {
			m_rigidbody.velocity[0] += 1;
		}

		if (keyboard.keyDown(Keyboard::KEY_A)) {
			m_rigidbody.velocity[0] -= 1;
		}

		if (keyboard.keyPressed(Keyboard::KEY_R) && reloadTimer.getElapsedTimeSec() > 1.5f) {
			if (totalAmmo > 0 && inHandAmmo < maxAmmo) {
				reloadTimer.reset();
				reload();
			}
		}

		if (keyboard.keyPressed(Keyboard::KEY_F)) {
			std::list<std::unique_ptr<BaseNode>>::iterator it = m_children.begin();
			std::advance(it, 4);
			dynamic_cast<Light*>((*it).get())->toggle();			
		}

		if (m_mouseDown && reloadTimer.getElapsedTimeSec() > 1.5f && shootTimer.getElapsedTimeSec() > 0.2f) {

			if (inHandAmmo > 0) {
				dispatchBullet();
				inHandAmmo--;
				shootTimer.reset();
				HUD::Get().setInHandAmmo(inHandAmmo);
				HUD::Get().setTotalAmmo(totalAmmo);
			}else {

				if (totalAmmo > 0 && reloadTimer.getElapsedTimeSec() > 1.5f) {
					reloadTimer.reset();
					reload();
				}
			}

			m_mouseDown = false;
		}
	}

	if (m_rigidbody.velocity != Vector3f(0.0f, 0.0f, 0.0f)) {
		Globals::soundManager.get("player").setPitchChannel(3u, keyboard.keyDown(Keyboard::KEY_LSHIFT) ? 1.5f : 1.0f);
		if (!Globals::soundManager.get("player").isPlaying(3u)) {
			Globals::soundManager.get("player").playChannel(3u);
		}
	}else {
		Globals::soundManager.get("player").stopChannel(3u);
	}

	if (m_rigidbody.velocity.lengthSq() > 0)
		Vector3f::Normalize(m_rigidbody.velocity);

	m_rigidbody.velocity *= keyboard.keyDown(Keyboard::KEY_LSHIFT) ? runningSpeed : speed;


	eularAngles[0] -= mouse.yDelta() * mouseSenstivity;
	eularAngles[1] -= mouse.xDelta() * mouseSenstivity;

	camera.setTarget({ m_position[0], m_position[1], m_position[2] });

	float dx = mouse.xDelta();
	float dy = mouse.yDelta();

	if (dx != 0.0f || dy != 0.0f) {
		camera.rotate(dx, dy, Vector3f(m_position[0], m_position[1], m_position[2]));
	}

	camera.moveRelative(Vector3f(0.0f, 0.5f, 0.0f));

	setOrientation(eularAngles);
	
	Vector2f positionOnY = { m_position[0], m_position[2] };
	if ((positionOnY[0] < mapMinLimit[0] || positionOnY[1] < mapMinLimit[1] ||
		positionOnY[0] > mapMaxLimit[0] || positionOnY[1] > mapMaxLimit[1]) && !hasFallen) {
		hasFallen = true;
		m_rigidbody.lockLinear(AXIS::x);
		m_rigidbody.lockLinear(AXIS::z);
		m_rigidbody.unlockLinear(AXIS::y);
		m_rigidbody.acceleration = { 0, -fallingAcceleration, 0 };
	}

	if (isRecoiling) {
		recoilAnim(dt);
	}

	if (isReloading) {
		reloadAnim(dt);
	}

	if (hasFallen) {
		fallingTime += dt;
	}

	if (fallingTime >= dieAfter) {
		damage(hp);
	}

	std::vector<Bullet>::iterator it = bullets.begin();
	while (it != bullets.end()) {

		if ((*it).timeOut()) {
			it = bullets.erase(it);
			damage(1);
		}
		else ++it;
	}
}

void Player::fixedUpdate(float fdt) {

	
	Entity::fixedUpdate(fdt);

	for (unsigned i = 0; i < bullets.size(); ++i) {
		bullets[i].fixedUpdate(fdt);
	}
}

void Player::dispatchBullet() {
	Globals::soundManager.get("player").replayChannel(0u);

	dynamic_cast<Entity*>(m_children.front().get())->setTextureIndex(6);
	bullets.push_back(Bullet(meshSequence,{ 0.0f, 0.0f, -1.0f }));
	bullets.back().setOrientation(m_orientation);
	bullets.back().setPosition(m_position);
	bullets.back().translateRelative({ 0.249067f, 0.47149f, -1.25759f });
	bullets.back().start();
	isRecoiling = true;
	recoilTime = 0.0f;	
}

void Player::reload() {
	isReloading = true;
	reloadTime = 0.0f;
	if (totalAmmo > 0)
		Globals::soundManager.get("player").playChannel(1u);

	if (totalAmmo > maxAmmo) {
		inHandAmmo = maxAmmo;
		totalAmmo -= maxAmmo;
	}else if (totalAmmo > 0) {
			inHandAmmo = totalAmmo;
			totalAmmo = 0;
	}

	HUD::Get().setInHandAmmo(inHandAmmo);
	HUD::Get().setTotalAmmo(totalAmmo);
}

bool Player::damage(float amount) {
	Globals::soundManager.get("player").replayChannel(2u);
	hp -= amount;
	return hp <= 0.0f;
}

void Player::recoilAnim(float deltaTime) {

	if (recoilTimeOut <= recoilTime) {
		isRecoiling = false;
		childrenEular = { 0, 0, 0 };
		childrenTranslation = { 0, 0, 0 };

	}else {
		recoilTime += deltaTime;
		if (recoilTime < (recoilTimeOut / 2.0f)) {
			childrenEular[0] += recoilImpact * deltaTime;
			childrenTranslation[2] += recoilImpact * deltaTime * 0.05f;

		}else {
			childrenEular[0] -= recoilImpact * deltaTime;
			childrenTranslation[2] -= recoilImpact * deltaTime * 0.05f;
			dynamic_cast<Entity*>(m_children.front().get())->setTextureIndex(5);
		}
	}

	for (auto const& child : m_children) {
		if (child->isFixed())
			continue;
		child->setOrientation(Vector3f(childrenEular[0], childrenEular[1], childrenEular[2]));
		child->setPosition({ childrenTranslation[0], childrenTranslation[1], childrenTranslation[2] });
		child->setOrigin({ childrenTranslation[0], childrenTranslation[1], childrenTranslation[2] });
	}
}

void Player::reloadAnim(float deltaTime) {

	if (reloadTimeOut <= reloadTime) {
		isReloading = false;
		childrenEular = { 0, 0, 0 };
		childrenTranslation = { 0, 0, 0 };

	}else {
		reloadTime += deltaTime;
		if (reloadTime < (reloadTimeOut / 2.0f)) {
			childrenEular[0] -= reloadPlaybackSpeed * deltaTime;
			childrenTranslation[2] += reloadPlaybackSpeed * deltaTime * 0.005f;
		}
		else {
			childrenEular[0] += reloadPlaybackSpeed * deltaTime;
			childrenTranslation[2] -= reloadPlaybackSpeed * deltaTime * 0.005f;
		}
	}

	for (auto const& child : m_children) {
		if (child->isFixed())
			continue;
		child->setOrientation(Vector3f(childrenEular[0], childrenEular[1],childrenEular[2]));
		child->setPosition({ childrenTranslation[0], childrenTranslation[1], childrenTranslation[2] });
		child->setOrigin({ childrenTranslation[0], childrenTranslation[1], childrenTranslation[2] });
	}
}

bool Player::isDead() {
	return hp <= 0.0f;
}

void Player::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_mouseDown = true;
	}
}

std::vector<Bullet>& Player::getBullets() {
	return bullets;
}

void Player::setTotalAmmo(unsigned int ammo) {
	totalAmmo = ammo;
}

void Player::stopFootstepsSound() {
	Globals::soundManager.get("player").stopChannel(3u);
}