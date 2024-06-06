#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

class ObjModelNew {

public:
	ObjModelNew(const std::string& filepath);

	const std::vector<float>& GetVertices() const;
	const std::vector<unsigned int>& GetIndices() const;

	unsigned int vao, vbo, ebo;

private:
	std::vector<float> vertices; // Combined vertex positions, normals, and texture coordinates
	std::vector<unsigned int> indices; // Vertex indices

	void LoadObj(const std::string& filepath);

};