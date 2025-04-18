#include "ShapeDrawer.h"
#include "HeightField.h"

RandomColor ShapeDrawer::RandomColor;
ShapeDrawer ShapeDrawer::Instance;
std::unique_ptr<Shader> ShapeDrawer::s_shader = nullptr;
bool ShapeDrawer::Init = false;

ShapeDrawer& ShapeDrawer::Get() {
	return Instance;
}

void ShapeDrawer::init(size_t maxTriangles) {

	if (!Init) {
		m_maxTriangles = maxTriangles;

		glGenBuffers(1, &m_ibo);
		glGenBuffers(1, &m_vbo);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, maxTriangles * 3 * sizeof(btVector3), nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxTriangles * 3 * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);		
	}

	if (!s_shader) {
		s_shader = std::unique_ptr<Shader>(new Shader(SHAPEDRAWER_VERTEX, SHAPEDRAWER_FRGAMENT, false));
	}
}

ShapeDrawer::~ShapeDrawer() {
	shutdown();
}

void ShapeDrawer::setCamera(const Camera& camera) {
	m_camera = &camera;
}

void ShapeDrawer::setProjectionView(const Matrix4f& projection, const Matrix4f& view) {
	m_perspective = projection;
	m_view = view;
	m_useCamera = false;
}

void ShapeDrawer::shutdown() {

	for (int i = 0; i<m_shapecache.size(); i++){
		delete m_shapecache[i];
	}
	m_shapecache.clear();

	for (int i = 0; i<m_shapecacheConvex.size(); i++) {
		delete m_shapecacheConvex[i];
	}

	m_shapecacheConvex.clear();

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}

	if (m_ibo) {
		glDeleteBuffers(1, &m_ibo);
		m_ibo = 0;
	}

	//s_shader.reset();
	Init = false;
	m_useCamera = true;
}

void ShapeDrawer::drawDynmicsWorld(btDynamicsWorld* dynamicsWorld) {
	
	if (m_useCamera) {
		m_perspective = m_camera->getPerspectiveMatrix();
		m_view = m_camera->getViewMatrix();
	}
	
	btScalar m[16];
	const int numObjects = dynamicsWorld->getNumCollisionObjects();
	
	btVector3 wireColor(1.0f, 0.0f, 0.0f);
	for (int i = 0; i < numObjects; i++) {
		btCollisionObject* colObj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(colObj);
		if (body&&body->getMotionState()) {
			btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
			myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
		}
		else {
			colObj->getWorldTransform().getOpenGLMatrix(m);
		}

		drawShape(m, colObj->getCollisionShape());
	}
}

void ShapeDrawer::drawShape(btScalar* m, btCollisionShape* shape) {
	if (shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE) {
		const btCompoundShape* compoundShape = static_cast<const btCompoundShape*>(shape);
		for (int i = compoundShape->getNumChildShapes() - 1; i >= 0; i--) {
			btTransform childTrans = compoundShape->getChildTransform(i);
			const btCollisionShape* colShape = compoundShape->getChildShape(i);
			ATTRIBUTE_ALIGNED16(btScalar) childMat[16];
			childTrans.getOpenGLMatrix(childMat);
			drawShape(&(Matrix4f(m) * Matrix4f(childMat))[0][0], const_cast<btCollisionShape*>(colShape));
		}

	}else if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE ||
			  shape->getShapeType() == CYLINDER_SHAPE_PROXYTYPE ||
			  shape->getShapeType() == CAPSULE_SHAPE_PROXYTYPE ||
			  shape->getShapeType() == CONVEX_HULL_SHAPE_PROXYTYPE ||
			  shape->getShapeType() == SPHERE_SHAPE_PROXYTYPE ||
			  shape->getShapeType() == CYLINDER_SHAPE_PROXYTYPE ||
              shape->getShapeType() == BOX_2D_SHAPE_PROXYTYPE) {

		ShapeCacheConvex* sc = cacheConvex(const_cast<btCollisionShape*>(shape));
		btShapeHull* hull = &sc->m_shapehull;
		btVector3 scale = shape->getLocalScaling();
		Matrix4f mat = Matrix4f(m);
		Matrix4f scaleMat = Matrix4f::Scale(scale.x(), scale.y(), scale.z());
		const unsigned int* idx = hull->getIndexPointer();
		const btVector3* vtx = hull->getVertexPointer();

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, hull->numVertices() * sizeof(btVector3), vtx);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(btVector3), 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, hull->numIndices() * sizeof(unsigned int), idx);
	
		s_shader->use();

		s_shader->loadMatrix("u_projection", m_perspective);
		s_shader->loadMatrix("u_view", m_view);
		s_shader->loadMatrix("u_model", Matrix4f(m) * scaleMat);
		s_shader->loadVector("u_color", sc->m_color);

		glDrawElements(GL_TRIANGLES, hull->numIndices(), GL_UNSIGNED_INT, 0);

		s_shader->unuse();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}else if (shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE) {

		ShapeCache*	sc = cache(const_cast<btCollisionShape*>(shape));
		const IndexedMeshArray& meshArray = dynamic_cast<btTriangleIndexVertexArray*>(dynamic_cast<btTriangleMeshShape*>(shape)->getMeshInterface())->getIndexedMeshArray();
		btVector3 scale = shape->getLocalScaling();
		Matrix4f scaleMat = Matrix4f::Scale(scale.x(), scale.y(), scale.z());

		for (int i = 0; i < meshArray.size(); i++) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, meshArray.at(i).m_numVertices * 3 * sizeof(btScalar), reinterpret_cast<const btScalar*>(meshArray.at(i).m_vertexBase));
			
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(btScalar), 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, meshArray.at(i).m_numTriangles * 3 * sizeof(unsigned int), reinterpret_cast<const unsigned int*>(meshArray.at(i).m_triangleIndexBase));
			

			s_shader->use();

			s_shader->loadMatrix("u_projection", m_perspective);
			s_shader->loadMatrix("u_view", m_view);
			s_shader->loadMatrix("u_model", Matrix4f(m) * scaleMat);
			s_shader->loadVector("u_color", sc->m_color);

			glDrawElements(GL_TRIANGLES, meshArray.at(i).m_numTriangles * 3, GL_UNSIGNED_INT, 0);

			s_shader->unuse();

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);			
		}

	}else if (shape->getShapeType() == TERRAIN_SHAPE_PROXYTYPE) {		
		DrawHelper* sc = (DrawHelper*)shape->getUserPointer();
		btVector3 scale = shape->getLocalScaling();
		Matrix4f scaleMat = Matrix4f::Scale(scale.x(), scale.y(), scale.z());

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sc->m_positions->size() * sizeof(btVector3), &(*sc->m_positions)[0]);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(btVector3), 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sc->m_indices->size() * sizeof(unsigned int), &(*sc->m_indices)[0]);

		s_shader->use();

		s_shader->loadMatrix("u_projection", m_perspective);
		s_shader->loadMatrix("u_view", m_view);
		s_shader->loadMatrix("u_model", Matrix4f(m) * scaleMat);
		s_shader->loadVector("u_color", sc->m_color);

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sc->m_indices->size()), GL_UNSIGNED_INT, 0);

		s_shader->unuse();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}else if (shape->getShapeType() == STATIC_PLANE_PROXYTYPE){
		
		const btStaticPlaneShape* staticPlaneShape = static_cast<const btStaticPlaneShape*>(shape);
		btScalar planeConst = staticPlaneShape->getPlaneConstant();
		const btVector3& planeNormal = staticPlaneShape->getPlaneNormal();
		btVector3 planeOrigin = planeNormal * planeConst;
		btVector3 vec0, vec1;
		btPlaneSpace1(planeNormal, vec0, vec1);
		btScalar vecLen = 1000.f;

		std::vector<btVector3> points = { planeOrigin - vec1 * vecLen, planeOrigin - vec0 * vecLen, planeOrigin + vec1 * vecLen, planeOrigin + vec0 * vecLen};
		//std::vector<unsigned int> indices = { 0, 1, 1, 2, 2, 3, 3, 0 };
		std::vector<unsigned int> indices = { 0, 1, 2 , 2, 3, 0};

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(btVector3), &points[0]);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(btVector3), 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), &indices[0]);

		s_shader->use();

		s_shader->loadMatrix("u_projection", m_perspective);
		s_shader->loadMatrix("u_view", m_view);
		s_shader->loadMatrix("u_model", Matrix4f(m));
		s_shader->loadVector("u_color", Vector4f(0.5f, 0.5f, 0.5f, 1.0f));

		//glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		s_shader->unuse();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

}

ShapeDrawer::ShapeCacheConvex* ShapeDrawer::cacheConvex(btCollisionShape* shape) {
	ShapeCacheConvex* sc = (ShapeCacheConvex*)shape->getUserPointer();

	if (!sc) {
		sc = new ShapeCacheConvex(dynamic_cast<btConvexShape*>(shape));
		sc->m_shapehull.buildHull(shape->getMargin());

		int intColor = RandomColor.generate();
		sc->m_color = Vector4f(((intColor >> 16) & 0xFF) / 255.0f, ((intColor >> 8) & 0xFF) / 255.0f, ((intColor) & 0xFF) / 255.0f, 1.0f);
		m_shapecacheConvex.push_back(sc);
		shape->setUserPointer(sc);
	}
	return(sc);
}

ShapeDrawer::ShapeCache* ShapeDrawer::cache(btCollisionShape* shape) {
	ShapeCache* sc = (ShapeCache*)shape->getUserPointer();

	if (!sc) {
		sc = new ShapeCache();

		int intColor = RandomColor.generate();
		sc->m_color = Vector4f(((intColor >> 16) & 0xFF) / 255.0f, ((intColor >> 8) & 0xFF) / 255.0f, ((intColor) & 0xFF) / 255.0f, 1.0f);
		m_shapecache.push_back(sc);
		shape->setUserPointer(sc);
	}

	return sc;
}