#include "Entity.h"
#include "Constants.h"

Entity::Entity(const Shape& shape, const Shader* shader, const Cubemap& texture) : RenderableObject() , m_shape(shape), m_shader(shader), m_texture(texture){

}

Entity::~Entity() {

}

void Entity::fixedUpdate() {

}

void Entity::update(const float dt) {
	m_update(dt);
}

void Entity::setUpdateFunction(std::function<void(const float dt)> fun) {
	m_update = fun;
}

void Entity::draw(const Camera& camera) {
	if (m_disabled) return;

	m_shader->use();
	
	m_shader->loadMatrix("projection", camera.getPerspectiveMatrix());
	m_shader->loadMatrix("view", camera.getViewMatrix());
	m_shader->loadMatrix("model", Transform.getTransformationMatrix());
	m_shader->loadInt("meshTexture", 0);

	m_texture.bind(0);
	m_shape.drawRaw();
	m_shader->unuse();
}

void Entity::draw(const Camera& camera, const Matrix4f& model) {
	if (m_disabled) return;

	m_shader->use();

	m_shader->loadMatrix("projection", camera.getPerspectiveMatrix());
	m_shader->loadMatrix("view", Matrix4f::IDENTITY);
	m_shader->loadMatrix("model", model);
	m_shader->loadInt("meshTexture", 0);

	m_texture.bind(0);
	m_shape.drawRaw();
	m_shader->unuse();
}