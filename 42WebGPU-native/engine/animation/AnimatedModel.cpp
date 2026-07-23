#include <queue>
#include <array>
#include <functional>
#include <iostream>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <engine/utils/BinaryIO.h>

#include "Bone.h"
#include "AnimatedModel.h"

bool AnimatedModel::CompareAnimationStates(const std::shared_ptr<AnimationState>& lhs, const std::shared_ptr<AnimationState>& rhs) {
	return lhs->getBlendLayer() < rhs->getBlendLayer();
}

AnimatedModel::AnimatedModel() {
	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_hasMaterial = false;
	m_isStacked = false;
	m_animationOrderDirty = true;
	m_hasAnimationController = false;

	m_numberOfTriangles = 0u;
	m_numberOfMeshes = 0u;
	m_stride = 0u;
}

AnimatedModel::~AnimatedModel() {
	cleanup();
}

void AnimatedModel::cleanup() {
	for (Mesh* mesh : m_meshes) {
		delete mesh;
	}
	m_meshes.clear();
	m_meshes.shrink_to_fit();
}

void AnimatedModel::update(float dt) {
	if (m_animationOrderDirty) {
		std::sort(m_animationStates.begin(), m_animationStates.end(), AnimatedModel::CompareAnimationStates);
		m_animationOrderDirty = false;
	}

	for (auto it = m_animationStates.begin(); it != m_animationStates.end(); ++it) {
		AnimationState* state = (*it).get();
		if (m_hasAnimationController) {			
			if (state->isEnabled()) {
				state->apply();
			}
		}else {

			if (state->isEnabled() || state->getAnimationBlendMode() == ABM_FADE) {
				state->addTime(dt);
				state->apply();
			}
		}
	}
}

void AnimatedModel::updateSkinning() {
	for (std::vector<Mesh*>::iterator mesh = m_meshes.begin(); mesh != m_meshes.end(); mesh++) {
		static_cast<AnimatedMesh*>(*mesh)->updateSkinning();
	}
}

void AnimatedModel::applyBindpose(bool onTransformChanged) {
	for (std::vector<Mesh*>::iterator mesh = m_meshes.begin(); mesh != m_meshes.end(); mesh++) {
		static_cast<AnimatedMesh*>(*mesh)->applyBindpose(onTransformChanged);
	}
}

void AnimatedModel::OnAnimationOrderChanged() {
	m_animationOrderDirty = true;
}

void AnimatedModel::loadModel(const std::string& path, const short addVirtualRoots) {
	Utils::MdlcIO mdlcIO;

	m_meshes.push_back(new AnimatedMesh(this));
	AnimatedMesh* mesh = static_cast<AnimatedMesh*>(m_meshes.back());
	mdlcIO.mdlcModelToBuffer(path.c_str(), mesh->vertexBuffer(), mesh->indexBuffer(), mesh->stride(), mesh->weights(), mesh->joints(), mesh->boneDescriptions());

	if (addVirtualRoots) {

		for (size_t i = 0u; i < mesh->m_boneDescriptions.size(); ++i) {
			BoneDescription& boneDescription = mesh->m_boneDescriptions[i];
			if (boneDescription.parentIndex != i) {
				boneDescription.parentIndex = boneDescription.parentIndex + addVirtualRoots;
			}else {
				boneDescription.parentIndex = (addVirtualRoots - 1);
			}
		}

		for (unsigned short count = 0; count < addVirtualRoots; count++) {
			mesh->m_boneDescriptions.insert(mesh->m_boneDescriptions.begin(), BoneDescription());
			mesh->m_boneDescriptions[0].name = "Root_" + std::to_string((addVirtualRoots - 1) - count);
			if (count + 1 != addVirtualRoots)
				mesh->m_boneDescriptions[0].parentIndex = (addVirtualRoots - 1) - count - 1;
		}

		for (size_t i = 0u; i < mesh->joints().size(); ++i) {
			mesh->joints()[i][0] = mesh->getWeights()[i][0] != 0.0f ? mesh->getJoints()[i][0] + addVirtualRoots : mesh->getJoints()[i][0];
			mesh->joints()[i][1] = mesh->getWeights()[i][1] != 0.0f ? mesh->getJoints()[i][1] + addVirtualRoots : mesh->getJoints()[i][1];
			mesh->joints()[i][2] = mesh->getWeights()[i][2] != 0.0f ? mesh->getJoints()[i][2] + addVirtualRoots : mesh->getJoints()[i][2];
			mesh->joints()[i][3] = mesh->getWeights()[i][3] != 0.0f ? mesh->getJoints()[i][3] + addVirtualRoots : mesh->getJoints()[i][3];
		}
	}
	mesh->createBones();
}

void AnimatedModel::loadModelAssimp(const std::string& path, const short addVirtualRoots, const bool reverseBoneList) {

	bool exportTangents = false;

	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

	const aiScene* pScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!pScene) {
		std::cout << path << "  " << importer.GetErrorString() << std::endl;
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
			mesh->m_boneDescriptions.resize(aiMesh->mNumBones);
			if (reverseBoneList) {
				for (int boneIndex = aiMesh->mNumBones - 1; boneIndex >= 0; boneIndex--) {
					BoneDescription& boneDescription = mesh->m_boneDescriptions[(aiMesh->mNumBones - 1) - boneIndex];

					aiBone* bone = aiMesh->mBones[boneIndex];
					const std::string boneName = bone->mName.C_Str();

					mesh->m_boneList.push_back(boneName);

					boneDescription.name = boneName;
					boneDescription.offsetMatrix.set(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1,
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
					BoneDescription& boneDescription = mesh->m_boneDescriptions[boneIndex];

					aiBone* bone = aiMesh->mBones[boneIndex];
					const std::string boneName = bone->mName.C_Str();
					mesh->m_boneList.push_back(boneName);

					boneDescription.name = boneName;
					boneDescription.offsetMatrix.set(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1,
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
				}else {
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
			//printAiHierarchy(pScene->mRootNode);
			aiNode* meshRootNode = searchNode(pScene->mRootNode, mesh->m_boneList);
			std::vector<BoneDescription>::iterator it = std::find_if(mesh->m_boneDescriptions.begin(), mesh->m_boneDescriptions.end(), [meshRootNode](BoneDescription& boneDescription) { return strcmp(meshRootNode->mName.C_Str(), boneDescription.name.c_str()) == 0; });
			fetchAiHierarchy(meshRootNode, mesh->m_boneDescriptions, static_cast<int>(std::distance(mesh->m_boneDescriptions.begin(), it)));

			if (addVirtualRoots) {

				for (size_t i = 0; i < mesh->m_boneDescriptions.size(); ++i) {
					BoneDescription& boneDescription = mesh->m_boneDescriptions[i];
					if (boneDescription.parentIndex != i) {
						boneDescription.parentIndex = boneDescription.parentIndex + addVirtualRoots;
					}
					else {
						boneDescription.parentIndex = (addVirtualRoots - 1);
					}
				}

				for (unsigned short count = 0; count < addVirtualRoots; count++) {
					mesh->m_boneDescriptions.insert(mesh->m_boneDescriptions.begin(), BoneDescription());
					mesh->m_boneDescriptions[0].name = "Root_" + std::to_string((addVirtualRoots - 1) - count);
					if (count + 1 != addVirtualRoots)
						mesh->m_boneDescriptions[0].parentIndex = (addVirtualRoots - 1) - count - 1;
				}
			}

			mesh->createBones();		
		}
		
		for (unsigned int t = 0; t < aiMesh->mNumFaces; ++t) {
			const aiFace* face = &aiMesh->mFaces[t];
			mesh->m_indexBuffer.push_back(face->mIndices[0]);
			mesh->m_indexBuffer.push_back(face->mIndices[1]);
			mesh->m_indexBuffer.push_back(face->mIndices[2]);
		}

		mesh->m_drawCount = aiMesh->mNumFaces * 3;		
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

void AnimatedModel::fetchAiHierarchy(aiNode* node, std::vector<BoneDescription>& boneDescriptions, int parentIndex) {
	aiMatrix4x4 transMatrix = node->mTransformation;
	std::vector<BoneDescription>::iterator it = std::find_if(boneDescriptions.begin(), boneDescriptions.end(), [node](BoneDescription& boneDescription) { return strcmp(node->mName.C_Str(), boneDescription.name.c_str()) == 0; });
	int _parentIndex = -1;

	if (it != boneDescriptions.end()) {
		aiVector3D pos, scale;
		aiQuaternion rot;
		transMatrix.Decompose(scale, rot, pos);

		(*it).initialPosition.set(pos.x, pos.y, pos.z);
		(*it).initialRotation.set(rot.x, rot.y, rot.z, rot.w);
		(*it).initialScale.set(scale.x, scale.y, scale.z);

		(*it).parentIndex = parentIndex;
		_parentIndex = static_cast<int>(std::distance(boneDescriptions.begin(), it));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		fetchAiHierarchy(node->mChildren[i], boneDescriptions, _parentIndex);
	}
}

void AnimatedModel::printAiHierarchy(aiNode * node) {
	std::cout << node->mName.data << std::endl;

	aiVector3D pos, scale;
	aiQuaternion rot;
	aiMatrix4x4 transMatrix = node->mTransformation;
	transMatrix.Decompose(scale, rot, pos);

	std::cout << "POS: " << pos.x << "  " << pos.y << "  " << pos.z << std::endl;
	std::cout << "SCALE: " << scale.x << "  " << scale.y << "  " << scale.z << std::endl;
	std::cout << "ROT: " << rot.x << "  " << rot.y << "  " << rot.z << "  " << rot.w << std::endl;

	std::cout << "NODE MATRIX: " << std::endl;
	std::cout << transMatrix.a1 << "  " << transMatrix.b1 << "  " << transMatrix.c1 << "  " << transMatrix.d1 << std::endl;
	std::cout << transMatrix.a2 << "  " << transMatrix.b2 << "  " << transMatrix.c2 << "  " << transMatrix.d2 << std::endl;
	std::cout << transMatrix.a3 << "  " << transMatrix.b3 << "  " << transMatrix.c3 << "  " << transMatrix.d3 << std::endl;
	std::cout << transMatrix.a4 << "  " << transMatrix.b4 << "  " << transMatrix.c4 << "  " << transMatrix.d4 << std::endl;
	std::cout << "########################" << std::endl;

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
			printAiHierarchy(node->mChildren[i]);
	}
}
void AnimatedModel::rotate(const float pitch, const float yaw, const float roll) {
	for (std::vector<Mesh*>::iterator mesh = m_meshes.begin(); mesh != m_meshes.end(); mesh++) {
		static_cast<AnimatedMesh*>(*mesh)->rotate(pitch, yaw, roll);
	}
}

void AnimatedModel::scale(const float sx, const float sy, const float sz) {
	for (std::vector<Mesh*>::iterator mesh = m_meshes.begin(); mesh != m_meshes.end(); mesh++) {
		static_cast<AnimatedMesh*>(*mesh)->scale(sx, sy, sz);
	}
}

void AnimatedModel::translate(const float dx, const float dy, const float dz) {
	for (std::vector<Mesh*>::iterator mesh = m_meshes.begin(); mesh != m_meshes.end(); mesh++) {
		static_cast<AnimatedMesh*>(*mesh)->translate(dx, dy, dz);
	}
}

void AnimatedModel::setScale(const float sx, const float sy, const float sz) {
	for (std::vector<Mesh*>::iterator mesh = m_meshes.begin(); mesh != m_meshes.end(); mesh++) {
		static_cast<AnimatedMesh*>(*mesh)->setScale(sx, sy, sz);
	}
}

AnimationState* AnimatedModel::findAnimationState(const Animation& animation) const {
	for (auto it = m_animationStates.begin(); it != m_animationStates.end(); ++it) {
		if (&(*it)->getAnimation() == &animation)
			return (*it).get();
	}
	return nullptr;
}

AnimationState* AnimatedModel::findAnimationState(const std::string& name) const {
	AnimationState* state = nullptr;
	for (auto it = m_animationStates.begin(); it != m_animationStates.end(); ++it) {
		const Animation& animation = (*it)->getAnimation();
		if (animation.m_animationName == name) {
			return (*it).get();
		}

	}
	return state;
}

AnimationState* AnimatedModel::addAnimationState(const Animation& animation) {
	for (Mesh* mesh : m_meshes) {
		AnimatedMesh* msh = static_cast<AnimatedMesh*>(mesh);
		if (!msh->m_numBones)
			return nullptr;

		AnimationState* existing = findAnimationState(animation);
		if (existing)
			return existing;

		m_animationStates.push_back(std::make_shared<AnimationState>(animation, msh->m_rootBone));
		OnAnimationOrderChanged();
	}
	return m_animationStates.back().get();
}

AnimationState* AnimatedModel::addAnimationStateFront(const Animation& animation) {
	for (Mesh* mesh : m_meshes) {
		AnimatedMesh* msh = static_cast<AnimatedMesh*>(mesh);
		if (!msh->m_numBones)
			return nullptr;

		AnimationState* existing = findAnimationState(animation);
		if (existing)
			return existing;
		std::cout << "############" << std::endl;
		m_animationStates.insert(m_animationStates.begin(), std::make_shared<AnimationState>(animation, msh->m_rootBone));
		OnAnimationOrderChanged();
	}
	return m_animationStates.front().get();
}

AnimationState* AnimatedModel::getAnimationState(size_t index) const {
	return (index < m_animationStates.size()) ? m_animationStates[index].get() : nullptr;
}

void AnimatedModel::removeAnimationState(const Animation& _animation) {
	for (auto it = m_animationStates.begin(); it != m_animationStates.end(); ++it) {
		AnimationState* state = (*it).get();
		const Animation& animation = state->getAnimation();

		if (animation.m_animationName == _animation.m_animationName) {
			m_animationStates.erase(it);
			return;
		}
	}
}

void AnimatedModel::removeAnimationState(const AnimationState* state) {
	for (auto it = m_animationStates.begin(); it != m_animationStates.end(); ++it) {
		if ((*it).get() == state) {
			m_animationStates.erase(it);
			OnAnimationOrderChanged();
			return;
		}
	}
}

void AnimatedModel::removeAllAnimationStates() {
	m_animationStates.clear();
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

Mesh* AnimatedModel::mesh(unsigned short index) const {
	return m_meshes[index];
}

///////////////////////////////////////////////////////////
AnimatedMesh::AnimatedMesh(AnimatedModel* model) : m_model(model), m_skinMatrices(nullptr), m_bones(nullptr), m_rootBone(nullptr){
	m_model = model;

	m_numBones = 0u;
	m_materialIndex = -1;
	m_textureIndex = -1;
}

AnimatedMesh::~AnimatedMesh() {
	cleanup();
}

void AnimatedMesh::cleanup() {
	if (m_bones) {
		delete[] m_bones;
		m_bones = nullptr;
	}
	if (m_skinMatrices) {
		delete[] m_skinMatrices;
		m_skinMatrices = nullptr;
	}
	m_rootBone = nullptr;
}

void AnimatedMesh::updateSkinning() {
	for (size_t i = 0u; i < m_numBones; ++i) 
		m_skinMatrices[i] = m_bones[i]->getWorldTransformation() * m_bones[i]->m_offsetMatrix;
}

void AnimatedMesh::applyBindpose(bool transformChanged) {
	for (size_t i = 0u; i < m_numBones; ++i) {
		Bone* bone = m_bones[i];
		const BoneDescription& boneDescription = m_boneDescriptions[i];
		if (bone->animationEnabled()) {
			transformChanged ? 
				bone->setTransform(boneDescription.initialPosition, boneDescription.initialRotation, boneDescription.initialScale) :
				bone->setTransformSilent(boneDescription.initialPosition, boneDescription.initialRotation, boneDescription.initialScale);
		}		
	}
}

void AnimatedMesh::createBones() {
	cleanup();
	m_numBones = static_cast<unsigned short>(m_boneDescriptions.size());

	m_bones = new Bone*[m_numBones];
	m_skinMatrices = new Matrix4f[m_numBones];

	for (size_t i = 0; i < m_numBones; ++i) {
		const BoneDescription& boneDescription = m_boneDescriptions[i];
		m_bones[i] = new Bone();
		m_bones[i]->setName(boneDescription.name);
		m_bones[i]->setPosition(boneDescription.initialPosition);
		m_bones[i]->setOrientation({ boneDescription.initialRotation[0], boneDescription.initialRotation[1], boneDescription.initialRotation[2], boneDescription.initialRotation[3] });
		m_bones[i]->setScale(boneDescription.initialScale);
		m_bones[i]->m_offsetMatrix = boneDescription.offsetMatrix;
	}

	for (size_t i = 0; i < m_numBones; ++i) {
		const BoneDescription& boneDescription = m_boneDescriptions[i];
		if (boneDescription.parentIndex == i) {
			m_bones[i]->setParent(nullptr);
			m_rootBone = m_bones[i];
			m_bones[i]->setIsRootBone(true);
		}else if (boneDescription.parentIndex == -1) {
			m_bones[i]->setParent(m_bones[0]);
			m_bones[i]->setIsRootBone(false);
			m_bones[i]->setHasParent(false);
		}else {
			m_bones[i]->setParent(m_bones[boneDescription.parentIndex]);
		}
	}

	for (size_t i = 0u; i < m_numBones; ++i)
		m_bones[i]->countChildBones();
}

const std::vector<BoneDescription>& AnimatedMesh::getBoneDescriptions() const {
	return m_boneDescriptions;
}

const std::vector<std::array<float, 4>>& AnimatedMesh::getWeights() const {
	return m_weights;
}

const std::vector<std::array<unsigned int, 4>>& AnimatedMesh::getJoints() const {
	return m_joints;
}

const Matrix4f* AnimatedMesh::getSkinMatrices() const {
	return m_skinMatrices;
}

const unsigned short AnimatedMesh::getNumBones() const {
	return m_numBones;
}

const bool AnimatedMesh::hasMaterial() const {
	return m_materialIndex >= 0;
}

const Material& AnimatedMesh::getMaterial() const {
	return Material::GetMaterials()[m_materialIndex];
}

void AnimatedMesh::rotate(const float pitch, const float yaw, const float roll) {
	for (size_t i = 0u; i < m_numBones; ++i) {
		if (m_bones[i]->isRootBone()) {
			m_boneDescriptions[i].initialRotation.rotate(pitch, yaw, roll);
			m_rootBone->OnTransformChanged();
			break;
		}
	}
}

void AnimatedMesh::scale(const float sx, const float sy, const float sz) {
	for (size_t i = 0u; i < m_numBones; ++i) {
		if (m_bones[i]->isRootBone()) {
			m_boneDescriptions[i].initialScale.scale(sx, sy, sz);
			m_rootBone->OnTransformChanged();
			break;
		}
	}
}

void AnimatedMesh::translate(const float dx, const float dy, const float dz) {
	for (size_t i = 0u; i < m_numBones; ++i) {
		if (m_bones[i]->isRootBone()) {
			m_boneDescriptions[i].initialPosition.translate(dx, dy, dz);
			m_rootBone->OnTransformChanged();
			break;
		}
	}
}

void AnimatedMesh::setScale(const float sx, const float sy, const float sz) {
	for (size_t i = 0u; i < m_numBones; ++i) {
		if (m_bones[i]->isRootBone()) {
			m_boneDescriptions[i].initialScale.set(sx, sy, sz);
			m_rootBone->OnTransformChanged();
			break;
		}
	}
}

std::vector<BoneDescription>& AnimatedMesh::boneDescriptions() const {
	return m_boneDescriptions;
}

std::vector<float>& AnimatedMesh::vertexBuffer() const {
	return m_vertexBuffer;
}

std::vector<unsigned int>& AnimatedMesh::indexBuffer() const {
	return m_indexBuffer;
}

std::vector<std::array<float, 4>>& AnimatedMesh::weights() const {
	return m_weights;
}

std::vector<std::array<unsigned int, 4>>& AnimatedMesh::joints() const {
	return m_joints;
}

Matrix4f* AnimatedMesh::skinMatrices() const {
	return m_skinMatrices;
}

unsigned int& AnimatedMesh::stride() const {
	return m_stride;
}

Bone**& AnimatedMesh::bones() const {
	return m_bones;
}

const Bone& AnimatedMesh::getBone(size_t index) const {
	return *m_bones[index];
}

const Matrix4f& AnimatedMesh::getSkinMatrix(size_t index) const {
	return m_skinMatrices[index];
}