#include "Portal.h"
#include "Terrain.h"
#include "Globals.h"

Portal::Portal() {

}

Portal::~Portal() {

}

void Portal::init(const Terrain& terrain) {
	setPosition(TERRAIN_SIZE / 2, terrain.heightAt(TERRAIN_SIZE / 2, TERRAIN_SIZE / 2 + 32.0f), TERRAIN_SIZE / 2 + 32.0f);

	std::vector<Vector4f> colors = { Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Vector4f(1.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.0f, 1.0f, 0.0f, 1.0f) , Vector4f(0.2f, 0.2f, 1.0f, 1.0f) , Vector4f(1.0f, 0.0f, 1.0f, 1.0f) };
	
	std::vector<Matrix4f> instances = { Matrix4f::Translate(PORTAL_SIDE * 0.5f, PORTAL_SIDE * 0.33f, 0.0f),
										Matrix4f::Translate(PORTAL_SIDE * 0.5f, PORTAL_SIDE * 1.0f, 0.0f),
										Matrix4f::Translate(0.0f, PORTAL_SIDE * 1.5f, 0.0f),
										Matrix4f::Translate(-PORTAL_SIDE * 0.5f, PORTAL_SIDE * 1.0f, 0.0f),
										Matrix4f::Translate(-PORTAL_SIDE * 0.5f, PORTAL_SIDE * 0.33f, 0.0f)};

	Globals::shapeManager.get("sphere_portal").setInstances(instances);
	Globals::shapeManager.get("sphere_portal").setVec4Attribute(colors, 1);

	m_vortex = RenderableObject("vortex", "texture", "vortex");
	m_vortex.setDrawFunction([&](const Camera& camera) {
		if (m_vortex.isDisabled()) return;
		glDisable(GL_CULL_FACE);
		auto shader = Globals::shaderManager.getAssetPointer(m_vortex.getShader());
		shader->use();
		shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", camera.getViewMatrix());
		shader->loadMatrix("u_model", m_vortex.getTransformationSOP());
		shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix() * m_vortex.getTransformationSOP()));

		Globals::textureManager.get(m_vortex.getTexture()).bind(0);
		Globals::shapeManager.get(m_vortex.getShape()).drawRaw();
		shader->unuse();
		glEnable(GL_CULL_FACE);
	});

	m_vortex.setScale(1.5f, 1.5f, 0.0f);
	m_vortex.setPosition(512.0f, 13.75f + PORTAL_SIDE * 0.5f, 544.0f);
	m_vortex.setDisabled(true);
	m_vortex.setUpdateFunction(
		[&](const float dt) {
		if (m_vortex.isDisabled()) return;
		m_vortex.rotate(0.0f, 0.0f, 0.5f);
	});

	
}

void Portal::create(btCollisionShape* shape, const btTransform& transform, btDynamicsWorld* physicsWorld, int collisionFilterGroup, int collisionFilterMask, void* rigidBodyUserPointer) {
	m_collisionObject = new btCollisionObject();

	m_collisionObject->setCollisionShape(shape);
	m_collisionObject->setWorldTransform(transform);

	m_collisionObject->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_collisionObject->forceActivationState(DISABLE_DEACTIVATION);

	physicsWorld->addCollisionObject(m_collisionObject, collisionFilterGroup, collisionFilterMask);

	m_debugPosition = Physics::VectorFrom(transform.getOrigin());
}

void Portal::draw(const Camera& camera) {
	
	auto shader = Globals::shaderManager.getAssetPointer("portal");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_model", getTransformationP());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix() * getTransformationP()));

	shader->loadVector("u_lightPos", Vector3f(50.0f, 50.0f, 50.0f));

	shader->loadFloat("invRadius", 0.0f);
	shader->loadFloat("alpha", 1.0f);
	shader->loadInt("u_texture", 0);
	shader->loadInt("u_normalMap", 1);

	Globals::textureManager.get("portal").bind(0);
	Globals::textureManager.get("portal_nmp").bind(1);

	Globals::shapeManager.get("portal").drawRaw();
	shader->unuse();

	glEnable(GL_BLEND);
	shader = Globals::shaderManager.getAssetPointer("energy");
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(camera.getViewMatrix()));

	Globals::shapeManager.get("sphere_portal").drawRawInstanced();
	shader->unuse();

	m_vortex.draw(camera);

	glDisable(GL_BLEND);	

#if DEBUGCOLLISION

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&camera.getPerspectiveMatrix()[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&(camera.getViewMatrix() * Matrix4f::Translate(m_debugPosition) * Matrix4f::Scale(1.5f, 1.5f, 0.01f))[0][0]);


	glBegin(GL_LINE_STRIP);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glEnd();
	
	glFlush();
#endif
}

void Portal::update(const float dt) {
	m_vortex.update(dt);
}

void Portal::setDisabled(bool disabled) {
	m_vortex.setDisabled(disabled);
}

const Vector3f Portal::getReceptor(int index) const {
	
	if (index == 0) {
		return Vector3f(m_position[0] + PORTAL_SIDE / 2, m_position[1] + 1.0f, m_position[2]);
	} else if (index == 1) {
		return Vector3f(m_position[0] + PORTAL_SIDE / 2, m_position[1] + PORTAL_SIDE, m_position[2]);
	}else if (index == 2) {
		return Vector3f(m_position[0], m_position[1] + PORTAL_SIDE * 3 / 2, m_position[2]);
	}else if (index == 3) {
		return Vector3f(m_position[0] - PORTAL_SIDE / 2, m_position[1] + PORTAL_SIDE, m_position[2]);
	} else {
		return Vector3f(m_position[0] - PORTAL_SIDE / 2, m_position[1] + 1.0f, m_position[2]);
	}
}

float Portal::getZ() {
	return m_position[2];
}

btCollisionObject* Portal::getCollisionObject() {
	return m_collisionObject;
}