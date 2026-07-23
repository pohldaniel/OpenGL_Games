#pragma once

#include <string>
#include <vector>
#include <functional>

#include <engine/Model.h>
#include <engine/Mesh.h>

#include "../Material.h"
#include "BoneDescription.h"
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
	friend class AnimationController;

public:

	AnimatedModel();
	virtual ~AnimatedModel();

	void update(float dt);
	void updateSkinning();
	void applyBindpose(bool onTransformChanged = false);
	void cleanup();

	void loadModelAssimp(const std::string& path, const short addVirtualRoots = 0, const bool reverseBoneList = false);
	void loadModel(const std::string& path, const short addVirtualRoots = 0);
	

	void rotate(const float pitch, const float yaw, const float roll);
	void scale(const float sx, const float sy, const float sz);
	void translate(const float dx, const float dy, const float dz);

	void setScale(const float sx, const float sy, const float sz);

	AnimationState* findAnimationState(const Animation& animation) const;
	AnimationState* findAnimationState(const std::string& name) const;
	AnimationState* addAnimationState(const Animation& animation);
	AnimationState* addAnimationStateFront(const Animation& animation);
	
	AnimationState* getAnimationState(size_t index) const;
	void removeAnimationState(const Animation& animation);
	void removeAnimationState(const AnimationState* state);
	void removeAllAnimationStates();
	
	const unsigned int getStride() const override;
	const Mesh* getMesh(unsigned short index = 0u) const;
	const std::vector<Mesh*>& getMeshes() const;
	Mesh* mesh(unsigned short index = 0u) const;

private:

	void OnAnimationOrderChanged();
	aiNode* searchNode(aiNode* node, std::vector<std::string>& boneList);
	void fetchAiHierarchy(aiNode* node, std::vector<BoneDescription>& boneDescriptions, int parentIndex = 0);
	void printAiHierarchy(aiNode* node);

	unsigned int m_numberOfTriangles, m_numberOfMeshes, m_stride;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents, m_hasMaterial;
	bool m_isStacked;

	bool m_animationOrderDirty, m_hasAnimationController;
	std::vector<std::shared_ptr<AnimationState>> m_animationStates;

	static bool CompareAnimationStates(const std::shared_ptr<AnimationState>& lhs, const std::shared_ptr<AnimationState>& rhs);
};

class AnimatedMesh : public Mesh {

	friend AnimatedModel;

public:

	AnimatedMesh(AnimatedModel* model);
	virtual ~AnimatedMesh();
	
	void updateSkinning();
	void applyBindpose(bool onTransformChanged = false);
	void createBones();
	void cleanup();

	void rotate(const float pitch, const float yaw, const float roll);
	void scale(const float sx, const float sy, const float sz);
	void translate(const float dx, const float dy, const float dz);

	void setScale(const float sx, const float sy, const float sz);

	const std::vector<BoneDescription>& getBoneDescriptions() const;
	const std::vector<std::array<float, 4>>& getWeights() const;
	const std::vector<std::array<unsigned int, 4>>& getJoints() const;
	const Matrix4f* getSkinMatrices() const;
	const unsigned short getNumBones() const;
	const bool hasMaterial() const;
	const Material& getMaterial() const;

	std::vector<BoneDescription>& boneDescriptions() const;
	std::vector<float>& vertexBuffer() const;
	std::vector<unsigned int>& indexBuffer() const;
	std::vector<std::array<float, 4>>& weights() const;
	std::vector<std::array<unsigned int, 4>>& joints() const;
	Matrix4f* skinMatrices() const;

	unsigned int& stride() const;
	Bone**& bones() const;
	const Bone& getBone(size_t index = 0u) const;
	const Matrix4f& getSkinMatrix(size_t index = 0u) const;

private:

	AnimatedModel* m_model;
	
	unsigned short m_numBones;
	Matrix4f* m_skinMatrices;
	mutable Bone** m_bones;
	Bone* m_rootBone;

	std::vector<std::string> m_boneList;
	mutable std::vector<std::array<float, 4>> m_weights;
	mutable std::vector<std::array<unsigned int, 4>> m_joints;
	mutable std::vector<BoneDescription> m_boneDescriptions;

	mutable short m_materialIndex;
	mutable short m_textureIndex;
};