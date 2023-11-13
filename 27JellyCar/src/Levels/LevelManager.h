#ifndef LevelManagr_H
#define LevelManagr_H

#include "JellyPhysics/JellyPhysics.h"
using namespace JellyPhysics;

#include "LevelSoftBody.h"
#include "../Car/Car.h"

#include "SimpleStruct/BodyObject.h"

#include <map>

class LevelManager
{
private:

	std::string _assetLocation;

	Car* _car;
	Vector2 _carPos;

	std::vector<ObjectInfo> objectInfos;
	std::vector<LevelSoftBody*> gameBodies;
	std::vector<std::string> gameBodiesNames;
	std::vector<int> gameBodiesNumbers;

	float finishX;
	float finishY;
	float fallLine;

	//for map limits
	AABB worldLimits;

	std::string GetPathName(std::string& fileName);

public:

	~LevelManager();

	void SetAssetsLocation(std::string location);

	
	void InitPhysic(World *world);

	bool LoadLevel(World *world, std::string levelName, std::string carFileName);	
	bool LoadCompiledLevel(World *world, std::string levelName, std::string carFileName);
	bool ClearLevel(World *world);

	Car* GetCar();
	Vector2 GetCarStartPos();

	std::vector<LevelSoftBody*> GetLevelBodies();
	std::vector<ObjectInfo>& GetObjectInfos();
	AABB GetWorldLimits();

	Vector2 GetWorldCenter();
	Vector2 GetWorldSize();

	Vector2 GetLevelTarget();
	float GetLevelLine();

	std::string m_currentName;

};

#endif