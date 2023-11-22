#include "RenderableObject.h"

RenderableObject::RenderableObject(unsigned int category) : Object(category) {
	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_quad = new Quad(true, 0.0f, 1.0f, 0.0f, 1.0f, m_size[0], m_size[1], 0.0f, 0.0f, 1.0f, 1.0f, 0, 0);
	m_sprites["gem"] = Globals::textureManager.get("gem").getTexture();
}

/*RenderableObject::~RenderableObject() {
	
}*/


void RenderableObject::setSize(const float x, const float y) {
	Object::setSize(x, y);
	delete m_quad;

	m_quad = new Quad(true, 0.0f, 1.0f, 0.0f, 1.0f, m_size[0], m_size[1], 0.0f, 0.0f, 1.0f, 1.0f, 0, 0);
}

void RenderableObject::setSize(const Vector2f &size) {
	Object::setSize(size);
	delete m_quad;

	m_quad = new Quad(true, 0.0f, 1.0f, 0.0f, 1.0f, m_size[0], m_size[1], 0.0f, 0.0f, 1.0f, 1.0f, 0, 0);
}

void RenderableObject::setDisabled(bool disabled) {
	m_disabled = disabled;
}

bool RenderableObject::isDisabled() {
	return m_disabled;
}

void RenderableObject::render() {
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Globals::projection * ViewEffect::get().getView() * m_transform);
	m_quad->render(m_sprites["gem"]);
	glUseProgram(0);
}