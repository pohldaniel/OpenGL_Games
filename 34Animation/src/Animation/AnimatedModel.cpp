#include <GL/glew.h>
#include "AnimatedModel.h"

#include "Utils/SolidIO.h"

static inline bool compareAnimationStates(const std::shared_ptr<AnimationState>& lhs, const std::shared_ptr<AnimationState>& rhs){
	return lhs->BlendLayer() < rhs->BlendLayer();
}

const Vector3f& AnimatedModel::getWorldPosition() const {
	return m_meshes[0]->m_rootBone->getWorldPosition();
}

const Vector3f& AnimatedModel::getWorldScale() const {
	return m_meshes[0]->m_rootBone->getWorldScale();
}

const Quaternion& AnimatedModel::getWorldOrientation() const {
	return m_meshes[0]->m_rootBone->getWorldOrientation();
}

void AnimatedModel::translate(const Vector3f& trans) {
	return m_meshes[0]->m_rootBone->translate(trans);
}

void AnimatedModel::translateRelative(const Vector3f& trans) {
	return m_meshes[0]->m_rootBone->translateRelative(trans);
}

void AnimatedModel::rotate(const float pitch, const float yaw, const float roll) {
	return m_meshes[0]->m_rootBone->rotate(pitch, yaw, roll);
}

AnimatedModel::AnimatedModel() : m_hasAnimationController(false), m_animationOrderDirty(false){

}

void AnimatedModel::loadModelAssimp(const std::string& path, const short addVirtualRoots, const bool reverseBoneList) {

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
		mesh = m_meshes.back();
		mesh->m_numberOfTriangles = aiMesh->mNumFaces;

		for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) {		
			mesh->m_vertexBuffer.push_back(aiMesh->mVertices[i].x); mesh->m_vertexBuffer.push_back(aiMesh->mVertices[i].y); mesh->m_vertexBuffer.push_back(aiMesh->mVertices[i].z);
			mesh->m_vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].x); mesh->m_vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].y);
			mesh->m_vertexBuffer.push_back(aiMesh->mNormals[i].x); mesh->m_vertexBuffer.push_back(aiMesh->mNormals[i].y); mesh->m_vertexBuffer.push_back(aiMesh->mNormals[i].z);
		}

		if (aiMesh->HasBones()) {

			mesh->m_meshBones.resize(aiMesh->mNumBones);

			if (reverseBoneList) {
				for (int boneIndex = aiMesh->mNumBones - 1; boneIndex >= 0; boneIndex--) {
					ModelBone& _bone = mesh->m_meshBones[(aiMesh->mNumBones - 1) - boneIndex];

					aiBone *bone = aiMesh->mBones[boneIndex];
					const std::string boneName = bone->mName.C_Str();

					mesh->m_boneList.push_back(boneName);

					_bone.name = boneName;
					_bone.nameHash = StringHash(boneName);
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
					ModelBone& _bone = mesh->m_meshBones[boneIndex];

					aiBone *bone = aiMesh->mBones[boneIndex];
					const std::string boneName = bone->mName.C_Str();
					mesh->m_boneList.push_back(boneName);

					_bone.name = boneName;
					_bone.nameHash = StringHash(boneName);
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
				}else {
					mesh->m_weights.push_back(jointWeight);
					mesh->m_boneIds.push_back(jointId);

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
			mesh->m_boneIds.push_back(jointId);

			aiNode *meshRootNode = searchNode(pScene->mRootNode, mesh->m_boneList);

			std::vector<ModelBone>::iterator it = std::find_if(mesh->m_meshBones.begin(), mesh->m_meshBones.end(), [meshRootNode](ModelBone& _node) { return strcmp(meshRootNode->mName.C_Str(), _node.name.c_str()) == 0; });

			fetchAiHierarchy(meshRootNode, mesh->m_meshBones, static_cast<int>(std::distance(mesh->m_meshBones.begin(), it)));
		}

		for (unsigned int t = 0; t < aiMesh->mNumFaces; ++t) {
			const aiFace* face = &aiMesh->mFaces[t];
			mesh->m_indexBuffer.push_back(face->mIndices[0]);
			mesh->m_indexBuffer.push_back(face->mIndices[1]);
			mesh->m_indexBuffer.push_back(face->mIndices[2]);
		}

		mesh->m_drawCount = aiMesh->mNumFaces * 3;
		CreateBuffer(mesh->m_vertexBuffer, mesh->m_indexBuffer, mesh->m_vao, mesh->m_vbo, mesh->m_ibo, 8, mesh->m_weights, mesh->m_boneIds);

		if (addVirtualRoots) {
			
			for (size_t i = 0; i < mesh->m_meshBones.size(); ++i) {
				ModelBone& modelBone = mesh->m_meshBones[i];
				if (modelBone.parentIndex != i) {
					modelBone.parentIndex = modelBone.parentIndex + addVirtualRoots;
				}else {
					modelBone.parentIndex = (addVirtualRoots - 1);
				}
			}

			for (unsigned short count = 0; count < addVirtualRoots; count++) {
				mesh->m_meshBones.insert(mesh->m_meshBones.begin(), ModelBone());
				mesh->m_meshBones[0].name = "Root_" + count;
				mesh->m_meshBones[0].nameHash = StringHash(mesh->m_meshBones[0].name);
			}
		}
	}
}

aiNode* AnimatedModel::searchNode(aiNode *node, std::vector<std::string> &boneList) {
	std::vector<std::string>::iterator it = std::find(boneList.begin(), boneList.end(), node->mName.C_Str());

	if (it != boneList.end()) {
		return node;
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		aiNode *result = searchNode(node->mChildren[i], boneList);
		if (result) {
			return result;
		}
	}

	return nullptr;
}

void AnimatedModel::fetchAiHierarchy(aiNode *node, std::vector<ModelBone>& meshBones, int parentIndex) {
	aiMatrix4x4 transMatrix = node->mTransformation;
	std::vector<ModelBone>::iterator it = std::find_if(meshBones.begin(), meshBones.end(), [node](ModelBone& _node) { return strcmp(node->mName.C_Str(), _node.name.c_str()) == 0; });
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
		//std::cout << "Name: " << (*it).name << "  " << parentIndex  << std::endl;
	}/*else {
		std::cout << "Name: " << node->mName.C_Str() << std::endl;
	}*/

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		fetchAiHierarchy(node->mChildren[i], meshBones, _parentIndex);
	}
}

void AnimatedModel::loadModelMdl(const std::string& path) {
	Utils::MdlIO mdlConverter;
	m_meshes.push_back(new AnimatedMesh(this));
	AnimatedMesh* mesh = m_meshes.back();

	BoundingBox box;
	std::vector<std::vector<Utils::GeometryDesc>> geomDescs;

	mdlConverter.mdlToBuffer(path.c_str(), 1.0f, mesh->m_vertexBuffer, mesh->m_indexBuffer, mesh->m_weights, mesh->m_boneIds, geomDescs, mesh->m_meshBones, box);
	mesh->m_drawCount = static_cast<unsigned int>(mesh->m_indexBuffer.size());
	CreateBuffer(mesh->m_vertexBuffer, mesh->m_indexBuffer, mesh->m_vao, mesh->m_vbo, mesh->m_ibo, 8, mesh->m_weights, mesh->m_boneIds);
}

void AnimatedModel::AnimatedModel::drawRaw() {
	for (auto&& mesh : m_meshes) {
		mesh->drawRaw();
	}
}

void AnimatedModel::update(float dt) {
	for (auto&& mesh : m_meshes) {
		mesh->update(dt);
	}
}

void AnimatedModel::updateSkinning() {
	for (auto&& mesh : m_meshes) {
		mesh->updateSkinning();
	}
}

void AnimatedModel::CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& vao, unsigned int(&vbo)[3], unsigned int& ibo, unsigned int stride, std::vector<std::array<float, 4>>& weights, std::vector<std::array<unsigned int, 4>>& boneIds) {
	if (vao)
		glDeleteVertexArrays(1, &vao);

	if (vbo[0])
		glDeleteBuffers(1, &vbo[0]);

	if (vbo[1])
		glDeleteBuffers(1, &vbo[1]);

	if (vbo[2])
		glDeleteBuffers(1, &vbo[2]);

	if (ibo)
		glDeleteBuffers(1, &ibo);


	glGenBuffers(3, vbo);
	glGenBuffers(1, &ibo);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Positions
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), &vertexBuffer[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//Texture Coordinates
	if (stride == 5 || stride == 8 || stride == 14) {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
	}

	//Normals
	if (stride == 6 || stride == 8 || stride == 14) {
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)((stride == 8 || stride == 14) ? 5 * sizeof(float) : 3 * sizeof(float)));
	}

	//Tangents Bitangents
	if (stride == 14) {
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(8 * sizeof(float)));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(11 * sizeof(float)));

	}

	//bone weights and id's
	if (!weights.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, weights.size() * sizeof(float) * 4, &weights.front(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, boneIds.size() * sizeof(std::array<unsigned int, 4>), &boneIds.front(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(6);
		glVertexAttribIPointer(6, 4, GL_UNSIGNED_INT, 0, 0);

	}

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int), &indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

AnimatedMesh::AnimatedMesh(AnimatedModel* model) {
	m_model = model;
}

AnimatedMesh::~AnimatedMesh() {
	glDeleteVertexArrays(1, &m_vao);
}

void AnimatedMesh::createBones() {
	m_numBones = static_cast<unsigned short>(m_meshBones.size());

	m_bones = new Bone*[m_numBones];
	m_skinMatrices = new Matrix4f[m_numBones];

	for (size_t i = 0; i < m_meshBones.size(); ++i) {
		ModelBone& modelBone = m_meshBones[i];
		m_bones[i] = new Bone();
		m_bones[i]->SetName(modelBone.name);
		m_bones[i]->setPosition(modelBone.initialPosition);
		m_bones[i]->setOrientation({ modelBone.initialRotation[0], modelBone.initialRotation[1], modelBone.initialRotation[2], modelBone.initialRotation[3] });
		m_bones[i]->setScale(modelBone.initialScale);
		m_bones[i]->offsetMatrix = modelBone.offsetMatrix;
	}

	for (size_t i = 0; i < m_meshBones.size(); ++i) {
		const ModelBone& desc = m_meshBones[i];

		if (desc.parentIndex == i) {
			m_bones[i]->setParent(nullptr);
			m_rootBone = m_bones[i];
			m_bones[i]->setRootBone(true);
		}else {
			m_bones[i]->setParent(m_bones[desc.parentIndex]);
			//bones[i]->setOrigin(bones[i]->getPosition());
		}
	}

	for (size_t i = 0; i < m_meshBones.size(); ++i)
		m_bones[i]->CountChildBones();
}

AnimationState* AnimatedModel::addAnimationState(Animation* animation) {

	if (!animation || !m_meshes[0]->m_numBones)
		return nullptr;

	// Check for not adding twice
	AnimationState* existing = findAnimationState(animation);
	if (existing)
		return existing;

	m_meshes[0]->m_animationStates.push_back(std::make_shared<AnimationState>(animation, m_meshes[0]->m_rootBone));
	OnAnimationOrderChanged();
	return m_meshes[0]->m_animationStates.back().get();
}

AnimationState* AnimatedModel::addAnimationStateFront(Animation* animation) {

	if (!animation || !m_meshes[0]->m_numBones)
		return nullptr;

	// Check for not adding twice
	AnimationState* existing = findAnimationState(animation);
	if (existing)
		return existing;

	m_meshes[0]->m_animationStates.insert(m_meshes[0]->m_animationStates.begin(), std::make_shared<AnimationState>(animation, m_meshes[0]->m_rootBone));
	//modelDrawable->OnAnimationOrderChanged();

	return m_meshes[0]->m_animationStates.front().get();
}

AnimationState* AnimatedModel::getAnimationState(size_t index) const {
	return (index < m_meshes[0]->m_animationStates.size()) ? m_meshes[0]->m_animationStates[index].get() : nullptr;
}

AnimationState* AnimatedModel::findAnimationState(Animation* animation) const {
	
	for (auto it = m_meshes[0]->m_animationStates.begin(); it != m_meshes[0]->m_animationStates.end(); ++it){
		if ((*it)->GetAnimation() == animation)
			return (*it).get();
	}

	return nullptr;
}

AnimationState* AnimatedModel::findAnimationState(const std::string& animationName) const{
	return getAnimationState(StringHash(animationName));
}

AnimationState* AnimatedModel::findAnimationState(const char* animationName) const{
	return getAnimationState(StringHash(animationName));
}

AnimationState* AnimatedModel::findAnimationState(StringHash animationNameHash) const{

	//std::cout << "Size: " << m_meshes[0]->m_animationStates.size() << std::endl;

	AnimationState* state = nullptr;

	for (auto it = m_meshes[0]->m_animationStates.begin(); it != m_meshes[0]->m_animationStates.end(); ++it){
		//std::cout << "State Pointer: " << (*it) << std::endl;
		
		Animation* animation = (*it)->GetAnimation();
		if (animation->animationNameHash == animationNameHash) {
			state = (*it).get();
			//return (*it).get();
		}
			
	}
	//std::cout << "---------" << std::endl;
	return state;
}

void AnimatedModel::removeAnimationState(Animation* animation){
	if (animation)
		removeAnimationState(animation->animationNameHash);
}

void AnimatedModel::removeAnimationState(const std::string& animationName){
	removeAnimationState(StringHash(animationName));
}

void AnimatedModel::removeAnimationState(const char* animationName){
	removeAnimationState(StringHash(animationName));
}

void AnimatedModel::removeAnimationState(StringHash animationNameHash){
	for (auto it = m_meshes[0]->m_animationStates.begin(); it != m_meshes[0]->m_animationStates.end(); ++it){
		AnimationState* state = (*it).get();
		Animation* animation = state->GetAnimation();

		if (animation->animationNameHash == animationNameHash){
			m_meshes[0]->m_animationStates.erase(it);
			//modelDrawable->OnAnimationChanged();
			return;
		}
	}
}

void AnimatedModel::removeAnimationState(AnimationState* state){
	for (auto it = m_meshes[0]->m_animationStates.begin(); it != m_meshes[0]->m_animationStates.end(); ++it){
		if ((*it).get() == state){
			m_meshes[0]->m_animationStates.erase(it);
			//modelDrawable->OnAnimationChanged();
			return;
		}
	}
}

void AnimatedModel::removeAnimationState(size_t index){
	if (index < m_meshes[0]->m_animationStates.size()){
		m_meshes[0]->m_animationStates.erase(m_meshes[0]->m_animationStates.begin() + index);
		//modelDrawable->OnAnimationChanged();
	}
}

void AnimatedModel::removeAllAnimationStates(){
	if (m_meshes[0]->m_animationStates.size()){
		m_meshes[0]->m_animationStates.clear();
		//modelDrawable->OnAnimationChanged();
	}
}

void AnimatedModel::OnAnimationOrderChanged() {
	m_animationOrderDirty = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AnimatedMesh::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void AnimatedMesh::update(float dt) {

	if (m_model->m_animationOrderDirty) {
		std::sort(m_animationStates.begin(), m_animationStates.end(), compareAnimationStates);
		m_model->m_animationOrderDirty = false;
	}

	for (size_t i = 0; i < m_numBones; ++i) {
		Bone* bone = m_bones[i];
		const ModelBone& modelBone = m_meshBones[i];
		if (bone->AnimationEnabled()) {
			bone->SetTransformSilent(modelBone.initialPosition, modelBone.initialRotation, modelBone.initialScale);
		}
	}

	for (auto it = m_animationStates.begin(); it != m_animationStates.end(); ++it) {
		AnimationState* state = (*it).get();

		if (m_model->m_hasAnimationController) {
			if (state->Enabled()) {
				state->Apply();
			}
		}else {

			if (state->Enabled() || state->getAnimationBlendMode() == ABM_FADE) {
				state->AddTime(dt);
				state->Apply();
			}
		}
	}

}

void AnimatedMesh::updateSkinning() {
	for (size_t i = 0; i < m_numBones; ++i) {
		m_skinMatrices[i] = m_bones[i]->getWorldTransformation() * m_meshBones[i].offsetMatrix;
	}
}