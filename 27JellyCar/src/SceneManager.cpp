#include <tinyxml.h>
#include <iostream>
#include "SceneManager.h"

#include "Levels/LevelSoftBody.h"
#include "Levels/KinematicPlatform.h"
#include "Levels/KinematicMotor.h"

Scene::Scene() :
	m_currentPosition(0),
	m_carCurrentPosition(0),
	m_init(false),
	m_useCompiledLevel(false){
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

void Scene::configure(const std::string scenesPath, bool useCompiledLevel) {
	m_scenesPath = scenesPath;
	m_useCompiledLevel = useCompiledLevel;
}

const std::string& Scene::getScenesPath() const {
	return m_scenesPath;
}

const std::vector<ObjectInfo>& Scene::getObjectInfos() const {
	return m_objectInfos;
}

const std::vector<SoftBodyInfo>& Scene::getSoftBodyInfos() const {
	return m_softBodyInfos;
}

const CarInfo& Scene::getCarInfo() const {
	return m_carInfo;
}

const LevelInfo& Scene::getLevelInfo() const {
	return m_levelInfo;
}

const Vector2 Scene::getWorldCenter() const {
	float x = (((m_worldLimits.Max.X - m_worldLimits.Min.X) / 2) + m_worldLimits.Min.X);
	float y = (((m_worldLimits.Max.Y - m_worldLimits.Min.Y) / 2) + m_worldLimits.Min.Y);

	return Vector2(x, y);
}

const Vector2 Scene::getWorldSize() const {
	float x = (m_worldLimits.Max.X - m_worldLimits.Min.X);
	float y = (m_worldLimits.Max.Y - m_worldLimits.Min.Y);

	return Vector2(x, y);
}

const AABB Scene::getWorldLimits() const {
	return m_worldLimits;
}

const Vector2 Scene::getLevelTarget() const {
	return Vector2(m_levelInfo.finishX, m_levelInfo.finishY);
}

const float Scene::getLevelLine() const {
	return m_levelInfo.fallLine;
}

Vector2 Scene::getCarStartPos() {
	return  Vector2(m_carInfo.posX, m_carInfo.posY);
}

void Scene::loadCarSkins(std::string path) {
	if (m_init) return;
	
	TiXmlDocument doc(path.c_str());
	if (!doc.LoadFile()) {
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
	
	TiXmlDocument doc(path.c_str());
	if (!doc.LoadFile()) {
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
	
	TiXmlDocument doc(path.c_str());
	if (!doc.LoadFile()) {
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Levels").FirstChild().Element();
	
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement()) {

		std::string nameAtr = ObjectNode->Attribute("name");
		std::vector<SceneInfo>::iterator it = find_if(m_sceneInfos.begin(), m_sceneInfos.end(), [&](const SceneInfo& s) { return s.name.compare(nameAtr) == 0; });

		if (it != m_sceneInfos.end()) {
			(*it).time = std::stof(ObjectNode->Attribute("score"));
			(*it).jump = std::stof(ObjectNode->Attribute("jump"));
		}
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

void Scene::buildLevel(World *world, std::vector<LevelSoftBody*>& gameBodies) {
	m_worldLimits.clear();

	for (auto objectInfo : m_objectInfos) {

		SoftBodyInfo softBody = *std::find_if(m_softBodyInfos.begin(), m_softBodyInfos.end(), [&](const SoftBodyInfo m) -> bool {
			return strcmp(m.softBodyAttributes.name, objectInfo.name) == 0;
		});
		LevelSoftBody* gameBody = new LevelSoftBody(softBody, world, objectInfo);

		//ballon and tire item
		if (gameBody->GetName() == "itemballoon" || gameBody->GetName() == "itemstick") {
			gameBody->SetVisible(false);
		}

		gameBodies.push_back(gameBody);

		Body* body = gameBody->GetBody();

		if (objectInfo.isPlatform) {
			Vector2 end(objectInfo.posX, objectInfo.posY);

			end.X += objectInfo.offsetX;
			end.Y += objectInfo.offsetY;
			float seconds = objectInfo.secondsPerLoop;
			float offset = objectInfo.startOffset;

			gameBody->AddKinematicControl(new KinematicPlatform(body, Vector2(objectInfo.posX, objectInfo.posY), end, seconds, offset));

			if (offset != 0.0f) {
				Vector2 newPos = Vector2(objectInfo.posX, objectInfo.posY).lerp(end, 0.5f + (sinf((PI / 2.0f) + (PI*2.0*offset))*0.5f));
				body->setPositionAngle(newPos, VectorTools::degToRad(objectInfo.angle), Vector2(objectInfo.scaleX, objectInfo.scaleY));
			}
		}

		if (objectInfo.isMotor) {
			float rps = objectInfo.radiansPerSecond;
			gameBody->AddKinematicControl(new KinematicMotor(body, rps));
		}

		body->updateAABB(0.0f, true);

		m_worldLimits.expandToInclude(body->getAABB().Min);
		m_worldLimits.expandToInclude(body->getAABB().Max);

		// finalize this one!
		gameBody->Finalize();
	}

	//very important to set this at the end...
	world->setWorldLimits(m_worldLimits.Min, m_worldLimits.Max);
}

void Scene::buildCar(World *world, Car*& car, const std::string& carFileName) {
	car = new Car(carFileName, world, Vector2(m_carInfo.posX, m_carInfo.posY), 2, 3);
}

void Scene::loadLevel(const std::string path) {
	if (m_useCompiledLevel)
		loadCompiledLevel(m_scenesPath + path);
	else
		loadXmlLevel(m_scenesPath + path);
}

void Scene::loadXmlLevel(const std::string path) {
	m_objectInfos.shrink_to_fit();
	m_objectInfos.clear();
	m_softBodyInfos.shrink_to_fit();
	m_softBodyInfos.clear();

	TiXmlDocument doc(path.c_str());
	if (!doc.LoadFile()) {
		return;
	}

	TiXmlElement* root = doc.FirstChild()->ToElement();
	TiXmlHandle hRoot = TiXmlHandle(root);

	m_name = root->Attribute("name");

	int bodyNumber = 0;

	TiXmlElement* objectNode = hRoot.FirstChild("Objects").FirstChild().Element();
	for (objectNode; objectNode; objectNode = objectNode->NextSiblingElement()){
		ObjectInfo objectInfo;
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
		SoftBodyInfo softBodyInfo;
		
		if (softBodyNode->Attribute("name") != NULL){
			std::strncpy(softBodyInfo.softBodyAttributes.name, softBodyNode->Attribute("name"), sizeof(softBodyInfo.softBodyAttributes.name));
		}

		if (softBodyNode->Attribute("colorR") != NULL){
			softBodyInfo.softBodyAttributes.colorR = std::stof(softBodyNode->Attribute("colorR"));
			softBodyInfo.softBodyAttributes.colorG = std::stof(softBodyNode->Attribute("colorG"));
			softBodyInfo.softBodyAttributes.colorB = std::stof(softBodyNode->Attribute("colorB"));
		}

		softBodyInfo.softBodyAttributes.massPerPoint = std::stof(softBodyNode->Attribute("massPerPoint"));
		softBodyInfo.softBodyAttributes.edgeK = std::stof(softBodyNode->Attribute("edgeK"));
		softBodyInfo.softBodyAttributes.edgeDamping = std::stof(softBodyNode->Attribute("edgeDamping"));

		if (softBodyNode->Attribute("kinematic") != NULL){
			const char* sKinematic = softBodyNode->Attribute("kinematic");
			if (strcmp(sKinematic, "True") == 0)
				softBodyInfo.softBodyAttributes.isKinematic = true;
			else
				softBodyInfo.softBodyAttributes.isKinematic = false;
		}

		//shape matching
		if (softBodyNode->Attribute("shapeMatching") != NULL){
			const char* sShapeMatching = softBodyNode->Attribute("shapeMatching");
			if (strcmp(sShapeMatching, "True") == 0){
				softBodyInfo.softBodyAttributes.shapeMatching = true;
				softBodyInfo.softBodyAttributes.shapeK = std::stof(softBodyNode->Attribute("shapeK"));
				softBodyInfo.softBodyAttributes.shapeDamping = std::stof(softBodyNode->Attribute("shapeDamping"));
			}else {
				softBodyInfo.softBodyAttributes.shapeMatching = false;
			}
		}

		//shape matching
		if (softBodyNode->Attribute("velDamping") != NULL){
			softBodyInfo.softBodyAttributes.velDamping = std::stof(softBodyNode->Attribute("velDamping"));
		}

		//pressure
		TiXmlElement* pressureNode = softBodyNode->FirstChild("Pressure") != nullptr ? softBodyNode->FirstChild("Pressure")->ToElement() : nullptr;
		if (pressureNode){
			softBodyInfo.softBodyAttributes.pressureized = true;
			softBodyInfo.softBodyAttributes.pressure = std::stof(pressureNode->Attribute("amount"));
		}else {
			softBodyInfo.softBodyAttributes.pressureized = false;
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

void Scene::loadCompiledLevel(const std::string path) {
	m_softBodyInfos.clear();
	m_objectInfos.clear();
	
	std::ifstream stream(path, std::ios::binary);

	int softBodyCount = 0;
	stream.read(reinterpret_cast<char*>(&softBodyCount), sizeof(int));

	for (int i = 0; i < softBodyCount; i++){

		SoftBodyInfo softBodyInfo;
		memset(&softBodyInfo.softBodyAttributes, 0, sizeof(SoftBodyAttributes));
		stream.read(reinterpret_cast<char*>(&softBodyInfo.softBodyAttributes), sizeof(SoftBodyAttributes));

		stream.read(reinterpret_cast<char*>(&softBodyInfo.pointCount), sizeof(int));
		softBodyInfo.points.resize(softBodyInfo.pointCount);

		stream.read(reinterpret_cast<char*>(&softBodyInfo.points[0]), sizeof(Point) * softBodyInfo.pointCount);

		stream.read(reinterpret_cast<char*>(&softBodyInfo.springCount), sizeof(int));
		softBodyInfo.springs.resize(softBodyInfo.springCount);
		stream.read(reinterpret_cast<char*>(&softBodyInfo.springs[0]), sizeof(Spring) * softBodyInfo.springCount);


		stream.read(reinterpret_cast<char*>(&softBodyInfo.polygonCount), sizeof(int));
		softBodyInfo.polygons.resize(softBodyInfo.polygonCount);
		stream.read(reinterpret_cast<char*>(&softBodyInfo.polygons[0]), sizeof(Triangle) * softBodyInfo.polygonCount);

		m_softBodyInfos.push_back(softBodyInfo);
	}
	
	int objectCount = 0;
	stream.read(reinterpret_cast<char*>(&objectCount), sizeof(int));
	m_objectInfos.resize(objectCount);
	stream.read(reinterpret_cast<char*>(&m_objectInfos[0]), sizeof(ObjectInfo) * objectCount);

	stream.read(m_carInfo.name, sizeof(char) * 64);
	stream.read(reinterpret_cast<char*>(&m_carInfo.posX), sizeof(float));
	stream.read(reinterpret_cast<char*>(&m_carInfo.posY), sizeof(float));

	stream.read(reinterpret_cast<char*>(&m_levelInfo.finishX), sizeof(float));
	stream.read(reinterpret_cast<char*>(&m_levelInfo.finishY), sizeof(float));
	stream.read(reinterpret_cast<char*>(&m_levelInfo.fallLine), sizeof(float));

	stream.close();
}

void Scene::loadOriginLevel(const std::string path) {	
	m_objectInfos.shrink_to_fit();
	m_objectInfos.clear();
	m_softBodyInfos.shrink_to_fit();
	m_softBodyInfos.clear();

	std::set<std::string> softBodyFiles;

	TiXmlDocument doc(path.c_str());
	if (!doc.LoadFile()) {
		return;
	}

	TiXmlElement* root = doc.FirstChild()->ToElement();
	TiXmlHandle hRoot = TiXmlHandle(root);

	m_name = root->Attribute("name");

	int bodyNumber = 0;

	TiXmlElement* objectNode = hRoot.FirstChild("Objects").FirstChild().Element();
	for (objectNode; objectNode; objectNode = objectNode->NextSiblingElement()) {
		ObjectInfo objectInfo;
		std::strncpy(objectInfo.name, objectNode->Attribute("name"), sizeof(objectInfo.name));
		softBodyFiles.insert(objectInfo.name);

		objectInfo.posX = std::stof(objectNode->Attribute("posX"));
		objectInfo.posY = std::stof(objectNode->Attribute("posY"));
		objectInfo.angle = std::stof(objectNode->Attribute("angle"));
		objectInfo.scaleX = std::stof(objectNode->Attribute("scaleX"));
		objectInfo.scaleY = std::stof(objectNode->Attribute("scaleY"));
		objectInfo.material = 0;
		if (objectNode->Attribute("material") != NULL) {
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
			}
			else {
				objectInfo.isMotor = true;
				objectInfo.radiansPerSecond = std::stof(Element->Attribute("radiansPerSecond"));
			}
		}
		else {
			objectInfo.isPlatform = false;
			objectInfo.isMotor = false;
		}
		m_objectInfos.push_back(objectInfo);
	}

	TiXmlElement* carNode = hRoot.FirstChild("Car").Element();
	std::strncpy(m_carInfo.name, carNode->Attribute("name"), sizeof(m_carInfo.name));
	m_carInfo.posX = std::stof(carNode->Attribute("posX"));
	m_carInfo.posY = std::stof(carNode->Attribute("posY"));

	TiXmlElement* levelNode = hRoot.FirstChild("Settings").Element();
	m_levelInfo.finishX = std::stof(levelNode->Attribute("finishX"));
	m_levelInfo.finishY = std::stof(levelNode->Attribute("finishY"));
	m_levelInfo.fallLine = std::stof(levelNode->Attribute("fallLine"));
	 
	for (auto& softBodyFile : softBodyFiles) {
		SoftBodyInfo softBodyInfo;
		
		TiXmlDocument doc(std::string("Assets/Jelly/Scenes/" + softBodyFile + ".softbody").c_str());
		if (!doc.LoadFile()) {
			continue;
		}

		TiXmlElement* softBodyNode = doc.FirstChild()->ToElement();
		TiXmlHandle hRoot = TiXmlHandle(softBodyNode);

		std::strncpy(softBodyInfo.softBodyAttributes.name, softBodyNode->Attribute("name"), sizeof(softBodyInfo.softBodyAttributes.name));

		if (softBodyNode->Attribute("colorR") != NULL) {
			softBodyInfo.softBodyAttributes.colorR = std::stof(softBodyNode->Attribute("colorR"));
			softBodyInfo.softBodyAttributes.colorG = std::stof(softBodyNode->Attribute("colorG"));
			softBodyInfo.softBodyAttributes.colorB = std::stof(softBodyNode->Attribute("colorB"));
		}else {
			softBodyInfo.softBodyAttributes.colorR = 0.0f;
			softBodyInfo.softBodyAttributes.colorG = 0.0f;
			softBodyInfo.softBodyAttributes.colorB = 0.0f;
		}

		softBodyInfo.softBodyAttributes.massPerPoint = std::stof(softBodyNode->Attribute("massPerPoint"));
		softBodyInfo.softBodyAttributes.edgeK = std::stof(softBodyNode->Attribute("edgeK"));
		softBodyInfo.softBodyAttributes.edgeDamping = std::stof(softBodyNode->Attribute("edgeDamping"));

		if (softBodyNode->Attribute("kinematic") != NULL) {
			const char* sKinematic = softBodyNode->Attribute("kinematic");
			if (strcmp(sKinematic, "True") == 0)
				softBodyInfo.softBodyAttributes.isKinematic = true;
			else
				softBodyInfo.softBodyAttributes.isKinematic = false;
		}

		//shape matching
		if (softBodyNode->Attribute("shapeMatching") != NULL) {
			const char* sShapeMatching = softBodyNode->Attribute("shapeMatching");
			if (strcmp(sShapeMatching, "True") == 0) {
				softBodyInfo.softBodyAttributes.shapeMatching = true;
				softBodyInfo.softBodyAttributes.shapeK = std::stof(softBodyNode->Attribute("shapeK"));
				softBodyInfo.softBodyAttributes.shapeDamping = std::stof(softBodyNode->Attribute("shapeDamping"));
			}
			else {
				softBodyInfo.softBodyAttributes.shapeMatching = false;
				softBodyInfo.softBodyAttributes.shapeK = 100.0f;
				softBodyInfo.softBodyAttributes.shapeDamping = 10.0f;
			}
		}

		//shape matching
		if (softBodyNode->Attribute("velDamping") != NULL) {
			softBodyInfo.softBodyAttributes.velDamping = std::stof(softBodyNode->Attribute("velDamping"));
		}else{
			softBodyInfo.softBodyAttributes.velDamping = 0.993f;
		}

		//pressure
		TiXmlElement* pressureNode = softBodyNode->FirstChild("Pressure") != nullptr ? softBodyNode->FirstChild("Pressure")->ToElement() : nullptr;
		if (pressureNode) {
			softBodyInfo.softBodyAttributes.pressureized = true;
			softBodyInfo.softBodyAttributes.pressure = std::stof(pressureNode->Attribute("amount"));
		}else {
			softBodyInfo.softBodyAttributes.pressureized = false;
			softBodyInfo.softBodyAttributes.pressure = 0.0f;
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
}

void Scene::saveCompiledLevel(const std::string path) {
	
	std::ofstream stream(path, std::ios::binary);

	int softBodyCount = m_softBodyInfos.size();
	stream.write(reinterpret_cast<char*>(&softBodyCount), sizeof(int));
	for (int i = 0; i < softBodyCount; i++) {
		
		stream.write(reinterpret_cast<char*>(&m_softBodyInfos[i].softBodyAttributes), sizeof(SoftBodyAttributes));
		stream.write(reinterpret_cast<char*>(&m_softBodyInfos[i].pointCount), sizeof(int));
		m_softBodyInfos[i].points.resize(m_softBodyInfos[i].pointCount);
		stream.write(reinterpret_cast<char*>(&m_softBodyInfos[i].points[0]), sizeof(Point) * m_softBodyInfos[i].pointCount);

		stream.write(reinterpret_cast<char*>(&m_softBodyInfos[i].springCount), sizeof(int));
		m_softBodyInfos[i].springs.resize(m_softBodyInfos[i].springCount);
		stream.write(reinterpret_cast<char*>(&m_softBodyInfos[i].springs[0]), sizeof(Spring) * m_softBodyInfos[i].springCount);

		stream.write(reinterpret_cast<char*>(&m_softBodyInfos[i].polygonCount), sizeof(int));
		m_softBodyInfos[i].polygons.resize(m_softBodyInfos[i].polygonCount);
		stream.write(reinterpret_cast<char*>(&m_softBodyInfos[i].polygons[0]), sizeof(Triangle) * m_softBodyInfos[i].polygonCount);		
	}

	int objectCount = m_objectInfos.size();
	stream.write(reinterpret_cast<char*>(&objectCount), sizeof(int));
	m_objectInfos.resize(objectCount);
	stream.write(reinterpret_cast<char*>(&m_objectInfos[0]), sizeof(ObjectInfo) * objectCount);

	stream.write(m_carInfo.name, sizeof(char) * 64);
	stream.write(reinterpret_cast<char*>(&m_carInfo.posX), sizeof(float));
	stream.write(reinterpret_cast<char*>(&m_carInfo.posY), sizeof(float));

	stream.write(reinterpret_cast<char*>(&m_levelInfo.finishX), sizeof(float));
	stream.write(reinterpret_cast<char*>(&m_levelInfo.finishY), sizeof(float));
	stream.write(reinterpret_cast<char*>(&m_levelInfo.fallLine), sizeof(float));

	stream.close();
}

void Scene::saveLevel(const std::string path) {
	TiXmlDocument doc;
	TiXmlElement * root = new TiXmlElement("Scene");
	root->SetAttribute("name", m_name.c_str());
	TiXmlNode* objectsNode = root->LinkEndChild(new TiXmlElement("Objects"));

	for (int i = 0; i < m_objectInfos.size(); i++) {
		TiXmlElement * objectNode = new TiXmlElement("Object");
		objectsNode->LinkEndChild(objectNode);
		objectNode->SetAttribute("name", m_objectInfos[i].name);
		objectNode->SetFloatAttribute("posX", m_objectInfos[i].posX);
		objectNode->SetFloatAttribute("posY", m_objectInfos[i].posY);
		objectNode->SetFloatAttribute("angle", m_objectInfos[i].angle);
		objectNode->SetFloatAttribute("scaleX", m_objectInfos[i].scaleX);
		objectNode->SetFloatAttribute("scaleY", m_objectInfos[i].scaleY);
		objectNode->SetAttribute("material", m_objectInfos[i].material);

		if (m_objectInfos[i].isMotor || m_objectInfos[i].isPlatform) {
			TiXmlElement * kninematicControlsNode = new TiXmlElement("KinematicControls");
			objectNode->LinkEndChild(kninematicControlsNode);
			if (m_objectInfos[i].isMotor) {
				TiXmlElement * motorNode = new TiXmlElement("Motor");
				kninematicControlsNode->LinkEndChild(motorNode);
				motorNode->SetFloatAttribute("radiansPerSecond", m_objectInfos[i].radiansPerSecond);
			}

			if (m_objectInfos[i].isPlatform) {
				TiXmlElement * platformMotionNode = new TiXmlElement("PlatformMotion");
				kninematicControlsNode->LinkEndChild(platformMotionNode);
				platformMotionNode->SetFloatAttribute("offsetX", m_objectInfos[i].offsetX);
				platformMotionNode->SetFloatAttribute("offsetY", m_objectInfos[i].offsetY);
				platformMotionNode->SetFloatAttribute("secondsPerLoop", m_objectInfos[i].secondsPerLoop);
				platformMotionNode->SetFloatAttribute("startOffset", m_objectInfos[i].startOffset);
			}
		}
	}

	TiXmlNode* softBodiesNode = root->LinkEndChild(new TiXmlElement("SoftBodies"));

	for (int i = 0; i < m_softBodyInfos.size(); i++) {
		TiXmlElement * softBody = new TiXmlElement("SoftBody");
		softBodiesNode->LinkEndChild(softBody);
		softBody->SetAttribute("name", m_softBodyInfos[i].softBodyAttributes.name);
		softBody->SetFloatAttribute("massPerPoint", m_softBodyInfos[i].softBodyAttributes.massPerPoint);
		softBody->SetFloatAttribute("edgeK", m_softBodyInfos[i].softBodyAttributes.edgeK);
		softBody->SetFloatAttribute("edgeDamping", m_softBodyInfos[i].softBodyAttributes.edgeDamping);
		softBody->SetFloatAttribute("colorR", m_softBodyInfos[i].softBodyAttributes.colorR);
		softBody->SetFloatAttribute("colorG", m_softBodyInfos[i].softBodyAttributes.colorG);
		softBody->SetFloatAttribute("colorB", m_softBodyInfos[i].softBodyAttributes.colorB);
		softBody->SetAttribute("kinematic", m_softBodyInfos[i].softBodyAttributes.isKinematic ? "True" : "False");
		softBody->SetAttribute("shapeMatching", m_softBodyInfos[i].softBodyAttributes.shapeMatching ? "True" : "False");
		softBody->SetAttribute("shapeK", m_softBodyInfos[i].softBodyAttributes.shapeK);
		softBody->SetAttribute("shapeDamping", m_softBodyInfos[i].softBodyAttributes.shapeDamping);
		softBody->SetFloatAttribute("velDamping", m_softBodyInfos[i].softBodyAttributes.velDamping);

		if (m_softBodyInfos[i].softBodyAttributes.pressureized) {
			TiXmlElement * pressure = new TiXmlElement("Pressure");
			softBody->LinkEndChild(pressure);
			pressure->SetFloatAttribute("amount", m_softBodyInfos[i].softBodyAttributes.pressure);
		}

		TiXmlElement * pointsNode = new TiXmlElement("Points");
		softBody->LinkEndChild(pointsNode);

		for (auto point : m_softBodyInfos[i].points) {
			TiXmlElement * pointNode = new TiXmlElement("Point");
			pointsNode->LinkEndChild(pointNode);
			pointNode->SetFloatAttribute("x", point.x);
			pointNode->SetFloatAttribute("y", point.y);
			pointNode->SetFloatAttribute("mass", point.mass);
		}

		TiXmlElement * springsNode = new TiXmlElement("Springs");
		softBody->LinkEndChild(springsNode);

		for (auto spring : m_softBodyInfos[i].springs) {
			TiXmlElement * springNode = new TiXmlElement("Spring");
			springsNode->LinkEndChild(springNode);
			springNode->SetAttribute("pt1", spring.pt1);
			springNode->SetAttribute("pt2", spring.pt2);
			springNode->SetFloatAttribute("k", spring.k);
			springNode->SetFloatAttribute("damp", spring.damp);
		}

		TiXmlElement * polygonsNode = new TiXmlElement("Polygons");
		softBody->LinkEndChild(polygonsNode);

		for (auto triangle : m_softBodyInfos[i].polygons) {
			TiXmlElement *polygonNode = new TiXmlElement("Poly");
			polygonsNode->LinkEndChild(polygonNode);
			polygonNode->SetAttribute("pt0", triangle.pt0);
			polygonNode->SetAttribute("pt1", triangle.pt1);
			polygonNode->SetFloatAttribute("pt2", triangle.pt2);
		}
	}

	TiXmlElement* carNode = root->LinkEndChild(new TiXmlElement("Car"))->ToElement();
	carNode->SetAttribute("name", m_carInfo.name);
	carNode->SetFloatAttribute("posX", m_carInfo.posX);
	carNode->SetFloatAttribute("posY", m_carInfo.posY);


	TiXmlElement* settingsNode = root->LinkEndChild(new TiXmlElement("Settings"))->ToElement();
	settingsNode->SetFloatAttribute("finishX", m_levelInfo.finishX);
	settingsNode->SetFloatAttribute("finishY", m_levelInfo.finishY);
	settingsNode->SetFloatAttribute("fallLine", m_levelInfo.fallLine);

	doc.LinkEndChild(root);
	doc.SaveFile(path.c_str());
}

void Scene::RemoveDuplicates(std::vector<LevelSoftBody*> &vector) {
	for (std::vector<LevelSoftBody*>::iterator itei = vector.begin(); itei != vector.end(); itei++) {
		LevelSoftBody* ch = *itei;
		for (std::vector<LevelSoftBody*>::iterator itej = itei + 1; itej != vector.end(); itej++) {
			if (ch->GetName().compare((*itej)->GetName()) == 0) {
				vector.erase(itej);
				--itei;
				break;
			}
		}
	}
}

void Scene::SaveLevel(const std::string path, const std::vector<ObjectInfo>& objectInfos, std::vector<LevelSoftBody*>& bodies, const Vector2& carPos, const Vector2& target, const float fallLine, const std::string levelName) {

	RemoveDuplicates(bodies);

	TiXmlDocument doc;
	TiXmlElement * root = new TiXmlElement("Scene");
	root->SetAttribute("name", levelName.c_str());
	TiXmlNode* objectsNode = root->LinkEndChild(new TiXmlElement("Objects"));

	for (int i = 0; i < objectInfos.size(); i++) {
		TiXmlElement * objectNode = new TiXmlElement("Object");
		objectsNode->LinkEndChild(objectNode);
		objectNode->SetAttribute("name", objectInfos[i].name);
		objectNode->SetFloatAttribute("posX", objectInfos[i].posX);
		objectNode->SetFloatAttribute("posY", objectInfos[i].posY);
		objectNode->SetFloatAttribute("angle", objectInfos[i].angle);
		objectNode->SetFloatAttribute("scaleX", objectInfos[i].scaleX);
		objectNode->SetFloatAttribute("scaleY", objectInfos[i].scaleY);
		objectNode->SetAttribute("material", objectInfos[i].material);

		if (objectInfos[i].isMotor || objectInfos[i].isPlatform) {
			TiXmlElement * kninematicControlsNode = new TiXmlElement("KinematicControls");
			objectNode->LinkEndChild(kninematicControlsNode);
			if (objectInfos[i].isMotor) {
				TiXmlElement * motorNode = new TiXmlElement("Motor");
				kninematicControlsNode->LinkEndChild(motorNode);
				motorNode->SetFloatAttribute("radiansPerSecond", objectInfos[i].radiansPerSecond);
			}

			if (objectInfos[i].isPlatform) {
				TiXmlElement * platformMotionNode = new TiXmlElement("PlatformMotion");
				kninematicControlsNode->LinkEndChild(platformMotionNode);
				platformMotionNode->SetFloatAttribute("offsetX", objectInfos[i].offsetX);
				platformMotionNode->SetFloatAttribute("offsetY", objectInfos[i].offsetY);
				platformMotionNode->SetFloatAttribute("secondsPerLoop", objectInfos[i].secondsPerLoop);
				platformMotionNode->SetFloatAttribute("startOffset", objectInfos[i].startOffset);
			}
		}
	}

	TiXmlNode* softBodiesNode = root->LinkEndChild(new TiXmlElement("SoftBodies"));

	for (int i = 0; i < bodies.size(); i++) {
		TiXmlElement * softBody = new TiXmlElement("SoftBody");
		softBodiesNode->LinkEndChild(softBody);

		LevelSoftBody* body = *std::find_if(bodies.begin(), bodies.end(), [&](const LevelSoftBody* m) -> bool { return m->GetName().compare(bodies[i]->GetName()) == 0; });
		softBody->SetAttribute("name", body->m_objectInfo.name);
		softBody->SetFloatAttribute("massPerPoint", body->massPerPoint);
		softBody->SetFloatAttribute("edgeK", body->edgeK);
		softBody->SetFloatAttribute("edgeDamping", body->edgeDamping);
		softBody->SetFloatAttribute("colorR", body->colorR);
		softBody->SetFloatAttribute("colorG", body->colorG);
		softBody->SetFloatAttribute("colorB", body->colorB);
		softBody->SetAttribute("kinematic", body->isKinematic ? "True" : "False");
		softBody->SetAttribute("shapeMatching", body->shapeMatching ? "True" : "False");
		softBody->SetAttribute("shapeK", body->shapeK);
		softBody->SetAttribute("shapeDamping", body->shapeDamping);
		softBody->SetFloatAttribute("velDamping", body->velDamping);

		if (body->pressureized) {
			TiXmlElement * pressure = new TiXmlElement("Pressure");
			softBody->LinkEndChild(pressure);
			pressure->SetFloatAttribute("amount", body->pressure);
		}

		TiXmlElement * pointsNode = new TiXmlElement("Points");
		softBody->LinkEndChild(pointsNode);

		for (auto point : body->m_softBodyInfo.points) {
			TiXmlElement * pointNode = new TiXmlElement("Point");
			pointsNode->LinkEndChild(pointNode);
			pointNode->SetFloatAttribute("x", point.x);
			pointNode->SetFloatAttribute("y", point.y);
			pointNode->SetFloatAttribute("mass", point.mass);
		}

		TiXmlElement * springsNode = new TiXmlElement("Springs");
		softBody->LinkEndChild(springsNode);

		for (auto spring : body->m_softBodyInfo.springs) {
			TiXmlElement * springNode = new TiXmlElement("Spring");
			springsNode->LinkEndChild(springNode);
			springNode->SetAttribute("pt1", spring.pt1);
			springNode->SetAttribute("pt2", spring.pt2);
			springNode->SetFloatAttribute("k", spring.k);
			springNode->SetFloatAttribute("damp", spring.damp);
		}

		TiXmlElement * polygonsNode = new TiXmlElement("Polygons");
		softBody->LinkEndChild(polygonsNode);

		for (auto triangle : body->m_softBodyInfo.polygons) {
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
	settingsNode->SetFloatAttribute("fallLine", fallLine);

	doc.LinkEndChild(root);
	doc.SaveFile(path.c_str());
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
		cxn->SetFloatAttribute("score", levelInfos[i].time);
		cxn->SetFloatAttribute("jump", levelInfos[i].jump);
	}

	doc.SaveFile(path.c_str());
}

void Scene::InitPhysic(World *world) {
	//add materials
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();
	world->addMaterial();

	// car tires don't collide with the chassis.
	world->setMaterialPairCollide(2, 3, false);

	//??
	world->setMaterialPairCollide(3, 3, false);

	//
	world->setMaterialPairCollide(0, 2, true);
	world->setMaterialPairCollide(0, 3, true);

	// car tires have a lot of friction against the ground.
	world->setMaterialPairData(0, 3, 1.0f, 0.0f);
	world->setMaterialPairData(1, 3, 1.0f, 0.0f);

	// ground cannot collide with itself, ever.
	world->setMaterialPairCollide(0, 0, false);

	// default ground vs, dynamic object settings.
	world->setMaterialPairData(0, 1, 0.4f, 1.0f);

	// default car tire vs ice.
	world->setMaterialPairData(3, 4, 0.0f, 0.0f);

	// default car chassis vs. ice
	world->setMaterialPairData(2, 4, 0.0f, 0.0f);

	// np collision between ice and other grounb objects.
	world->setMaterialPairCollide(0, 4, false);

	// no collision between ice and ice.
	world->setMaterialPairCollide(4, 4, false);

	// default friction for objects and ice.
	world->setMaterialPairData(1, 4, 0.0f, 0.0f);

	// no collision between items and other objects
	world->setMaterialPairCollide(0, 5, false);
	world->setMaterialPairCollide(1, 5, false);
	world->setMaterialPairCollide(2, 5, true);
	world->setMaterialPairCollide(3, 5, true);
	world->setMaterialPairCollide(4, 5, false);
	world->setMaterialPairCollide(5, 5, false);
	world->setMaterialPairCollide(6, 5, false);

	world->setMaterialPairCollide(0, 7, true);
	world->setMaterialPairCollide(7, 7, true);

	world->setMaterialPairCollide(2, 7, false);
	world->setMaterialPairCollide(3, 7, false);

	//no collision between ballon and car
	world->setMaterialPairCollide(2, 6, false);
	world->setMaterialPairCollide(3, 6, false);
}

void Scene::ClearLevel(World *world, std::vector<LevelSoftBody*> bodies, Car* car) {
	world->killing();

	for (unsigned int i = 0; i < bodies.size(); i++){
		world->removeBody(bodies[i]->GetBody());
	}

	for (unsigned int i = 0; i < bodies.size(); i++){
		delete bodies[i];
	}

	world->removeAllBodies();
	bodies.clear();
	delete world;
	if(car)
		delete car;
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
