#ifndef Chassis_H
#define Chassis_H

#include <engine/Texture.h>
#include <engine/Vector.h>

#include "JellyPhysics/JellyPhysics.h"
using namespace JellyPhysics;

#include "Tire.h"
#include "SuspensionShock.h"
#include "Mesh.h"

class Chassis : public SpringBody
{
public:

	Mesh* _vertexObject;
	Mesh* _texturedObject;

	bool _created;

	bool _useNearestGravity;

	void GenerateTextures();
	void GenerateVertices();

public:

	ClosedShape		mShapeA;
	ClosedShape		mShapeB;

	std::vector<SuspensionShock>	mShocks;
	std::vector<Vector2>			mTexture1;
	std::vector<Vector2>			mTexture2;

	std::vector<int>				mPolys;
	int*							mIndices;
	int								mIndexCount;

	float							torqueForce;
	float							torque;
	float							maxOmega;

	bool							smallBig;

	Vector2 shockForce;
	bool ballon;

public:

	Chassis(World *w, const ClosedShape& sA, const ClosedShape& sB,
		float mpp, float shapeK, float shapeDamp, float edgeK, float edgeDamp,
		const Vector2& pos, const Vector2& scale);

	~Chassis();

	void accumulateExternalForces();

	void AddShock(int PM, Tire* t, float d1, float d2, float k1, float k2, float damp1, float damp2);
	
	void InterpolateShape(float x);

	void SetTorque(float t);
	float GetTorque();

	void SetShockForce(const Vector2& f);
	void AddShockForce(const Vector2& f);

	void SetBallonState(bool state);
	void UseNearestGracity(bool state);

	void StartPolys();
	void AddPoly(int a, int b, int c);
	void EndPolys();

	void Draw(Matrix4f& proj, Texture* texture);

};

#endif 