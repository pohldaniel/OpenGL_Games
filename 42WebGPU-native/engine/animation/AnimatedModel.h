#pragma once

#include <string>
#include <vector>
#include <functional>

#include <engine/Model.h>
#include <engine/Mesh.h>

#include "MeshBone.h"
#include "AnimationState.h"

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
class Bone;
class AnimatedMesh;
class AnimatedModel : public Model {

	friend class AnimatedMesh;

public:

	AnimatedModel();
	virtual ~AnimatedModel();

	void update(float dt);
	void updateSkinning();

	void loadModelAssimp(const std::string& path, const short addVirtualRoots = 0, const bool reverseBoneList = false);
	const unsigned int getStride() const override;
	const Mesh* getMesh(unsigned short index = 0u) const;
	const std::vector<Mesh*>& getMeshes() const;

	AnimationState* addAnimationState(const Animation& animation);
	AnimationState* findAnimationState(const Animation& animation) const;
	AnimationState* getAnimationState(size_t index) const;
	void removeAnimationState(const Animation& animation);

	void rotate(const float pitch, const float yaw, const float roll);
	void scale(const float sx, const float sy, const float sz);
	void translate(const float dx, const float dy, const float dz);

private:

	void OnAnimationOrderChanged();
	aiNode* searchNode(aiNode* node, std::vector<std::string>& boneList);
	void fetchAiHierarchy(aiNode* node, std::vector<MeshBone>& meshBones, int parentIndex = 0);
	void printAiHierarchy(aiNode* node);

	unsigned int m_stride;
	bool m_hasTextureCoords, m_hasNormals, m_hasTangents, m_isStacked;

	bool m_animationOrderDirty;
	std::vector<std::shared_ptr<AnimationState>> m_animationStates;

	static bool CompareAnimationStates(const std::shared_ptr<AnimationState>& lhs, const std::shared_ptr<AnimationState>& rhs);
};

class AnimatedMesh : public Mesh {

	friend AnimatedModel;

public:

	AnimatedMesh(AnimatedModel* model);
	virtual ~AnimatedMesh();

	void update(float dt);
	void updateSkinning();
	void createBones();

	std::vector<MeshBone>& getMeshBones();

	const std::vector<std::array<float, 4>>& getWeights() const;
	const std::vector<std::array<unsigned int, 4>>& getJoints() const;
	const Matrix4f* getSkinMatrices() const;
	const unsigned short getNumBones() const;

	void rotate(const float pitch, const float yaw, const float roll);
	void scale(const float sx, const float sy, const float sz);
	void translate(const float dx, const float dy, const float dz);

private:

	AnimatedModel* m_model;
	
	unsigned short m_numBones = 0;
	Matrix4f* m_skinMatrices;

	Bone* m_rootBone;
	Bone** m_bones;

	std::vector<std::array<float, 4>> m_weights;
	std::vector<std::array<unsigned int, 4>> m_joints;
	std::vector<std::string> m_boneList;
	std::vector<MeshBone> m_meshBones;

	mutable short m_materialIndex;
	mutable short m_textureIndex;
};