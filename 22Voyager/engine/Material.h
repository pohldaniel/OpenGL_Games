#ifndef __materialH__
#define __materialH__

#include <unordered_map>
#include "engine/Texture.h"

struct Material {
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float shininess;
	//avoid unwanted copy costructor calls using std::unordered_ma over std::vector
	std::unordered_map<unsigned short, Texture> textures;
	void updateMaterialUbo(unsigned int& ubo);
	void bind();
	void bind(unsigned short index);
	void unbind();
	void unbind(unsigned short index);

	static std::vector<Material>& GetMaterials();

	static std::vector<Material> Materials;
};
#endif