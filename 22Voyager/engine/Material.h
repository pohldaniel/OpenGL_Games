#ifndef __materialH__
#define __materialH__

#include <unordered_map>
#include "engine/Texture.h"

struct Material {

	friend bool operator== (const Material& m1, const Material& m2);

	float ambient[4] = {0.0f};
	float diffuse[4] = { 0.0f };
	float specular[4] = { 0.0f };
	float shininess =  0.0f;
	//avoid unwanted copy costructor calls using std::unordered_map over std::vector
	std::unordered_map<unsigned short, Texture> textures;
	void updateMaterialUbo(unsigned int& ubo);
	void bind();
	void bind(unsigned short index);
	void unbind();
	void unbind(unsigned short index);
	void cleanup();

	static std::vector<Material>& GetMaterials();
	static std::vector<Material> Materials;
	static void Cleanup(unsigned short index);
};
#endif