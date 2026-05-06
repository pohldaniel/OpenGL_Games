#include <iostream>
#include <queue>
#include <array>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AnimatedModel.h"

AnimatedModel::AnimatedModel() : m_isStacked(false), m_stride(0u){

}

AnimatedModel::~AnimatedModel() {
	for (Mesh* mesh : m_meshes) {
		delete mesh;
	}
}

void AnimatedModel::loadModelAssimp(const std::string& path, const short addVirtualRoots, const bool reverseBoneList) {

	bool exportTangents = false;

	Assimp::Importer Importer;
	Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	Importer.SetPropertyBool(AI_CONFIG_IMPORT_REMOVE_EMPTY_BONES, false);

	const aiScene* pScene = Importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!pScene) {
		std::cout << path << "  " << Importer.GetErrorString() << std::endl;
		return;
	}

	std::priority_queue<WeightData> pq;

	unsigned short maxBones = 4;
	AnimatedMesh* mesh;

	for (unsigned int j = 0; j < pScene->mNumMeshes; j++) {

		const aiMesh* aiMesh = pScene->mMeshes[j];
		const aiMaterial* aiMaterial = pScene->mMaterials[aiMesh->mMaterialIndex];

		m_meshes.push_back(new AnimatedMesh(this));
		mesh = static_cast<AnimatedMesh*>(m_meshes.back());

		m_isStacked ? m_hasTextureCoords = aiMesh->HasTextureCoords(0) : mesh->m_hasTextureCoords = aiMesh->HasTextureCoords(0);
		m_isStacked ? m_hasNormals = aiMesh->HasNormals() : mesh->m_hasNormals = aiMesh->HasNormals();
		m_isStacked ? m_hasTangents = aiMesh->HasTangentsAndBitangents()  : mesh->m_hasTangents = aiMesh->HasTangentsAndBitangents() && exportTangents;

		m_isStacked ? m_stride = m_hasTangents ? 14 : (m_hasNormals && m_hasTextureCoords) ? 8 : m_hasNormals ? 6 : m_hasTextureCoords ? 5 : 3
			: mesh->m_stride = mesh->m_hasTangents ? 14 : (mesh->m_hasNormals && mesh->m_hasTextureCoords) ? 8 : mesh->m_hasNormals ? 6 : mesh->m_hasTextureCoords ? 5 : 3;

		for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) {
			mesh->m_vertexBuffer.push_back(aiMesh->mVertices[i].x); mesh->m_vertexBuffer.push_back(aiMesh->mVertices[i].y); mesh->m_vertexBuffer.push_back(aiMesh->mVertices[i].z);
			mesh->m_vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].x); mesh->m_vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].y);
			mesh->m_vertexBuffer.push_back(aiMesh->mNormals[i].x); mesh->m_vertexBuffer.push_back(aiMesh->mNormals[i].y); mesh->m_vertexBuffer.push_back(aiMesh->mNormals[i].z);
		}

		if (aiMesh->HasBones()) {

			mesh->m_meshBones.resize(aiMesh->mNumBones);

			if (reverseBoneList) {
				for (int boneIndex = aiMesh->mNumBones - 1; boneIndex >= 0; boneIndex--) {
					MeshBone& _bone = mesh->m_meshBones[(aiMesh->mNumBones - 1) - boneIndex];

					aiBone* bone = aiMesh->mBones[boneIndex];
					const std::string boneName = bone->mName.C_Str();

					mesh->m_boneList.push_back(boneName);

					_bone.name = boneName;
					_bone.offsetMatrix.set(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1,
						bone->mOffsetMatrix.a2, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.d2,
						bone->mOffsetMatrix.a3, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.d3,
						bone->mOffsetMatrix.a4, bone->mOffsetMatrix.b4, bone->mOffsetMatrix.c4, bone->mOffsetMatrix.d4);

					for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++) {
						aiVertexWeight w = bone->mWeights[weightIndex];
						pq.push(WeightData(w.mVertexId, w.mWeight, (aiMesh->mNumBones - 1) - boneIndex));
					}
				}
			}else {
				for (unsigned int boneIndex = 0; boneIndex < aiMesh->mNumBones; boneIndex++) {
					MeshBone& _bone = mesh->m_meshBones[boneIndex];

					aiBone* bone = aiMesh->mBones[boneIndex];
					const std::string boneName = bone->mName.C_Str();
					mesh->m_boneList.push_back(boneName);

					_bone.name = boneName;
					_bone.offsetMatrix.set(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1,
						bone->mOffsetMatrix.a2, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.d2,
						bone->mOffsetMatrix.a3, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.d3,
						bone->mOffsetMatrix.a4, bone->mOffsetMatrix.b4, bone->mOffsetMatrix.c4, bone->mOffsetMatrix.d4);

					for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++) {
						aiVertexWeight w = bone->mWeights[weightIndex];
						pq.push(WeightData(w.mVertexId, w.mWeight, boneIndex));
					}
				}
			}

			WeightData first = pq.top();
			short k = -1;

			std::array<unsigned int, 4> jointId = { 0, 0, 0, 0 };
			std::array<float, 4> jointWeight = { 0.0f, 0.0f, 0.0f, 0.0f };

			while (!pq.empty()) {
				WeightData current = pq.top();

				if (first.vertexId == current.vertexId) {
					k++;
				}
				else {
					mesh->m_weights.push_back(jointWeight);
					mesh->m_joints.push_back(jointId);

					jointId = { 0, 0, 0, 0 };
					jointWeight = { 0.0f, 0.0f, 0.0f, 0.0f };
					k = 0;
					first = current;
				}

				if (k < maxBones) {
					jointWeight[k] = pq.top().weight;
					jointId[k] = pq.top().boneId + addVirtualRoots;
				}
				pq.pop();
			}

			mesh->m_weights.push_back(jointWeight);
			mesh->m_joints.push_back(jointId);

			aiNode* meshRootNode = searchNode(pScene->mRootNode, mesh->m_boneList);

			std::vector<MeshBone>::iterator it = std::find_if(mesh->m_meshBones.begin(), mesh->m_meshBones.end(), [meshRootNode](MeshBone& meshBone) { return strcmp(meshRootNode->mName.C_Str(), meshBone.name.c_str()) == 0; });

			fetchAiHierarchy(meshRootNode, mesh->m_meshBones, static_cast<int>(std::distance(mesh->m_meshBones.begin(), it)));
		}

		for (unsigned int t = 0; t < aiMesh->mNumFaces; ++t) {
			const aiFace* face = &aiMesh->mFaces[t];
			mesh->m_indexBuffer.push_back(face->mIndices[0]);
			mesh->m_indexBuffer.push_back(face->mIndices[1]);
			mesh->m_indexBuffer.push_back(face->mIndices[2]);
		}

		mesh->m_drawCount = aiMesh->mNumFaces * 3;

		if (addVirtualRoots) {

			for (size_t i = 0; i < mesh->m_meshBones.size(); ++i) {
				MeshBone& meshBone = mesh->m_meshBones[i];
				if (meshBone.parentIndex != i) {
					meshBone.parentIndex = meshBone.parentIndex + addVirtualRoots;
				}
				else {
					meshBone.parentIndex = (addVirtualRoots - 1);
				}
			}

			for (unsigned short count = 0; count < addVirtualRoots; count++) {
				mesh->m_meshBones.insert(mesh->m_meshBones.begin(), MeshBone());
				mesh->m_meshBones[0].name = "Root_" + (addVirtualRoots - 1) - count;
				if (count + 1 != addVirtualRoots)
					mesh->m_meshBones[0].parentIndex = (addVirtualRoots - 1) - count - 1;
			}
		}
	}
}

aiNode* AnimatedModel::searchNode(aiNode* node, std::vector<std::string>& boneList) {
	std::vector<std::string>::iterator it = std::find(boneList.begin(), boneList.end(), node->mName.C_Str());

	if (it != boneList.end()) {
		return node;
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		aiNode* result = searchNode(node->mChildren[i], boneList);
		if (result) {
			return result;
		}
	}

	return nullptr;
}

void AnimatedModel::fetchAiHierarchy(aiNode* node, std::vector<MeshBone>& meshBones, int parentIndex) {
	aiMatrix4x4 transMatrix = node->mTransformation;
	std::vector<MeshBone>::iterator it = std::find_if(meshBones.begin(), meshBones.end(), [node](MeshBone& meshBone) { return strcmp(node->mName.C_Str(), meshBone.name.c_str()) == 0; });
	int _parentIndex = -1;

	if (it != meshBones.end()) {
		aiVector3D pos, scale;
		aiQuaternion rot;
		transMatrix.Decompose(scale, rot, pos);

		(*it).initialPosition.set(pos.x, pos.y, pos.z);
		(*it).initialRotation.set(rot.x, rot.y, rot.z, rot.w);
		(*it).initialScale.set(scale.x, scale.y, scale.z);

		(*it).parentIndex = parentIndex;
		_parentIndex = static_cast<int>(std::distance(meshBones.begin(), it));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		fetchAiHierarchy(node->mChildren[i], meshBones, _parentIndex);
	}
}

const unsigned int AnimatedModel::getStride() const {
	return m_isStacked ? m_stride : m_meshes.back()->getStride();
}

const Mesh* AnimatedModel::getMesh(unsigned short index) const {
	return m_meshes[index];
}

const std::vector<Mesh*>& AnimatedModel::getMeshes() const {
	return m_meshes;
}

AnimatedMesh::AnimatedMesh(AnimatedModel* model) : m_model(model) {
	m_model = model;
}

AnimatedMesh::~AnimatedMesh() {

}

std::vector<MeshBone>& AnimatedMesh::getMeshBones() {
	return m_meshBones;
}

const std::vector<std::array<float, 4>>& AnimatedMesh::getWeights() const {
	return m_weights;
}

const std::vector<std::array<unsigned int, 4>>& AnimatedMesh::getJoints() const {
	return m_joints;
}