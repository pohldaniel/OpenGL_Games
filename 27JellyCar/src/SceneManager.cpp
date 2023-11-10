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

const std::vector<std::string>& SceneInfo::getThumbFiles() const {
	return m_thumbFiles;
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
	std::streamoff length = is.tellg();
	is.seekg(0, is.beg);

	unsigned char* buffer = new unsigned char[length];
	is.read(reinterpret_cast<char*>(buffer), length);
	is.close();

	//load data
	TiXmlDocument doc;
	if (!doc.LoadContent(buffer, static_cast<int>(length))){
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
	m_thumbFiles = thumbFilesFromLevelInfos(m_levelInfos);
}

void SceneInfo::loadCarSkins(std::string path) {
	if (m_init) return;
	std::ifstream is(path, std::ifstream::in);

	is.seekg(0, is.end);
	std::streamoff length = is.tellg();
	is.seekg(0, is.beg);

	unsigned char* buffer = new unsigned char[length];
	is.read(reinterpret_cast<char*>(buffer), length);
	is.close();

	//load data
	TiXmlDocument doc;
	if (!doc.LoadContent(buffer, static_cast<int>(length))) {
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

void SceneInfo::loadScores(std::string path) {
	//loac main level file
	std::ifstream is(path, std::ifstream::in);

	if (!is.is_open())
		return;

	is.seekg(0, is.end);
	std::streamoff length = is.tellg();
	is.seekg(0, is.beg);

	unsigned char* buffer = new unsigned char[length];
	is.read(reinterpret_cast<char*>(buffer), length);
	is.close();

	//load data
	TiXmlDocument doc;
	if (!doc.LoadContent(buffer, static_cast<int>(length))) {
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Levels").FirstChild().Element();
	
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement()) {
		int index = std::stoi(ObjectNode->Attribute("id"));
		m_levelInfos[index].time = std::stof(ObjectNode->Attribute("score"));
		m_levelInfos[index].jump = std::stof(ObjectNode->Attribute("jump"));
	}
}

void SceneInfo::saveScores(std::string path) {
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//root
	TiXmlElement * root = new TiXmlElement("Levels");
	doc.LinkEndChild(root);

	for (int i = 0; i < m_levelInfos.size(); i++){
		TiXmlElement * cxn = new TiXmlElement("Level");
		root->LinkEndChild(cxn);
		cxn->SetAttribute("name", m_levelInfos[i].name.c_str());
		cxn->SetAttribute("id", i);
		cxn->SetDoubleAttribute("score", m_levelInfos[i].time);
		cxn->SetDoubleAttribute("jump", m_levelInfos[i].jump);
	}

	doc.SaveFile(path.c_str());
}

float SceneInfo::getTime(std::string levelName){
	for (size_t i = 0; i < m_levelInfos.size(); i++){
		if (m_levelInfos[i].name == levelName){
			return m_levelInfos[i].time;
		}
	}

	return 999.0f;
}

float SceneInfo::getJump(std::string levelName){
	for (size_t i = 0; i < m_levelInfos.size(); i++){
		if (m_levelInfos[i].name == levelName){
			return m_levelInfos[i].jump;
		}
	}

	return 0.0f;
}

void SceneInfo::setTime(std::string levelName, float time){
	for (size_t i = 0; i < m_levelInfos.size(); i++){
		if (m_levelInfos[i].name == levelName){
			m_levelInfos[i].time = time;
			return;
		}
	}
}

void SceneInfo::setJump(std::string levelName, float jump){
	for (size_t i = 0; i < m_levelInfos.size(); i++){
		if (m_levelInfos[i].name == levelName){
			m_levelInfos[i].jump = jump;
			return;
		}
	}
}

const std::vector<std::string> SceneInfo::sceneFilesFromLevelInfos(const std::vector<LevelInfo2>& levelInfos) {
	std::vector<std::string> sceneFiles;
	std::transform(levelInfos.begin(), levelInfos.end(), std::back_inserter(sceneFiles), [](const LevelInfo2& info)-> std::string { return info.file; });
	return sceneFiles;
}

const std::vector<std::string> SceneInfo::thumbFilesFromLevelInfos(const std::vector<LevelInfo2>& levelInfos) {
	std::vector<std::string> thumbs;
	std::transform(levelInfos.begin(), levelInfos.end(), std::back_inserter(thumbs), [](const LevelInfo2& info)-> std::string { return "Assets/Jelly/Thumbs/" + info.thumb + ".png"; });
	return thumbs;
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
