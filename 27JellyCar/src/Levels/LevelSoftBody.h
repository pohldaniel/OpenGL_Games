#ifndef LevelSoftBody_H
#define LevelSoftBody_H

#include <engine/Rect.h>
#include <engine/Texture.h>

#include "JellyPhysics/JellyPhysics.h"
#include "SimpleStruct/BodyObject.h"


#include "ObjectInfo.h"
#include <glm/mat4x2.hpp>

using namespace JellyPhysics;

#include "KinematicControl.h"

struct SoftBodyInfo2;

struct Point {
	float x;
	float y;
	float mass;
};

struct  BodyInfo{
	std::string name;
	float massPerPoint;
	float edgeK;
	float edgeDamping;
	float colorR, colorG, colorB;
	bool kinematic;
	bool shapeMatching;
	float shapeK;
	float shapeDamping;
	float velDamping;

	bool pressureized;
	float pressure;
};

struct Spring{
	int pt1;
	int pt2;
	float k;
	float damp;
};

struct Triangle {
	int pt0;
	int pt1;
	int pt2;
};

class LevelSoftBody
{
private:

	struct MassID
	{
		int _id;
		float _mass;
	};

	Body* mBody;

	typedef std::vector<KinematicControl*>	ControlList;

	ControlList				mControls;

	//---------------------------------------------------------------------
	//float*				mColorArray;
	JellyPhysics::AABB		mIgnoreAABB;


	//----------------------

	std::vector<int> mIndexList;
	int *mIndices;
	int mIndicesCount;

	float colorR, colorG, colorB;
	std::string name;
	float massPerPoint;// = 0.0f;
	float edgeK;//= 100.0f;
	float edgeDamping;//= 1.0f;
	bool kinematic;//= false;
	bool shapeMatching;//= true;
	float shapeK;//= 100.0f;
	float shapeDamping;//= 10.0f;
	float velDamping;//= 10.0f;
	bool pressureized;//= false;
	float pressure;//= 0.0f;

	bool _visible;

	std::vector<MassID> massExceptions;
	ObjectInfo _bodyInfo;

public:

	LevelSoftBody(std::string fileName, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material);
	LevelSoftBody(const SoftBodyInfo2& softBodyInfo, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material);

	LevelSoftBody(LevelSoftBody *exBody, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material);
	LevelSoftBody(BodyObject *exBody, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material);

	LevelSoftBody(std::string fileName, World *mWorld, ObjectInfo bodyInfo);
	LevelSoftBody(const SoftBodyInfo2& softBodyInfo, World *mWorld, ObjectInfo bodyInfo);
	LevelSoftBody(LevelSoftBody *exBody, World *mWorld, ObjectInfo bodyInfo);
	LevelSoftBody(BodyObject *exBody, World *mWorld, ObjectInfo bodyInfo);

	~LevelSoftBody();

	void AddKinematicControl(KinematicControl* kinematicControl);
	void Finalize();
	void Update(float elapsed);
	void SetIgnore(bool state);

	Body* GetBody();
	const AABB& GetIgnoreAABB();

	std::string GetName();
	glm::vec2 GetStartPosition();

	void SetVisible(bool visible);
	bool IsVisible();

	void AddTriangle(int a, int b, int c);
	void FinalizeTriangles();

	void SetTextureRect(const TextureRect& rect);
	void SetTexture(Texture* texture);
	void SetLineColor(glm::vec4 color);

	void Draw(glm::mat4 &proj);

	BodyInfo m_bodyInfo;
	std::vector<Point> m_points;
	std::vector<Spring> m_springs;
	std::vector<Triangle> m_triangles;
};

#endif