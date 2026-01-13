#include <iostream>
#include "Material.h"

std::vector<Material> Material::Materials;

bool operator== (const Material& m1, const Material& m2) {
	return m1.buffer.ambient[0] == m2.buffer.ambient[0] && m1.buffer.ambient[1] == m2.buffer.ambient[1] && m1.buffer.ambient[2] == m2.buffer.ambient[2] && m1.buffer.ambient[3] == m2.buffer.ambient[3]
		&& m1.buffer.diffuse[0] == m2.buffer.diffuse[0] && m1.buffer.diffuse[1] == m2.buffer.diffuse[1] && m1.buffer.diffuse[2] == m2.buffer.diffuse[2] && m1.buffer.diffuse[3] == m2.buffer.diffuse[3]
		&& m1.buffer.specular[0] == m2.buffer.specular[0] && m1.buffer.specular[1] == m2.buffer.specular[1] && m1.buffer.specular[2] == m2.buffer.specular[2] && m1.buffer.specular[3] == m2.buffer.specular[3]
		&& m1.buffer.shininess == m2.buffer.shininess
		&& m1.buffer.alpha == m2.buffer.alpha;
}

void Material::cleanup() {

}

void Material::addTexture(TextureSlot textureSlot, const std::string& texture) {
	textures[textureSlot] = texture;
}

const std::unordered_map<TextureSlot, std::string>& Material::getTextures() const {
	return textures;
}

void Material::print() {
	std::cout << "Ambient: " << buffer.ambient[0] << "  " << buffer.ambient[1] << "  " << buffer.ambient[2] << "  " << buffer.ambient[3] << std::endl;
	std::cout << "Diffuse: " << buffer.diffuse[0] << "  " << buffer.diffuse[1] << "  " << buffer.diffuse[2] << "  " << buffer.diffuse[3] << std::endl;
	std::cout << "Specular: " << buffer.specular[0] << "  " << buffer.specular[1] << "  " << buffer.specular[2] << "  " << buffer.specular[3] << std::endl;
	std::cout << "Shininess: " << buffer.shininess << std::endl;
	std::cout << "Alpha: " << buffer.alpha << std::endl;
	std::cout << "-------------------" << std::endl;
}

void Material::setAmbient(std::array<float, 4> _ambient) const {
	buffer.ambient[0] = _ambient[0];
	buffer.ambient[1] = _ambient[1];
	buffer.ambient[2] = _ambient[2];
	buffer.ambient[3] = _ambient[3];
}

void Material::setDiffuse(std::array<float, 4> _diffuse) const {
	buffer.diffuse[0] = _diffuse[0];
	buffer.diffuse[1] = _diffuse[1];
	buffer.diffuse[2] = _diffuse[2];
	buffer.diffuse[3] = _diffuse[3];
}

void Material::setSpecular(std::array<float, 4> _specular) const {
	buffer.specular[0] = _specular[0];
	buffer.specular[1] = _specular[1];
	buffer.specular[2] = _specular[2];
	buffer.specular[3] = _specular[3];
}

void Material::setShininess(float shininess) const {
	buffer.shininess = shininess;
}

const float Material::getShininess() const {
	return buffer.shininess;
}

void Material::setAlpha(float alpha) const {
	buffer.alpha = alpha;
}

const float Material::getAlpha() const {
	return buffer.alpha;
}

void Material::SetMaterials(const std::vector<Material>& materials) {
	Materials = materials;
}

std::vector<Material>& Material::GetMaterials() {
	return Materials;
}

void Material::Cleanup(unsigned short index) {
	Materials.erase(Materials.begin() + index);
}

void Material::CleanupMaterials() {
	Materials.clear();
}

Material& Material::AddMaterial(const MaterialBuffer& _material) {
	Materials.resize(Materials.size() + 1);
	Material& material = Materials.back();

	material.buffer.ambient[0] = _material.ambient[0];
	material.buffer.ambient[1] = _material.ambient[1];
	material.buffer.ambient[2] = _material.ambient[2];
	material.buffer.ambient[3] = _material.ambient[3];

	material.buffer.diffuse[0] = _material.diffuse[0];
	material.buffer.diffuse[1] = _material.diffuse[1];
	material.buffer.diffuse[2] = _material.diffuse[2];
	material.buffer.diffuse[3] = _material.diffuse[3];

	material.buffer.specular[0] = _material.specular[0];
	material.buffer.specular[1] = _material.specular[1];
	material.buffer.specular[2] = _material.specular[2];
	material.buffer.specular[3] = _material.specular[3];

	material.buffer.shininess = _material.shininess;
	material.buffer.alpha = _material.alpha;
	return material;
}