#include <iostream>
#include <GL/glew.h>
#include <engine/BuiltInShader.h>

#include "Light.h"

std::vector<Light> Light::Lights;
LightBuffer Light::Buffer[20];

/*bool operator== (const Light& l1, const Light& l2) {
	return l1.buffer.ambient[0] == l2.buffer.ambient[0] && l1.buffer.ambient[1] == l2.buffer.ambient[1] && l1.buffer.ambient[2] == l2.buffer.ambient[2] && l1.buffer.ambient[3] == l2.buffer.ambient[3]
		&& l1.buffer.diffuse[0] == l2.buffer.diffuse[0] && l1.buffer.diffuse[1] == l2.buffer.diffuse[1] && l1.buffer.diffuse[2] == l2.buffer.diffuse[2] && l1.buffer.diffuse[3] == l2.buffer.diffuse[3]
		&& l1.buffer.specular[0] == l2.buffer.specular[0] && l1.buffer.specular[1] == l2.buffer.specular[1] && l1.buffer.specular[2] == l2.buffer.specular[2] && l1.buffer.specular[3] == l2.buffer.specular[3]
		&& l1.buffer.position[0] == l2.buffer.position[0] && l1.buffer.position[1] == l2.buffer.position[1] && l1.buffer.position[2] == l2.buffer.position[2]
		&& l1.buffer.direction[0] == l2.buffer.direction[0] && l1.buffer.direction[1] == l2.buffer.direction[1] && l1.buffer.direction[2] == l2.buffer.direction[2]
		&& l1.buffer.angle == l2.buffer.angle;
}*/

Light::Light() : SceneNode() {

}

void Light::update(const float dt) {
	updateLightUbo(BuiltInShader::lightUbo, 20);
}

void Light::updateLightUbo(unsigned int& ubo, size_t size) {
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer) * size, &Buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Light::cleanup() {

	//std::vector<Light>::iterator ligt = std::find(Lights.begin(), Lights.end(), *this);
	//if (ligt != Lights.end()) {
	//	Lights.erase(ligt);
	//}
}

void Light::Print() {
	for (int i = 0; i < GetCount(); i++) {
		std::cout << "Ambient: " << Buffer[i].ambient[0] << "  " << Buffer[i].ambient[1] << "  " << Buffer[i].ambient[2] << "  " << Buffer[i].ambient[3] << std::endl;
		std::cout << "Diffuse: " << Buffer[i].diffuse[0] << "  " << Buffer[i].diffuse[1] << "  " << Buffer[i].diffuse[2] << "  " << Buffer[i].diffuse[3] << std::endl;
		std::cout << "Specular: " << Buffer[i].specular[0] << "  " << Buffer[i].specular[1] << "  " << Buffer[i].specular[2] << "  " << Buffer[i].specular[3] << std::endl;
		std::cout << "Position: " << Buffer[i].position[0] << "  " << Buffer[i].position[1] << "  " << Buffer[i].position[2] << std::endl;
		std::cout << "Direction: " << Buffer[i].direction[0] << "  " << Buffer[i].direction[1] << "  " << Buffer[i].direction[2] << std::endl;
		std::cout << "Angle: " << Buffer[i].angle << std::endl;
		std::cout << "-------------------" << std::endl;
	}
}

void Light::setAmbient(std::array<float, 4> ambient) const {
	int index = GetCount() - 1;
	Buffer[index].ambient[0] = ambient[0];
	Buffer[index].ambient[1] = ambient[1];
	Buffer[index].ambient[2] = ambient[2];
	Buffer[index].ambient[3] = ambient[3];
}

void Light::setDiffuse(std::array<float, 4> diffuse) const {
	int index = GetCount() - 1;
	Buffer[index].diffuse[0] = diffuse[0];
	Buffer[index].diffuse[1] = diffuse[1];
	Buffer[index].diffuse[2] = diffuse[2];
	Buffer[index].diffuse[3] = diffuse[3];
}

void Light::setSpecular(std::array<float, 4> specular) const {
	int index = GetCount() - 1;
	Buffer[index].specular[0] = specular[0];
	Buffer[index].specular[1] = specular[1];
	Buffer[index].specular[2] = specular[2];
	Buffer[index].specular[3] = specular[3];
}

void Light::setPosition(const Vector3f& position) {
	Object::setPosition(position);
	
	int index = GetCount() - 1;
	Buffer[index].position[0] = position[0];
	Buffer[index].position[1] = position[1];
	Buffer[index].position[2] = position[2];
}

void Light::setDirection(std::array<float, 3> direction) const {
	int index = GetCount() - 1;
	Buffer[index].direction[0] = direction[0];
	Buffer[index].direction[1] = direction[1];
	Buffer[index].direction[2] = direction[2];
}

void Light::setAngle(float _angle) const {
	int index = GetCount() - 1;
	Buffer[index].angle = _angle;
}

void Light::SetLights(const std::vector<Light>& lights) {
	Lights = lights;
}

std::vector<Light>& Light::GetLights() {
	return Lights;
}

Light& Light::AddLight(const LightBuffer& _light) {
	Lights.resize(Lights.size() + 1);
	Light& light = Lights.back();

	int index = GetCount() - 1;

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

	return light;
}
