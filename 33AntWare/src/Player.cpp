#include <iostream>
#include <engine/input/Mouse.h>
#include <engine/input/Keyboard.h>
#include <Player.h>
#include "Application.h"

using namespace aw;
using namespace std;
using namespace glm;

Player::Player(Camera& camera, shared_ptr<Mesh> mesh, Material material, shared_ptr<Mesh> bulletMesh, vec2 mapMinLimit, vec2 mapMaxLimit,
	GameObject *parent) : GameObject(mesh, material, parent, false, 2),
	bulletMesh(bulletMesh),
	mapMinLimit(mapMinLimit),
	mapMaxLimit(mapMaxLimit),
	camera(camera)
{
	//gunShotSoundBuffer.loadFromFile("res/audio/gunshot.wav");
	//reloadSoundBuffer.loadFromFile("res/audio/reload.wav");
	//hurtSoundBuffer.loadFromFile("res/audio/playerHurt.wav");
	//footstepsSoundBuffer.loadFromFile("res/audio/playerFootsteps.ogg");

	//gunShotSound.setBuffer(gunShotSoundBuffer);
	//reloadSound.setBuffer(reloadSoundBuffer);
	//hurtSound.setBuffer(hurtSoundBuffer);
	//footstepsSound.setBuffer(footstepsSoundBuffer);
	//footstepsSound.setLoop(true);
	EventDispatcher::AddMouseListener(this);
	aabb.maximize(0.2f);
}

Player::~Player() {
	EventDispatcher::RemoveMouseListener(this);
}

void Player::start()
{
	transparentTexture = Mesh::createTexture("res/textures/transparent.png");
	flashTexture = Mesh::createTexture("res/textures/flash.png");
	children[0]->getMesh()->setTexture(transparentTexture);
	isStatic = false;
	rigidbody.lockLinear(AXIS::y);
	rigidbody.lockAngular(AXIS::z);

	// Reset mouse's position on player (and therefore scene) start
	Mouse::instance().setPosition(Application::Width / 2, Application::Height / 2);

}

void Player::update(const float dt) {
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

		if (keyboard.keyPressed(Keyboard::KEY_R)) {
			reload();
		}

		//if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		//	dispatchBullet();
		//}

		if (m_mouseDown) {
			dispatchBullet();
			m_mouseDown = false;
		}
	}

	if (rigidbody.velocity != glm::vec3(0, 0, 0)) {
		//footstepsSound.setPitch(Keyboard::isKeyPressed(Keyboard::LShift) ? 1.5f : 1.0f);
		//if (footstepsSound.getStatus() != sf::Sound::Playing)
			//footstepsSound.play();
	}
	else {
		//footstepsSound.pause();
	}

	if (length(rigidbody.velocity) > 0)
		rigidbody.velocity = normalize(rigidbody.velocity);

	rigidbody.velocity *= keyboard.keyDown(Keyboard::KEY_LSHIFT) ? runningSpeed : speed;

	
	eularAngles.x -= mouse.yDelta() * mouseSenstivity;
	eularAngles.y -= mouse.xDelta() * mouseSenstivity;
	eularAngles.x = glm::clamp<float>(eularAngles.x, -60.0f, 80.0f);
	
	camera.setPosition(transform.getPosition().x, transform.getPosition().y, transform.getPosition().z);
	camera.setRotation(-eularAngles.x, -eularAngles.y);
	camera.moveRelative(Vector3f(0.0f, 0.5f, 0.0f));

	transform.setRotation(eularAngles);

	auto bulletsSize = bullets.size();
	for (unsigned i = 0; i < bulletsSize; ++i) {
		if (bullets[i].timeOut()) {
			destroyBullet(i);
			--i;
			--bulletsSize;
		}
	}

	vec2 positionOnY = { transform.getPosition().x, transform.getPosition().z };
	if ((positionOnY.x < mapMinLimit.x || positionOnY.y < mapMinLimit.y ||
		positionOnY.x > mapMaxLimit.x || positionOnY.y > mapMaxLimit.y) && !hasFallen) {
		hasFallen = true;
		rigidbody.lockLinear(AXIS::x);
		rigidbody.lockLinear(AXIS::z);
		rigidbody.unlockLinear(AXIS::y);
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
}

void Player::fixedUpdate(float deltaTime) {
	GameObject::fixedUpdate(deltaTime);
	for (unsigned i = 0; i < bullets.size(); ++i) {
		bullets[i].fixedUpdate(deltaTime);
	}	
}

void Player::dispatchBullet() {
	//gunShotSound.play();
	bullets.push_back(Bullet(bulletMesh, Material(), nullptr, vec3(0, 0, -1))); // TODO custom mat
	bullets[bullets.size() - 1].transform = transform;
	bullets[bullets.size() - 1].transform.translate({ 0.249067f, 0.47149f, -1.25759f });
	isRecoiling = true;
	recoilTime = 0.0f;
	children[0]->getMesh()->setTexture(flashTexture);
}

void Player::reload() {
	isReloading = true;
	reloadTime = 0.0f;
	if (totalAmmo > 0)
		//reloadSound.play();

		if (totalAmmo > maxAmmo) {
			inHandAmmo = maxAmmo;
			totalAmmo -= maxAmmo;
		}
		else if (totalAmmo > 0) {
			inHandAmmo = totalAmmo;
			totalAmmo = 0;
		}

	//HUD.setInHandAmmo(inHandAmmo);
	//HUD.setTotalAmmo(totalAmmo);
}

void Player::draw() {

	GameObject::draw();
	for (unsigned i = 0; i < bullets.size(); ++i) {
		bullets[i].draw();
	}
}

void Player::destroyBullet(int index) {
	bullets.erase(bullets.begin() + index);
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

	}
	else {
		recoilTime += deltaTime;
		if (recoilTime < (recoilTimeOut / 2.0f)) {
			childrenEular.x += recoilImpact * deltaTime;
			childrenTranslation.z += recoilImpact * deltaTime * 0.05f;

		}
		else {
			childrenEular.x -= recoilImpact * deltaTime;
			childrenTranslation.z -= recoilImpact * deltaTime * 0.05f;
			children[0]->getMesh()->setTexture(transparentTexture);
		}
	}

	for (unsigned i = 0; i < children.size(); ++i) {
		children[i]->transform.setRotation(childrenEular);
		children[i]->transform.setPosition(childrenTranslation);
	}
}

void Player::reloadAnim(float deltaTime) {

	if (reloadTimeOut <= reloadTime) {
		isReloading = false;
		childrenEular = { 0, 0, 0 };
		childrenTranslation = { 0, 0, 0 };

	}
	else {
		reloadTime += deltaTime;
		if (reloadTime < (reloadTimeOut / 2.0f)) {
			childrenEular.x -= reloadPlaybackSpeed * deltaTime;
			childrenTranslation.z += reloadPlaybackSpeed * deltaTime * 0.005f;
		}
		else {
			childrenEular.x += reloadPlaybackSpeed * deltaTime;
			childrenTranslation.z -= reloadPlaybackSpeed * deltaTime * 0.005f;
		}
	}

	for (unsigned i = 0; i < children.size(); ++i) {
		children[i]->transform.setRotation(childrenEular);
		children[i]->transform.setPosition(childrenTranslation);
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