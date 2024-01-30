#pragma once
#include <engine/Vector.h>

enum AXIS {
	x = 1,
	y = 2,
	z = 4
};
   
class Rigidbody{
	
public:

	Rigidbody();
	Rigidbody(Rigidbody const& rhs);
	Rigidbody(Rigidbody&& rhs);
	Rigidbody& operator=(const Rigidbody& rhs);

	Vector3f velocity;
	Vector3f angularVelocity;
	Vector3f acceleration;
	Vector3f angularAcceleration;
	float mass;
	int linearLock;
	int angularLock;

       
	void lockLinear(AXIS axis);      
	void lockAngular(AXIS axis);    
	void unlockLinear(AXIS axis);    
	void unlockAngular(AXIS axis);     
	bool isLinearLocked(AXIS axis);
	bool isAngularLocked(AXIS axis);
};