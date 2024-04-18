#ifndef __materialH__
#define __materialH__

#include <unordered_map>
#include <array>
#include "Texture.h"

struct MaterialBuffer {
	float ambient[4] = { 0.0f };
	float diffuse[4] = { 0.0f };
	float specular[4] = { 0.0f };
	float shininess = 0.0f;
	float alpha = 1.0f;
};

struct Material {

	friend bool operator== (const Material& m1, const Material& m2);


	//avoid unwanted copy costructor calls using std::unordered_map over std::vector
	std::unordered_map<unsigned short, Texture> textures;
	void updateMaterialUbo(unsigned int& ubo);
	void bind();
	void bind(unsigned short index = 1u) const;
	void unbind();
	void unbind(unsigned short index = 1u) const;
	void cleanup();
	void print();

	void setAmbient(std::array<float, 4> ambient) const;
	void setDiffuse(std::array<float, 4> diffuse) const;
	void setSpecular(std::array<float, 4> specular) const;
	void setShininess(float shininess) const;
	void setAlpha(float alpha) const;

	const float getAlpha() const;
	const float getShininess() const;

	void addTexture(const Texture& texture, unsigned short index);
	const Texture& getTexture(unsigned short index) const;

	mutable MaterialBuffer buffer;
	std::string name;

	static std::vector<Material>& GetMaterials();
	static void SetMaterials(const std::vector<Material>& materials);
	static std::vector<Material> Materials;
	static void Cleanup(unsigned short index);
	static void CleanupMaterials();
	static Material& AddMaterial(const MaterialBuffer& material =  { {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, 1.0f} );

	static Texture& AddTexture(std::string path);
	static std::vector<Texture>& GetTextures();
	static void SetTextures(const std::vector<Texture>& textures);
	static void CleanupTextures();
	static std::vector<Texture> Textures;
};

#endif