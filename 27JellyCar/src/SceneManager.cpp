#include <tinyxml.h>
#include <iostream>
#include "SceneManager.h"
#include "Levels/LevelManager.h"

Scene::Scene() :
	m_currentPosition(0),
	m_carCurrentPosition(0),
	m_init(false){
}

const std::vector<SkinInfo>& Scene::getSkinInfos() const {
	return m_carSkins;
}

const std::vector<std::string>& Scene::getSceneFiles() const {
	return m_sceneFiles;
}

const std::vector<std::string>& Scene::getThumbFiles() const {
	return m_thumbFiles;
}

const std::vector<SceneInfo>& Scene::getSceneInfos() const {
	return m_sceneInfos;
}

const SkinInfo& Scene::getCurrentSkinInfo() const {
	return m_carSkins[m_carCurrentPosition];
}

const std::string& Scene::getCurrentSceneFile() const {
	return m_sceneFiles[m_currentPosition];
}

const SceneInfo& Scene::getCurrentSceneInfo() const {
	return m_sceneInfos[m_currentPosition];
}

const std::string& Scene::getName() const {
	return m_name;
}

void Scene::loadCarSkins(std::string path) {
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

void Scene::loadSceneInfo(std::string path) {
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
		SceneInfo info;

		info.name = ObjectNode->Attribute("name");
		info.file = ObjectNode->Attribute("file");
		info.thumb = ObjectNode->Attribute("thumb");
		info.time = 999.0f;
		info.jump = 0.0f;

		m_sceneInfos.push_back(info);

	}

	m_sceneFiles = sceneFilesFromLevelInfos(m_sceneInfos);
	m_thumbFiles = thumbFilesFromLevelInfos(m_sceneInfos);

	m_init = true;
}

void Scene::loadScores(std::string path) {
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
		m_sceneInfos[index].time = std::stof(ObjectNode->Attribute("score"));
		m_sceneInfos[index].jump = std::stof(ObjectNode->Attribute("jump"));
	}
}

float Scene::getTime(std::string levelName){
	for (size_t i = 0; i < m_sceneInfos.size(); i++){
		if (m_sceneInfos[i].name == levelName){
			return m_sceneInfos[i].time;
		}
	}

	return 999.0f;
}

float Scene::getJump(std::string levelName){
	for (size_t i = 0; i < m_sceneInfos.size(); i++){
		if (m_sceneInfos[i].name == levelName){
			return m_sceneInfos[i].jump;
		}
	}

	return 0.0f;
}

void Scene::setTime(std::string levelName, float time){
	for (size_t i = 0; i < m_sceneInfos.size(); i++){
		if (m_sceneInfos[i].name == levelName){
			m_sceneInfos[i].time = time;
			return;
		}
	}
}

void Scene::setJump(std::string levelName, float jump){
	for (size_t i = 0; i < m_sceneInfos.size(); i++){
		if (m_sceneInfos[i].name == levelName){
			m_sceneInfos[i].jump = jump;
			return;
		}
	}
}

const std::vector<std::string> Scene::sceneFilesFromLevelInfos(const std::vector<SceneInfo>& levelInfos) {
	std::vector<std::string> sceneFiles;
	std::transform(levelInfos.begin(), levelInfos.end(), std::back_inserter(sceneFiles), [](const SceneInfo& info)-> std::string { return info.file; });
	return sceneFiles;
}

const std::vector<std::string> Scene::thumbFilesFromLevelInfos(const std::vector<SceneInfo>& levelInfos) {
	std::vector<std::string> thumbs;
	std::transform(levelInfos.begin(), levelInfos.end(), std::back_inserter(thumbs), [](const SceneInfo& info)-> std::string { return "Assets/Jelly/Texture/Thumbs/" + info.thumb + ".png"; });
	return thumbs;
}

std::vector<std::string> Scene::mergeAlternately(std::vector<std::string> a, std::vector<std::string> b, std::vector<std::string> c, std::vector<std::string> d) {
	std::vector<std::string> result;

	auto v1 = a.begin();
	auto v2 = b.begin();
	auto v3 = c.begin();
	auto v4 = d.begin();


	while (v1 != a.end() && v2 != b.end() && v2 != c.end() && v2 != d.end()){
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

void Scene::SaveScores(const std::string path, const std::vector<SceneInfo>& levelInfos) {
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

void  Scene::SaveLevel(const std::string path, const std::vector<ObjectInfo>& objectInfos, const std::vector<LevelSoftBody*>& bodies, const Vector2& carPos, const Vector2& target, const float flallLine, const std::string levelName) {
	TiXmlDocument doc;
	//TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	//doc.LinkEndChild(decl);
	TiXmlElement * root = new TiXmlElement("Scene");
	root->SetAttribute("name", levelName.c_str());
	TiXmlNode* objectsNode = root->LinkEndChild(new TiXmlElement("Objects"));

	for (int i = 0; i < objectInfos.size(); i++) {
		TiXmlElement * objectNode = new TiXmlElement("Object");
		objectsNode->LinkEndChild(objectNode);
		objectNode->SetAttribute("name", objectInfos[i].name.c_str());
		objectNode->SetFloatAttribute("posX", objectInfos[i].posX);
		objectNode->SetFloatAttribute("posY", objectInfos[i].posY);
		objectNode->SetFloatAttribute("angle", objectInfos[i].angle);
		objectNode->SetFloatAttribute("scaleX", objectInfos[i].scaleX);
		objectNode->SetFloatAttribute("scaleY", objectInfos[i].scaleY);
		objectNode->SetAttribute("material", objectInfos[i].material);

		if (objectInfos[i].kinematicControls.isMotor || objectInfos[i].kinematicControls.isPlatform) {
			TiXmlElement * kninematicControlsNode = new TiXmlElement("KinematicControls");
			objectNode->LinkEndChild(kninematicControlsNode);
			if (objectInfos[i].kinematicControls.isMotor) {
				TiXmlElement * motorNode = new TiXmlElement("Motor");
				kninematicControlsNode->LinkEndChild(motorNode);
				motorNode->SetFloatAttribute("radiansPerSecond", objectInfos[i].kinematicControls.radiansPerSecond);
			}

			if (objectInfos[i].kinematicControls.isPlatform) {
				TiXmlElement * platformMotionNode = new TiXmlElement("PlatformMotion");
				kninematicControlsNode->LinkEndChild(platformMotionNode);
				platformMotionNode->SetFloatAttribute("offsetX", objectInfos[i].kinematicControls.offsetX);
				platformMotionNode->SetFloatAttribute("offsetY", objectInfos[i].kinematicControls.offsetY);
				platformMotionNode->SetFloatAttribute("secondsPerLoop", objectInfos[i].kinematicControls.secondsPerLoop);
				platformMotionNode->SetFloatAttribute("startOffset", objectInfos[i].kinematicControls.startOffset);
			}
		}
	}

	TiXmlNode* softBodiesNode = root->LinkEndChild(new TiXmlElement("SoftBodies"));
	std::vector<int> indices;
	
	
	for (int i = 0; i < bodies.size(); i++) {
		TiXmlElement * softBody = new TiXmlElement("SoftBody");
		softBodiesNode->LinkEndChild(softBody);

		LevelSoftBody* _body = *std::find_if(bodies.begin(), bodies.end(), [&](const LevelSoftBody* m) -> bool { return m->m_bodyInfo.name == bodies[i]->m_bodyInfo.name; });
		softBody->SetAttribute("name", _body->m_bodyInfo.name.c_str());
		softBody->SetFloatAttribute("massPerPoint", _body->m_bodyInfo.massPerPoint);
		softBody->SetFloatAttribute("edgeK", _body->m_bodyInfo.edgeK);
		softBody->SetFloatAttribute("edgeDamping", _body->m_bodyInfo.edgeDamping);
		softBody->SetFloatAttribute("colorR", _body->m_bodyInfo.colorR);
		softBody->SetFloatAttribute("colorG", _body->m_bodyInfo.colorG);
		softBody->SetFloatAttribute("colorB", _body->m_bodyInfo.colorB);
		softBody->SetAttribute("kinematic", _body->m_bodyInfo.kinematic ? "True" : "False");
		softBody->SetAttribute("shapeMatching", _body->m_bodyInfo.shapeMatching ? "True" : "False");

		softBody->SetAttribute("shapeK", _body->m_bodyInfo.shapeK);
		softBody->SetAttribute("shapeDamping", _body->m_bodyInfo.shapeDamping);

		if (_body->m_bodyInfo.pressureized) {
			TiXmlElement * pressure = new TiXmlElement("Pressure");
			softBody->LinkEndChild(pressure);
			pressure->SetFloatAttribute("amount", _body->m_bodyInfo.pressure);
		}

		TiXmlElement * pointsNode= new TiXmlElement("Points");
		softBody->LinkEndChild(pointsNode);

		for (auto point : _body->m_points) {
			TiXmlElement * pointNode = new TiXmlElement("Point");
			pointsNode->LinkEndChild(pointNode);
			pointNode->SetFloatAttribute("x", point.x);
			pointNode->SetFloatAttribute("y", point.y);
			if(point.mass != -1.0)
				pointNode->SetFloatAttribute("mass", point.mass);
		}

		TiXmlElement * springsNode = new TiXmlElement("Springs");
		softBody->LinkEndChild(springsNode);

		for (auto spring : _body->m_springs) {
			TiXmlElement * springNode = new TiXmlElement("Spring");
			springsNode->LinkEndChild(springNode);
			springNode->SetAttribute("pt1", spring.pt1);
			springNode->SetAttribute("pt2", spring.pt2);
			springNode->SetFloatAttribute("k", spring.k);
			springNode->SetFloatAttribute("damp", spring.damp);
			
		}

		TiXmlElement * polygonsNode = new TiXmlElement("Polygons");
		softBody->LinkEndChild(polygonsNode);

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

void Scene::loadLevel(const std::string path) {
	m_objectInfos.shrink_to_fit();
	m_objectInfos.clear();
	m_softBodyInfos.shrink_to_fit();
	m_softBodyInfos.clear();

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
	TiXmlHandle hRoot = TiXmlHandle(root);

	m_name = root->Attribute("name");

	int bodyNumber = 0;

	TiXmlElement* objectNode = hRoot.FirstChild("Objects").FirstChild().Element();
	for (objectNode; objectNode; objectNode = objectNode->NextSiblingElement()){
		ObjectInfo2 objectInfo;
		std::strncpy(objectInfo.name, objectNode->Attribute("name"), sizeof(objectInfo.name));

		objectInfo.posX = std::stof(objectNode->Attribute("posX"));
		objectInfo.posY = std::stof(objectNode->Attribute("posY"));
		objectInfo.angle = std::stof(objectNode->Attribute("angle"));
		objectInfo.scaleX = std::stof(objectNode->Attribute("scaleX"));
		objectInfo.scaleY = std::stof(objectNode->Attribute("scaleY"));
		objectInfo.material = 0;
		if (objectNode->Attribute("material") != NULL){
			objectInfo.material = atoi(objectNode->Attribute("material"));
		}
		TiXmlElement* kinematicControls = objectNode->FirstChildElement();
		if (kinematicControls) {
			TiXmlElement* Element = kinematicControls->FirstChild()->ToElement();
			if (strcmp(Element->Value(), "PlatformMotion") == 0) {			
				objectInfo.isPlatform = true;
				objectInfo.offsetX = std::stof(Element->Attribute("offsetX"));
				objectInfo.offsetY = std::stof(Element->Attribute("offsetY"));
				objectInfo.secondsPerLoop = std::stof(Element->Attribute("secondsPerLoop"));	
				objectInfo.startOffset = std::stof(Element->Attribute("startOffset"));
			}else {
				objectInfo.isMotor = true;
				objectInfo.radiansPerSecond = std::stof(Element->Attribute("radiansPerSecond"));
			}
		}else {
			objectInfo.isPlatform = false;
			objectInfo.isMotor = false;
		}
		m_objectInfos.push_back(objectInfo);		
	}

	TiXmlElement* softBodyNode = hRoot.FirstChild("SoftBodies").FirstChild().Element();
	for (softBodyNode; softBodyNode; softBodyNode = softBodyNode->NextSiblingElement()) {
		SoftBodyInfo2 softBodyInfo;
		
		if (softBodyNode->Attribute("name") != NULL){
			std::strncpy(softBodyInfo.name, softBodyNode->Attribute("name"), sizeof(softBodyInfo.name));
		}

		if (softBodyNode->Attribute("colorR") != NULL){
			softBodyInfo.colorR = std::stof(softBodyNode->Attribute("colorR"));
			softBodyInfo.colorG = std::stof(softBodyNode->Attribute("colorG"));
			softBodyInfo.colorB = std::stof(softBodyNode->Attribute("colorB"));
		}

		softBodyInfo.massPerPoint = std::stof(softBodyNode->Attribute("massPerPoint"));
		softBodyInfo.edgeK = std::stof(softBodyNode->Attribute("edgeK"));
		softBodyInfo.edgeDamping = std::stof(softBodyNode->Attribute("edgeDamping"));

		if (softBodyNode->Attribute("kinematic") != NULL){
			const char* sKinematic = softBodyNode->Attribute("kinematic");
			if (strcmp(sKinematic, "True") == 0)
				softBodyInfo.isKinematic = true;
			else
				softBodyInfo.isKinematic = false;
		}

		//shape matching
		if (softBodyNode->Attribute("shapeMatching") != NULL){
			const char* sShapeMatching = softBodyNode->Attribute("shapeMatching");
			if (strcmp(sShapeMatching, "True") == 0){
				softBodyInfo.shapeMatching = true;
				softBodyInfo.shapeK = std::stof(softBodyNode->Attribute("shapeK"));
				softBodyInfo.shapeDamping = std::stof(softBodyNode->Attribute("shapeDamping"));
			}else {
				softBodyInfo.shapeMatching = false;
			}
		}

		//shape matching
		if (softBodyNode->Attribute("velDamping") != NULL){
			softBodyInfo.velDamping = std::stof(softBodyNode->Attribute("velDamping"));
		}

		//pressure
		TiXmlElement* pressureNode = softBodyNode->FirstChild("Pressure") != nullptr ? softBodyNode->FirstChild("Pressure")->ToElement() : nullptr;
		if (pressureNode){
			softBodyInfo.pressureized = true;
			softBodyInfo.pressure = std::stof(pressureNode->Attribute("amount"));
		}else {
			softBodyInfo.pressureized = false;
		}

		TiXmlElement* pointNode = softBodyNode->FirstChild("Points")->FirstChild() != nullptr ? softBodyNode->FirstChild("Points")->FirstChild()->ToElement() : nullptr;
		if (pointNode) {
			for (pointNode; pointNode; pointNode = pointNode->NextSiblingElement()) {
				softBodyInfo.points.push_back({ std::stof(pointNode->Attribute("x")), std::stof(pointNode->Attribute("y")), pointNode->Attribute("mass") != NULL ? std::stof(pointNode->Attribute("mass")) : -1.0f });
			}
		}

		TiXmlElement* spingNode = softBodyNode->FirstChild("Springs")->FirstChild() != nullptr ? softBodyNode->FirstChild("Springs")->FirstChild()->ToElement() : nullptr;
		if (spingNode) {
			for (spingNode; spingNode; spingNode = spingNode->NextSiblingElement()) {
				softBodyInfo.springs.push_back({ atoi(spingNode->Attribute("pt1")), atoi(spingNode->Attribute("pt2")), std::stof(spingNode->Attribute("k")), std::stof(spingNode->Attribute("damp")) });
			}
		}

		TiXmlElement* polygonNode = softBodyNode->FirstChild("Polygons")->FirstChild() != nullptr ? softBodyNode->FirstChild("Polygons")->FirstChild()->ToElement() : nullptr;
		if (polygonNode) {
			for (polygonNode; polygonNode; polygonNode = polygonNode->NextSiblingElement()) {
				softBodyInfo.polygons.push_back({ atoi(polygonNode->Attribute("pt0")) , atoi(polygonNode->Attribute("pt1")), atoi(polygonNode->Attribute("pt2")) });
			}
		}

		m_softBodyInfos.push_back(softBodyInfo);
	}

	TiXmlElement* carNode = hRoot.FirstChild("Car").Element();
	std::strncpy(m_carInfo.name, carNode->Attribute("name"), sizeof(m_carInfo.name));
	m_carInfo.posX = std::stof(carNode->Attribute("posX"));
	m_carInfo.posY = std::stof(carNode->Attribute("posY"));

	TiXmlElement* levelNode = hRoot.FirstChild("Settings").Element();
	m_levelInfo.finishX = std::stof(levelNode->Attribute("finishX"));
	m_levelInfo.finishY = std::stof(levelNode->Attribute("finishY"));
	m_levelInfo.fallLine = std::stof(levelNode->Attribute("fallLine"));
}

const std::vector<ObjectInfo2>& Scene::getObjectInfos() const {
	return m_objectInfos;
}

const std::vector<SoftBodyInfo2>& Scene::getSoftBodyInfos() const {
	return m_softBodyInfos;
}

const CarInfo& Scene::getCarInfo() const {
	return m_carInfo;
}

const LevelInfo& Scene::getLevelInfo() const {
	return m_levelInfo;
}

///////////////////////ScenetManager//////////////////////////
SceneManager SceneManager::s_instance;

Scene& SceneManager::getScene(std::string name) {
	return m_sceneInfos[name];
}

bool SceneManager::containsScene(std::string name) {
	return m_sceneInfos.count(name) == 1;
}

SceneManager& SceneManager::Get() {
	return s_instance;
}
