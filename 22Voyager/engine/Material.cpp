#include <GL/glew.h>
#include "Material.h"

std::vector<Material> Material::Materials;

void Material::updateMaterialUbo(unsigned int& ubo) {
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, &ambient);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 32, &diffuse);
	glBufferSubData(GL_UNIFORM_BUFFER, 32, 48, &specular);
	glBufferSubData(GL_UNIFORM_BUFFER, 48, 52, &shininess);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Material::bind() {
	for (unsigned short i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].getTexture());
	}
}

void Material::unbind() {
	for (unsigned short i = 0; textures.size() < 1; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Material::bind(unsigned short index) {
	for (unsigned short i = 0; i < index; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].getTexture());
	}
}

void Material::unbind(unsigned short index) {
	for (unsigned short i = 0; i < index; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

std::vector<Material>& Material::GetMaterials() {
	return Materials;
}
