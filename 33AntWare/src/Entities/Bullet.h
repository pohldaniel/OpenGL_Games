#pragma once
#include <engine/Camera.h>
#include <engine/Clock.h>
#include <Objects/Object.h>

#include "MaterialAW.h"
#include "Mesh.h"
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

	static void Init(std::shared_ptr<aw::Mesh> mesh, aw::Material material);

	aw::Rigidbody rigidbody;
	float lifeSpan = 5.0f;
	float speed = 40.0f;
	Clock timer;

	static std::shared_ptr<aw::Mesh> Mesh;
	static aw::Material Material;
};