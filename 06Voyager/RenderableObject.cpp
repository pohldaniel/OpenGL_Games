#include "RenderableObject.h"

unsigned int RenderableObject::s_id = 0;

RenderableObject::RenderableObject() {
	m_colorShader = Globals::shaderManager.getAssetPointer("color");
	m_id = RenderableObject::s_id++;
	m_pickColor = Vector4f(((m_id & 0x000000FF) >> 0)* (1.0f / 255.0f), ((m_id & 0x0000FF00) >> 8)* (1.0f / 255.0f), ((m_id & 0x00FF0000) >> 16)* (1.0f / 255.0f), 0.0f);
}

/*RenderableObject::~RenderableObject() {

}*/

void RenderableObject::setDisabled(bool disabled) {
	m_disabled = disabled;
}

bool RenderableObject::isDisabled() {
	return m_disabled;
}

void RenderableObject::drawRaw() {
	m_model->drawRaw();
};

void RenderableObject::draw(const Camera& camera) {};

void RenderableObject::drawShadow(const Camera& camera) {
	drawRaw();
};

void RenderableObject::drawAABB(const Camera& camera) {
	glUseProgram(m_colorShader->m_program);
	m_colorShader->loadMatrix("u_projection", Globals::projection, false);
	m_colorShader->loadMatrix("u_view", camera.getViewMatrix(), false);
	m_colorShader->loadMatrix("u_model", m_transform.getTransformationMatrix(), false);
	m_colorShader->loadVector("u_color", m_pickColor);
	m_model->drawAABB();
	glUseProgram(0);
};

void RenderableObject::drawSphere(const Camera& camera) {
	glUseProgram(m_colorShader->m_program);
	m_colorShader->loadMatrix("u_projection", Globals::projection, false);
	m_colorShader->loadMatrix("u_view", camera.getViewMatrix(), false);
	m_colorShader->loadMatrix("u_model", m_transform.getTransformationMatrix(), false);
	m_colorShader->loadVector("u_color", m_pickColor);
	m_model->drawSphere();
	glUseProgram(0);
};

void RenderableObject::drawHull(const Camera& camera) {
	glUseProgram(m_colorShader->m_program);
	m_colorShader->loadMatrix("u_projection", Globals::projection, false);
	m_colorShader->loadMatrix("u_view", camera.getViewMatrix(), false);
	m_colorShader->loadMatrix("u_model", m_transform.getTransformationMatrix(), false);
	m_colorShader->loadVector("u_color", m_pickColor);
	m_model->drawHull();
	glUseProgram(0);
};

BoundingBox& RenderableObject::getAABB() {
	return m_model->getAABB();
}

BoundingSphere& RenderableObject::getBoundingSphere() {
	return m_model->getBoundingSphere();
}

ConvexHull& RenderableObject::getConvexHull() {
	return m_model->getConvexHull();
}

void RenderableObject::setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz) {
	m_transform.setRotPos(axis, degrees, dx, dy, dz);
}

void RenderableObject::setRotPosScale(const Vector3f &axis, float degrees, float dx, float dy, float dz, float x, float y, float z) {
	m_transform.setRotPosScale(axis, degrees, dx, dy, dz, x, y, z);
}

void RenderableObject::rotate(const Vector3f &axis, float degrees) {
	m_transform.rotate(axis, degrees);
}

void RenderableObject::translate(float dx, float dy, float dz) {
	m_transform.translate(dx, dy, dz);
}

void RenderableObject::scale(float a, float b, float c) {
	m_transform.scale(a, b, c);
}

const Matrix4f &RenderableObject::getTransformationMatrix() const {
	return m_transform.getTransformationMatrix();
}

const Matrix4f &RenderableObject::getInvTransformationMatrix() {
	return m_transform.getInvTransformationMatrix();
}

const ObjModel* RenderableObject::getModel() const {
	return m_model;
}

unsigned int RenderableObject::getId() const {
	return m_id;
}

Transform &RenderableObject::getTransform() {
	return m_transform;
}
