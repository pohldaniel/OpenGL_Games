#include "ShapeDrawer.h"

ShapeDrawer ShapeDrawer::s_instance;
std::unique_ptr<Shader> ShapeDrawer::s_shader = nullptr;

ShapeDrawer& ShapeDrawer::Get() {
	return s_instance;
}

void ShapeDrawer::init(size_t maxTriangles) {
	
	if (!s_shader) {
		m_maxTriangles = maxTriangles;
	
		glGenBuffers(1, &m_ibo);
		glGenBuffers(1, &m_vbo);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		//Position
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, maxTriangles * 3 * sizeof(btVector3), nullptr, GL_DYNAMIC_DRAW);


		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(btVector3), 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxTriangles * 3 * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);
		glBindVertexArray(0);

		s_shader = std::unique_ptr<Shader>(new Shader(SHAPEDRAWER_VERTEX, SHAPEDRAWER_FRGAMENT, false));
	}
}

ShapeDrawer::~ShapeDrawer() {
	shutdown();
}

void ShapeDrawer::setCamera(const Camera& camera) {
	m_camera = &camera;
}

void ShapeDrawer::shutdown() {

	/*for (int i = 0; i<m_shapecaches.size(); i++){
		m_shapecaches[i]->~ShapeCache();
		btAlignedFree(m_shapecaches[i]);
	}
	m_shapecaches.clear();

	for (int i = 0; i<m_shapecachesConvex.size(); i++) {
		m_shapecachesConvex[i]->~ShapeCacheConvex();
		delete m_shapecachesConvex[i];
	}

	m_shapecachesConvex.clear();*/

	if (m_vao)
		glDeleteVertexArrays(1, &m_vao);

	if (m_vbo)
		glDeleteBuffers(1, &m_vbo);

	if (m_ibo)
		glDeleteBuffers(1, &m_ibo);
}

void ShapeDrawer::drawDynmicsWorld(btDynamicsWorld* dynamicsWorld) {
	btScalar	m[16];
	const int	numObjects = dynamicsWorld->getNumCollisionObjects();
	btVector3 wireColor(1, 0, 0);
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

	}else if(shape->getShapeType() == BOX_SHAPE_PROXYTYPE || 
			shape->getShapeType() == CYLINDER_SHAPE_PROXYTYPE || 
			shape->getShapeType() == CAPSULE_SHAPE_PROXYTYPE) {

		ShapeCacheConvex* sc = cacheConvex(const_cast<btCollisionShape*>(shape));
		btShapeHull* hull = &sc->m_shapehull;

		const unsigned int* idx = hull->getIndexPointer();
		const btVector3* vtx = hull->getVertexPointer();


		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, hull->numVertices() * sizeof(btVector3), vtx);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, hull->numIndices() * sizeof(unsigned int), idx);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(s_shader->m_program);

		s_shader->loadMatrix("u_projection", m_camera->getPerspectiveMatrix());
		s_shader->loadMatrix("u_view", m_camera->getViewMatrix());
		s_shader->loadMatrix("u_model", m);
		s_shader->loadVector("u_color", sc->m_color);

		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, hull->numIndices(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUseProgram(0);


	}else if (shape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE) {

		ShapeCache*	sc = cache(const_cast<btCollisionShape*>(shape));
		const IndexedMeshArray& meshArray = dynamic_cast<btTriangleIndexVertexArray*>(dynamic_cast<btTriangleMeshShape*>(shape)->getMeshInterface())->getIndexedMeshArray();

		for (int i = 0; i < meshArray.size(); i++) {
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, meshArray.at(i).m_numVertices * sizeof(btVector3), meshArray.at(i).m_vertexBase);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, meshArray.at(i).m_numTriangles * 3 * sizeof(unsigned int), meshArray.at(i).m_triangleIndexBase);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glUseProgram(s_shader->m_program);

			s_shader->loadMatrix("u_projection", m_camera->getPerspectiveMatrix());
			s_shader->loadMatrix("u_view", m_camera->getViewMatrix());
			s_shader->loadMatrix("u_model", m);
			s_shader->loadVector("u_color", sc->m_color);

			glBindVertexArray(m_vao);
			glDrawElements(GL_TRIANGLES, meshArray.at(i).m_numTriangles * 3, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glUseProgram(0);
		}
		return;
	}

}

ShapeDrawer::ShapeCacheConvex* ShapeDrawer::cacheConvex(btCollisionShape* shape) {
	ShapeCacheConvex* sc = (ShapeCacheConvex*)shape->getUserPointer();

	if (!sc) {
		sc = new(btAlignedAlloc(sizeof(ShapeCacheConvex), 16)) ShapeCacheConvex(dynamic_cast<btConvexShape*>(shape));
		sc->m_shapehull.buildHull(shape->getMargin());

		int intColor = randomColor.generate();
		sc->m_color = Vector4f(((intColor >> 16) & 0xFF) / 255.0f, ((intColor >> 8) & 0xFF) / 255.0f, ((intColor) & 0xFF) / 255.0f, 1.0f);
		m_shapecachesConvex.push_back(sc);
		shape->setUserPointer(sc);
	}
	return(sc);
}

ShapeDrawer::ShapeCache* ShapeDrawer::cache(btCollisionShape* shape) {
	ShapeCache* sc = (ShapeCache*)shape->getUserPointer();

	if (!sc) {
		sc = new ShapeCache();

		int intColor = randomColor.generate();
		sc->m_color = Vector4f(((intColor >> 16) & 0xFF) / 255.0f, ((intColor >> 8) & 0xFF) / 255.0f, ((intColor) & 0xFF) / 255.0f, 1.0f);
		m_shapecaches.push_back(sc);
		shape->setUserPointer(sc);
	}

	return sc;
}