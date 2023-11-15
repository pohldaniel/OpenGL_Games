#pragma once

#include <fstream>
#include <algorithm>
#include <iterator>

#include <vector>
#include <unordered_map>
#include <string>

#include <engine/Texture.h>
#include "Levels/LevelSoftBody.h"
#include "Levels/KinematicPlatform.h"
#include "Levels/KinematicMotor.h"
#include "Car/Car.h"

struct SceneInfo {
	std::string name;
	std::string file;
	std::string thumb;
	float time;
	float jump;
};

struct SkinTexture {
	Texture* chassisSmall;
	Texture* chassisBig;
	Texture* tireSmall;
	Texture* tireBig;
};

struct SkinInfo {
	std::string name;
	std::string chassisSmall;
	std::string chassisBig;
	std::string tireSmall;
	std::string tireBig;
	SkinTexture skinTexture;
};

struct ObjectInfo2 {
	char name[64];
	float posX;
	float posY;
	float angle;
	float scaleX;
	float scaleY;
	int material;
	bool isPlatform;
	bool isMotor;
	float offsetX;
	float offsetY;
	float secondsPerLoop;
	float startOffset;
	float radiansPerSecond;
};


struct Point2 {
	float x;
	float y;
	float mass;
};

struct Spring2 {
	int pt1;
	int pt2;
	float k;
	float damp;
};

struct Triangle2 {
	int pt0;
	int pt1;
	int pt2;
};

struct Triangle3 {
	float pt0;
	float pt1;
	float pt2;
};

struct CarInfo {
	char name[64];
	float posX;
	float posY;
};

struct LevelInfo {
	float finishX;
	float finishY;
	float fallLine;
};

struct SoftBodyAttributes {
	char name[64];

	float colorR;
	float colorG;
	float colorB;

	float massPerPoint;
	float edgeK;
	float edgeDamping;

	bool isKinematic;
	bool shapeMatching;
	float shapeK;
	float shapeDamping;
	float velDamping;

	bool pressureized;
	float pressure;
};


struct SoftBodyInfo2 {
	SoftBodyAttributes softBodyAttributes;

	int pointCount;
	std::vector<Point2> points;

	int springCount;
	std::vector<Spring2> springs;

	int polygonCount;
	std::vector<Triangle2> polygons;
};

struct SoftBodyAttributes3 {
	char name[64];

	float colorR;
	float colorG;
	float colorB;

	float massPerPoint;
	float edgeK;
	float edgeDamping;

	bool isKinematic;
	bool shapeMatching;
	float shapeK;
	float shapeDamping;
	//float velDamping;

	bool pressureized;
	float pressure;
};

struct SoftBodyInfo3 {
	SoftBodyAttributes3 softBodyAttributes;

	int pointCount;
	std::vector<Point2> points;

	int springCount;
	std::vector<Spring2> springs;

	int polygonCount;
	std::vector<Triangle3> polygons;
};



class Scene {

	friend class SceneManager;

public:

	Scene();
	void loadSceneInfo(std::string path);
	void loadCarSkins(std::string path);
	void loadScores(std::string path);
	
	float getTime(std::string levelName);
	float getJump(std::string levelName);
	void setTime(std::string levelName, float time);
	void setJump(std::string levelName, float jump);

	const std::vector<SkinInfo>& getSkinInfos() const;
	const std::vector<std::string>& getSceneFiles() const;
	const std::vector<std::string>& getThumbFiles() const;
	const std::vector<SceneInfo>& getSceneInfos() const;

	const std::string& getName() const;
	const std::string& getCurrentSceneFile() const;
	const SceneInfo& getCurrentSceneInfo() const;
	const SkinInfo& getCurrentSkinInfo() const;

	const std::vector<ObjectInfo2>& getObjectInfos() const;
	const std::vector<SoftBodyInfo2>& getSoftBodyInfos() const;
	const CarInfo& getCarInfo() const;
	const LevelInfo& getLevelInfo() const;

	const Vector2 getWorldCenter() const;
	const Vector2 getWorldSize() const;
	const AABB getWorldLimits() const;
	const Vector2 getLevelTarget() const;
	const float getLevelLine() const;
	Vector2 getCarStartPos();

	void loadLevel(const std::string path);
	void loadCar(const std::string path);
	void loadCompiledLevel(const std::string path, bool reinterprate = false);
	void saveCompiledLevel(const std::string path, bool reinterprate = false);

	void buildLevel(World *world, std::vector<LevelSoftBody*>& gameBodies);
	void buildCar(World *world, Car*& car, const std::string& carFileName);
	

	int m_currentPosition;
	int m_carCurrentPosition;

	static void SaveLevel(const std::string path, const std::vector<ObjectInfo>& objectInfos, const std::vector<LevelSoftBody*>& bodies, const Vector2& carPos, const Vector2& target, const float flallLine, const std::string levelName);
	static void SaveScores(const std::string path, const std::vector<SceneInfo>& levelInfos);
	static void ClearLevel(World *world, std::vector<LevelSoftBody*> bodies, Car* car);
	static void InitPhysic(World *world);

private:

	const std::vector<std::string> sceneFilesFromLevelInfos(const std::vector<SceneInfo>& levelInfos);
	const std::vector<std::string> thumbFilesFromLevelInfos(const std::vector<SceneInfo>& levelInfos);

	std::vector<std::string> mergeAlternately(std::vector<std::string> a, std::vector<std::string> b, std::vector<std::string> c, std::vector<std::string> d);

	std::vector<SkinInfo> m_carSkins;
	std::vector<std::string> m_sceneFiles;
	std::vector<std::string> m_thumbFiles;
	std::vector<SceneInfo> m_sceneInfos;
	bool m_init;

	std::string m_name;
	CarInfo m_carInfo;
	LevelInfo m_levelInfo;
	std::vector<ObjectInfo2> m_objectInfos;
	std::vector<SoftBodyInfo2> m_softBodyInfos;

	std::vector<SoftBodyInfo3> m_softBodyInfos3;

	AABB m_worldLimits;
};


class SceneManager {

public:

	Scene& getScene(std::string name);
	bool containsScene(std::string name);

	static SceneManager& Get();

private:
	SceneManager() = default;


	std::unordered_map<std::string, Scene> m_sceneInfos;
	static SceneManager s_instance;
};