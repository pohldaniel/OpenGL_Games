#ifndef GameBallonBody_H
#define GameBallonBody_H

#include <glm/gtc/matrix_transform.hpp>

#include "JellyPhysics/JellyPhysics.h"
using namespace JellyPhysics;

#include "Mesh.h"

class Chassis;

class GameBallonBody : public SpringBody
{
private:

	Mesh* _vertexObject;
	Mesh* _shapeObject;
	glm::vec4 _color;
	bool _created;

	std::vector<int> mIndexList;
	int *mIndices;
	int mIndicesCount;

private:

	Vector2 ballonPosition;
	Vector2 ballonVelocity;

	float torque;

	bool ballonActive;

	Chassis* gameBallonBody;

public:

	GameBallonBody(World* w, const ClosedShape& shape, float massPerPoint,
		float shapeSpringK, float shapeSpringDamp,
		float edgeSpringK, float edgeSpringDamp,
		const Vector2& pos, float angleinRadians, const Vector2& scale,
		bool kinematic);

	~GameBallonBody();

	void AddTriangle(int a, int b, int c);
	void FinalizeTriangles();

	void accumulateExternalForces();

	void SetTorque(float t);

	void SetBallonBody(Chassis* body);

	void SetBallonState(bool state);

	void SetBallonPosition(Vector2 pos, Vector2 velocity);

	void Draw(glm::mat4 &proj);
};

#endif 