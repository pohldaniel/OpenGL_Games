#ifndef LevelSoftBody_H
#define LevelSoftBody_H

#include <engine/Rect.h>
#include <engine/Texture.h>

#include "JellyPhysics/JellyPhysics.h"
#include "Levels/SimpleStructs.h"

#include <glm/mat4x2.hpp>

using namespace JellyPhysics;

#include "KinematicControl.h"

class LevelSoftBody{

	friend class Scene;

private:

	struct MassID{
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
	//std::string name;
	float massPerPoint;// = 0.0f;
	float edgeK;//= 100.0f;
	float edgeDamping;//= 1.0f;
	bool isKinematic;//= false;
	bool shapeMatching;//= true;
	float shapeK;//= 100.0f;
	float shapeDamping;//= 10.0f;
	float velDamping;//= 10.0f;
	bool pressureized;//= false;
	float pressure;//= 0.0f;

	bool _visible;

	std::vector<MassID> massExceptions;
	
public:

	LevelSoftBody(std::string fileName, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material);
	LevelSoftBody(const SoftBodyInfo& softBodyInfo, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material);

	LevelSoftBody(LevelSoftBody *exBody, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material);
	LevelSoftBody(SoftBodyInfo *exBody, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material);

	LevelSoftBody(std::string fileName, World *mWorld, ObjectInfo objectInfo);
	LevelSoftBody(const SoftBodyInfo& softBodyInfo, World *mWorld, ObjectInfo objectInfo);
	LevelSoftBody(LevelSoftBody *exBody, World *mWorld, ObjectInfo objectInfo);

	~LevelSoftBody();

	void AddKinematicControl(KinematicControl* kinematicControl);
	void Finalize();
	void Update(float elapsed);
	void SetIgnore(bool state);

	Body* GetBody();
	const AABB& GetIgnoreAABB();

	std::string GetName() const;
	glm::vec2 GetStartPosition();

	void SetVisible(bool visible);
	bool IsVisible();

	void AddTriangle(int a, int b, int c);
	void FinalizeTriangles();

	void SetTextureRect(const TextureRect& rect);
	void SetTexture(Texture* texture);
	void SetLineColor(glm::vec4 color);

	void Draw(glm::mat4 &proj);

	ObjectInfo m_objectInfo;
};

#endif