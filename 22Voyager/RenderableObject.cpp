#include "RenderableObject.h"
#include "Constants.h"

RenderableObject::RenderableObject(const Vector3f &position, const Vector3f &scale) : Object() {
	m_position = position;
	m_scale = scale;

	//m_draw = std::bind(&RenderableObject::drawDefault, this, std::placeholders::_1, std::placeholders::_2);
	//m_draw = std::function<void(const Camera& camera, bool viewIndependent)>{ [&](const Camera& camera, bool viewIndependent) {drawDefault(camera, viewIndependent); } };
}

RenderableObject::RenderableObject(std::string shape, std::string shader, std::string texture) : Object() {
	m_shader = shader;
	m_texture = texture;
	m_shape = shape;

	//m_draw = std::bind(&RenderableObject::drawDefault, this, std::placeholders::_1, std::placeholders::_2);
	//m_draw = std::function<void(const Camera& camera, bool viewIndependent)>{ [&](const Camera& camera, bool viewIndependent) {drawDefault(camera, viewIndependent); } };
}

RenderableObject::RenderableObject(std::string shader) : Object() {
	m_shader = shader;
	m_texture = std::string();
	m_shape = std::string();
}

RenderableObject::RenderableObject(RenderableObject const& rhs) {
	m_position = rhs.m_position;
	m_scale = rhs.m_scale;
	m_orientation = rhs.m_orientation;
	
	m_shape = rhs.m_shape;
	m_shader = rhs.m_shader;
	m_texture = rhs.m_texture;
	m_disabled = rhs.m_disabled;
	m_update = std::function<void(const float dt)>(rhs.m_update);
	m_draw = std::function<void(const Camera& camera, bool viewIndependent)>(rhs.m_draw);
}

RenderableObject &RenderableObject::operator=(const RenderableObject &rhs) {
	m_position = rhs.m_position;
	m_scale = rhs.m_scale;
	m_orientation = rhs.m_orientation;

	m_shape = rhs.m_shape;
	m_shader = rhs.m_shader;
	m_texture = rhs.m_texture;
	m_disabled = rhs.m_disabled;
	m_update = std::function<void(const float dt)>(rhs.m_update);
	m_draw = std::function<void(const Camera& camera, bool viewIndependent)>(rhs.m_draw);

	//std::function<void(const Camera& camera, bool viewIndependent)> bind = std::bind(&RenderableObject::drawDefault, this, std::placeholders::_1, std::placeholders::_2);

	//if (rhs.m_draw.target_type() == bind.target_type()) {
	//	m_draw = std::bind(&RenderableObject::drawDefault, this, std::placeholders::_1, std::placeholders::_2);
	//} else {
	//	m_draw = std::function<void(const Camera& camera, bool viewIndependent)>(rhs.m_draw);
	//}

	//if (rhs.m_draw.target_type() == typeid(&RenderableObject::drawDefault)) {
	//
	//	m_draw = std::bind(&RenderableObject::drawDefault, this, std::placeholders::_1, std::placeholders::_2);
	//} else {
	//	m_draw = std::function<void(const Camera& camera, bool viewIndependent)>(rhs.m_draw);
	//}

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

void RenderableObject::draw(const Camera& camera, bool viewIndependent) {
	if (m_draw) {
		m_draw(camera, viewIndependent);
	}else {
		drawDefault(camera, viewIndependent);
	}
}

void RenderableObject::setUpdateFunction(std::function<void(const float dt)> fun) {
	m_update = fun;
}

void RenderableObject::setDrawFunction(std::function<void(const Camera& camera, bool viewIndependent)> fun) {
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

void RenderableObject::drawDefault(const Camera& camera, bool viewIndependent) {
	if (m_disabled) return;

	auto shader = Globals::shaderManager.getAssetPointer(m_shader);

	glUseProgram(shader->m_program);

	shader->loadMatrix("projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("view", viewIndependent ? Matrix4f::IDENTITY : camera.getViewMatrix());
	shader->loadMatrix("model", getTransformationSOP());

	Globals::textureManager.get(m_texture).bind(0);
	Globals::shapeManager.get(m_shape).drawRaw();
	glUseProgram(0);
}