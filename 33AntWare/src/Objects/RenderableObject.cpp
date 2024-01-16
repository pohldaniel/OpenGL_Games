#include "RenderableObject.h"
#include "Globals.h"

RenderableObject::RenderableObject(const Vector3f &position, const Vector3f &scale) : Object() {
	m_position = position;
	m_scale = scale;
}

RenderableObject::RenderableObject(std::string shape, std::string shader, std::string texture) : Object() {
	m_shader = shader;
	m_texture = texture;
	m_shape = shape;
}

RenderableObject::RenderableObject(std::string shader) : Object() {
	m_shader = shader;
	m_texture = std::string();
	m_shape = std::string();
}

RenderableObject::RenderableObject(RenderableObject const& rhs) : Object(rhs) {
	m_shape = rhs.m_shape;
	m_shader = rhs.m_shader;
	m_texture = rhs.m_texture;
	m_disabled = rhs.m_disabled;
	m_update = std::function<void(const float dt)>(rhs.m_update);
	m_draw = std::function<void(const Camera& camera)>(rhs.m_draw);
}

RenderableObject &RenderableObject::operator=(const RenderableObject &rhs) {
	Object::operator=(rhs);

	m_shape = rhs.m_shape;
	m_shader = rhs.m_shader;
	m_texture = rhs.m_texture;
	m_disabled = rhs.m_disabled;
	m_update = std::function<void(const float dt)>(rhs.m_update);
	m_draw = std::function<void(const Camera& camera)>(rhs.m_draw);

	return *this;
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

void RenderableObject::update(const float dt) {	
	if (m_update)
		m_update(dt);
}

void RenderableObject::draw(const Camera& camera) {
	if (m_draw) {
		m_draw(camera);
	}else {
		drawDefault(camera);
	}
}

void RenderableObject::setUpdateFunction(std::function<void(const float dt)> fun) {
	m_update = fun;
}

void RenderableObject::setDrawFunction(std::function<void(const Camera& camera)> fun) {
	m_draw = fun;
}

std::string& RenderableObject::getShape(){
	return m_shape;
}

std::string& RenderableObject::getShader(){
	return m_shader;
}

std::string& RenderableObject::getTexture(){
	return m_texture;
}

void RenderableObject::drawDefault(const Camera& camera) {
	if (m_disabled) return;

	auto shader = Globals::shaderManager.getAssetPointer(m_shader);

	glUseProgram(shader->m_program);

	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", camera.getViewMatrix());
	shader->loadMatrix("u_model", getTransformationSOP());

	Globals::textureManager.get(m_texture).bind(0);
	Globals::shapeManager.get(m_shape).drawRaw();
	glUseProgram(0);
}