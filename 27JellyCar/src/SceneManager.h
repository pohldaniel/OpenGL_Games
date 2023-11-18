#pragma once

#include <fstream>
#include <algorithm>
#include <iterator>

#include <vector>
#include <unordered_map>
#include <set>
#include <string>

#include <engine/Texture.h>
#include "Levels/SimpleStructs.h"
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

struct SoundSettings {
	float carVolume;
	float soundsVolume;
	float musicVolume;
};

class LevelSoftBody;
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
	void configure(const std::string scenesPath, bool useCompiledLevel = true);

	const std::vector<SkinInfo>& getSkinInfos() const;
	const std::vector<std::string>& getSceneFiles() const;
	const std::vector<std::string>& getThumbFiles() const;
	const std::vector<SceneInfo>& getSceneInfos() const;

	const std::string& getName() const;
	const std::string& getScenesPath() const;
	const std::string& getCurrentSceneFile() const;
	const SceneInfo& getCurrentSceneInfo() const;
	const SkinInfo& getCurrentSkinInfo() const;

	const std::vector<ObjectInfo>& getObjectInfos() const;
	const std::vector<SoftBodyInfo>& getSoftBodyInfos() const;
	const CarInfo& getCarInfo() const;
	const LevelInfo& getLevelInfo() const;

	const Vector2 getWorldCenter() const;
	const Vector2 getWorldSize() const;
	const AABB getWorldLimits() const;
	const Vector2 getLevelTarget() const;
	const float getLevelLine() const;
	Vector2 getCarStartPos();

	void loadLevel(const std::string path);
	void loadXmlLevel(const std::string path);
	void loadCompiledLevel(const std::string path);
	void loadOriginLevel(const std::string path);

	void loadSettings(std::string path);
	void saveSettings(std::string path);

	void saveCompiledLevel(const std::string path);
	void saveLevel(const std::string path);

	void buildLevel(World *world, std::vector<LevelSoftBody*>& gameBodies);
	void buildCar(World *world, Car*& car, const std::string& carFileName);

	int m_currentPosition;
	int m_carCurrentPosition;

	SoundSettings m_soundSettings;

	static void SaveLevel(const std::string path, const std::vector<ObjectInfo>& objectInfos, std::vector<LevelSoftBody*>& bodies, const Vector2& carPos, const Vector2& target, const float fallLine, const std::string levelName);
	static void SaveScores(const std::string path, const std::vector<SceneInfo>& levelInfos);
	static void ClearLevel(World *world, std::vector<LevelSoftBody*> bodies, Car* car);
	static void InitPhysic(World *world);

private:

	const std::vector<std::string> sceneFilesFromLevelInfos(const std::vector<SceneInfo>& levelInfos);
	const std::vector<std::string> thumbFilesFromLevelInfos(const std::vector<SceneInfo>& levelInfos);
	

	std::vector<SkinInfo> m_carSkins;
	std::vector<std::string> m_sceneFiles;
	std::vector<std::string> m_thumbFiles;
	std::vector<SceneInfo> m_sceneInfos;
	bool m_init;

	std::string m_name;
	std::string m_scenesPath;
	bool m_useCompiledLevel;

	CarInfo m_carInfo;
	LevelInfo m_levelInfo;
	std::vector<ObjectInfo> m_objectInfos;
	std::vector<SoftBodyInfo> m_softBodyInfos;

	AABB m_worldLimits;

	static void RemoveDuplicates(std::vector<LevelSoftBody*> &vector);
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