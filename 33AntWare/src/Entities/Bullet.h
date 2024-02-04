#pragma once
#include <engine/Camera.h>
#include <engine/Clock.h>
#include <engine/AssimpModel.h>
#include <Scene/Object.h>

#include "Rigidbody.h"

class Bullet : public Object {

public:

	Bullet(const Vector3f& projectVec);
	Bullet(Bullet const& rhs);
	Bullet(Bullet&& rhs);
	Bullet &operator=(const Bullet& rhs);

	void start();
	void draw(const Camera& camera);
	void update(const float dt);
	void fixedUpdate(float fdt);
	bool timeOut();

	static void Init(AssimpModel* model);

	float lifeSpan = 5.0f;
	float speed = 40.0f;
	Clock timer;

	Rigidbody m_rigidbody;
	static AssimpModel* Model;
};