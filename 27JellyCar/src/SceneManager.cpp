#include <tinyxml.h>
#include <iostream>
#include "SceneManager.h"
#include "Levels/LevelManager.h"

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

const std::vector<LevelInfo>& SceneInfo::getLevelInfos() const {
	return m_levelInfos;
}

const SkinInfo& SceneInfo::getCurrentSkinInfo() const {
	return m_carSkins[m_carCurrentPosition];
}

const std::string& SceneInfo::getCurrentSceneFile() const {
	return m_sceneFiles[m_currentPosition];
}

const LevelInfo& SceneInfo::getCurrentLevelInfo() const {
	return m_levelInfos[m_currentPosition];
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

		skinInfo.skinTexture = { new Texture("Assets/Jelly/Car_Skins/" + skinInfo.chassisSmall), new Texture("Assets/Jelly/Car_Skins/" + skinInfo.chassisBig), new Texture("Assets/Jelly/Car_Skins/" + skinInfo.tireSmall), new Texture("Assets/Jelly/Car_Skins/" + skinInfo.tireBig) };

		m_carSkins.push_back(skinInfo);
	}	
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
	if (!doc.LoadContent(buffer, static_cast<int>(length))) {
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Scenes").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement()) {
		LevelInfo info;

		info.name = ObjectNode->Attribute("name");
		info.file = ObjectNode->Attribute("file");
		info.thumb = ObjectNode->Attribute("thumb");
		info.time = 999.0f;
		info.jump = 0.0f;

		m_levelInfos.push_back(info);

	}

	m_sceneFiles = sceneFilesFromLevelInfos(m_levelInfos);
	m_thumbFiles = thumbFilesFromLevelInfos(m_levelInfos);

	m_init = true;
}

void SceneInfo::loadScores(std::string path) {
	std::ifstream is(path, std::ifstream::in);

	if (!is.is_open())
		return;

	is.seekg(0, is.end);
	std::streamoff length = is.tellg();
	is.seekg(0, is.beg);

	unsigned char* buffer = new unsigned char[length];
	is.read(reinterpret_cast<char*>(buffer), length);
	is.close();

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

const std::vector<std::string> SceneInfo::sceneFilesFromLevelInfos(const std::vector<LevelInfo>& levelInfos) {
	std::vector<std::string> sceneFiles;
	std::transform(levelInfos.begin(), levelInfos.end(), std::back_inserter(sceneFiles), [](const LevelInfo& info)-> std::string { return info.file; });
	return sceneFiles;
}

const std::vector<std::string> SceneInfo::thumbFilesFromLevelInfos(const std::vector<LevelInfo>& levelInfos) {
	std::vector<std::string> thumbs;
	std::transform(levelInfos.begin(), levelInfos.end(), std::back_inserter(thumbs), [](const LevelInfo& info)-> std::string { return "Assets/Jelly/Texture/Thumbs/" + info.thumb + ".png"; });
	return thumbs;
}

std::vector<std::string> SceneInfo::mergeAlternately(std::vector<std::string> a, std::vector<std::string> b, std::vector<std::string> c, std::vector<std::string> d) {
	std::vector<std::string> result;

	auto v1 = a.begin();
	auto v2 = b.begin();
	auto v3 = c.begin();
	auto v4 = d.begin();


	while (v1 != a.end() && v2 != b.end() && v2 != c.end() && v2 != d.end())
	{
		result.push_back(*v1);
		result.push_back(*v2);
		result.push_back(*v2);
		result.push_back(*v3);
		++v1;
		++v2;
		++v3;
		++v4;
	}
	// if both the vectors have the same size we would be finished 
	/*if (v1 != a.end()) // v1 is the longer one
	{
		while (v1 != a.end())
		{
			result.push_back(*v1);
			++v1;
		}
	}
	if (v2 != b.end()) // v2 is the longer one
	{
		while (v2 != b.end())
		{
			result.push_back(*v2);
			++v2;
		}
	}*/
	return result;
}

void SceneInfo::SaveScores(const std::string path, const std::vector<LevelInfo>& levelInfos) {
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//root
	TiXmlElement * root = new TiXmlElement("Levels");
	doc.LinkEndChild(root);

	for (int i = 0; i < levelInfos.size(); i++) {
		TiXmlElement * cxn = new TiXmlElement("Level");
		root->LinkEndChild(cxn);
		cxn->SetAttribute("name", levelInfos[i].name.c_str());
		cxn->SetAttribute("id", i);
		cxn->SetDoubleAttribute("score", levelInfos[i].time);
		cxn->SetDoubleAttribute("jump", levelInfos[i].jump);
	}

	doc.SaveFile(path.c_str());
}

void  SceneInfo::SaveLevel(const std::string path, const std::vector<ObjectInfo>& objectInfos, const std::vector<LevelSoftBody*>& bodies, const Vector2& carPos, const Vector2& target, const float flallLine, const std::string levelName) {
	TiXmlDocument doc;
	//TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	//doc.LinkEndChild(decl);
	
	TiXmlElement * root = new TiXmlElement("Scene");
	root->SetAttribute("name", levelName.c_str());
	TiXmlNode* objectsNode = root->LinkEndChild(new TiXmlElement("Objects"));

	for (int i = 0; i < objectInfos.size(); i++) {
		TiXmlElement * cxn = new TiXmlElement("Object");
		objectsNode->LinkEndChild(cxn);
		cxn->SetAttribute("name", objectInfos[i].name.c_str());
		cxn->SetFloatAttribute("posX", objectInfos[i].posX);
		cxn->SetFloatAttribute("posY", objectInfos[i].posY);
		cxn->SetFloatAttribute("angle", objectInfos[i].angle);
		cxn->SetFloatAttribute("scaleX", objectInfos[i].scaleX);
		cxn->SetFloatAttribute("scaleY", objectInfos[i].scaleY);
		cxn->SetAttribute("material", objectInfos[i].material);

		if (objectInfos[i].kinematicControls.radiansPerSecond > 0.0f || objectInfos[i].kinematicControls.secondsPerLoop > 0.0f) {
			TiXmlElement * kninematicControlsNode = new TiXmlElement("KinematicControls");
			cxn->LinkEndChild(kninematicControlsNode);

			if (objectInfos[i].kinematicControls.radiansPerSecond > 0.0f) {
				TiXmlElement * motorNode = new TiXmlElement("Motor");
				kninematicControlsNode->LinkEndChild(motorNode);
				motorNode->SetFloatAttribute("radiansPerSecond", objectInfos[i].kinematicControls.radiansPerSecond);
			}

			if (objectInfos[i].kinematicControls.secondsPerLoop > 0.0f) {
				TiXmlElement * platformMotionNode = new TiXmlElement("PlatformMotion");
				kninematicControlsNode->LinkEndChild(platformMotionNode);
				platformMotionNode->SetFloatAttribute("offsetX", objectInfos[i].kinematicControls.offsetX);
				platformMotionNode->SetFloatAttribute("offsetY", objectInfos[i].kinematicControls.offsetY);
				platformMotionNode->SetFloatAttribute("secondsPerLoop", objectInfos[i].kinematicControls.secondsPerLoop);
				platformMotionNode->SetFloatAttribute("startOffset", objectInfos[i].kinematicControls.startOffset);
			}
		}
	}

	TiXmlNode* bodiesNode = root->LinkEndChild(new TiXmlElement("SoftBodies"));
	std::vector<int> indices;
	
	
	for (int i = 0; i < bodies.size(); i++) {
		TiXmlElement * cxn = new TiXmlElement("SoftBody");
		bodiesNode->LinkEndChild(cxn);

		LevelSoftBody* _body = *std::find_if(bodies.begin(), bodies.end(), [&](const LevelSoftBody* m) -> bool { return m->m_bodyInfo.name == bodies[i]->m_bodyInfo.name; });
		cxn->SetAttribute("name", _body->m_bodyInfo.name.c_str());
		cxn->SetFloatAttribute("massPerPoint", _body->m_bodyInfo.massPerPoint);
		cxn->SetFloatAttribute("edgeK", _body->m_bodyInfo.edgeK);
		cxn->SetFloatAttribute("edgeDamping", _body->m_bodyInfo.edgeDamping);
		cxn->SetFloatAttribute("colorR", _body->m_bodyInfo.colorR);
		cxn->SetFloatAttribute("colorG", _body->m_bodyInfo.colorG);
		cxn->SetFloatAttribute("colorB", _body->m_bodyInfo.colorB);
		cxn->SetAttribute("kinematic", _body->m_bodyInfo.kinematic ? "True" : "False");
		cxn->SetAttribute("shapeMatching", _body->m_bodyInfo.shapeMatching ? "True" : "False");

		cxn->SetAttribute("shapeK", _body->m_bodyInfo.shapeK);
		cxn->SetAttribute("shapeDamping", _body->m_bodyInfo.shapeDamping);

		if (_body->m_bodyInfo.pressure) {
			TiXmlElement * pressure = new TiXmlElement("Pressure");
			cxn->LinkEndChild(pressure);
			cxn->SetFloatAttribute("amount", _body->m_bodyInfo.pressure);
		}

		TiXmlElement * pointsNode= new TiXmlElement("Points");
		cxn->LinkEndChild(pointsNode);

		for (auto point : _body->m_points) {
			TiXmlElement * pointNode = new TiXmlElement("Points");
			pointsNode->LinkEndChild(pointNode);
			pointNode->SetFloatAttribute("x", point.x);
			pointNode->SetFloatAttribute("y", point.y);
			if(point.mass != -1.0)
				pointNode->SetFloatAttribute("mass", point.mass);
		}

		TiXmlElement * springsNode = new TiXmlElement("Springs");
		cxn->LinkEndChild(springsNode);

		for (auto spring : _body->m_springs) {
			TiXmlElement * springNode = new TiXmlElement("Spring");
			springsNode->LinkEndChild(springNode);
			springNode->SetAttribute("pt0", spring.pt1);
			springNode->SetAttribute("pt1", spring.pt2);
			springNode->SetFloatAttribute("k", spring.k);
			springNode->SetFloatAttribute("damp", spring.damp);
			
		}

		TiXmlElement * polygonsNode = new TiXmlElement("Polygons");
		cxn->LinkEndChild(polygonsNode);

		for (auto triangle : _body->m_triangles) {
			TiXmlElement *polygonNode = new TiXmlElement("Poly");
			polygonsNode->LinkEndChild(polygonNode);
			polygonNode->SetAttribute("pt0", triangle.pt0);
			polygonNode->SetAttribute("pt1", triangle.pt1);
			polygonNode->SetFloatAttribute("pt2", triangle.pt2);
		}
	}

	TiXmlElement* carNode = root->LinkEndChild(new TiXmlElement("Car"))->ToElement();
	carNode->SetAttribute("name", "car_and_truck");
	carNode->SetFloatAttribute("posX", carPos.X);
	carNode->SetFloatAttribute("posY", carPos.Y);

	
	TiXmlElement* settingsNode = root->LinkEndChild(new TiXmlElement("Settings"))->ToElement();
	settingsNode->SetFloatAttribute("finishX", target.X);
	settingsNode->SetFloatAttribute("finishY", target.Y);
	settingsNode->SetFloatAttribute("fallLine", flallLine);

	doc.LinkEndChild(root);
	doc.SaveFile(path.c_str());
}

void SceneInfo::LoadLevel(const std::string path) {
	std::ifstream is(path, std::ifstream::in);

	if (!is.is_open())
		return;

	is.seekg(0, is.end);
	std::streamoff length = is.tellg();
	is.seekg(0, is.beg);

	unsigned char* buffer = new unsigned char[length];
	is.read(reinterpret_cast<char*>(buffer), length);
	is.close();

	TiXmlDocument doc;
	if (!doc.LoadContent(buffer, static_cast<int>(length))) {
		return;
	}

	TiXmlElement* root = doc.FirstChild()->ToElement();
	std::cout << "Name: " << root->Attribute("name") << std::endl;
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
