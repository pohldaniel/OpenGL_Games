#ifndef __gameObjectH__
#define __gameObjectH__

#include "Vector.h"

class GameObject{

public:
	
	virtual void update(float elapsedTime) = 0;
	virtual void render() = 0;
	virtual ~GameObject() {};

	const Vector2f &getPosition();
	const Matrix4f &getTransform();

protected:
	Vector2f m_position;
	Matrix4f m_transform = Matrix4f::IDENTITY;
};

#endif // __gameObjectH__