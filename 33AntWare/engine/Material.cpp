#include <iostream>
#include <GL/glew.h>
#include "Material.h"

std::vector<Material> Material::Materials;
std::vector<Texture> Material::Textures;

bool operator== (const Material& m1, const Material& m2){
	return m1.textures.size() == m2.textures.size() 
		   && std::equal(m1.textures.begin(), m1.textures.end(), m2.textures.begin())
		   && m1.ambient[0] == m2.ambient[0] && m1.ambient[1] == m2.ambient[1] && m1.ambient[2] == m2.ambient[2] && m1.ambient[3] == m2.ambient[3]
		   && m1.diffuse[0] == m2.diffuse[0] && m1.diffuse[1] == m2.diffuse[1] && m1.diffuse[2] == m2.diffuse[2] && m1.diffuse[3] == m2.diffuse[3]
		   && m1.specular[0] == m2.specular[0] && m1.specular[1] == m2.specular[1] && m1.specular[2] == m2.specular[2] && m1.specular[3] == m2.specular[3]
		   && m1.shininess == m2.shininess;
}

void Material::cleanup() {
	
	std::vector<Material>::iterator position = std::find(Materials.begin(), Materials.end(), *this);
	if (position != Materials.end()) {
		(*position).textures.clear();
		//Materials.erase(position);
	}
}

const Texture& Material::getTexture(unsigned short index) const{
	return textures.at(index);
}

void Material::updateMaterialUbo(unsigned int& ubo) {
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, ambient);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, diffuse);
	glBufferSubData(GL_UNIFORM_BUFFER, 32, 16, specular);
	glBufferSubData(GL_UNIFORM_BUFFER, 48, 4, &shininess);
	glBufferSubData(GL_UNIFORM_BUFFER, 52, 4, &alpha);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Material::bind(){
	for (unsigned short i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures.at(i).getTexture());
	}
}

void Material::unbind(){
	for (unsigned short i = 0; textures.size() < 1; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Material::bind(unsigned short index) const {
	for (unsigned short i = 0; i < index; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures.at(i).getTexture());
	}
}

void Material::unbind(unsigned short index) const {
	for (unsigned short i = 0; i < index; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

std::vector<Material>& Material::GetMaterials() {
	return Materials;
}

void Material::Cleanup(unsigned short index) {
	Materials[index].textures.clear();
	Materials.erase(Materials.begin() + index);
}

Material& Material::AddMaterial(const Material& _material) {
	Materials.resize(Materials.size() + 1);
	Material& material = Materials.back();

	material.ambient[0] = _material.ambient[0];
	material.ambient[1] = _material.ambient[1];
	material.ambient[2] = _material.ambient[2];
	material.ambient[3] = _material.ambient[3];

	material.diffuse[0] = _material.diffuse[0];
	material.diffuse[1] = _material.diffuse[1];
	material.diffuse[2] = _material.diffuse[2];
	material.diffuse[3] = _material.diffuse[3];

	material.specular[0] = _material.specular[0];
	material.specular[1] = _material.specular[1];
	material.specular[2] = _material.specular[2];
	material.specular[3] = _material.specular[3];

	material.shininess = _material.shininess;
	return material;
}

void Material::print() {
	std::cout << "Ambient: " << ambient[0] << "  " << ambient[1] << "  " << ambient[2] << "  " << ambient[3] << std::endl;
	std::cout << "Diffuse: " << diffuse[0] << "  " << diffuse[1] << "  " << diffuse[2] << "  " << diffuse[3] << std::endl;
	std::cout << "Specular: " << specular[0] << "  " << specular[1] << "  " << specular[2] << "  " << specular[3] << std::endl;
	std::cout << "Shininess: " << shininess << std::endl;
	std::cout << "-------------------" << std::endl;
}

void Material::setAmbient(std::array<float, 4> _ambient) const {
	ambient[0] = _ambient[0];
	ambient[1] = _ambient[1];
	ambient[2] = _ambient[2];
	ambient[3] = _ambient[3];
}

void Material::setDiffuse(std::array<float, 4> _diffuse) const {
	diffuse[0] = _diffuse[0];
	diffuse[1] = _diffuse[1];
	diffuse[2] = _diffuse[2];
	diffuse[3] = _diffuse[3];
}

void Material::setSpecular(std::array<float, 4> _specular) const {
	specular[0] = _specular[0];
	specular[1] = _specular[1];
	specular[2] = _specular[2];
	specular[3] = _specular[3];
}

void Material::setShininess(float _shininess) const {
	shininess = _shininess;
}

void Material::setAlpha(float _alpha) const {
	alpha = _alpha;
}

const float Material::getAlpha() const {
	return alpha;
}

Texture& Material::AddTexture(std::string path) {
	Textures.resize(Textures.size() + 1);
	Texture& texture = Textures.back();
	texture.setDeepCopy(true);
	texture.loadFromFile(path, true);
	return texture;
}

std::vector<Texture>& Material::GetTextures() {
	return Textures;
}