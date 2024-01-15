#include <engine/input/Keyboard.h>
#include <engine/input/Mouse.h>

#include "PlayerNew.h"
#include "HUD.h"

PlayerNew::PlayerNew(Camera& camera, std::shared_ptr<aw::Mesh> mesh, aw::Material material, const Vector2f& mapMinLimit, const Vector2f& mapMaxLimit) : 
	Entity(mesh, material), 
	camera(camera),
	mapMinLimit(mapMinLimit),
	mapMaxLimit(mapMaxLimit) {
	childrenEular.set(0.0f, 0.0f, 0.0f);
	childrenTranslation.set(0.0f, 0.0f, 0.0f);
	eularAngles.set(0.0f, 0.0f, 0.0f);

	EventDispatcher::AddMouseListener(this);
}

void PlayerNew::draw(const Camera& camera) {

	Entity::draw(camera);
	for (unsigned i = 0; i < bullets.size(); ++i) {
		bullets[i].draw();
	}
}

void PlayerNew::update(const float dt) {
	Keyboard &keyboard = Keyboard::instance();
	Mouse &mouse = Mouse::instance();

	rigidbody.velocity = { 0, 0, 0 };
	rigidbody.angularVelocity = { 0, 0, 0 };

	if (!hasFallen) {

		if (keyboard.keyDown(Keyboard::KEY_W)) {
			rigidbody.velocity.z -= 1;
		}

		if (keyboard.keyDown(Keyboard::KEY_S)) {
			rigidbody.velocity.z += 1;
		}

		if (keyboard.keyDown(Keyboard::KEY_D)) {
			rigidbody.velocity.x += 1;
		}

		if (keyboard.keyDown(Keyboard::KEY_A)) {
			rigidbody.velocity.x -= 1;
		}

		//if (keyboard.keyPressed(Keyboard::KEY_R)) {
		//	reload();
		//}

		if (keyboard.keyPressed(Keyboard::KEY_R) && reloadTimer.getElapsedTimeSec() > 1.5f) {
			if (totalAmmo > 0 && inHandAmmo < maxAmmo) {
				reloadTimer.reset();
				reload();
			}
		}

		//if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		//	dispatchBullet();
		//}

		if (m_mouseDown && reloadTimer.getElapsedTimeSec() > 1.5f && shootTimer.getElapsedTimeSec() > 0.2f) {
			dispatchBullet();

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

	if (rigidbody.velocity != glm::vec3(0, 0, 0)) {
		//footstepsSound.setPitch(Keyboard::isKeyPressed(Keyboard::LShift) ? 1.5f : 1.0f);
		//if (footstepsSound.getStatus() != sf::Sound::Playing)
			//footstepsSound.play();
	}else {
		//footstepsSound.pause();
	}

	if (length(rigidbody.velocity) > 0)
		rigidbody.velocity = normalize(rigidbody.velocity);

	rigidbody.velocity *= keyboard.keyDown(Keyboard::KEY_LSHIFT) ? runningSpeed : speed;


	eularAngles[0] -= mouse.yDelta() * mouseSenstivity;
	eularAngles[1] -= mouse.xDelta() * mouseSenstivity;
	camera.lookAt({ m_position[0], m_position[1], m_position[2] }, -eularAngles[0], -eularAngles[1], 180.0f);
	camera.moveRelative(Vector3f(0.0f, 0.5f, 0.0f));

	setOrientation(Vector3f(eularAngles[0], eularAngles[1], eularAngles[2]));

	auto bulletsSize = bullets.size();
	for (unsigned i = 0; i < bulletsSize; ++i) {
		if (bullets[i].timeOut()) {
			destroyBullet(i);
			--i;
			--bulletsSize;
		}
	}

	Vector2f positionOnY = { m_position[0], m_position[2] };
	if ((positionOnY[0] < mapMinLimit[0] || positionOnY[1] < mapMinLimit[1] ||
		positionOnY[0] > mapMaxLimit[0] || positionOnY[1] > mapMaxLimit[1]) && !hasFallen) {
		hasFallen = true;
		rigidbody.lockLinear(aw::AXIS::x);
		rigidbody.lockLinear(aw::AXIS::z);
		rigidbody.unlockLinear(aw::AXIS::y);
		rigidbody.acceleration = { 0, -fallingAcceleration, 0 };
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

	updateSelfAndChild();
}

void PlayerNew::fixedUpdate(float fdt) {
	Entity::fixedUpdate(fdt);
	for (unsigned i = 0; i < bullets.size(); ++i) {
		bullets[i].fixedUpdate(fdt);
	}
}

void PlayerNew::start() {
	transparentTexture = aw::Mesh::createTexture("res/textures/transparent.png");
	flashTexture = aw::Mesh::createTexture("res/textures/flash.png");
	//m_children.front()->getMesh()->setTexture(transparentTexture);
	m_isStatic = false;
	rigidbody.lockLinear(aw::AXIS::y);
	rigidbody.lockAngular(aw::AXIS::z);
}

void PlayerNew::dispatchBullet() {
	//gunShotSound.play();
	/*bullets.push_back(aw::Bullet(bulletMesh, Material(), nullptr, vec3(0, 0, -1)));
	bullets[bullets.size() - 1].transform = transform;
	bullets[bullets.size() - 1].transform.translate({ 0.249067f, 0.47149f, -1.25759f });

	m_children.front()->getMesh()->setTexture(flashTexture);*/

	isRecoiling = true;
	recoilTime = 0.0f;
}

void PlayerNew::reload() {
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

	HUD.setInHandAmmo(inHandAmmo);
	HUD.setTotalAmmo(totalAmmo);
}

void PlayerNew::destroyBullet(int index) {
	bullets.erase(bullets.begin() + index);
}

bool PlayerNew::damage(float amount) {
	//hurtSound.play();
	hp -= amount;
	return hp <= 0.0f;
}

void PlayerNew::recoilAnim(float deltaTime) {

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
			//m_children.front()->getMesh()->setTexture(transparentTexture);
		}
	}

	for (auto const& child : m_children) {
		child->setOrientation(childrenEular);
		child->setPosition(childrenTranslation);
	}
}

void PlayerNew::reloadAnim(float deltaTime) {
	
	if (reloadTimeOut <= reloadTime) {
		isReloading = false;
		childrenEular = { 0, 0, 0 };
		childrenTranslation = { 0, 0, 0 };

	}else {
		reloadTime += deltaTime;
		if (reloadTime < (reloadTimeOut / 2.0f)) {
			childrenEular[0] -= reloadPlaybackSpeed * deltaTime;
			childrenTranslation[2] += reloadPlaybackSpeed * deltaTime * 0.005f;
		}else {
			childrenEular[0] += reloadPlaybackSpeed * deltaTime;
			childrenTranslation[2] -= reloadPlaybackSpeed * deltaTime * 0.005f;
		}
	}

	for (auto const& child : m_children) {
		child->setOrientation(childrenEular);
		child->setPosition(childrenTranslation);
	}
}

bool PlayerNew::isDead() {

	return hp <= 0.0f;
}

void PlayerNew::killSound() {
	//footstepsSound.resetBuffer();
	//reloadSound.resetBuffer();
	//gunShotSound.resetBuffer();
}

void PlayerNew::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_mouseDown = true;
	}
}