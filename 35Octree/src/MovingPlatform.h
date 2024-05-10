#pragma once
#include <memory>
#include <Physics/Physics.h>
#include <engine/scene/ShapeNode.h>


class MovingPlatform {

public:

	MovingPlatform();
	~MovingPlatform();
	void fixedUpdate(float fdt);

	void initialize(ShapeNode* model, btCollisionObject* collisionShape, const Vector3f &finishPosition);
	void setPlatformSpeed(float maxLiftSpeed);
	ShapeNode* getShapeNode();

protected:

	ShapeNode* m_shapeNode;
	btCollisionObject* m_platformShape;

	Vector3f m_initialPosition;
	Vector3f m_finishPosition;
	Vector3f m_directionToFinish;
	float m_maxLiftSpeed;
	float m_minLiftSpeed;
	float m_curLiftSpeed;

	int m_platformState;

	enum PlatformStateType{
		PLATFORM_STATE_START,
		PLATFORM_STATE_MOVETO_FINISH,
		PLATFORM_STATE_MOVETO_START,
		PLATFORM_STATE_FINISH
	};
};