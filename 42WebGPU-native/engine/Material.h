#ifndef __materialH__
#define __materialH__

#include <unordered_map>
#include <array>
#include <string>

enum TextureSlot {
	TEXTURE_DIFFUSE = 0u,
	TEXTURE_NORMAL = 1u,
	TEXTURE_SPECULAR = 2u,
	TEXTURE_AMBIENT = 3u,
	TEXTURE_EMISSIVE = 4u,
	TEXTURE_HEIGHT = 5u,

	TEXTURE_SHININESS = 6u,
	TEXTURE_OPACITY = 7u,
	TEXTURE_DISPLACEMENT = 8u,
	TEXTURE_LIGHTMAP = 9u,
	TEXTURE_REFLECTION = 10u,

	TEXTURE_BASE_COLOR = 11u,
	TEXTURE_NORMAL_CAMERA = 12u,
	TEXTURE_EMISSION_COLOR = 13u,
	TEXTURE_METALNESS = 14u,
	TEXTURE_DIFFUSE_ROUGHNESS = 15u,
	TEXTURE_AMBIENT_OCCLUSION = 16u,

	TEXTURE_SHEEN = 17u,
	TEXTURE_CLEARCOAT = 18u,
	TEXTURE_TRANSMISSION = 19u
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

	void addTexture(TextureSlot textureSlot, const std::string& texture);
	const bool hasTexture(TextureSlot textureSlot) const;
	const std::unordered_map<TextureSlot, std::string>& getTextures() const;
	
	const float getAlpha() const;
	const float getShininess() const;

	mutable MaterialBuffer m_buffer;
	std::string m_name;
	std::unordered_map<TextureSlot, std::string> m_textures;
	
	static std::vector<Material>& GetMaterials();
	static void SetMaterials(const std::vector<Material>& materials);	
	static void Cleanup(unsigned short index);
	static void CleanupMaterials();
	static Material& AddMaterial(const MaterialBuffer& material = { {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, 1.0f });
	static std::string GetTextureSlotString(TextureSlot textureSlot);

	static std::vector<Material> Materials;
};
#endif