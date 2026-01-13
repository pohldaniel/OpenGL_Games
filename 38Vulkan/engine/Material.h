#pragma once

#include <unordered_map>
#include <array>
#include <string>

enum TextureSlot {
	TEXTURE_DIFFUSE = 0,
	TEXTURE_NORMAL = 1,
	TEXTURE_SPECULAR = 2
};

struct MaterialBuffer {
	float ambient[4] = { 0.0f };
	float diffuse[4] = { 0.0f };
	float specular[4] = { 0.0f };
	float shininess = 0.0f;
	float alpha = 1.0f;
};

struct Material {

	friend bool operator== (const Material& m1, const Material& m2);

	void cleanup();
	void print();

	void setAmbient(std::array<float, 4> ambient) const;
	void setDiffuse(std::array<float, 4> diffuse) const;
	void setSpecular(std::array<float, 4> specular) const;
	void setShininess(float shininess) const;
	void setAlpha(float alpha) const;

	void addTexture(TextureSlot textureSlot,const std::string& texture);
	const std::unordered_map<TextureSlot, std::string>& getTextures() const;

	const float getAlpha() const;
	const float getShininess() const;

	mutable MaterialBuffer buffer;
	std::string name;
	std::unordered_map<TextureSlot, std::string> textures;

	static std::vector<Material>& GetMaterials();
	static void SetMaterials(const std::vector<Material>& materials);
	static std::vector<Material> Materials;
	static void Cleanup(unsigned short index);
	static void CleanupMaterials();
	static Material& AddMaterial(const MaterialBuffer& material =  { {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, 1.0f} );	
};