#include <GL/glew.h>
#include "Material.h"

std::vector<Material> Material::Materials;

bool operator== (const Material& m1, const Material& m2){
	return m1.textures.size() == m2.textures.size() 
		   && std::equal(m1.textures.begin(), m1.textures.end(), m2.textures.begin())
		   && m1.ambient[0] == m2.ambient[0] && m1.ambient[1] == m2.ambient[1] && m1.ambient[2] == m2.ambient[2] && m1.ambient[3] == m2.ambient[3]
		   && m1.diffuse[0] == m2.diffuse[0] && m1.diffuse[1] == m2.diffuse[1] && m1.diffuse[2] == m2.diffuse[2] && m1.diffuse[3] == m2.diffuse[3]
		   && m1.specular[0] == m2.specular[0] && m1.specular[1] == m2.specular[1] && m1.specular[2] == m2.specular[2] && m1.specular[3] == m2.specular[3]
		   && m1.shininess == m2.shininess;
}

Material::Material(const Material& rhs) {
	textures = rhs.textures;

	ambient[0] = rhs.ambient[0];
	ambient[1] = rhs.ambient[1];
	ambient[2] = rhs.ambient[2];
	ambient[3] = rhs.ambient[3];

	diffuse[0] = rhs.diffuse[0];
	diffuse[1] = rhs.diffuse[1];
	diffuse[2] = rhs.diffuse[2];
	diffuse[3] = rhs.diffuse[3];

	specular[0] = rhs.specular[0];
	specular[1] = rhs.specular[1];
	specular[2] = rhs.specular[2];
	specular[3] = rhs.specular[3];

	shininess = rhs.shininess;
	name = rhs.name;
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
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, &ambient);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, &diffuse);
	glBufferSubData(GL_UNIFORM_BUFFER, 32, 16, &specular);
	glBufferSubData(GL_UNIFORM_BUFFER, 48, 4, &shininess);
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