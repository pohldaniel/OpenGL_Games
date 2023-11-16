#include "LevelSoftBody.h"
#include "GameSpringBody.h"
#include "GamePressureBody.h"

#include "tinyxml.h"
#include <string>
#include <iostream>

#include <_Andromeda/FileSystem/FileManager.h>
#include "SceneManager.h"

LevelSoftBody::LevelSoftBody(std::string fileName, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material) {

	massPerPoint = 0.0f;
	edgeK = 100.0f;
	edgeDamping = 1.0f;
	isKinematic = false;
	shapeMatching = false;
	shapeK = 100.0f;
	shapeDamping = 10.0f;
	pressureized = false;
	pressure = 0.0f;
	_visible = true;

	JellyPhysics::ClosedShape shape;

	//loac main level file
	BaseFile* file = FileManager::Instance()->GetFile(fileName);

	if (file == 0)
		return;

	file->Open(Read, Binary);

	int dataSize = 0;
	unsigned char* _buffer = file->GetData(dataSize);

	file->Close();
	delete file;

	//load object
	TiXmlDocument doc;
	if (!doc.LoadContent(_buffer, dataSize))
	{
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
	std::string name;
	pElem = hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem)
	{
		//Error - Can't find root :/
		return;
	}


	colorR = colorG = colorB = 0.0f;
	const char* sKinematic, *sShapeMatching;
	//bool kinematic,shapeMatching;

	//load color of the body
	if (pElem->Attribute("name") != NULL)
	{
		name = pElem->Attribute("name");
	}

	//basic info
	massPerPoint = std::stof(pElem->Attribute("massPerPoint"));
	edgeK = std::stof(pElem->Attribute("edgeK"));
	edgeDamping = std::stof(pElem->Attribute("edgeDamping"));

	//load color of the body
	if (pElem->Attribute("colorR") != NULL)
	{
		colorR = std::stof(pElem->Attribute("colorR"));
		colorG = std::stof(pElem->Attribute("colorG"));
		colorB = std::stof(pElem->Attribute("colorB"));
	}

	//is body kinematic
	if (pElem->Attribute("kinematic") != NULL)
	{
		sKinematic = pElem->Attribute("kinematic");
		if (strcmp(sKinematic, "True") == 0)
			isKinematic = true;
		else
			isKinematic = false;
	}

	//shape matching
	if (pElem->Attribute("shapeMatching") != NULL)
	{
		sShapeMatching = pElem->Attribute("shapeMatching");
		if (strcmp(sShapeMatching, "True") == 0)
		{
			shapeMatching = true;
			shapeK = std::stof(pElem->Attribute("shapeK"));
			shapeDamping = std::stof(pElem->Attribute("shapeDamping"));
		}
	}

	//shape matching
	if (pElem->Attribute("velDamping") != NULL)
	{
		velDamping = std::stof(pElem->Attribute("velDamping"));
	}

	//end of first section
	hRoot = TiXmlHandle(pElem);

	//pressure
	pElem = hRoot.FirstChild("Pressure").Element();
	if (pElem){
		pressureized = true;
		pressure = std::stof(pElem->Attribute("amount"));
	}

	strcpy(m_objectInfo.name, name.c_str());
	//SHAPE

	//Points
	shape.begin();

	int _id = 0;
	TiXmlElement* PointNode = hRoot.FirstChild("Points").FirstChild().Element();
	for (PointNode; PointNode; PointNode = PointNode->NextSiblingElement()){
		float x = 0.0f, y = 0.0f;
		x = std::stof(PointNode->Attribute("x"));
		y = std::stof(PointNode->Attribute("y"));
		shape.addVertex(Vector2(x, y));

		if (PointNode->Attribute("mass") != NULL){
			MassID m;
			m._id = _id;
			m._mass = std::stof(PointNode->Attribute("mass"));

			massExceptions.push_back(m);
		}
		_id++;
	}

	//shape->finish(false);
	shape.finish(((massPerPoint != 0.0f) && (!isKinematic)));

	if (!pressureized){
		mBody = new GameSpringBody(mWorld, shape, massPerPoint, edgeK, edgeDamping, pos, angle, scale, isKinematic);
	}else{
		mBody = new GamePressureBody(mWorld, shape, massPerPoint, pressure, shapeK, shapeDamping, edgeK, edgeDamping, pos, angle, scale, isKinematic);
	}

	mBody->setMaterial(material);
	mBody->setVelocityDamping(0.993f);

	if (shapeMatching && (!pressureized)){
		static_cast<GameSpringBody*>(mBody)->setShapeMatching(true);
		static_cast<GameSpringBody*>(mBody)->setShapeMatchingConstants(shapeK, shapeDamping);
	}

	// were there any mass exceptions?
	for (unsigned int i = 0; i < massExceptions.size(); i++){
		mBody->setMassIndividual(massExceptions[i]._id, massExceptions[i]._mass);
	}

	//Springs
	int springCount = 0;
	TiXmlElement* SpingNode = hRoot.FirstChild("Springs").FirstChild().Element();
	for (SpingNode; SpingNode; SpingNode = SpingNode->NextSiblingElement()){
		int pt1, pt2;
		float k, damp;
		pt1 = atoi(SpingNode->Attribute("pt1"));
		pt2 = atoi(SpingNode->Attribute("pt2"));
		k = std::stof(SpingNode->Attribute("k"));
		damp = std::stof(SpingNode->Attribute("damp"));



		if (!pressureized){
			static_cast<GameSpringBody*>(mBody)->addInternalSpring(pt1, pt2, k, damp);
		}else{
			static_cast<GamePressureBody*>(mBody)->addInternalSpring(pt1, pt2, k, damp);
		}
	}

	//Polygons
	TiXmlElement* PolygonNode = hRoot.FirstChild("Polygons").FirstChild().Element();
	for (PolygonNode; PolygonNode; PolygonNode = PolygonNode->NextSiblingElement()){
		int pt0 = 0, pt1 = 0, pt2 = 0;
		pt0 = atoi(PolygonNode->Attribute("pt0"));
		pt1 = atoi(PolygonNode->Attribute("pt1"));
		pt2 = atoi(PolygonNode->Attribute("pt2"));
		// polygons!
		AddTriangle(pt0, pt1, pt2);
	}

	FinalizeTriangles();
}

LevelSoftBody::LevelSoftBody(const SoftBodyInfo& softBodyInfo, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material) {
	
	massPerPoint = 0.0f;
	edgeK = 100.0f;
	edgeDamping = 1.0f;
	isKinematic = false;
	shapeMatching = false;
	shapeK = 100.0f;
	shapeDamping = 10.0f;
	pressureized = false;
	pressure = 0.0f;
	_visible = true;

	JellyPhysics::ClosedShape shape;

	colorR = colorG = colorB = 0.0f;

	strcpy(m_objectInfo.name, softBodyInfo.softBodyAttributes.name);
	massPerPoint = softBodyInfo.softBodyAttributes.massPerPoint;
	edgeK = softBodyInfo.softBodyAttributes.edgeK;
	edgeDamping = softBodyInfo.softBodyAttributes.edgeDamping;

	colorR = softBodyInfo.softBodyAttributes.colorR;
	colorG = softBodyInfo.softBodyAttributes.colorG;
	colorB = softBodyInfo.softBodyAttributes.colorB;

	isKinematic = softBodyInfo.softBodyAttributes.isKinematic;
	shapeMatching = softBodyInfo.softBodyAttributes.shapeMatching;
	shapeK = softBodyInfo.softBodyAttributes.shapeK;
	shapeDamping = softBodyInfo.softBodyAttributes.shapeDamping;
	velDamping = softBodyInfo.softBodyAttributes.velDamping;
	
	pressureized = softBodyInfo.softBodyAttributes.pressureized;
	pressure = softBodyInfo.softBodyAttributes.pressure;

	int _id = 0;
	for (auto& point: softBodyInfo.points){
		float x = 0.0f, y = 0.0f;
		x = point.x;
		y = point.y;
		
		shape.addVertex(Vector2(x, y));

		if (point.mass != -1.0f){
			MassID m;
			m._id = _id;
			m._mass = point.mass;
			massExceptions.push_back(m);
		}
		_id++;
	}

	shape.finish(((massPerPoint != 0.0f) && (!isKinematic)));

	if (!pressureized){
		mBody = new GameSpringBody(mWorld, shape, massPerPoint, edgeK, edgeDamping, pos, angle, scale, isKinematic);
	}else{
		mBody = new GamePressureBody(mWorld, shape, massPerPoint, pressure, shapeK, shapeDamping, edgeK, edgeDamping, pos, angle, scale, isKinematic);
	}

	mBody->setMaterial(material);
	mBody->setVelocityDamping(softBodyInfo.softBodyAttributes.velDamping);

	if (shapeMatching && (!pressureized)){
		static_cast<GameSpringBody*>(mBody)->setShapeMatching(true);
		static_cast<GameSpringBody*>(mBody)->setShapeMatchingConstants(shapeK, shapeDamping);
	}

	// were there any mass exceptions?
	for (unsigned int i = 0; i < massExceptions.size(); i++){
		mBody->setMassIndividual(massExceptions[i]._id, massExceptions[i]._mass);
	}

	//Springs
	int springCount = 0;
	for (auto& spring : softBodyInfo.springs){
		int pt1, pt2;
		float k, damp;
		pt1 = spring.pt1;
		pt2 = spring.pt2;
		k = spring.k;
		damp = spring.damp;

		if (!pressureized){
			static_cast<GameSpringBody*>(mBody)->addInternalSpring(pt1, pt2, k, damp);
		}else{
			static_cast<GamePressureBody*>(mBody)->addInternalSpring(pt1, pt2, k, damp);
		}
	}

	//Polygons
	for (auto& triangle : softBodyInfo.polygons){
		int pt0 = 0, pt1 = 0, pt2 = 0;
		pt0 = triangle.pt0;
		pt1 = triangle.pt1;
		pt2 = triangle.pt2;
		AddTriangle(pt0, pt1, pt2);
	}

	FinalizeTriangles();
}

LevelSoftBody::LevelSoftBody(LevelSoftBody *exBody, World *mWorld, const Vector2& pos, float angle, const Vector2& scale, int material) {
	massPerPoint = 0.0f;
	edgeK = 100.0f;
	edgeDamping = 1.0f;
	isKinematic = false;
	shapeMatching = false;
	shapeK = 100.0f;
	shapeDamping = 10.0f;
	pressureized = false;
	pressure = 0.0f;
	_visible = true;

	JellyPhysics::ClosedShape shape;

	colorR = exBody->colorR;
	colorG = exBody->colorG;
	colorB = exBody->colorB;

	//basic info
	massPerPoint = exBody->massPerPoint;
	edgeK = exBody->edgeK;
	edgeDamping = exBody->edgeDamping;

	//is body kinematic
	isKinematic = exBody->isKinematic;

	//shape matching
	shapeMatching = exBody->shapeMatching;
	shapeK = exBody->shapeK;
	shapeDamping = exBody->shapeDamping;

	//pressure
	pressureized = exBody->pressureized;
	pressure = exBody->pressure;

	//SHAPE
	//Points
	shape.begin();

	for (unsigned int i = 0; i < exBody->mBody->mBaseShape.getVertices().size(); i++){
		shape.addVertex(exBody->mBody->mBaseShape.getVertices()[i]);
	}

	//shape->finish(false);
	shape.finish(((massPerPoint != 0.0f) && (!isKinematic)));

	if (!pressureized){
		mBody = new GameSpringBody(mWorld, shape, massPerPoint, edgeK, edgeDamping, pos, angle, scale, isKinematic);
	}else{
		mBody = new GamePressureBody(mWorld, shape, massPerPoint, pressure, shapeK, shapeDamping, edgeK, edgeDamping, pos, angle, scale, isKinematic);
	}

	mBody->setMaterial(exBody->mBody->getMaterial());
	mBody->setVelocityDamping(0.993f);

	if (shapeMatching && (!pressureized)){
		((GameSpringBody*)mBody)->setShapeMatching(true);
		((GameSpringBody*)mBody)->setShapeMatchingConstants(shapeK, shapeDamping);
	}

	// were there any mass exceptions?
	for (unsigned int i = 0; i < exBody->massExceptions.size(); i++){
		mBody->setMassIndividual(exBody->massExceptions[i]._id, exBody->massExceptions[i]._mass);
	}

	//Springs
	if (!pressureized){
		for (unsigned int i = 0; i < exBody->mBody->mSprings.size(); i++){
			((GameSpringBody*)mBody)->addInternalSpring(exBody->mBody->mSprings[i].pointMassA, exBody->mBody->mSprings[i].pointMassB, exBody->mBody->mSprings[i].springK, exBody->mBody->mSprings[i].damping);
		}
	}else{
		for (unsigned int i = 0; i < exBody->mBody->mSprings.size(); i++){
			((GamePressureBody*)mBody)->addInternalSpring(exBody->mBody->mSprings[i].pointMassA, exBody->mBody->mSprings[i].pointMassB, exBody->mBody->mSprings[i].springK, exBody->mBody->mSprings[i].damping);
		}
	}

	//Polygons
	for (unsigned int i = 0; i < exBody->mIndexList.size(); i++){
		mIndexList.push_back(exBody->mIndexList[i]);
	}
	
	FinalizeTriangles();
}

LevelSoftBody::LevelSoftBody(SoftBodyInfo* exBody, World* mWorld, const Vector2& pos, float angle, const Vector2& scale, int material) {
	massPerPoint = 0.0f;
	edgeK = 100.0f;
	edgeDamping = 1.0f;
	isKinematic = false;
	shapeMatching = false;
	shapeK = 100.0f;
	shapeDamping = 10.0f;
	pressureized = false;
	pressure = 0.0f;

	_visible = true;

	JellyPhysics::ClosedShape shape;

	colorR = exBody->softBodyAttributes.colorR;
	colorG = exBody->softBodyAttributes.colorG;
	colorB = exBody->softBodyAttributes.colorB;

	//basic info
	massPerPoint = exBody->softBodyAttributes.massPerPoint;
	edgeK = exBody->softBodyAttributes.edgeK;
	edgeDamping = exBody->softBodyAttributes.edgeDamping;

	//is body kinematic
	isKinematic = exBody->softBodyAttributes.isKinematic;

	//shape matching
	shapeMatching = exBody->softBodyAttributes.shapeMatching;
	shapeK = exBody->softBodyAttributes.shapeK;
	shapeDamping = exBody->softBodyAttributes.shapeDamping;

	//pressure
	pressureized = exBody->softBodyAttributes.pressureized;
	pressure = exBody->softBodyAttributes.pressure;

	//Points
	shape.begin();
	int _id = 0;

	for (int i = 0; i < exBody->pointCount; i++){
		shape.addVertex(Vector2(exBody->points[i].x, exBody->points[i].y));
		if (exBody->points[i].mass != -1){
			MassID m;
			m._id = _id;
			m._mass = exBody->points[i].mass;
			massExceptions.push_back(m);
		}

		_id++;
	}

	//shape->finish(false);
	shape.finish(((massPerPoint != 0.0f) && (!isKinematic)));

	if (!pressureized){
		mBody = new GameSpringBody(mWorld, shape, massPerPoint, edgeK, edgeDamping, pos, angle, scale, isKinematic);
	}else{
		mBody = new GamePressureBody(mWorld, shape, massPerPoint, pressure, shapeK, shapeDamping, edgeK, edgeDamping, pos, angle, scale, isKinematic);
	}

	mBody->setMaterial(material);
	mBody->setVelocityDamping(exBody->softBodyAttributes.velDamping);

	if (shapeMatching && (!pressureized)){
		static_cast<GameSpringBody*>(mBody)->setShapeMatching(true);
		static_cast<GameSpringBody*>(mBody)->setShapeMatchingConstants(shapeK, shapeDamping);
	}

	// were there any mass exceptions?
	for (unsigned int i = 0; i < massExceptions.size(); i++){
		mBody->setMassIndividual(massExceptions[i]._id, massExceptions[i]._mass);
	}

	//Springs
	for (int i = 0; i < exBody->springCount; i++){
		int pt1, pt2;
		float k, damp;

		pt1 = exBody->springs[i].pt1;
		pt2 = exBody->springs[i].pt2;
		k = exBody->springs[i].k;
		damp = exBody->springs[i].damp;

		if (!pressureized){
			static_cast<GameSpringBody*>(mBody)->addInternalSpring(pt1, pt2, k, damp);
		}else{
			static_cast<GamePressureBody*>(mBody)->addInternalSpring(pt1, pt2, k, damp);
		}
	}

	//polygons
	for (int i = 0; i <  exBody->pointCount; i++){
		int pt0 = 0, pt1 = 0, pt2 = 0;

		pt0 = exBody->polygons[i].pt0;
		pt1 = exBody->polygons[i].pt1;
		pt2 = exBody->polygons[i].pt2;
		AddTriangle(pt0, pt1, pt2);
	}

	FinalizeTriangles();
}

LevelSoftBody::LevelSoftBody(std::string fileName, World *mWorld, ObjectInfo objectInfo) : LevelSoftBody(fileName, mWorld, Vector2(objectInfo.posX, objectInfo.posY), VectorTools::degToRad(objectInfo.angle), Vector2(objectInfo.scaleX, objectInfo.scaleY), objectInfo.material) {
	m_objectInfo = objectInfo;
	mBody->SetName(m_objectInfo.name);
}

LevelSoftBody::LevelSoftBody(const SoftBodyInfo& softBodyInfo, World *mWorld, ObjectInfo objectInfo) : LevelSoftBody(softBodyInfo, mWorld, Vector2(objectInfo.posX, objectInfo.posY), VectorTools::degToRad(objectInfo.angle), Vector2(objectInfo.scaleX, objectInfo.scaleY), objectInfo.material) {
	m_objectInfo = objectInfo;
	mBody->SetName(softBodyInfo.softBodyAttributes.name);
}

LevelSoftBody::LevelSoftBody(LevelSoftBody *exBody, World *mWorld, ObjectInfo objectInfo) : LevelSoftBody(exBody, mWorld, Vector2(objectInfo.posX, objectInfo.posY), VectorTools::degToRad(objectInfo.angle), Vector2(objectInfo.scaleX, objectInfo.scaleY), objectInfo.material) {
	m_objectInfo = objectInfo;
	mBody->SetName(m_objectInfo.name);
}

LevelSoftBody::~LevelSoftBody() {
	massExceptions.clear();

	for (unsigned int i = 0; i < mControls.size(); i++)
		delete mControls[i];

	mControls.clear();

	delete[] mIndices;
	mIndexList.clear();

	delete mBody;
}

void LevelSoftBody::AddKinematicControl(KinematicControl* kinematicControl)
{
	mControls.push_back(kinematicControl);
}

void LevelSoftBody::Finalize()
{
	// generate the "ignoreMe" AABB.
	if (mBody->getIsKinematic())
	{
		Vector2 originalPos = mBody->getDerivedPosition();
		float originalAngle = mBody->getDerivedAngle();

		mIgnoreAABB.clear();

		float maxTime = 1.0f;
		for (ControlList::iterator it = mControls.begin(); it != mControls.end(); it++)
		{
			if ((*it)->GetPeriod() > maxTime) { maxTime = (*it)->GetPeriod(); }
		}

		float step = maxTime / 10.0f;

		mBody->updateAABB(0.0f, true);
		mIgnoreAABB.expandToInclude(mBody->getAABB());

		for (int i = 0; i < 10; i++)
		{
			for (ControlList::iterator it = mControls.begin(); it != mControls.end(); it++)
				(*it)->Update(step);

			mBody->setPositionAngle(mBody->getDerivedPosition(), mBody->getDerivedAngle(), mBody->getScale());
			mBody->updateAABB(0.0f, true);
			mIgnoreAABB.expandToInclude(mBody->getAABB());
		}

		mBody->setPositionAngle(originalPos, originalAngle, mBody->getScale());
		mBody->updateAABB(0.0f, true);
	}
}

void LevelSoftBody::Update(float elapsed)
{
	for (std::vector<KinematicControl*>::iterator it = mControls.begin(); it != mControls.end(); it++)
		(*it)->Update(elapsed);
}

void LevelSoftBody::SetIgnore(bool state)
{
	bool prevIg = mBody->getIgnoreMe();

	mBody->setIgnoreMe(state);

	if ((prevIg) && (!state) && (mBody->getIsKinematic()))
	{
		mBody->setPositionAngle(mBody->getDerivedPosition(), mBody->getDerivedAngle(), mBody->getScale());
	}
}

Body* LevelSoftBody::GetBody()
{
	return mBody;
}

const AABB& LevelSoftBody::GetIgnoreAABB()
{
	if (mBody->getIsKinematic())
		return mIgnoreAABB;
	else
		return mBody->getAABB();
}

std::string LevelSoftBody::GetName() const {
	return m_objectInfo.name;
}

glm::vec2 LevelSoftBody::GetStartPosition(){
	return glm::vec2(m_objectInfo.posX, m_objectInfo.posY);
}

void LevelSoftBody::SetVisible(bool visible)
{
	_visible = visible;
}

bool LevelSoftBody::IsVisible()
{
	return _visible;
}

void LevelSoftBody::AddTriangle(int a, int b, int c)
{
	mIndexList.push_back(a);
	mIndexList.push_back(b);
	mIndexList.push_back(c);
}

void LevelSoftBody::FinalizeTriangles()
{
	mIndicesCount = mIndexList.size();
	mIndices = new int[mIndexList.size()];
	for (unsigned int i = 0; i < mIndexList.size(); i++)
		mIndices[i] = mIndexList[i];
}

void LevelSoftBody::SetTextureRect(const TextureRect& rect) {
	if (pressureized == true){
		static_cast<GamePressureBody*>(mBody)->SetTextureRect(rect);
	}else{
		static_cast<GameSpringBody*>(mBody)->SetTextureRect(rect);
	}
}

void LevelSoftBody::SetTexture(Texture* texture){
	if (pressureized == true){
		static_cast<GamePressureBody*>(mBody)->SetTexture(texture);
	}else{
		static_cast<GameSpringBody*>(mBody)->SetTexture(texture);
	}
}

void LevelSoftBody::SetLineColor(glm::vec4 color){
	if (pressureized == true){
		static_cast<GamePressureBody*>(mBody)->SetLineColor(color);
	}else{
		static_cast<GameSpringBody*>(mBody)->SetLineColor(color);
	}
}

void LevelSoftBody::Draw(glm::mat4 &proj){
	if (_visible){
		if (pressureized == true){
			static_cast<GamePressureBody*>(mBody)->Draw(proj, mIndices, mIndicesCount, colorR, colorG, colorB);
		}else{
			static_cast<GameSpringBody*>(mBody)->Draw(proj, mIndices, mIndicesCount, colorR, colorG, colorB);
		}
	}
}