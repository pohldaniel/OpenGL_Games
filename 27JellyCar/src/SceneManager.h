#pragma once

#include <fstream>
#include <algorithm>
#include <iterator>

#include <vector>
#include <unordered_map>
#include <string>

#include "Levels/SkinInfo.h"

struct LevelInfo2 {
	std::string name;
	std::string file;
	std::string thumb;
	float time;
	float jump;
};

class SceneInfo {

	friend class SceneManager;

public:

	SceneInfo();
	void loadLevelInfo(std::string path);
	void loadCarSkins(std::string path);
	void loadScores(std::string path);
	void saveScores(std::string path);

	const std::vector<SkinInfo>& getSkinInfos() const;
	const std::vector<std::string>& getSceneFiles() const;
	const std::vector<std::string>& getThumbFiles() const;
	const std::vector<LevelInfo2>& getLevelInfos() const;

	const SkinInfo& getCurrentSkinInfo() const;
	const std::string& getCurrentSceneFile() const;
	const LevelInfo2& getCurrentLevelInfo() const;

	int m_currentPosition;
	int m_carCurrentPosition;

private:

	const std::vector<std::string> sceneFilesFromLevelInfos(const std::vector<LevelInfo2>& levelInfos);
	const std::vector<std::string> thumbFilesFromLevelInfos(const std::vector<LevelInfo2>& levelInfos);

	std::vector<SkinInfo> m_carSkins;
	std::vector<std::string> m_sceneFiles;
	std::vector<std::string> m_thumbFiles;
	std::vector<LevelInfo2> m_levelInfos;
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