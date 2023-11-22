#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <array>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Vector.h"
#include "../Camera.h"
#include "../Transform.h"
#include "../Texture.h"


#include "Render/AnimationShader.h"
#include "AssimpAnimation.h"
#include "AssimpAnimator.h"

struct AssimpWeightData {
	unsigned int vertexId;
	float weight;
	unsigned int boneId;

	AssimpWeightData(unsigned int vertexId, float weight, unsigned int boneId) : vertexId(vertexId), weight(weight), boneId(boneId) {}

	bool operator<(const struct AssimpWeightData& other) const {

		return vertexId == other.vertexId ? weight < other.weight : vertexId > other.vertexId;
	}
};

struct AssimpBoneData {
	unsigned int index;
	std::string nameId;
	Matrix4f localBindTransform;
	Matrix4f offsteMatrix;

	std::vector<AssimpBoneData> children;


	AssimpBoneData(unsigned int _index, std::string _nameId, Matrix4f _localBindTransform, Matrix4f _offsteMatrix) {
		index = _index;
		nameId = _nameId;
		localBindTransform = _localBindTransform;
		offsteMatrix = _offsteMatrix;
	}

	AssimpBoneData() {}

	void addChild(AssimpBoneData child) {
		children.push_back(child);
	}
};

struct AssimpSkeletonData {

	int jointCount;
	AssimpBoneData headJoint;

	AssimpSkeletonData(int _jointCount, AssimpBoneData _headJoint) {
		jointCount = _jointCount;
		headJoint = _headJoint;
	}

	AssimpSkeletonData() {}
};

class AssimpAnimatedMesh;
class AssimpAnimatedModel : public std::enable_shared_from_this<AssimpAnimatedModel> {

public:

	struct AssimpBone {
		int index;
		std::string name = "";
		std::vector<AssimpBone> children;

		Matrix4f localBindTransform;
		Matrix4f inverseBindTransform;

		Matrix4f animatedTransform;
	};


	AssimpAnimatedModel();
	virtual ~AssimpAnimatedModel() {}

	void update(float elapsedTime);

	void addTwoAnimations(float deltaTime, std::string base, std::string layer, float speed = 1.0f);
	void addTwoAnimationsDisjoint(float deltaTime, std::string base, std::string layer, float speed = 1.0f);
	void blendTwoAnimations(float deltaTime, std::string base, std::string layer, float blendTime, float speed = 1.0f);
	void blendTwoAnimationsDisjoint(float deltaTime, std::string base, std::string layer, float blendTime, float speed = 1.0f);

	void draw(Camera& camera);
	void drawRaw();

	void loadModel(const std::string &filename, const std::string &texture);

	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix();

	void setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz);

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);

	AnimationShader	 m_shader;

	std::vector<AssimpAnimatedMesh*> m_meshes;
	std::shared_ptr<Texture> m_texture;
	
	std::shared_ptr<AssimpAnimator> getAnimator() { return m_animator; }
	
	int m_numberOfTriangles;
	AssimpBoneData fetchAiHierarchy(aiNode *node, std::vector<std::string>& boneList, std::vector<Matrix4f>& offsetMatrices);
	AssimpSkeletonData skeletonData;

	aiNode* searchNode(aiNode *node, std::vector<std::string> &boneList);

	void printSkeletonData(AssimpBoneData& boneData);
	void printAiHierarchy(aiNode *node);


private:
	Transform m_transform;

	std::shared_ptr<AssimpAnimator> m_animator;

	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texCoords;
	std::vector<Vector3f> m_normals;
	std::vector<std::array<unsigned int, 4>> m_jointIds;
	std::vector<Vector4f> m_jointWeights;
	std::vector<unsigned int> m_indexBuffer;

	static void CreateJoints(AssimpBone& bone, AssimpSkeletonData& skeletonData, std::vector<std::string> &boneList);
	static AssimpBone CreateJoints(AssimpBoneData data, Matrix4f parentBindTransform, std::vector<std::string> &boneList);
	static void CreateBuffer(std::vector<Vector3f>& positions, std::vector<Vector2f>& texCoords, std::vector<Vector3f>& normals, std::vector<std::array<unsigned int, 4>>& jointIds, std::vector<Vector4f>& jointWeights, std::vector<unsigned int>& indices, unsigned int& vao, unsigned int(&vbo)[5], unsigned int& ibo);
};

class AssimpAnimatedMesh {

	friend AssimpAnimatedModel;
	friend AssimpAnimator;

public:

	
	AssimpAnimatedMesh(AssimpAnimatedModel* model);
	virtual ~AssimpAnimatedMesh();

	void drawRaw();
	std::vector<Matrix4f> getBoneArray();
	void applyPoseToJoints(std::unordered_map<std::string, Matrix4f>& currentPose, std::vector<Matrix4f>& boneArray);

//private:

	void addJointsToArray(AssimpAnimatedModel::AssimpBone rootJoint, std::vector<Matrix4f> &boneArray);
	void applyPoseToJoints(std::unordered_map<std::string, Matrix4f>& currentPose, AssimpAnimatedModel::AssimpBone& joint, std::vector<Matrix4f>& boneArray, Matrix4f parentTransform);

	unsigned int m_vao = 0;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_ibo = 0;
	unsigned int m_drawCount;
	AssimpAnimatedModel* m_model;
	
	std::vector<std::string> m_boneList;

	AssimpAnimatedModel::AssimpBone m_rootBone;

	std::unordered_map<std::string, Matrix4f> currentPose;

	AssimpAnimatedModel::AssimpBone GetLocalTransform(std::string joint);
	AssimpAnimatedModel::AssimpBone GetLocalTransform(AssimpAnimatedModel::AssimpBone& joint, std::string jointName);
};