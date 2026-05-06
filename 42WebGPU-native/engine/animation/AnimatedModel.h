#pragma once

#include <string>
#include <vector>

#include <engine/Model.h>
#include <engine/Mesh.h>

#include "MeshBone.h"

struct WeightData {
	unsigned int vertexId;
	float weight;
	unsigned int boneId;

	WeightData(unsigned int vertexId, float weight, unsigned int boneId) : vertexId(vertexId), weight(weight), boneId(boneId) {}

	bool operator<(const struct WeightData& other) const {
		return vertexId == other.vertexId ? weight < other.weight : vertexId > other.vertexId;
	}
};

struct aiNode;
class AnimatedMesh;
class AnimatedModel : public Model {

	friend class AnimatedMesh;

public:

	AnimatedModel();
	virtual ~AnimatedModel();

	void loadModelAssimp(const std::string& path, const short addVirtualRoots = 0, const bool reverseBoneList = false);
	const unsigned int getStride() const override;
	const Mesh* getMesh(unsigned short index = 0u) const;
	const std::vector<Mesh*>& getMeshes() const;

private:

	aiNode* searchNode(aiNode* node, std::vector<std::string>& boneList);
	void fetchAiHierarchy(aiNode* node, std::vector<MeshBone>& meshBones, int parentIndex = 0);

	unsigned int m_stride;
	bool m_hasTextureCoords, m_hasNormals, m_hasTangents, m_isStacked;
};

class AnimatedMesh : public Mesh {

	friend AnimatedModel;

public:

	AnimatedMesh(AnimatedModel* model);
	virtual ~AnimatedMesh();

	std::vector<MeshBone>& getMeshBones();

	const std::vector<std::array<float, 4>>& getWeights() const;
	const std::vector<std::array<unsigned int, 4>>& getJoints() const;

private:

	AnimatedModel* m_model;

	std::vector<std::array<float, 4>> m_weights;
	std::vector<std::array<unsigned int, 4>> m_joints;
	std::vector<std::string> m_boneList;
	std::vector<MeshBone> m_meshBones;

	mutable short m_materialIndex;
	mutable short m_textureIndex;
};