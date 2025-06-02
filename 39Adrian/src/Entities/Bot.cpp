#include "Bot.h"

Bot::Bot(const Md2Model& md2Model) : Md2Entity(md2Model) {
	start.set(620.0f, 0.0f, -380.0f);
	end.set(820.0f, 0.0f, -280.0f);
	currentWaypoint = end;
	setAnimationType(AnimationType::RUN);
	setSpeed(0.3f);
}

Bot::~Bot() {

}

void Bot::update(const float dt) {
	Md2Node::update(dt);
	followPath(dt);
}

void Bot::followPath(float dt) {
	Vector3f nextWaypoint = currentWaypoint;
	Vector3f pos = getWorldPosition();

	float move = 35.0f * dt;
	float distance = (pos - nextWaypoint).length();

	if (move > distance)
		move = distance;

	setOrientation(Quaternion(pos, nextWaypoint));
	translateRelative(Vector3f::FORWARD * move);

	if (distance < 0.2f) {
		currentWaypoint = start;
		start = end;
		end = currentWaypoint;
	}
}