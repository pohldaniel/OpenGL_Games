#pragma once
#include <engine/Camera.h>
#include <engine/Clock.h>
#include <engine/AssimpModel.h>
#include <Scene/Object.h>

#include "MaterialAW.h"
#include "Rigidbody.h"
#include "TransformAW.h"

class Bullet : public Object {

public:

	Bullet(const Vector3f& projectVec);
	Bullet(Bullet const& rhs);
	Bullet(Bullet&& source);
	Bullet &operator=(const Bullet& rhs);

	void start();
	void draw(const Camera& camera);
	void update(const float dt);
	void fixedUpdate(float fdt);
	bool timeOut();

	static void Init(AssimpModel* model, aw::Material material);

	float lifeSpan = 5.0f;
	float speed = 40.0f;
	Clock timer;

	aw::Rigidbody rigidbody;
	static AssimpModel* Model;
	static aw::Material Material;
};