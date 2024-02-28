#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <array>
#include <memory>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <engine/Vector.h>

#include "ModelBone.h"
#include "Bone.h"
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

class AnimatedMesh;
class AnimatedModel {

public:

	AnimatedModel();
	virtual ~AnimatedModel() {}

	void drawRaw();
	void update(float dt);
	void updateSkinning();

	void loadModelAssimp(const std::string& path, const bool addVirtualRoot = false, const bool reverseBoneList = false);
	void loadModelMdl(const std::string& path);

	AnimationState* addAnimationState(Animation* animation);
	AnimationState* getAnimationState(size_t index) const;
	AnimationState* findAnimationState(Animation* animation) const;
	AnimationState* findAnimationState(const std::string& animationName) const;
	AnimationState* findAnimationState(const char* animationName) const;
	AnimationState* findAnimationState(StringHash animationNameHash) const;
	void removeAnimationState(Animation* animation);
	void removeAnimationState(const std::string& animationName);
	void removeAnimationState(const char* animationName);
	void removeAnimationState(StringHash animationNameHash);
	void removeAnimationState(AnimationState* state);
	void removeAnimationState(size_t index);
	void removeAllAnimationStates();

	std::vector<AnimatedMesh*> m_meshes;
	bool m_hasAnimationController;
private:

	aiNode* searchNode(aiNode *node, std::vector<std::string> &boneList);
	void fetchAiHierarchy(aiNode *node, std::vector<ModelBone>& meshBones, int parentIndex = 0);
	static void CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& vao, unsigned int(&vbo)[3], unsigned int& ibo, unsigned int stride, std::vector<std::array<float, 4>>& weights, std::vector<std::array<unsigned int, 4>>& boneIds);
};

class AnimatedMesh {

	friend AnimatedModel;

public:

	AnimatedMesh(AnimatedModel* model);
	virtual ~AnimatedMesh();

	void drawRaw();
	void update(float dt);
	void updateSkinning();

	AnimatedModel* m_model;
	std::vector<std::shared_ptr<AnimationState>> m_animationStates;

	Bone* m_rootBone;
	Bone** m_bones;

	unsigned short m_numBones = 0;
	Matrix4f* m_skinMatrices;

	void createBones();

	unsigned int m_vao;
	unsigned int m_vbo[3];
	unsigned int m_vboInstances;
	unsigned int m_ibo;

	unsigned int m_drawCount;
	unsigned int m_instanceCount;
	bool m_markForDelete;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<std::array<unsigned int, 4>> m_boneIds;
	std::vector<std::array<float, 4>> m_weights;
	std::vector<std::string> m_boneList;
	std::vector<ModelBone> m_meshBones;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	unsigned int m_numberOfTriangles, m_stride, m_baseVertex, m_baseIndex;

	mutable short m_materialIndex;
	mutable short m_textureIndex;
};