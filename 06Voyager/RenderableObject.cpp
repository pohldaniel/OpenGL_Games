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

void RenderableObject::render(const Camera& camera) {
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform",  Globals::projection);
	m_model->draw(camera);
	glUseProgram(0);
}

void RenderableObject::renderShadow(const Camera& camera) {}