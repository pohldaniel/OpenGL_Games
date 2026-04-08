#include <iostream>
#include "Material.h"

std::vector<Material> Material::Materials;

bool operator== (const Material& m1, const Material& m2) {
	return m1.m_buffer.ambient[0] == m2.m_buffer.ambient[0] && m1.m_buffer.ambient[1] == m2.m_buffer.ambient[1] && m1.m_buffer.ambient[2] == m2.m_buffer.ambient[2] && m1.m_buffer.ambient[3] == m2.m_buffer.ambient[3]
		&& m1.m_buffer.diffuse[0] == m2.m_buffer.diffuse[0] && m1.m_buffer.diffuse[1] == m2.m_buffer.diffuse[1] && m1.m_buffer.diffuse[2] == m2.m_buffer.diffuse[2] && m1.m_buffer.diffuse[3] == m2.m_buffer.diffuse[3]
		&& m1.m_buffer.specular[0] == m2.m_buffer.specular[0] && m1.m_buffer.specular[1] == m2.m_buffer.specular[1] && m1.m_buffer.specular[2] == m2.m_buffer.specular[2] && m1.m_buffer.specular[3] == m2.m_buffer.specular[3]
		&& m1.m_buffer.shininess == m2.m_buffer.shininess
		&& m1.m_buffer.alpha == m2.m_buffer.alpha;
}

void Material::cleanup() {

}

void Material::addTexture(TextureSlot textureSlot, const std::string& texture) {
	m_textures[textureSlot] = texture;
}

const bool Material::hasTexture(TextureSlot textureSlot) const {
	return m_textures.count(textureSlot);
}

const std::unordered_map<TextureSlot, std::string>& Material::getTextures() const {
	return m_textures;
}

void Material::print() {
	std::cout << "Ambient: " << m_buffer.ambient[0] << "  " << m_buffer.ambient[1] << "  " << m_buffer.ambient[2] << "  " << m_buffer.ambient[3] << std::endl;
	std::cout << "Diffuse: " << m_buffer.diffuse[0] << "  " << m_buffer.diffuse[1] << "  " << m_buffer.diffuse[2] << "  " << m_buffer.diffuse[3] << std::endl;
	std::cout << "Specular: " << m_buffer.specular[0] << "  " << m_buffer.specular[1] << "  " << m_buffer.specular[2] << "  " << m_buffer.specular[3] << std::endl;
	std::cout << "Shininess: " << m_buffer.shininess << std::endl;
	std::cout << "Alpha: " << m_buffer.alpha << std::endl;
	std::cout << "-------------------" << std::endl;
}

void Material::setAmbient(std::array<float, 4> _ambient) const {
	m_buffer.ambient[0] = _ambient[0];
	m_buffer.ambient[1] = _ambient[1];
	m_buffer.ambient[2] = _ambient[2];
	m_buffer.ambient[3] = _ambient[3];
}

void Material::setDiffuse(std::array<float, 4> _diffuse) const {
	m_buffer.diffuse[0] = _diffuse[0];
	m_buffer.diffuse[1] = _diffuse[1];
	m_buffer.diffuse[2] = _diffuse[2];
	m_buffer.diffuse[3] = _diffuse[3];
}

void Material::setSpecular(std::array<float, 4> _specular) const {
	m_buffer.specular[0] = _specular[0];
	m_buffer.specular[1] = _specular[1];
	m_buffer.specular[2] = _specular[2];
	m_buffer.specular[3] = _specular[3];
}

void Material::setShininess(float shininess) const {
	m_buffer.shininess = shininess;
}

const float Material::getShininess() const {
	return m_buffer.shininess;
}

void Material::setAlpha(float alpha) const {
	m_buffer.alpha = alpha;
}

const float Material::getAlpha() const {
	return m_buffer.alpha;
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

	material.m_buffer.ambient[0] = _material.ambient[0];
	material.m_buffer.ambient[1] = _material.ambient[1];
	material.m_buffer.ambient[2] = _material.ambient[2];
	material.m_buffer.ambient[3] = _material.ambient[3];

	material.m_buffer.diffuse[0] = _material.diffuse[0];
	material.m_buffer.diffuse[1] = _material.diffuse[1];
	material.m_buffer.diffuse[2] = _material.diffuse[2];
	material.m_buffer.diffuse[3] = _material.diffuse[3];

	material.m_buffer.specular[0] = _material.specular[0];
	material.m_buffer.specular[1] = _material.specular[1];
	material.m_buffer.specular[2] = _material.specular[2];
	material.m_buffer.specular[3] = _material.specular[3];

	material.m_buffer.shininess = _material.shininess;
	material.m_buffer.alpha = _material.alpha;
	return material;
}

std::string Material::GetTextureSlotString(TextureSlot textureSlot) {
	switch (textureSlot) {
		case TextureSlot::TEXTURE_DIFFUSE:
			return "diffuse";
			break;
		case TextureSlot::TEXTURE_NORMAL:
			return "normal";
			break;
		case TextureSlot::TEXTURE_SPECULAR:
			return "specular";
			break;
		case TextureSlot::TEXTURE_AMBIENT:
			return "ambient";
			break;
		case TextureSlot::TEXTURE_EMISSIVE:
			return "emission";
			break;
		case TextureSlot::TEXTURE_HEIGHT:
			return "height";
			break;
		case TextureSlot::TEXTURE_SHININESS:
			return "shininess";
			break;
		case TextureSlot::TEXTURE_OPACITY:
			return "opacity";
			break;
		case TextureSlot::TEXTURE_DISPLACEMENT:
			return "displacement";
			break;
		case TextureSlot::TEXTURE_LIGHTMAP:
			return "lightmap";
			break;
		case TextureSlot::TEXTURE_REFLECTION:
			return "reflection";
			break;
		case TextureSlot::TEXTURE_BASE_COLOR:
			return "diffuse";
			break;
		case TextureSlot::TEXTURE_NORMAL_CAMERA:
			return "normal";
			break;
		case TextureSlot::TEXTURE_EMISSION_COLOR:
			return "emission";
			break;
		case TextureSlot::TEXTURE_DIFFUSE_ROUGHNESS:
			return "roughness";
			break;
		case TextureSlot::TEXTURE_AMBIENT_OCCLUSION:
			return "ambient_occlussion";
			break;
		case TextureSlot::TEXTURE_METALNESS:
			return "metalness";
			break;
		case TextureSlot::TEXTURE_SHEEN:
			return "sheen";
			break;
		case TextureSlot::TEXTURE_CLEARCOAT:
			return "clearcoat";
			break;
		case TextureSlot::TEXTURE_TRANSMISSION:
			return "transmission";
			break;
		default:
			return "";
	}
}