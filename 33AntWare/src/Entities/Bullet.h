#pragma once
#include <engine/Camera.h>
#include <engine/Clock.h>
#include <Scene/Object.h>

#include "MeshSequence.h"
#include "Rigidbody.h"

class Bullet : public Object {

public:

	Bullet(const MeshSequence& meshSequence, const Vector3f& projectVec);
	Bullet(Bullet const& rhs);
	Bullet(Bullet&& rhs);
	Bullet &operator=(const Bullet& rhs);
	Bullet& operator=(Bullet&& rhs);

	void start();
	void draw();
	void update(const float dt);
	void fixedUpdate(float fdt);
	bool timeOut();

private:

	float m_lifeSpan;
	float m_speed;
	Clock m_timer;

	Rigidbody m_rigidbody;
	short m_meshIndex;
	short m_textureIndex;
	short m_materialIndex;
	
	std::reference_wrapper<const MeshSequence> meshSequence;
};