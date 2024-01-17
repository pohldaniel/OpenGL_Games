#include <iostream>
#include "Ant.h"

Ant::Ant(const ObjSequence& objSequence, std::shared_ptr<aw::Mesh> mesh, aw::Material material, Entity *target) :
	Entity(mesh, material),
	objSequence(objSequence),
	target(target){

	originalMaterial = material;
	m_isStatic = false;
	constructAABB();
	
	for (unsigned i = 0; i < 8; ++i){
		if (i < 4){
			aabb.bounds[i].x -= 2.5f;
		}else{
			aabb.bounds[i].x += 2.5f;
		}

		if (i < 2 || i == 4 || i == 5){
			aabb.bounds[i].y -= 0.1f;
		}else{
			aabb.bounds[i].y += 0.6f;
		}

		if (i % 2 == 0){
			aabb.bounds[i].z -= 0.5f;
		}else{
			aabb.bounds[i].z += 1.3f;
		}
	}
}

void Ant::start() {
	baseIndex = objSequence.getNumberOfMeshes() - 1;
	index = baseIndex;
}

void Ant::fixedUpdate(float fdt) {
	
	if (m_isStatic)
		return;

	Vector3f appliedVelocity = Quaternion::Rotate(m_orientation, Vector3f(rigidbody.velocity.x, rigidbody.velocity.y, rigidbody.velocity.z));
	Vector3f appliedAngularVelocity = Quaternion::Rotate(m_orientation, Vector3f(rigidbody.angularVelocity.x, rigidbody.angularVelocity.y, rigidbody.angularVelocity.z));

	if (rigidbody.isLinearLocked(aw::AXIS::x))
		appliedVelocity[0] = 0;
	if (rigidbody.isLinearLocked(aw::AXIS::y))
		appliedVelocity[1] = 0;
	if (rigidbody.isLinearLocked(aw::AXIS::z))
		appliedVelocity[2] = 0;
	if (rigidbody.isAngularLocked(aw::AXIS::x))
		appliedAngularVelocity[0] = 0;
	if (rigidbody.isAngularLocked(aw::AXIS::y))
		appliedAngularVelocity[1] = 0;
	if (rigidbody.isAngularLocked(aw::AXIS::z))
		appliedAngularVelocity[2] = 0;

	translate(appliedVelocity * fdt);
	rotate(appliedAngularVelocity * fdt);
}

void Ant::update(float dt) {

	if (isAlive && isHurting && timeSinceDamage.getElapsedTimeSec() >= 0.3f) {
		isHurting = false;
		material = originalMaterial;
	}

	if (!isAlive && timeSinceLastAlphaDecrease.getElapsedTimeSec() >= 0.1f) {
		if (material.getAlpha() > 0.0f) {
			material.setAlpha(material.getAlpha() - 0.05f);
		}
		timeSinceLastAlphaDecrease.reset();
	}

	if (isAlive && Vector3f::Length(m_position, target->getPosition()) <= detectionRaduis) {
		Vector3f targetPos = target->getPosition();
		targetPos[1] = m_position[1];

		Matrix4f lookAt = Matrix4f::LookAt(m_position, targetPos, { 0.0f, 1.0f, 0.0f });
		Quaternion rotation = Quaternion(lookAt);

		setOrientation(rotation);
		Vector3f rotationAxis = rotation.getRotationAxis();

		if (rotationAxis.lengthSq() > 0.0f)
			setOrientation(rotationAxis, -rotation.getYaw());
		else
			setOrientation({ 1.0f, 0.0f, 0.0f }, 0.0f);

		if (aabb.isColliding(target->aabb))
			rigidbody.velocity = { 0.0f, 0.0f, 0.0f };
		else
			rigidbody.velocity = { 0.0f, 0.0f, -speed };
	}else {
		rigidbody.velocity = { 0.0f, 0.0f, 0.0f };
	}

	if (length(rigidbody.velocity) > 0.0f) {
		animate(dt);
	}else {
		index = baseIndex;
	}

	updateSelfAndChild();
	recalculateAABB();
}

void Ant::animate(float dt){

    animTime += dt;
    if (animTime > walkcycleLength){
        animTime = 0.0f;
    }

    float progress = animTime / walkcycleLength;
    progress *= baseIndex;
    index = round(progress);
    if (index >= baseIndex){
        index = 0;
    }
}

void Ant::draw(const Camera& camera) {
	objSequence.drawRaw(index);
}

void Ant::damage(unsigned int amount){
	if (!isAlive)
		return;

	isHurting = true;
	//hurtSound.play();
	timeSinceDamage.reset();
	material.setDiffuse({ 1, 0, 0, 1 });

	if (hp <= amount){
		hp = 0;
		die();
	}else{
		hp -= amount;
	}
}

unsigned Ant::getHp(){
	return hp;
}

void Ant::die(){
	isAlive = false;
	isHurting = false;
	material = originalMaterial;
	rotate({ 1.0f, 0, 0 }, 180.0f);
	translate(0.0f, 1.0f, 0.0f);
	rigidbody.velocity = { 0, 0, 0 };
}

bool Ant::timeToDestroy(){
	return !isAlive && material.getAlpha() <= 0.0f;
}