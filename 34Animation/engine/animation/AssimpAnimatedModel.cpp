#include "AssimpAnimatedModel.h"


AssimpAnimatedModel::AssimpAnimatedModel() : m_shader("res/shader/animation.vert", "res/shader/animation.frag") {
	m_animator = std::make_shared<AssimpAnimator>(this);
	m_transform = Transform();
}

void AssimpAnimatedModel::loadModel(const std::string &a_filename, const std::string &texture, const bool addVirtualRoot) {
	m_texture = std::make_shared<Texture>(texture);

	Assimp::Importer Importer;
	Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	Importer.SetPropertyBool(AI_CONFIG_IMPORT_REMOVE_EMPTY_BONES, false);

	const aiScene* pScene = Importer.ReadFile(a_filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!pScene) {
		std::cout << a_filename << "  " << Importer.GetErrorString() << std::endl;
		return;
	}

	std::priority_queue<AssimpWeightData> pq;

	unsigned short maxBones = 4;

	std::vector<Matrix4f> offsetMatrices;

	AssimpAnimatedMesh* mesh;

	for (int j = 0; j < pScene->mNumMeshes; j++) {

		const aiMesh* aiMesh = pScene->mMeshes[j];
		const aiMaterial* aiMaterial = pScene->mMaterials[aiMesh->mMaterialIndex];

		m_numberOfTriangles = aiMesh->mNumFaces;


		m_meshes.push_back(new AssimpAnimatedMesh(this)); 
		mesh = m_meshes.back();

		for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) {
			m_positions.push_back(Vector3f(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z));
			m_texCoords.push_back(Vector2f(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y));
			m_normals.push_back(Vector3f(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z));	
		}

		if (aiMesh->HasBones()) {
			
			mesh->meshBones.resize(aiMesh->mNumBones);

			for (int boneIndex = aiMesh->mNumBones -1 ; boneIndex >= 0; boneIndex--) {
				ModelBone& _bone = mesh->meshBones[(aiMesh->mNumBones - 1) - boneIndex];

				aiBone *bone = aiMesh->mBones[boneIndex];
				const std::string boneName = bone->mName.C_Str();

				mesh->m_boneList.push_back(boneName);

				_bone.name = boneName;
				_bone.nameHash = StringHash(boneName);
				_bone.offsetMatrix.set(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1,
									   bone->mOffsetMatrix.a2, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.d2,
                                       bone->mOffsetMatrix.a3, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.d3,
                                       bone->mOffsetMatrix.a4, bone->mOffsetMatrix.b4, bone->mOffsetMatrix.c4, bone->mOffsetMatrix.d4);


				offsetMatrices.push_back(Matrix4f(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1,
												  bone->mOffsetMatrix.a2, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.d2,
												  bone->mOffsetMatrix.a3, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.d3,
												  bone->mOffsetMatrix.a4, bone->mOffsetMatrix.b4, bone->mOffsetMatrix.c4, bone->mOffsetMatrix.d4));
			
				for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++) {
					aiVertexWeight w = bone->mWeights[weightIndex];
					pq.push(AssimpWeightData(w.mVertexId, w.mWeight,(aiMesh->mNumBones - 1)- boneIndex));
				}
			}

			AssimpWeightData first = pq.top();
			short k = -1;
					
			std::array<unsigned int, 4> jointId = { 0, 0, 0, 0 };
			Vector4f jointWeight = { 0.0f, 0.0f, 0.0f, 0.0f };

			while (!pq.empty()) {
				AssimpWeightData current = pq.top();

				if (first.vertexId == current.vertexId) {
					k++;					
				}else {
					m_jointWeights.push_back(jointWeight);
					m_jointIds.push_back(jointId);

					jointId = { 0, 0, 0, 0 };
					jointWeight = { 0.0f, 0.0f, 0.0f, 0.0f };
					k = 0;
					first = current;
				}

				if (k < maxBones) {					
					jointWeight[k] = pq.top().weight;
					jointId[k] = pq.top().boneId + addVirtualRoot;
				}				
				pq.pop();
			}
			
			m_jointWeights.push_back(jointWeight);
			m_jointIds.push_back(jointId);
	
			aiNode *meshRootNode = searchNode(pScene->mRootNode, mesh->m_boneList);			
			//printAiHierarchy(pScene->mRootNode);
			
			skeletonData.headJoint = fetchAiHierarchy(meshRootNode, mesh->m_boneList, offsetMatrices, mesh->meshBones);
			skeletonData.jointCount = mesh->m_boneList.size();

			printSkeletonData(skeletonData.headJoint);
			CreateJoints(mesh->m_rootBone, skeletonData, mesh->m_boneList);
		}
		
		for (unsigned int t = 0; t < aiMesh->mNumFaces; ++t) {
			const aiFace* face = &aiMesh->mFaces[t];
			m_indexBuffer.push_back(face->mIndices[0]);
			m_indexBuffer.push_back(face->mIndices[1]);
			m_indexBuffer.push_back(face->mIndices[2]);
		}

		mesh->m_drawCount = aiMesh->mNumFaces * 3;
		CreateBuffer(m_positions, m_texCoords, m_normals, m_jointIds, m_jointWeights, m_indexBuffer, mesh->m_vao, mesh->m_vbo, mesh->m_ibo);	

		if (addVirtualRoot) {
			for (size_t i = 0; i < mesh->meshBones.size(); ++i) {
				ModelBone& modelBone = mesh->meshBones[i];
				if (modelBone.parentIndex != i) {
					modelBone.parentIndex++;
				}
			}		

			mesh->meshBones.insert(mesh->meshBones.begin(), ModelBone());
			mesh->meshBones[0].name = "Root";
			mesh->meshBones[0].nameHash = StringHash("Root");
		}
	}





	//CreateBones(mesh->meshBones);
}

void AssimpAnimatedModel::CreateBuffer(std::vector<Vector3f>& positions, std::vector<Vector2f>& texCoords, std::vector<Vector3f>& normals, std::vector<std::array<unsigned int, 4>>& jointIds, std::vector<Vector4f>& jointWeights, std::vector<unsigned int>& indices, unsigned int& vao, unsigned int(&vbo)[5], unsigned int& ibo){

	if (vao)
		glDeleteVertexArrays(1, &vao);

	if (vbo[0])
		glDeleteBuffers(1, &vbo[0]);

	if (vbo[1])
		glDeleteBuffers(1, &vbo[1]);

	if (vbo[2])
		glDeleteBuffers(1, &vbo[2]);

	if (vbo[3])
		glDeleteBuffers(1, &vbo[3]);

	if (vbo[4])
		glDeleteBuffers(1, &vbo[4]);

	if (ibo)
		glDeleteBuffers(1, &ibo);


	glGenBuffers(5, vbo);
	glGenBuffers(1, &ibo);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Positions
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texcoords
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Normals
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);


	//Joint Weights
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, jointWeights.size() * sizeof(Vector4f), &jointWeights.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// Joint Ids
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, jointIds.size() * sizeof(std::array<unsigned int, 4>), &jointIds.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(6);
	glVertexAttribIPointer(6, 4, GL_UNSIGNED_INT, 0, 0);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

};

void AssimpAnimatedModel::printAiHierarchy(aiNode *node) {
	std::cout << node->mName.data << std::endl;
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		printAiHierarchy(node->mChildren[i]);
	}
}

void AssimpAnimatedModel::printSkeletonData(AssimpBoneData& boneData) {
	//std::cout << "H Name: " << boneData.nameId << std::endl;
	//boneData.localBindTransform.print();
	for (unsigned int i = 0; i < boneData.children.size(); i++) {
		printSkeletonData(boneData.children[i]);
	}
}

void AssimpAnimatedModel::CreateJoints(AssimpBone& bone, AssimpSkeletonData& skeletonData, std::vector<std::string> &boneList) {	
	bone.index = skeletonData.headJoint.index;
	bone.name = skeletonData.headJoint.nameId;

	bone.inverseBindTransform = skeletonData.headJoint.offsteMatrix;
	bone.localBindTransform = skeletonData.headJoint.localBindTransform;

	for (AssimpBoneData child : skeletonData.headJoint.children) {
		bone.children.push_back(CreateJoints(child, bone.localBindTransform, boneList));
	}
}

AssimpAnimatedModel::AssimpBone AssimpAnimatedModel::CreateJoints(AssimpBoneData data, Matrix4f parentBindTransform, std::vector<std::string> &boneList) {
	AssimpBone bone;

	bone.index = data.index;
	bone.name = data.nameId;

	bone.inverseBindTransform = data.offsteMatrix;
	bone.localBindTransform = data.localBindTransform;
	
	for (AssimpBoneData child : data.children) {
		bone.children.push_back(CreateJoints(child, bone.localBindTransform, boneList));
	}
	return bone;
}

AssimpAnimatedModel::AssimpBone AssimpAnimatedMesh::GetLocalTransform(std::string jointName) {
	return GetLocalTransform(m_rootBone, jointName);
}

AssimpAnimatedModel::AssimpBone AssimpAnimatedMesh::GetLocalTransform(AssimpAnimatedModel::AssimpBone& joint, std::string jointName) {
	if (joint.name.compare(jointName) == 0) {
		return joint;
	}

	for (unsigned int i = 0; i < joint.children.size(); i++) {
		AssimpAnimatedModel::AssimpBone bone = GetLocalTransform(joint.children[i], jointName);
		bool result = bone.name.compare(jointName) == 0;
		if (result) {
			return bone;
		}
	}
	return AssimpAnimatedModel::AssimpBone();
}

AssimpBoneData AssimpAnimatedModel::fetchAiHierarchy(aiNode *node, std::vector<std::string> &boneList, std::vector<Matrix4f>& offsetMatrices, std::vector<ModelBone>& meshBones, int parentIndex) {

	std::vector<std::string>::iterator it = std::find(boneList.begin(), boneList.end(), node->mName.C_Str());
	
	aiMatrix4x4 tmp = node->mTransformation;
	Matrix4f transMatrix = Matrix4f(tmp.a1, tmp.b1, tmp.c1, tmp.d1,
									tmp.a2, tmp.b2, tmp.c2, tmp.d2,
									tmp.a3, tmp.b3, tmp.c3, tmp.d3,
									tmp.a4, tmp.b4, tmp.c4, tmp.d4);

	AssimpBoneData data;
	unsigned int index = std::distance(boneList.begin(), it);

	std::vector<ModelBone>::iterator it2 = std::find_if(meshBones.begin(), meshBones.end(), [node](ModelBone& _node) { return strcmp(node->mName.C_Str(), _node.name.c_str()) == 0; });
	int _parentIndex = -1;
	
	if (it2 != meshBones.end()) {
		aiVector3D pos, scale;
		aiQuaternion rot;
		tmp.Decompose(scale, rot, pos);

		(*it2).initialPosition.set(pos.x, pos.y, pos.z);
		(*it2).initialRotation.set(rot.x, rot.y, rot.z, rot.w);
		(*it2).initialScale.set(scale.x, scale.y, scale.z);

		(*it2).parentIndex = parentIndex;
		_parentIndex = std::distance(meshBones.begin(), it2);
		//std::cout << "Name: " << (*it2).name << "  " << node->mNumChildren << "  " << parentIndex  << std::endl;
	}/*else {
		std::cout << "Name: " << node->mName.C_Str()<< "  " << node->mNumChildren  << std::endl;
	}*/

	data = AssimpBoneData(index, node->mName.data, transMatrix, offsetMatrices[index]);
	
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		AssimpBoneData innerData = fetchAiHierarchy(node->mChildren[i], boneList, offsetMatrices, meshBones, _parentIndex);
		std::vector<std::string>::iterator innerIt = std::find(boneList.begin(), boneList.end(), innerData.nameId);
		if (innerIt != boneList.end()) {
			data.addChild(innerData);
		}
	}

	return data;
}

aiNode* AssimpAnimatedModel::searchNode(aiNode *node, std::vector<std::string> &boneList) {
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

	return NULL;
}

void AssimpAnimatedModel::update(float elapsedTime) {
	m_animator->update(elapsedTime);
}

void AssimpAnimatedModel::addTwoAnimations(float deltaTime, std::string base, std::string layer, float speed) {
	m_animator->addTwoAnimations(deltaTime, base, layer, speed);
}

void AssimpAnimatedModel::addTwoAnimationsDisjoint(float deltaTime, std::string base, std::string layer, float speed) {
	m_animator->addTwoAnimationsDisjoint(deltaTime, base, layer, speed);
}

void AssimpAnimatedModel::blendTwoAnimations(float deltaTime, std::string base, std::string layer, float blendTime, float speed) {
	m_animator->blendTwoAnimations(deltaTime, base, layer, blendTime, speed);
}

void AssimpAnimatedModel::blendTwoAnimationsDisjoint(float deltaTime, std::string base, std::string layer, float blendTime, float speed) {
	m_animator->blendTwoAnimationsDisjoint(deltaTime, base, layer, blendTime, speed);
}

void AssimpAnimatedModel::draw(Camera& camera) {

	m_shader.bind();
	for (auto&& mesh : m_meshes) {

		m_texture->bind(0);
		m_shader.update(*this, camera, mesh->getBoneArray());
		mesh->drawRaw();
	}
}

void AssimpAnimatedModel::drawRaw() {
	for (auto&& mesh : m_meshes) {
		mesh->drawRaw();
	}
}

void AssimpAnimatedModel::setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz) {
	m_transform.setRotPos(axis, degrees, dx, dy, dz);
}

void AssimpAnimatedModel::rotate(const Vector3f &axis, float degrees) {
	m_transform.rotate(axis, degrees);
}

void AssimpAnimatedModel::translate(float dx, float dy, float dz) {
	m_transform.translate(dx, dy, dz);
}

void AssimpAnimatedModel::scale(float a, float b, float c) {
	m_transform.scale(a, b, c);
}

const Matrix4f &AssimpAnimatedModel::getTransformationMatrix() const {
	return m_transform.getTransformationMatrix();
}

const Matrix4f &AssimpAnimatedModel::getInvTransformationMatrix() {
	return m_transform.getInvTransformationMatrix();
}

void AssimpAnimatedModel::CreateBones(std::vector<ModelBone>& meshBones) {

	
	numBones = (unsigned short)meshBones.size();

	bones = new Bone*[numBones];
	skinMatrices = new Matrix4f[numBones];

	for (size_t i = 0; i < meshBones.size(); ++i) {
		ModelBone& modelBone = meshBones[i];
		

		bones[i] = new Bone();
		bones[i]->SetName(modelBone.name);
		bones[i]->setPosition(modelBone.initialPosition);
		bones[i]->setOrientation({ modelBone.initialRotation[0], modelBone.initialRotation[1], modelBone.initialRotation[2], modelBone.initialRotation[3] });
		bones[i]->setScale(modelBone.initialScale);
		bones[i]->offsetMatrix = modelBone.offsetMatrix;
		//bones[i]->SetAnimationEnabled(false);
		//std::cout << "Parent Index: " << modelBone.parentIndex << "  " << modelBone.name << "  " << bones[i] << std::endl;
	}

	for (size_t i = 0; i < meshBones.size(); ++i) {
		const ModelBone& desc = meshBones[i];
		if (desc.parentIndex == i) {
			bones[i]->setParent(nullptr);
			rootBone = bones[i];
		}else {
			bones[i]->setParent(bones[desc.parentIndex]);
			//bones[i]->setOrigin(bones[i]->getPosition());
		}
	}

	for (size_t i = 0; i < meshBones.size(); ++i)
		bones[i]->CountChildBones();

}

////////////////////////////////////////////////////////////////////////////////

AssimpAnimatedMesh::AssimpAnimatedMesh(AssimpAnimatedModel* model) {
	m_model = model;
}

AssimpAnimatedMesh::~AssimpAnimatedMesh() {
	glDeleteVertexArrays(1, &m_vao);
}

void AssimpAnimatedMesh::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

std::vector<Matrix4f> AssimpAnimatedMesh::getBoneArray() {
	
	std::vector<Matrix4f> boneArray;

	boneArray.assign(m_boneList.size(), Matrix4f::IDENTITY);
	addJointsToArray(m_rootBone, boneArray);

	return boneArray;
}

void AssimpAnimatedMesh::addJointsToArray(AssimpAnimatedModel::AssimpBone rootJoint, std::vector<Matrix4f> &boneArray) {
	applyPoseToJoints(currentPose, m_rootBone, boneArray, Matrix4f::IDENTITY);
}

void AssimpAnimatedMesh::applyPoseToJoints(std::unordered_map<std::string, Matrix4f>& currentPose, std::vector<Matrix4f> &boneArray) {
	applyPoseToJoints(currentPose, m_rootBone, boneArray, Matrix4f::IDENTITY);
}

void AssimpAnimatedMesh::applyPoseToJoints(std::unordered_map<std::string, Matrix4f>& currentPose, AssimpAnimatedModel::AssimpBone& joint, std::vector<Matrix4f> &boneArray, Matrix4f currentTransform) {
	
	currentTransform = (currentPose.find(joint.name) != currentPose.end()) ? currentTransform * currentPose.at(joint.name) : currentTransform * joint.localBindTransform;

	boneArray[joint.index] = currentTransform * joint.inverseBindTransform;
	for (int i = 0; i < joint.children.size(); i++) {
		applyPoseToJoints(currentPose, joint.children[i], boneArray, currentTransform);
	}
}

void AssimpAnimatedMesh::createBones() {

}