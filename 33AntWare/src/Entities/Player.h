#pragma once
#include <engine/input/EventDispatcher.h>
#include <engine/input/MouseEventListener.h>
#include <engine/Camera.h>
#include <engine/Clock.h>

#include "Entity.h"
#include "Bullet.h"

class Player: public Entity, public MouseEventListener {

public:

	Player(Camera& camera, std::shared_ptr<aw::Mesh> mesh, aw::Material material, const Vector2f& mapMinLimit, const Vector2f& mapMaxLimit);

	float speed = 10.0f, runningSpeed = 20.0f;
	float mouseSenstivity = 0.1f;

	float timeSinceDamage = 0.0f;
	float hp = 10.0f;
	unsigned totalAmmo = 36, inHandAmmo = 12, maxAmmo = 12;

	int transparentTexture, flashTexture;
	bool isRecoiling = false;
	float recoilImpact = 40.0f;
	float recoilTimeOut = 1.0f / 5.0f;
	float recoilTime;

	bool isReloading = false;
	float reloadTime;
	float reloadTimeOut = 1.5f;
	float reloadPlaybackSpeed = 50.0f;
	void recoilAnim(float deltaTime);
	void reloadAnim(float deltaTime);


	bool hasFallen = false;
	float fallingAcceleration = 1000.0f;
	float fallingTime = 0.0f;
	float dieAfter = 5.0f;

	Camera& camera;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	bool m_mouseDown = false;
	Clock shootTimer, reloadTimer;

	void draw(const Camera& camera) override;
	void update(const float dt) override;
	void fixedUpdate(float fdt) override;

	Vector3f childrenEular;
	Vector3f childrenTranslation;
	Vector3f eularAngles;
	std::vector<Bullet> bullets;

	bool isDead();
	void killSound();
	void dispatchBullet();
	void reload();
	void destroyBullet(int index);
	bool damage(float amount);
	void start();
	std::vector<Bullet>& getBullets();

	Vector2f mapMinLimit;
	Vector2f mapMaxLimit;
};