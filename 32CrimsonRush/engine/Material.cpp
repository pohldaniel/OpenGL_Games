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
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 32, &diffuse);
	glBufferSubData(GL_UNIFORM_BUFFER, 32, 48, &specular);
	glBufferSubData(GL_UNIFORM_BUFFER, 48, 52, &shininess);
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