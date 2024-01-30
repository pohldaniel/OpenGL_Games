#include <Rigidbody.h>

Rigidbody::Rigidbody() : 
	velocity({ 0, 0, 0 }), 
	angularVelocity({ 0, 0, 0 }), 
	acceleration({ 0, 0, 0 }), 
	angularAcceleration({ 0, 0, 0 }), 
	mass(1.0f), 
	linearLock(0), 
	angularLock(0){
}

Rigidbody::Rigidbody(Rigidbody const& rhs) {
	velocity = rhs.velocity;
	angularVelocity = rhs.angularVelocity;
	acceleration = rhs.acceleration;

	angularAcceleration = rhs.angularAcceleration;
	mass = rhs.mass;
	linearLock = rhs.linearLock;
	angularLock = rhs.angularLock;
}

Rigidbody::Rigidbody(Rigidbody&& rhs) {
	velocity = rhs.velocity;
	angularVelocity = rhs.angularVelocity;
	acceleration = rhs.acceleration;

	angularAcceleration = rhs.angularAcceleration;
	mass = rhs.mass;
	linearLock = rhs.linearLock;
	angularLock = rhs.angularLock;
}

Rigidbody& Rigidbody::operator=(const Rigidbody& rhs) {
	velocity = rhs.velocity;
	angularVelocity = rhs.angularVelocity;
	acceleration = rhs.acceleration;

	angularAcceleration = rhs.angularAcceleration;
	mass = rhs.mass;
	linearLock = rhs.linearLock;
	angularLock = rhs.angularLock;
	return *this;
}

void Rigidbody::lockLinear(AXIS axis){
    linearLock |= axis;
}

void Rigidbody::lockAngular(AXIS axis){
    angularLock |= axis;
}

void Rigidbody::unlockLinear(AXIS axis){
    linearLock ^= axis;
}

void Rigidbody::unlockAngular(AXIS axis){
    angularLock ^= axis;
}

bool Rigidbody::isLinearLocked(AXIS axis){
    return linearLock & axis;
}

bool Rigidbody::isAngularLocked(AXIS axis){
    return angularLock & axis;
}