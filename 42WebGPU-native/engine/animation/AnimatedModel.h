#pragma once
#include <vector>

struct WeightData {
	unsigned int vertexId;
	float weight;
	unsigned int boneId;

	WeightData(unsigned int vertexId, float weight, unsigned int boneId) : vertexId(vertexId), weight(weight), boneId(boneId) {}

	bool operator<(const struct WeightData& other) const {
		return vertexId == other.vertexId ? weight < other.weight : vertexId > other.vertexId;
	}
};

class AnimatedMesh;
class AnimatedModel {

	friend class AnimatedMesh;

public:

	AnimatedModel();
	virtual ~AnimatedModel();

private:

	mutable std::vector<AnimatedMesh*> m_meshes;

};

class AnimatedMesh {

	friend AnimatedModel;

public:

	AnimatedMesh(AnimatedModel* model);
	virtual ~AnimatedMesh();

private:

	AnimatedModel* m_model;
};