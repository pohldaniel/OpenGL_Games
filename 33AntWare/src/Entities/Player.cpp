#include <iostream>
#include <engine/input/Mouse.h>
#include <engine/input/Keyboard.h>

#include "Player.h"
#include "Application.h"
#include "HUD.h"
#include "Light.h"

Player::Player(Camera& camera, AssimpModel* model, const Vector2f& mapMinLimit, const Vector2f& mapMaxLimit) : Entity(model),
	camera(camera),
	mapMinLimit(mapMinLimit),
	mapMaxLimit(mapMaxLimit) {

	childrenEular.set(0.0f, 0.0f, 0.0f);
	childrenTranslation.set(0.0f, 0.0f, 0.0f);
	eularAngles.set(0.0f, 0.0f, 0.0f);
	m_isSubroot = true;

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
			Vector3f lightPos = dynamic_cast<Light*>((*it).get())->getWorldPosition();
			std::cout << "Light Pos: " << lightPos[0] << "  " << lightPos[1] << "  " << lightPos[2] << std::endl;
		}

		if (m_mouseDown && reloadTimer.getElapsedTimeSec() > 1.5f && shootTimer.getElapsedTimeSec() > 0.2f) {

			if (inHandAmmo > 0) {
				dispatchBullet();
				inHandAmmo--;
				shootTimer.reset();
				HUD.setInHandAmmo(inHandAmmo);
				HUD.setTotalAmmo(totalAmmo);
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
		//footstepsSound.setPitch(Keyboard::isKeyPressed(Keyboard::LShift) ? 1.5f : 1.0f);
		//if (footstepsSound.getStatus() != sf::Sound::Playing)
			//footstepsSound.play();
	}else {
		//footstepsSound.pause();
	}

	if (m_rigidbody.velocity.lengthSq() > 0)
		Vector3f::Normalize(m_rigidbody.velocity);

	m_rigidbody.velocity *= keyboard.keyDown(Keyboard::KEY_LSHIFT) ? runningSpeed : speed;


	eularAngles[0] -= mouse.yDelta() * mouseSenstivity;
	eularAngles[1] -= mouse.xDelta() * mouseSenstivity;
	//camera.lookAt({ m_position[0], m_position[1], m_position[2] }, -eularAngles[0], -eularAngles[1], 180.0f);
	//camera.moveRelative(Vector3f(0.0f, 0.5f, 0.0f));
	//camera.lookAt({ m_position[0], m_position[1], m_position[2] }, { m_position[0], m_position[1] + 0.5f, m_position[2] }, -eularAngles[0], -eularAngles[1], 180.0f);

	camera.setTarget({ m_position[0], m_position[1] + 0.5f, m_position[2] });

	float dx = mouse.xDelta();
	float dy = mouse.yDelta();

	if (dx != 0.0f || dy != 0.0f) {
		camera.rotate(dx, dy, Vector3f(m_position[0], m_position[1] + 0.5f, m_position[2]));
	}
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
		killSound();
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
	dynamic_cast<Entity*>(m_children.front().get())->getModel()->getMesh()->setTextureIndex(7);
	bullets.push_back(Bullet({ 0.0f, 0.0f, -1.0f }));
	bullets.back().setOrientation(m_orientation);
	bullets.back().setPosition(m_position);
	bullets.back().translateRelative({ 0.249067f, 0.47149f, -1.25759f });
	isRecoiling = true;
	recoilTime = 0.0f;	
}

void Player::reload() {
	isReloading = true;
	reloadTime = 0.0f;
	if (totalAmmo > 0)
		//reloadSound.play();

		if (totalAmmo > maxAmmo) {
			inHandAmmo = maxAmmo;
			totalAmmo -= maxAmmo;
		}else if (totalAmmo > 0) {
			inHandAmmo = totalAmmo;
			totalAmmo = 0;
		}

	//HUD.setInHandAmmo(inHandAmmo);
	//HUD.setTotalAmmo(totalAmmo);
}

bool Player::damage(float amount) {
	//hurtSound.play();
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
			dynamic_cast<Entity*>(m_children.front().get())->getModel()->getMesh()->setTextureIndex(6);
		}
	}

	for (auto const& child : m_children) {
		child->setOrientation(Vector3f(childrenEular[0], childrenEular[1], childrenEular[2]));
		child->setPosition({ childrenTranslation[0], childrenTranslation[1], childrenTranslation[2] });
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
		child->setOrientation(Vector3f(childrenEular[0], childrenEular[1],childrenEular[2]));
		child->setPosition({ childrenTranslation[0], childrenTranslation[1], childrenTranslation[2] });
	}
}

bool Player::isDead() {

	return hp <= 0.0f;
}

void Player::killSound() {
	//footstepsSound.resetBuffer();
	//reloadSound.resetBuffer();
	//gunShotSound.resetBuffer();
}

void Player::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_mouseDown = true;
	}
}

std::vector<Bullet>& Player::getBullets() {
	return bullets;
}