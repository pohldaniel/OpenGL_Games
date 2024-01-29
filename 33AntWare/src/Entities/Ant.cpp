#include <iostream>
#include "Ant.h"

Ant::Ant(const ObjSequence& objSequence, AssimpModel* model, Entity *target) : Entity(model),
	objSequence(objSequence),
	target(target) {
	m_isStatic = false;

	m_material.setAmbient({ 0.1f, 0.1f, 0.1f, 1.0f });
	m_material.setDiffuse({ 0.6f, 0.6f, 0.6f, 1.0f });
	m_material.setSpecular({ 0.3f, 0.3f, 0.3f, 1.0f });
	m_material.setShininess(8.0f);
	m_material.setAlpha(1.0f);
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
	}

	if (!isAlive && timeSinceLastAlphaDecrease.getElapsedTimeSec() >= 0.1f) {
		if (getMaterial().getAlpha() > 0.0f) {
			getMaterial().setAlpha(getMaterial().getAlpha() - 0.05f);
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

		if (getWorldBoundingBox().isColliding(target->getWorldBoundingBox()))
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
	m_material.updateMaterialUbo(BuiltInShader::materialUbo);
	objSequence.drawRaw(index);
}

void Ant::damage(unsigned int amount){
	if (!isAlive)
		return;

	isHurting = true;
	//hurtSound.play();
	timeSinceDamage.reset();
	getMaterial().setDiffuse({ 1.0f, 0.0f, 0.0f, 1.0f });
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
	rotate({ 1.0f, 0.0f, 0.0f }, 180.0f);
	translate(0.0f, 1.0f, 0.0f);
	rigidbody.velocity = { 0, 0, 0 };
}

bool Ant::timeToDestroy(){
	return !isAlive && m_material.getAlpha() <= 0.0f;
}

const Material& Ant::getMaterial() const {
	return m_material;
}