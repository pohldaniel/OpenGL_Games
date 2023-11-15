#include "LevelManager.h"
#include "ObjectInfo.h"
#include "KinematicMotor.h"
#include "KinematicPlatform.h"

#include "SimpleStruct/GameObject.h"

#include "SimpleStruct/BodyPoint.h"
#include "SimpleStruct/BodySpring.h"
#include "SimpleStruct/BodyPolygon.h"

#include "tinyxml.h"

#include <_Andromeda/FileSystem/FileManager.h>

#include <_Andromeda/TextureManager.h>
#include <iostream>

std::string LevelManager::GetPathName(std::string& fileName){
	char sep = '/';

	size_t i = fileName.rfind(sep, fileName.length());
	if (i != std::string::npos)
	{
		return(fileName.substr(0, i));
	}

	return("");
}

void LevelManager::SetAssetsLocation(std::string location){
	_assetLocation = location;
}

LevelManager::~LevelManager(){
	
}

void LevelManager::InitPhysic(World *world){
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

bool LevelManager::LoadCompiledLevel(World *world, std::string levelName, std::string carFileName){
	worldLimits.clear();
	
	std::string levelFile = "";
	levelFile = levelName;
	levelFile = _assetLocation + "Scenes_compiled/" + levelFile + "c";

	BaseFile* loadFile = FileManager::Instance()->GetFile(levelFile);

	if (loadFile == 0)
		return false;

	loadFile->Open(Read, Binary);

	//object container
	std::vector<BodyObject> bodyObjects;

	//load body objects and info
	int number = 0;
	loadFile->Read(&number, sizeof(int), 1);
	m_currentName = levelName;
	//save info of each body
	for (int i = 0; i < number; i++)
	{
		BodyObject bodyObject;
		memset(&bodyObject.info, 0, sizeof(BodyObjectInfo));

		//load info struct
		loadFile->Read(&bodyObject.info, sizeof(BodyObjectInfo), 1);

		//save points
		loadFile->Read(&bodyObject.points, sizeof(int), 1);
		bodyObject.bodyPoints = new BodyPoint[bodyObject.points];
		loadFile->Read(bodyObject.bodyPoints, sizeof(BodyPoint), bodyObject.points);

		//save springs
		loadFile->Read(&bodyObject.springs, sizeof(int), 1);
		bodyObject.bodySprings = new BodySpring[bodyObject.springs];
		loadFile->Read(bodyObject.bodySprings, sizeof(BodySpring), bodyObject.springs);

		//save polygons
		loadFile->Read(&bodyObject.polygons, sizeof(int), 1);
		bodyObject.bodyPolygons = new BodyPolygon[bodyObject.polygons];
		loadFile->Read(bodyObject.bodyPolygons, sizeof(BodyPolygon), bodyObject.polygons);

		bodyObjects.push_back(bodyObject);
	}

	//object count
	int objectsCount = 0;
	loadFile->Read(&objectsCount, sizeof(int), 1);

	//objects
	GameObject* objectsArray = new GameObject[objectsCount];
	loadFile->Read(objectsArray, sizeof(GameObject), objectsCount);

	//create game level bodies
	for (int i = 0; i < objectsCount; i++)
	{
		ObjectInfo bodyInfo;
		
		//name
		bodyInfo.name = objectsArray[i].name;

		//position
		bodyInfo.posX = objectsArray[i].posX;
		bodyInfo.posY = objectsArray[i].posY;

		//angle
		bodyInfo.angle = objectsArray[i].angle;

		//scale
		bodyInfo.scaleX = objectsArray[i].scaleX;
		bodyInfo.scaleY = objectsArray[i].scaleY;

		//material
		bodyInfo.material = objectsArray[i].material;

		bodyInfo.kinematicControls.startOffset = 0.0f;
		
		if (objectsArray[i].isPlatform) {
			bodyInfo.kinematicControls.secondsPerLoop = objectsArray[i].platformSecondsPerLoop;

			bodyInfo.kinematicControls.offsetX = objectsArray[i].platformOffsetX;
			bodyInfo.kinematicControls.offsetY = objectsArray[i].platformOffsetY;
			bodyInfo.kinematicControls.startOffset = objectsArray[i].platformStartOffset;
			bodyInfo.kinematicControls.isPlatform = true;
		}else {
			bodyInfo.kinematicControls.isPlatform = false;
		}

		
		//get body info
		int bodyNumber = -1;
		for (size_t i = 0; i < bodyObjects.size(); i++)
		{
			if (bodyInfo.name.compare(bodyObjects[i].info.name) == 0)
			{
				bodyNumber = i;
				break;
			}
		}

		//create body
		LevelSoftBody * gameBody = new LevelSoftBody(&bodyObjects[bodyNumber], world, bodyInfo);

		//ballon and tire item
		if (gameBody->GetName() == "itemballoon" || gameBody->GetName() == "itemstick")
		{
			gameBody->SetVisible(false);
		}

		gameBodies.push_back(gameBody);

		Body* body = gameBody->GetBody();

		if (objectsArray[i].isPlatform)
		{
			Vector2 end(bodyInfo.posX, bodyInfo.posY);

			end.X += objectsArray[i].platformOffsetX;
			end.Y += objectsArray[i].platformOffsetY;

			float seconds = objectsArray[i].platformSecondsPerLoop;
			float offset = objectsArray[i].platformStartOffset;

			gameBody->AddKinematicControl(new KinematicPlatform(body, Vector2(bodyInfo.posX, bodyInfo.posY), end, seconds, offset));

			if (offset != 0.0f)
			{
				Vector2 newPos = Vector2(bodyInfo.posX, bodyInfo.posY).lerp(end, 0.5f + (sinf((PI / 2.0f) + (PI*2.0*offset))*0.5f));
				body->setPositionAngle(newPos, VectorTools::degToRad(bodyInfo.angle), Vector2(bodyInfo.scaleX, bodyInfo.scaleY));
			}
		}

		if (objectsArray[i].isMotor){			
			float rps = objectsArray[i].motorRadiansPerSecond;
			gameBody->AddKinematicControl(new KinematicMotor(body, rps));
			bodyInfo.kinematicControls.radiansPerSecond = rps;
			bodyInfo.kinematicControls.isMotor = true;
		}else {
			bodyInfo.kinematicControls.isMotor = false;
		}


		objectInfos.push_back(bodyInfo);
		body->updateAABB(0.0f, true);

		worldLimits.expandToInclude(body->getAABB().Min);
		worldLimits.expandToInclude(body->getAABB().Max);

		// finalize this one!
		gameBody->Finalize();
	}

    //remove loaded data
    delete[] objectsArray;

    //delete body objects
    for (size_t i = 0; i < bodyObjects.size(); i++)
    {
        delete[] bodyObjects[i].bodyPoints;
        delete[] bodyObjects[i].bodyPolygons;
        delete[] bodyObjects[i].bodySprings;
    }

	//car info
	char carName[64];
	memset(carName, 0, 64);

	loadFile->Read(carName, sizeof(char), 64);

	//car position
	loadFile->Read(&_carPos.X, sizeof(float), 1);
	loadFile->Read(&_carPos.Y, sizeof(float), 1);

	if (!carFileName.empty())
		_car = new Car(carFileName, world, _carPos, 2, 3);

	//Settings
	loadFile->Read(&finishX, sizeof(float), 1);
	loadFile->Read(&finishY, sizeof(float), 1);
	loadFile->Read(&fallLine, sizeof(float), 1);

	//very important to set this at the end...
	world->setWorldLimits(worldLimits.Min, worldLimits.Max);

	loadFile->Close();
	delete loadFile;

	return true;
}


bool LevelManager::LoadLevel(World *world, std::string levelName, std::string carFileName){
	worldLimits.clear();
	
	std::string levelFile = "";
	levelFile = levelName;
	levelFile = _assetLocation +"Scenes/" + levelFile;

	std::ifstream is(levelFile, std::ifstream::in);

	if (!is.is_open())
		return false;

	is.seekg(0, is.end);
	std::streamoff length = is.tellg();
	is.seekg(0, is.beg);

	unsigned char* buffer = new unsigned char[length];
	is.read(reinterpret_cast<char*>(buffer), length);
	is.close();
	
	//load data
	TiXmlDocument doc;
	if (!doc.LoadContent(buffer, static_cast<int>(length))) {
		return false;
	}

	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	pElem = doc.RootElement();
	// should always have a valid root but handle gracefully if it does
	if (!pElem)
	{
		//Error can't find root
		return false;
	}

	hRoot = TiXmlHandle(pElem);

	m_currentName = doc.RootElement()->Attribute("name");
	int bodyNumber = 0;
	//look for some objects ;)
	TiXmlElement* ObjectNode = hRoot.FirstChild("Objects").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement())
	{
		ObjectInfo bodyInfo;
		bodyInfo.kinematicControls.radiansPerSecond = -1.0f;
		bodyInfo.kinematicControls.secondsPerLoop = -1.0f;
		bodyInfo.kinematicControls.startOffset = 0.0f;
		//name
		bodyInfo.name = ObjectNode->Attribute("name");

		//position
		bodyInfo.posX = (float)std::stof(ObjectNode->Attribute("posX"));
		bodyInfo.posY = (float)std::stof(ObjectNode->Attribute("posY"));

		//angle
		bodyInfo.angle = (float)std::stof(ObjectNode->Attribute("angle"));

		//scale
		bodyInfo.scaleX = (float)std::stof(ObjectNode->Attribute("scaleX"));
		bodyInfo.scaleY = (float)std::stof(ObjectNode->Attribute("scaleY"));

		//material
		bodyInfo.material = 0.0f;
		if (ObjectNode->Attribute("material") != NULL)
		{
			bodyInfo.material = atoi(ObjectNode->Attribute("material"));
		}

		

		//main path to scene file
		std::string scenePath = GetPathName(levelFile);

		//Add this object
		std::string name = scenePath + "/" + bodyInfo.name + ".softbody";

		bool found = false;
		int index = 0;

		for (unsigned int i = 0; i < gameBodiesNames.size(); i++)
		{
			if (name.compare(gameBodiesNames[i]) == 0)
			{
				found = true;
				index = i;
			}
		}

		if (found == false)
		{
			gameBodiesNames.push_back(name);
			gameBodiesNumbers.push_back(bodyNumber);
		}

		LevelSoftBody * gameBody;

		if (found == false)
		{
			gameBody = new LevelSoftBody(name, world, bodyInfo);
		}
		else
		{
			gameBody = new LevelSoftBody(gameBodies[gameBodiesNumbers[index]], world, bodyInfo);
		}

		//ballon and tire item
		if (gameBody->GetName() == "itemballoon" || gameBody->GetName() == "itemstick")
		{
			gameBody->SetVisible(false);
		}

		gameBodies.push_back(gameBody);
		bodyNumber++;

		Body* body = gameBody->GetBody();

		//Add kinematic controls to the body
		TiXmlElement* KinematicElements = ObjectNode->FirstChildElement();
		for (KinematicElements; KinematicElements; KinematicElements = KinematicElements->NextSiblingElement())
		{

			TiXmlElement* Element = KinematicElements->FirstChild()->ToElement();
			while (Element != NULL)
			{
				const char* sKinematic;
				if (Element->Value() != NULL)
				{
					sKinematic = Element->Value();

					if (strcmp(sKinematic, "PlatformMotion") == 0)
					{
						Vector2 end(bodyInfo.posX, bodyInfo.posY);

						end.X += std::stof(Element->Attribute("offsetX"));
						end.Y += std::stof(Element->Attribute("offsetY"));

						bodyInfo.kinematicControls.offsetX = std::stof(Element->Attribute("offsetX"));
						bodyInfo.kinematicControls.offsetY = std::stof(Element->Attribute("offsetY"));

						float seconds = std::stof(Element->Attribute("secondsPerLoop"));
						bodyInfo.kinematicControls.secondsPerLoop = seconds;

						float offset = 0.0f;

						if (Element->Attribute("startOffset") != NULL)
						{
							offset = std::stof(Element->Attribute("startOffset"));
							bodyInfo.kinematicControls.startOffset = offset;
						}

						gameBody->AddKinematicControl(new KinematicPlatform(body, Vector2(bodyInfo.posX, bodyInfo.posY), end, seconds, offset));

						if (offset != 0.0f)
						{
							Vector2 newPos = Vector2(bodyInfo.posX, bodyInfo.posY).lerp(end, 0.5f + (sinf((PI / 2.0f) + (PI*2.0*offset))*0.5f));
							//Vector2 newPos = Vector2::Lerp(Vector2(bodyInfo.posX,bodyInfo.posY),end,0.5f + (Vector2::sinf((Vector2::PI/2.0f)+(Vector2::PI*2*offset))*0.5f));
							body->setPositionAngle(newPos, VectorTools::degToRad(bodyInfo.angle), Vector2(bodyInfo.scaleX, bodyInfo.scaleY));
						}

					}
					if (strcmp(sKinematic, "Motor") == 0)
					{
						float rps = std::stof(Element->Attribute("radiansPerSecond"));
						bodyInfo.kinematicControls.radiansPerSecond = rps;
						gameBody->AddKinematicControl(new KinematicMotor(body, rps));
					}
				}
				Element = Element->NextSiblingElement();
			}
		}
		objectInfos.push_back(bodyInfo);
		body->updateAABB(0.0f, true);

		worldLimits.expandToInclude(body->getAABB().Min);
		worldLimits.expandToInclude(body->getAABB().Max);

		// finalize this one!
		gameBody->Finalize();
	}

	//load car
	TiXmlElement* CarNode = hRoot.FirstChild("Car").Element();
	const char * carName = CarNode->Attribute("name");

	_carPos.X = std::stof(CarNode->Attribute("posX"));
	_carPos.Y = std::stof(CarNode->Attribute("posY"));

	if(!carFileName.empty())
		_car = new Car(carFileName,world, _carPos,2,3);

	//Settings
	TiXmlElement* SettingsNode = hRoot.FirstChild("Settings").Element();
	finishX = std::stof(SettingsNode->Attribute("finishX"));
	finishY = std::stof(SettingsNode->Attribute("finishY"));
	fallLine = std::stof(SettingsNode->Attribute("fallLine"));

	//very important to set this at the end...
	world->setWorldLimits(worldLimits.Min, worldLimits.Max);

	return true;
}

bool LevelManager::ClearLevel(World *world){
	world->killing();

	for (unsigned int i = 0; i < gameBodies.size(); i++)
	{
		world->removeBody(gameBodies[i]->GetBody());
	}

	for (unsigned int i = 0; i < gameBodies.size(); i++)
	{
		delete gameBodies[i];
	}

	world->removeAllBodies();

	gameBodies.clear();
	gameBodiesNames.clear();
	gameBodiesNumbers.clear();
	objectInfos.clear();
	delete _car;

	return true;
}

Car* LevelManager::GetCar(){
	return _car;
}

Vector2 LevelManager::GetCarStartPos(){
	return _carPos;
}

std::vector<LevelSoftBody*> LevelManager::GetLevelBodies(){
	return gameBodies;
}

AABB LevelManager::GetWorldLimits(){
	return worldLimits;
}

Vector2 LevelManager::GetWorldCenter(){
	float x = (((worldLimits.Max.X - worldLimits.Min.X) / 2) + worldLimits.Min.X);
	float y = (((worldLimits.Max.Y - worldLimits.Min.Y) / 2) + worldLimits.Min.Y);

	return Vector2(x, y);
}

Vector2 LevelManager::GetWorldSize(){
	float x = (worldLimits.Max.X - worldLimits.Min.X);
	float y = (worldLimits.Max.Y - worldLimits.Min.Y);

	return Vector2(x, y);
}

Vector2 LevelManager::GetLevelTarget(){
	return Vector2(finishX, finishY);
}

float LevelManager::GetLevelLine(){
	return fallLine;
}

std::vector<ObjectInfo>& LevelManager::GetObjectInfos() {
	return objectInfos;
}

void LevelManager::BuildLevel(World *world, const CarInfo& carInfo, const LevelInfo& levelInfo, const std::vector<ObjectInfo2>& objectInfos, const std::vector<SoftBodyInfo2>& softBodyInfos, const std::string& _name, const std::string& carFileName) {
	m_currentName = _name;
	int bodyNumber = 0;

	int id = 0;
	for (auto objectInfo : objectInfos){

		ObjectInfo bodyInfo;
		bodyInfo.kinematicControls.radiansPerSecond = -1.0f;
		bodyInfo.kinematicControls.secondsPerLoop = -1.0f;
		bodyInfo.kinematicControls.startOffset = 0.0f;
		//name
		bodyInfo.name = objectInfo.name;
		
		//position
		bodyInfo.posX = objectInfo.posX;
		bodyInfo.posY = objectInfo.posY;

		//angle
		bodyInfo.angle = objectInfo.angle;

		//scale
		bodyInfo.scaleX = objectInfo.scaleX;
		bodyInfo.scaleY = objectInfo.scaleY;

		//material
		bodyInfo.material = objectInfo.material;
		

		SoftBodyInfo2 _body = *std::find_if(softBodyInfos.begin(), softBodyInfos.end(), [&](const SoftBodyInfo2 m) -> bool { 
			return strcmp(m.softBodyAttributes.name, objectInfo.name) == 0;
		});
		
		LevelSoftBody* gameBody = new LevelSoftBody(softBodyInfos[id], world, bodyInfo);
		id++;

		//ballon and tire item
		if (gameBody->GetName() == "itemballoon" || gameBody->GetName() == "itemstick"){
			gameBody->SetVisible(false);
		}

		gameBodies.push_back(gameBody);
		bodyNumber++;

		Body* body = gameBody->GetBody();

		if (objectInfo.isPlatform) {
			Vector2 end(bodyInfo.posX, bodyInfo.posY);

			end.X += objectInfo.offsetX;
			end.Y += objectInfo.offsetY;
			float seconds = objectInfo.secondsPerLoop;
			float offset = objectInfo.startOffset;

			gameBody->AddKinematicControl(new KinematicPlatform(body, Vector2(bodyInfo.posX, bodyInfo.posY), end, seconds, offset));

			if (offset != 0.0f){
				Vector2 newPos = Vector2(bodyInfo.posX, bodyInfo.posY).lerp(end, 0.5f + (sinf((PI / 2.0f) + (PI*2.0*offset))*0.5f));
				body->setPositionAngle(newPos, VectorTools::degToRad(bodyInfo.angle), Vector2(bodyInfo.scaleX, bodyInfo.scaleY));
			}
		}

		if(objectInfo.isMotor) {
			float rps = objectInfo.radiansPerSecond;
			gameBody->AddKinematicControl(new KinematicMotor(body, rps));
		}	

		body->updateAABB(0.0f, true);

		worldLimits.expandToInclude(body->getAABB().Min);
		worldLimits.expandToInclude(body->getAABB().Max);

		// finalize this one!
		gameBody->Finalize();
	}

	//load car
	_carPos.X = carInfo.posX;
	_carPos.Y = carInfo.posY;

	if (!carFileName.empty())
		_car = new Car(carFileName, world, _carPos, 2, 3);

	//Settings

	finishX = levelInfo.finishX;
	finishY = levelInfo.finishY;
	fallLine = levelInfo.fallLine;

	//very important to set this at the end...
	world->setWorldLimits(worldLimits.Min, worldLimits.Max);
}