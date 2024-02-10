#include <iostream>
#include <GL/glew.h>
#include <engine/BuiltInShader.h>

#include "Light.h"

std::vector<Light*> Light::Lights;
LightBuffer Light::Buffer[20];

bool operator== (const Light& l1, const Light& l2) {
	return Light::Buffer[l1.m_index].ambient[0] == Light::Buffer[l1.m_index].ambient[0] && Light::Buffer[l1.m_index].ambient[1] == Light::Buffer[l1.m_index].ambient[1] && Light::Buffer[l1.m_index].ambient[2] == Light::Buffer[l1.m_index].ambient[2] && Light::Buffer[l1.m_index].ambient[3] == Light::Buffer[l1.m_index].ambient[3]
		&& Light::Buffer[l1.m_index].diffuse[0] == Light::Buffer[l1.m_index].diffuse[0] && Light::Buffer[l1.m_index].diffuse[1] == Light::Buffer[l1.m_index].diffuse[1] && Light::Buffer[l1.m_index].diffuse[2] == Light::Buffer[l1.m_index].diffuse[2] && Light::Buffer[l1.m_index].diffuse[3] == Light::Buffer[l1.m_index].diffuse[3]
		&& Light::Buffer[l1.m_index].specular[0] == Light::Buffer[l1.m_index].specular[0] && Light::Buffer[l1.m_index].specular[1] == Light::Buffer[l1.m_index].specular[1] && Light::Buffer[l1.m_index].specular[2] == Light::Buffer[l1.m_index].specular[2] && Light::Buffer[l1.m_index].specular[3] == Light::Buffer[l1.m_index].specular[3]
		&& Light::Buffer[l1.m_index].position[0] == Light::Buffer[l1.m_index].position[0] && Light::Buffer[l1.m_index].position[1] == Light::Buffer[l1.m_index].position[1] && Light::Buffer[l1.m_index].position[2] == Light::Buffer[l1.m_index].position[2]
		&& Light::Buffer[l1.m_index].direction[0] == Light::Buffer[l1.m_index].direction[0] && Light::Buffer[l1.m_index].direction[1] == Light::Buffer[l1.m_index].direction[1] && Light::Buffer[l1.m_index].direction[2] == Light::Buffer[l1.m_index].direction[2]
		&& Light::Buffer[l1.m_index].angle == Light::Buffer[l1.m_index].angle
		&& Light::Buffer[l1.m_index].type == Light::Buffer[l1.m_index].type;
}

Light::Light() : SceneNode(), m_isStatic(true) {

}

Light::Light(Light const& rhs) : SceneNode(rhs) {
	m_index = rhs.m_index;
	m_isStatic = rhs.m_isStatic;
	m_direction = rhs.m_direction;
}

Light::Light(Light&& rhs) : SceneNode(rhs) {
	m_index = rhs.m_index;
	m_isStatic = rhs.m_isStatic;
	m_direction = rhs.m_direction;
}

Light& Light::operator=(const Light& rhs) {
	SceneNode::operator=(rhs);
	m_index = rhs.m_index;
	m_isStatic = rhs.m_isStatic;
	m_direction = rhs.m_direction;
	return *this;
}

Light& Light::operator=(Light&& rhs) {
	SceneNode::operator=(rhs);
	m_index = rhs.m_index;
	m_isStatic = rhs.m_isStatic;
	m_direction = rhs.m_direction;
	return *this;
}

void Light::update(const float dt) {
	if (m_isStatic)
		return;

	if (m_parent) {
		setUboPosition(m_parent->getWorldTransformation() ^ Vector4f(m_position));
		setUboDirection(Quaternion::Rotate(getWorldOrientation(), m_direction));
	}
}

void Light::setDirection(const Vector3f& direction) {
	m_direction = direction;
}

void Light::setUboAmbient(std::array<float, 4> ambient) const {
	Buffer[m_index].ambient[0] = ambient[0];
	Buffer[m_index].ambient[1] = ambient[1];
	Buffer[m_index].ambient[2] = ambient[2];
	Buffer[m_index].ambient[3] = ambient[3];
}

void Light::setUboDiffuse(std::array<float, 4> diffuse) const {
	Buffer[m_index].diffuse[0] = diffuse[0];
	Buffer[m_index].diffuse[1] = diffuse[1];
	Buffer[m_index].diffuse[2] = diffuse[2];
	Buffer[m_index].diffuse[3] = diffuse[3];
}

void Light::setUboSpecular(std::array<float, 4> specular) const {
	Buffer[m_index].specular[0] = specular[0];
	Buffer[m_index].specular[1] = specular[1];
	Buffer[m_index].specular[2] = specular[2];
	Buffer[m_index].specular[3] = specular[3];
}

void Light::setUboPosition(const Vector3f& position) {
	Buffer[m_index].position[0] = position[0];
	Buffer[m_index].position[1] = position[1];
	Buffer[m_index].position[2] = position[2];
}

void Light::setUboDirection(const Vector3f& direction) const {
	Buffer[m_index].direction[0] = direction[0];
	Buffer[m_index].direction[1] = direction[1];
	Buffer[m_index].direction[2] = direction[2];
}

void Light::setUboAngle(float _angle) const {
	Buffer[m_index].angle = _angle;
}

void Light::setUboType(int type) {
	Buffer[m_index].type = static_cast<LightType>(type);
}

void Light::toggle() const {
	Buffer[m_index].enabled = !Buffer[m_index].enabled;
}

void Light::cleanup() {

}

void Light::print() {
	std::cout << "Ambient: " << Buffer[m_index].ambient[0] << "  " << Buffer[m_index].ambient[1] << "  " << Buffer[m_index].ambient[2] << "  " << Buffer[m_index].ambient[3] << std::endl;
	std::cout << "Diffuse: " << Buffer[m_index].diffuse[0] << "  " << Buffer[m_index].diffuse[1] << "  " << Buffer[m_index].diffuse[2] << "  " << Buffer[m_index].diffuse[3] << std::endl;
	std::cout << "Specular: " << Buffer[m_index].specular[0] << "  " << Buffer[m_index].specular[1] << "  " << Buffer[m_index].specular[2] << "  " << Buffer[m_index].specular[3] << std::endl;
	std::cout << "Position: " << Buffer[m_index].position[0] << "  " << Buffer[m_index].position[1] << "  " << Buffer[m_index].position[2] << std::endl;
	std::cout << "Direction: " << Buffer[m_index].direction[0] << "  " << Buffer[m_index].direction[1] << "  " << Buffer[m_index].direction[2] << std::endl;
	std::cout << "Angle: " << Buffer[m_index].angle << std::endl;
	std::cout << "Type: " << Buffer[m_index].type << std::endl;
	std::cout << "-------------------" << std::endl;
}

void Light::SetLights(const std::vector<Light*>& lights) {
	Lights = lights;
}

std::vector<Light*>& Light::GetLights() {
	return Lights;
}

Light* Light::AddLight(const LightBuffer& _light) {
	Lights.push_back(new Light());
	Light* light = Lights.back();

	int index = Lights.size() - 1;
	light->m_index = index;

	Buffer[index].ambient[0] = _light.ambient[0];
	Buffer[index].ambient[1] = _light.ambient[1];
	Buffer[index].ambient[2] = _light.ambient[2];
	Buffer[index].ambient[3] = _light.ambient[3];

	Buffer[index].diffuse[0] = _light.diffuse[0];
	Buffer[index].diffuse[1] = _light.diffuse[1];
	Buffer[index].diffuse[2] = _light.diffuse[2];
	Buffer[index].diffuse[3] = _light.diffuse[3];

	Buffer[index].specular[0] = _light.specular[0];
	Buffer[index].specular[1] = _light.specular[1];
	Buffer[index].specular[2] = _light.specular[2];
	Buffer[index].specular[3] = _light.specular[3];

	Buffer[index].position[0] = _light.position[0];
	Buffer[index].position[1] = _light.position[1];
	Buffer[index].position[2] = _light.position[2];

	Buffer[index].direction[0] = _light.direction[0];
	Buffer[index].direction[1] = _light.direction[1];
	Buffer[index].direction[2] = _light.direction[2];

	Buffer[index].angle = _light.angle;
	Buffer[index].enabled = _light.enabled;
	Buffer[index].type = _light.type;

	if (Buffer[index].type == SPOT_LIGHT)
		light->m_isStatic = false;

	return light;
}

void Light::UpdateLightUbo(unsigned int& ubo, size_t size) {
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer) * (size == 0 ? Lights.size() : size), &Buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Light::Print() {
	for (int i = 0; i < Lights.size(); i++) {
		std::cout << "Ambient: " << Buffer[i].ambient[0] << "  " << Buffer[i].ambient[1] << "  " << Buffer[i].ambient[2] << "  " << Buffer[i].ambient[3] << std::endl;
		std::cout << "Diffuse: " << Buffer[i].diffuse[0] << "  " << Buffer[i].diffuse[1] << "  " << Buffer[i].diffuse[2] << "  " << Buffer[i].diffuse[3] << std::endl;
		std::cout << "Specular: " << Buffer[i].specular[0] << "  " << Buffer[i].specular[1] << "  " << Buffer[i].specular[2] << "  " << Buffer[i].specular[3] << std::endl;
		std::cout << "Position: " << Buffer[i].position[0] << "  " << Buffer[i].position[1] << "  " << Buffer[i].position[2] << std::endl;
		std::cout << "Direction: " << Buffer[i].direction[0] << "  " << Buffer[i].direction[1] << "  " << Buffer[i].direction[2] << std::endl;
		std::cout << "Angle: " << Buffer[i].angle << std::endl;
		std::cout << "Type: " << Buffer[i].type << std::endl;
		std::cout << "-------------------" << std::endl;
	}
}

void Light::RemoveLight(size_t index) {
	Lights.erase(Lights.begin() + index);
}