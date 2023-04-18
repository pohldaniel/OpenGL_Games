#include "RenderableObject.h"
#include "Constants.h"

Transform RenderableObject::Transform;

RenderableObject::RenderableObject(const Vector3f &position, const Vector3f &scale) {
	m_position = position;
	m_scale = scale;
}

RenderableObject::RenderableObject(const Vector3f &position, const Vector3f &scale, std::string shape, std::string shader, std::string texture)  {
	m_position = position;
	m_scale = scale;
	m_shader = shader;
	m_texture = texture;
	m_shape = shape;
}

void RenderableObject::setAttributes(std::string shape, std::string shader, std::string texture) {
	m_shader = shader;
	m_texture = texture;
	m_shape = shape;
}

RenderableObject::~RenderableObject() {

}

void RenderableObject::setDisabled(bool disabled) {
	m_disabled = disabled;
}

bool RenderableObject::isDisabled() {
	return m_disabled;
}

void RenderableObject::draw(const Camera& camera) {
	if (m_disabled) return;

	auto shader = Globals::shaderManager.getAssetPointer(m_shader);
	glUseProgram(shader->m_program);
	Transform.reset();
	Transform.setPosition(m_position);
	Transform.scale(m_scale, m_position);
	shader->loadMatrix("projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("view", camera.getViewMatrix());
	shader->loadMatrix("model", Transform.getTransformationMatrix());
	shader->loadInt("meshTexture", 0);

	Globals::textureManager.get(m_texture).bind(0);
	Globals::shapeManager.get(m_shape).drawRaw();
	glUseProgram(0);
}

void RenderableObject::draw(const Camera& camera, const Matrix4f& model) {
	if (m_disabled) return;

	auto shader = Globals::shaderManager.getAssetPointer(m_shader);
	glUseProgram(shader->m_program);
	
	shader->loadMatrix("projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("view", Matrix4f::IDENTITY);
	shader->loadMatrix("model", model);
	shader->loadInt("meshTexture", 0);

	Globals::textureManager.get(m_texture).bind(0);

	Globals::shapeManager.get(m_shape).drawRaw();
	glUseProgram(0);
}