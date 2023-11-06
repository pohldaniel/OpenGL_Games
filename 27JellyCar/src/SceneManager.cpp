#include <tinyxml.h>
#include <iostream>
#include "SceneManager.h"

SceneInfo::SceneInfo() : 
	m_currentPosition(0),
	m_carCurrentPosition(0),
	m_init(false){
}

const std::vector<SkinInfo>& SceneInfo::getSkinInfos() const {
	return m_carSkins;
}

const std::vector<std::string>& SceneInfo::getSceneFiles() const {
	return m_sceneFiles;
}

const std::vector<LevelInfo2>& SceneInfo::getLevelInfos() const {
	return m_levelInfos;
}

const SkinInfo& SceneInfo::getCurrentSkinInfo() const {
	return m_carSkins[m_carCurrentPosition];
}

const std::string& SceneInfo::getCurrentSceneFile() const {
	return m_sceneFiles[m_currentPosition];
}

const LevelInfo2& SceneInfo::getCurrentLevelInfo() const {
	return m_levelInfos[m_currentPosition];
}

void SceneInfo::loadLevelInfo(std::string path) {
	if (m_init) return;
	std::ifstream is(path, std::ifstream::in);

	is.seekg(0, is.end);
	int length = is.tellg();
	is.seekg(0, is.beg);

	unsigned char* buffer = new unsigned char[length];
	is.read(reinterpret_cast<char*>(buffer), length);
	is.close();

	//load data
	TiXmlDocument doc;
	if (!doc.LoadContent(buffer, length)) {
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Scenes").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement()) {
		LevelInfo2 info;

		info.name = ObjectNode->Attribute("name");
		info.file = ObjectNode->Attribute("file");
		info.thumb = ObjectNode->Attribute("thumb");
		info.time = 999.0f;
		info.jump = 0.0f;

		m_levelInfos.push_back(info);

	}
	
	m_sceneFiles = sceneFilesFromLevelInfos(m_levelInfos);	
}

void SceneInfo::loadCarSkins(std::string path) {
	if (m_init) return;
	std::ifstream is(path, std::ifstream::in);

	is.seekg(0, is.end);
	int length = is.tellg();
	is.seekg(0, is.beg);

	unsigned char* buffer = new unsigned char[length];
	is.read(reinterpret_cast<char*>(buffer), length);
	is.close();

	//load data
	TiXmlDocument doc;
	if (!doc.LoadContent(buffer, length)) {
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Skins").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement()){
		SkinInfo skinInfo;
		skinInfo.name = ObjectNode->Attribute("name");
		skinInfo.tireSmall = ObjectNode->Attribute("tireSmall");
		skinInfo.tireBig = ObjectNode->Attribute("tireBig");
		skinInfo.chassisSmall = ObjectNode->Attribute("chassisSmall");
		skinInfo.chassisBig = ObjectNode->Attribute("chassisBig");
		m_carSkins.push_back(skinInfo);
	}
	m_init = true;
}

const std::vector<std::string> SceneInfo::sceneFilesFromLevelInfos(const std::vector<LevelInfo2>& levelInfos) {
	std::vector<std::string> sceneFiles;
	std::transform(levelInfos.begin(), levelInfos.end(), std::back_inserter(sceneFiles), [](const LevelInfo2& info)-> std::string { return info.file; });
	return sceneFiles;
}

///////////////////////ScenetManager//////////////////////////
SceneManager SceneManager::s_instance;

SceneInfo& SceneManager::getSceneInfo(std::string name) {
	return m_sceneInfos[name];
}

bool SceneManager::containsScene(std::string name) {
	return m_sceneInfos.count(name) == 1;
}

SceneManager& SceneManager::Get() {
	return s_instance;
}
