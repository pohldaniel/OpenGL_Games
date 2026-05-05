#pragma once

#include <vector>
#include <iterator>

enum ModelColor {
	MC_WHITE,
	MC_RED,
	MC_GREEN,
	MC_BLUE,
	MC_BLACK,
	MC_POSITION
};

class Mesh;
class Model {

public:

	virtual ~Model() = default;
	virtual const unsigned int getStride() const = 0;

protected:

	void static GenerateColors(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int& stride, unsigned int startIndex, unsigned int endIndex, ModelColor modelColor);
	void static PackBuffer(std::vector<float>& vertexBuffer, unsigned int stride);
	void static Rewind(const std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int stride);

	void static GenerateNormals(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, Model& model, bool& hasNormals, unsigned int& stride, unsigned int startIndex, unsigned int endIndex);
	void static GenerateNormals(std::vector<float>& vertexCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& normalCoords);
	void static GenerateTangents(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, Model& model, bool& hasNormals, bool& hasTangents, unsigned int& stride, unsigned int startIndex, unsigned int endIndex);
	void static GenerateTangents(std::vector<float>& vertexCoords, std::vector<float>& textureCoords, std::vector<float>& normalCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& tangentCoords, std::vector<float>& bitangentCoords);

	std::vector<Mesh*> m_meshes;

private:

	static std::array<float, 3> Normalize(const std::array<float, 3>& v);
	static std::array<float, 3> Cross(const std::array<float, 3>& p, const std::array<float, 3>& q);
	static float Dot(const std::array<float, 3>& p, const std::array<float, 3>& q);
};