#pragma once
#include <Entities/Md2Entity.h>

class Bot : public Md2Entity {

public:

	Bot(const Md2Model& md2Model);
	~Bot();

	void update(const float dt) override;
	void setStart(const Vector3f& start);
	void setEnd(const Vector3f& end);
	void setStart(const float x, const float y, const float z);
	void setEnd(const float x, const float y, const float z);
	void setMoveSpeed(const float moveSpeed);

private:

	void followPath(float dt);

	Vector3f m_start, m_end, m_currentWaypoint;
	float m_moveSpeed;
};