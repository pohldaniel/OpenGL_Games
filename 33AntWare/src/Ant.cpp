#include <iostream>
#include "Ant.h"

Ant::Ant(){
	
}

void Ant::start() {
	baseIndex = getNumberOfMeshes() - 1;
	index = baseIndex;
}

void Ant::die(){
    //rigidbody.velocity = {0, 0, 0};
}

void Ant::update(float deltaTime) {
	/*Ant::update();
	if (isAlive && distance(transform.getPosition(), target->transform.getPosition()) <= detectionRaduis){
		vec3 targetPos = target->transform.getPosition();
		targetPos.y = transform.getPosition().y;
		mat3 lookAt = glm::lookAt(transform.getPosition(), targetPos, {0, 1, 0});
		dquat rotation = dquat(lookAt);
		transform.setRotation(rotation);
		vec3 rotationAxis = transform.getRotationAxis();
		if (length(rotationAxis) > 0)
			transform.setRotation(transform.getRotationAxis(), -transform.getRotationAngle());
		else
			transform.setRotation(vec3{1, 0, 0}, 0);
		if (aabb.isColliding(target->aabb))
			rigidbody.velocity = {0, 0, 0};
		else
			rigidbody.velocity = {0, 0, -speed};
	}else{
		rigidbody.velocity = {0, 0, 0};
	}*/

	animate(deltaTime);
}

void Ant::animate(float deltaTime){

    animTime += deltaTime;
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

void Ant::drawRaw() {
	ObjSequence::drawRaw(index);
}
