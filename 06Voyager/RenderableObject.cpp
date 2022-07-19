#include "RenderableObject.h"

RenderableObject::RenderableObject() {
	m_shader = Globals::shaderManager.getAssetPointer("quad");
	
}

/*RenderableObject::~RenderableObject() {

}*/

void RenderableObject::setDisabled(bool disabled) {
	m_disabled = disabled;
}

bool RenderableObject::isDisabled() {
	return m_disabled;
}


void RenderableObject::draw(const Camera& camera) {};
void RenderableObject::drawShadow(const Camera& camera) {};
void RenderableObject::drawRaw() {};
void RenderableObject::drawAABB(const Camera& camera) {};

void RenderableObject::rotate(const Vector3f &axis, float degrees) {
	m_modelMatrix.rotate(axis, degrees);
}

void RenderableObject::translate(float dx, float dy, float dz) {
	m_modelMatrix.translate(dx, dy, dz);
}

void RenderableObject::scale(float a, float b, float c) {
	m_modelMatrix.scale(a, b, c);
}

const Matrix4f &RenderableObject::getTransformationMatrix() const {
	return m_modelMatrix.getTransformationMatrix();
}

const Matrix4f &RenderableObject::getInvTransformationMatrix() const {
	return m_modelMatrix.getInvTransformationMatrix();
}