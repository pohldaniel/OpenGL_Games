#pragma once

#include <Entities/Md2Entity.h>


class Bot : public Md2Entity {

public:

	Bot(const Md2Model& md2Model);
	~Bot();

	void update(const float dt) override;

private:

	void followPath(float dt);

	Vector3f start, end, currentWaypoint;
};