#pragma once

#include <fstream>
#include <algorithm>
#include <iterator>

#include <vector>
#include <unordered_map>
#include <string>

#include <engine/Texture.h>
#include "Levels/ObjectInfo.h"
#include "Levels/LevelSoftBody.h"
#include "Car/Car.h"

struct LevelInfo {
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

class SceneInfo {

	friend class SceneManager;

public:

	SceneInfo();
	void loadLevelInfo(std::string path);
	void loadCarSkins(std::string path);
	void loadScores(std::string path);
	
	float getTime(std::string levelName);
	float getJump(std::string levelName);
	void setTime(std::string levelName, float time);
	void setJump(std::string levelName, float jump);

	const std::vector<SkinInfo>& getSkinInfos() const;
	const std::vector<std::string>& getSceneFiles() const;
	const std::vector<std::string>& getThumbFiles() const;
	const std::vector<LevelInfo>& getLevelInfos() const;

	const SkinInfo& getCurrentSkinInfo() const;
	const std::string& getCurrentSceneFile() const;
	const LevelInfo& getCurrentLevelInfo() const;

	int m_currentPosition;
	int m_carCurrentPosition;

	static void SaveLevel(const std::string path, const std::vector<ObjectInfo>& objectInfos, const std::vector<LevelSoftBody*>& bodies, const Vector2& carPos, const Vector2& target, const float flallLine, const std::string levelName);
	static void LoadLevel(const std::string path);
	
	static void SaveScores(const std::string path, const std::vector<LevelInfo>& levelInfos);
private:

	const std::vector<std::string> sceneFilesFromLevelInfos(const std::vector<LevelInfo>& levelInfos);
	const std::vector<std::string> thumbFilesFromLevelInfos(const std::vector<LevelInfo>& levelInfos);

	std::vector<std::string> SceneInfo::mergeAlternately(std::vector<std::string> a, std::vector<std::string> b, std::vector<std::string> c, std::vector<std::string> d);

	std::vector<SkinInfo> m_carSkins;
	std::vector<std::string> m_sceneFiles;
	std::vector<std::string> m_thumbFiles;
	std::vector<LevelInfo> m_levelInfos;
	bool m_init;
};


class SceneManager {

public:

	SceneInfo& getSceneInfo(std::string name);
	bool containsScene(std::string name);

	static SceneManager& Get();

private:
	SceneManager() = default;


	std::unordered_map<std::string, SceneInfo> m_sceneInfos;
	static SceneManager s_instance;
};