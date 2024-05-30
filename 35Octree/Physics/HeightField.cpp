#include "HeightField.h"
#include "ShapeDrawer.h"

HeightField::HeightField() {

}

HeightField::~HeightField() {

}

void HeightField::create(char* heightfieldData, unsigned int dimX, unsigned int dimZ, const btTransform& transform, const btVector3& localScaling, int collisionFilterGroup, int collisionFilterMask) {
	m_localScaling = localScaling;
	btScalar maxHeight = 500.0f;

	bool useFloatDatam = false;
	bool flipQuadEdges = false;

	m_heightFieldShape = new btHeightfieldTerrainShape(dimX, dimZ, heightfieldData, maxHeight, 1, useFloatDatam, flipQuadEdges);;

	m_heightFieldShape->setUseDiamondSubdivision(false);
	
	m_rigidBody = Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(0.0f, 200.0f + 49.4f, 0.0f)), m_heightFieldShape, collisionFilterGroup, collisionFilterMask, btCollisionObject::CF_STATIC_OBJECT);
}

void HeightField::processAllTriangles() {
	btVector3	worldBoundsMin, worldBoundsMax;
	Physics::GetDynamicsWorld()->getBroadphase()->getBroadphaseAabb(worldBoundsMin, worldBoundsMax);

	TriangleCollector col;
	col.indexBufferOut = &m_indices;
	col.vertexBufferOut = &m_positions;

	m_heightFieldShape->setLocalScaling(btVector3(1.0f, 1.0f, 1.0f));
	m_heightFieldShape->processAllTriangles(&col, worldBoundsMin, worldBoundsMax);
	m_heightFieldShape->setLocalScaling(m_localScaling);

	int intColor = ShapeDrawer::RandomColor.generate();
	m_drawHelper = { &m_positions , &m_indices ,  Vector4f(((intColor >> 16) & 0xFF) / 255.0f, ((intColor >> 8) & 0xFF) / 255.0f, ((intColor) & 0xFF) / 255.0f, 1.0f)};

	m_heightFieldShape->setUserPointer((void*)&m_drawHelper);
}