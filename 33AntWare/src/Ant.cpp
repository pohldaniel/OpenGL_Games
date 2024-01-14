#include <iostream>
#include "Ant.h"

Ant::Ant(const ObjSequence& objSequence, std::shared_ptr<aw::Mesh> mesh, aw::Material material, GameObject *parent, GameObject *target) :
	GameObject(mesh, material, parent, false, 3),
	objSequence(objSequence),
	target(target){
	
	originalMaterial = material;
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
	GameObject::fixedUpdate(fdt);
}

void Ant::update(float dt) {

	if (isAlive && isHurting && timeSinceDamage.getElapsedTimeSec() >= 0.3f){
		isHurting = false;
		material = originalMaterial;
	}

	if (!isAlive && timeSinceLastAlphaDecrease.getElapsedTimeSec() >= 0.1f){
		if (material.getAlpha() > 0.0f){
			material.setAlpha(material.getAlpha() - 0.05f);
		}
		timeSinceLastAlphaDecrease.reset();
	}


	if (isAlive && distance(transform.getPosition(), target->transform.getPosition()) <= detectionRaduis){
		glm::vec3 targetPos = target->transform.getPosition();
		targetPos.y = transform.getPosition().y;
		glm::mat3 lookAt = glm::lookAt(transform.getPosition(), targetPos, {0.0f, 1.0f, 0.0f });
		glm::dquat rotation = glm::dquat(lookAt);
		transform.setRotation(rotation);
		glm::vec3 rotationAxis = transform.getRotationAxis();

		if(length(rotationAxis) > 0.0f)
			transform.setRotation(transform.getRotationAxis(), -transform.getRotationAngle());
		else
			transform.setRotation(glm::vec3{1.0f, 0.0f, 0.0f }, 0.0f);

		bool _isColliding = aabb.isColliding(target->aabb);

		if(_isColliding)
			rigidbody.velocity = {0.0f, 0.0f, 0.0f };
		else
			rigidbody.velocity = {0.0f, 0.0f, -speed};
	}else{
		rigidbody.velocity = {0.0f, 0.0f, 0.0f };
	}

	if (length(rigidbody.velocity) > 0.0f){
		animate(dt);
	}else{
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

void Ant::draw() {
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
	transform.rotate({ 1, 0, 0 }, 180.0f);
	transform.translate({ 0, -1, 0 });
	rigidbody.velocity = { 0, 0, 0 };
}

bool Ant::timeToDestroy(){
	return !isAlive && material.getAlpha() <= 0.0f;
}