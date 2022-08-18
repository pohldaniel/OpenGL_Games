#include "AssimpModel.h"

AssimpModel::AssimpModel() {
	
}

AssimpModel::~AssimpModel() {

}

void AssimpModel::rotate(const Vector3f &axis, float degrees) {
	m_transform.rotate(axis, degrees);
}

void AssimpModel::translate(float dx, float dy, float dz) {
	m_transform.translate(dx, dy, dz);
}

void AssimpModel::scale(float a, float b, float c) {
	m_transform.scale(a, b, c);
}

const Matrix4f &AssimpModel::getTransformationMatrix() const {
	return m_transform.getTransformationMatrix();
}

const Matrix4f &AssimpModel::getInvTransformationMatrix() {
	return m_transform.getInvTransformationMatrix();
}

std::string AssimpModel::getModelDirectory() {
	return m_modelDirectory;
}

bool AssimpModel::loadModel(const char* a_filename) {
	std::string filename(a_filename);

	const size_t index = filename.rfind('/');

	if (std::string::npos != index) {
		m_modelDirectory = filename.substr(0, index);
	}

	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile("res/models/dragon/dragon.obj", ASSIMP_LOAD_FLAGS);

	if (pScene) {
		std::cout << pScene->mNumMeshes << "  " << pScene->mNumMaterials << std::endl;
	}

	m_materialCount = pScene->mNumMaterials;
	for (unsigned int m = 0; m < pScene->mNumMaterials; ++m){
		aiMaterial* material = pScene->mMaterials[m];

		aiString materialName = material->GetName();
		
		int numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);

		if (numTextures > 0) {
			aiString name;
			material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), name);

			std::string texture = name.data;
			int foundSlash = texture.find_last_of("/\\");

			int foundDot = texture.find_last_of(".");
			foundDot = (foundDot < 0 ? texture.length() : foundDot);
			foundDot = foundSlash < 0 ? foundDot : foundDot - 1;
			std::string textureName = texture.substr(foundSlash + 1, foundDot);
			if (!m_textureManager.checkAsset(textureName)) {				
				m_textureManager.loadTexture(textureName, foundSlash < 0 ? m_modelDirectory + "/" + texture.substr(foundSlash + 1) : texture, true);
			}
			m_textures[pScene->mMeshes[m]->mMaterialIndex] = m_textureManager.get(textureName);
		}
	}

	m_numberOfMeshes = pScene->mNumMeshes;
	for (int i = 0; i < pScene->mNumMeshes; i++) {

		m_mesh.push_back(new AssimpMesh(this));

		AssimpMesh* mesh = m_mesh.back();

		mesh->m_numberOfTriangles = pScene->mMeshes[i]->mNumFaces;

		mesh->m_drawCount = pScene->mMeshes[i]->mNumFaces * 3;

		if (mesh->m_vao)
			glDeleteVertexArrays(1, &mesh->m_vao);

		if (mesh->m_vbo[0])
			glDeleteBuffers(1, &mesh->m_vbo[0]);

		if (mesh->m_vbo[1])
			glDeleteBuffers(1, &mesh->m_vbo[1]);

		if (mesh->m_vbo[2])
			glDeleteBuffers(1, &mesh->m_vbo[2]);

		if (mesh->m_vbo[3])
			glDeleteBuffers(1, &mesh->m_vbo[3]);

		if (mesh->m_vbo[4])
			glDeleteBuffers(1, &mesh->m_vbo[4]);

		if (mesh->m_ibo)
			glDeleteBuffers(1, &mesh->m_ibo);


		glGenBuffers(5, mesh->m_vbo);
		glGenBuffers(1, &mesh->m_ibo);

		glGenVertexArrays(1, &mesh->m_vao);
		glBindVertexArray(mesh->m_vao);
		
		//Positions
		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, pScene->mMeshes[i]->mNumVertices * sizeof(aiVector3D), pScene->mMeshes[i]->mVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//Texture Coords
		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, pScene->mMeshes[i]->mNumVertices * sizeof(aiVector3D), &pScene->mMeshes[i]->mTextureCoords[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(ai_real), 0);

		//Normals
		glBindBuffer(GL_ARRAY_BUFFER, mesh->m_vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, pScene->mMeshes[i]->mNumVertices * sizeof(aiVector3D), pScene->mMeshes[i]->mNormals, GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		
		

		unsigned int *faceArray;
		faceArray = (unsigned int *)malloc(sizeof(unsigned int) * pScene->mMeshes[i]->mNumFaces * 3);
		unsigned int faceIndex = 0;

		for (unsigned int t = 0; t < pScene->mMeshes[i]->mNumFaces; ++t) {
			const aiFace* face = &pScene->mMeshes[i]->mFaces[t];

			memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
			faceIndex += 3;
		}
		
		//Indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_drawCount * sizeof(unsigned int), faceArray, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}
	
	return true;
}

void AssimpModel::createInstancesStatic(std::vector<Matrix4f>& modelMTX) {

	m_instanceCount = modelMTX.size();

	glGenBuffers(1, &m_vboInstances);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferData(GL_ARRAY_BUFFER, modelMTX.size() * sizeof(GLfloat) * 4 * 4, modelMTX[0][0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindVertexArray(0);	
}

void AssimpModel::createInstancesDynamic(unsigned int numberOfInstances) {

	m_instanceCount = numberOfInstances;
	glGenBuffers(1, &m_vboInstances);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferData(GL_ARRAY_BUFFER, m_instanceCount * sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindVertexArray(0);	
}

void AssimpModel::updateInstances(std::vector<Matrix4f>& modelMTX) {

	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferSubData(GL_ARRAY_BUFFER, 0, modelMTX.size() * sizeof(GLfloat) * 4 * 4, modelMTX[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
}

void AssimpModel::drawRaw() {
	for (int j = 0; j < m_numberOfMeshes; j++) {
		m_mesh[j]->drawRaw();
	}
}

void AssimpModel::CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& drawCount, unsigned int& vao, unsigned int(&vbo)[5], unsigned int& ibo, unsigned int stride) {

}


AssimpMesh::AssimpMesh(AssimpModel* model) {

}

AssimpMesh::~AssimpMesh() {

}

void AssimpMesh::drawRaw() {
	
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}