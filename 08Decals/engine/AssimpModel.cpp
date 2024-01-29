#include "AssimpModel.h"

AssimpModel::AssimpModel() {
	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
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

const Vector3f &AssimpModel::getCenter() const {
	return m_center;
}

std::string AssimpModel::getModelDirectory() {
	return m_modelDirectory;
}

Transform& AssimpModel::getTransform() {
	return m_transform;
}

std::vector<AssimpMesh*> AssimpModel::getMeshes() {
	return m_meshes;
}

BoundingBox& AssimpModel::getAABB() {
	return aabb;
}

bool AssimpModel::loadModel(const char* a_filename, bool isStacked, bool generateTangents) {
	m_isStacked = isStacked;

	std::string filename(a_filename);

	const size_t index = filename.rfind('/');

	if (std::string::npos != index) {
		m_modelDirectory = filename.substr(0, index);
	}

	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile(a_filename, generateTangents ? ASSIMP_LOAD_FLAGS | aiProcess_CalcTangentSpace : ASSIMP_LOAD_FLAGS);
	
	bool exportTangents = generateTangents;

	m_numberOfMeshes = pScene->mNumMeshes;
	m_isStacked = !(pScene->mNumMeshes == 1) && isStacked;

	float xmin = FLT_MAX; float ymin = FLT_MAX; float zmin = FLT_MAX;
	float xmax = -FLT_MAX; float ymax = -FLT_MAX; float zmax = -FLT_MAX;

	for (int j = 0; j < pScene->mNumMeshes; j++) {
		const aiMesh* aiMesh = pScene->mMeshes[j];

		m_meshes.push_back(new AssimpMesh(this));
		AssimpMesh* mesh = m_meshes.back();
		mesh->m_numberOfTriangles = aiMesh->mNumFaces;
		
		
		const aiMaterial* aiMaterial = pScene->mMaterials[aiMesh->mMaterialIndex];

		AssimpModel::ReadAiMaterial(aiMaterial, mesh->m_material);

		m_isStacked ? m_hasTextureCoords = aiMesh->HasTextureCoords(0) : mesh->m_hasTextureCoords = aiMesh->HasTextureCoords(0);
		m_isStacked ? m_hasNormals = aiMesh->HasNormals() : mesh->m_hasNormals = aiMesh->HasNormals();
		m_isStacked ? m_hasTangents = aiMesh->HasTangentsAndBitangents() & exportTangents : mesh->m_hasTangents = aiMesh->HasTangentsAndBitangents() & exportTangents;

		m_isStacked ? m_stride = m_hasTangents ? 14 : (m_hasNormals && m_hasTextureCoords) ? 8 : m_hasNormals ? 6 : m_hasTextureCoords ? 5 : 3
				  : mesh->m_stride = mesh->m_hasTangents ? 14 : (mesh->m_hasNormals && mesh->m_hasTextureCoords) ? 8 : mesh->m_hasNormals ? 6 : mesh->m_hasTextureCoords ? 5 : 3;


		if (m_isStacked) {
			mesh->m_baseIndex = m_indexBuffer.size();
			mesh->m_baseVertex = m_vertexBuffer.size() / m_stride;
		}

		std::vector<float>& vertexBuffer = m_isStacked ? m_vertexBuffer : mesh->m_vertexBuffer;
		std::vector<unsigned int>& indexBuffer = m_isStacked ? m_indexBuffer : mesh->m_indexBuffer;

		for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) {

			xmin = (std::min)(aiMesh->mVertices[i].x, xmin);
			ymin = (std::min)(aiMesh->mVertices[i].y, ymin);
			zmin = (std::min)(aiMesh->mVertices[i].z, zmin);

			xmax = (std::max)(aiMesh->mVertices[i].x, xmax);
			ymax = (std::max)(aiMesh->mVertices[i].y, ymax);
			zmax = (std::max)(aiMesh->mVertices[i].z, zmax);

			vertexBuffer.push_back(aiMesh->mVertices[i].x); vertexBuffer.push_back(aiMesh->mVertices[i].y); vertexBuffer.push_back(aiMesh->mVertices[i].z);

			if (m_hasTextureCoords || mesh->m_hasTextureCoords) {
				vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].x); vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].y);
			}

			if (m_hasNormals || mesh->m_hasNormals) {
				vertexBuffer.push_back(aiMesh->mNormals[i].x); vertexBuffer.push_back(aiMesh->mNormals[i].y); vertexBuffer.push_back(aiMesh->mNormals[i].z);
			}

			if (m_hasTangents || mesh->m_hasTangents) {
				vertexBuffer.push_back(aiMesh->mTangents[i].x); vertexBuffer.push_back(aiMesh->mTangents[i].y); vertexBuffer.push_back(aiMesh->mTangents[i].z);
				vertexBuffer.push_back(aiMesh->mBitangents[i].x); vertexBuffer.push_back(aiMesh->mBitangents[i].y); vertexBuffer.push_back(aiMesh->mBitangents[i].z);
			}
	
		}

		for (unsigned int t = 0; t < aiMesh->mNumFaces; ++t) {
			const aiFace* face = &aiMesh->mFaces[t];
			indexBuffer.push_back(face->mIndices[0]);
			indexBuffer.push_back(face->mIndices[1]);
			indexBuffer.push_back(face->mIndices[2]);
		}

		mesh->m_drawCount = aiMesh->mNumFaces * 3;

		if (!m_isStacked) {

			AssimpModel::CreateBuffer(mesh->m_vertexBuffer,
				mesh->m_indexBuffer,
				mesh->m_vao,
				mesh->m_vbo,
				mesh->m_ibo,
				mesh->m_stride);
		}
		
	}

	if (m_isStacked) {
		AssimpModel::CreateBuffer(m_vertexBuffer, m_indexBuffer, m_vao, m_vbo, m_ibo, m_stride);
	}

	m_center = Vector3f((xmax + xmin) * 0.5f, (ymax + ymin) * 0.5f, (zmax + zmin) * 0.5f);

	aabb.position = Vector3f(xmin, ymin, zmin);
	aabb.size = Vector3f(xmax, ymax, zmax) - Vector3f(xmin, ymin, zmin);
	
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
		m_meshes[j]->drawRaw();
	}
}

void AssimpModel::drawRawInstanced() {
	for (int j = 0; j < m_numberOfMeshes; j++) {
		m_meshes[j]->drawRawInstanced();
	}
}

void  AssimpModel::drawRawStacked() {
	glBindVertexArray(m_vao);
	for (int i = 0; i < m_meshes.size(); i++) {
		glDrawElementsBaseVertex(GL_TRIANGLES, m_meshes[i]->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[i]->m_baseIndex), m_meshes[i]->m_baseVertex);
	}
	glBindVertexArray(0);
}

void AssimpModel::drawRawInstancedStacked() {
	glBindVertexArray(m_vao);
	for (int i = 0; i < m_meshes.size(); i++) {
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, m_meshes[i]->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[i]->m_baseIndex), m_instanceCount, m_meshes[i]->m_baseVertex, 0);
	}
	glBindVertexArray(0);
}

void AssimpModel::draw(Camera& camera) {
	for (int i = 0; i < m_meshes.size(); i++) {
		m_meshes[i]->updateMaterialUbo(BuiltInShader::materialUbo);
		glUseProgram(m_shader[i]->m_program);

		m_shader[i]->loadMatrix("u_projection", camera.getProjectionMatrix());
		m_shader[i]->loadMatrix("u_view", camera.getViewMatrix());
		m_shader[i]->loadMatrix("u_model", m_transform.getTransformationMatrix());

		m_textures[i].bind(0);
		m_meshes[i]->drawRaw();

		glUseProgram(0);
	}

	Texture::Unbind();
}

void AssimpModel::drawInstanced(Camera& camera) {
	for (int i = 0; i < m_meshes.size(); i++) {
		m_meshes[i]->updateMaterialUbo(BuiltInShader::materialUbo);

		glUseProgram(m_shader[i]->m_program);

		m_shader[i]->loadMatrix("u_projection", camera.getProjectionMatrix());

		m_textures[i].bind(0);
		m_meshes[i]->drawRawInstanced();

		glUseProgram(0);
	}

	Texture::Unbind();
}

void AssimpModel::drawStacked(Camera& camera) {
	glBindVertexArray(m_vao);
	for (int i = 0; i < m_meshes.size(); i++) {
		m_meshes[i]->updateMaterialUbo(BuiltInShader::materialUbo);

		glUseProgram(m_shader[i]->m_program);

		m_shader[i]->loadMatrix("u_projection", camera.getProjectionMatrix());
		m_shader[i]->loadMatrix("u_view", camera.getViewMatrix());
		m_shader[i]->loadMatrix("u_model", m_transform.getTransformationMatrix());

		m_textures[i].bind(0);
		glDrawElementsBaseVertex(GL_TRIANGLES, m_meshes[i]->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[i]->m_baseIndex), m_meshes[i]->m_baseVertex);
		glUseProgram(0);
	}
	Texture::Unbind();
	glBindVertexArray(0);
}

void AssimpModel::drawInstancedStacked(Camera& camera) {
	glBindVertexArray(m_vao);
	for (int i = 0; i < m_meshes.size(); i++) {
		m_meshes[i]->updateMaterialUbo(BuiltInShader::materialUbo);

		glUseProgram(m_shader[i]->m_program);

		m_shader[i]->loadMatrix("u_projection", camera.getProjectionMatrix());
		m_textures[i].bind(0);
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, m_meshes[i]->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[i]->m_baseIndex), m_instanceCount, m_meshes[i]->m_baseVertex, 0);

		glUseProgram(0);
	}
	Texture::Unbind();
	glBindVertexArray(0);
}

void AssimpModel::drawAABB() {
	aabb.drawRaw();
}

void AssimpModel::createAABB() {
	aabb.createBuffer();
	m_hasAABB = true;
}

void AssimpModel::initAssets(bool instanced) {

	if (!BuiltInShader::materialUbo) {
		glGenBuffers(1, &BuiltInShader::materialUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::materialUbo);
		glBufferData(GL_UNIFORM_BUFFER, 52, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, BuiltInShader::materialBinding, BuiltInShader::materialUbo, 0, 52);
	}

	if (!BuiltInShader::viewUbo && instanced) {
		glGenBuffers(1, &BuiltInShader::viewUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::viewUbo);
		glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, BuiltInShader::viewBinding, BuiltInShader::viewUbo, 0, 64);
	}

	for (int i = 0; i < m_meshes.size(); i++) {
		if (m_meshes[i]->m_material.diffuseTexPath.empty()) {

			if (!m_shaderManager.checkAsset(instanced ? "diffuse_instance" : "diffuse")) {
				m_shaderManager.loadShaderFromString(instanced ? "diffuse_instance" : "diffuse", instanced ? DIFFUSE_INSTANCE_VS : DIFFUSE_VS, instanced ? DIFFUSE_INSTANCE_FS : DIFFUSE_FS);

				glUniformBlockBinding(m_shaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse")->m_program, glGetUniformBlockIndex(m_shaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse")->m_program, "u_material"), BuiltInShader::materialBinding);

				if (instanced) {
					glUniformBlockBinding(m_shaderManager.getAssetPointer("diffuse_instance")->m_program, glGetUniformBlockIndex(m_shaderManager.getAssetPointer("diffuse_instance")->m_program, "u_view"), BuiltInShader::viewBinding);
				}
			}
			m_shader[i] = m_shaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse");
			m_textures[i] = Texture();

		}else {
			if (!m_shaderManager.checkAsset(instanced ? "diffuse_texture_instance" : "diffuse_texture")) {
				m_shaderManager.loadShaderFromString(instanced ? "diffuse_texture_instance" : "diffuse_texture", instanced ? DIFFUSE_TEXTURE_INSTANCE_VS : DIFFUSE_TEXTURE_VS, instanced ? DIFFUSE_TEXTURE_INSTANCE_FS : DIFFUSE_TEXTURE_FS);

				glUniformBlockBinding(m_shaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->m_program, glGetUniformBlockIndex(m_shaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->m_program, "u_material"), BuiltInShader::materialBinding);

				if (instanced) {
					glUniformBlockBinding(m_shaderManager.getAssetPointer("diffuse_texture_instance")->m_program, glGetUniformBlockIndex(m_shaderManager.getAssetPointer("diffuse_texture_instance")->m_program, "u_view"), BuiltInShader::viewBinding);
				}

				glUseProgram(m_shaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->m_program);
				m_shaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->loadInt("u_texture", 0);
				glUseProgram(0);
			}

			std::string texture = m_meshes[i]->m_material.diffuseTexPath;
			int foundSlash = texture.find_last_of("/\\");

			int foundDot = texture.find_last_of(".");
			foundDot = (foundDot < 0 ? texture.length() : foundDot);
			foundDot = foundSlash < 0 ? foundDot : foundDot - 1;
			std::string textureName = texture.substr(foundSlash + 1, foundDot);
			if (!m_textureManager.checkAsset(textureName)) {
				m_textureManager.loadTexture(textureName, foundSlash < 0 ? m_modelDirectory + "/" + texture.substr(foundSlash + 1) : texture, true);
			}
			m_textures[i] = m_textureManager.get(textureName);
			m_shader[i] = m_shaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture");
		}
	}
}

void AssimpModel::initAssets(AssetManager<Shader>& shaderManager, AssetManager<Texture>& textureManager, bool instanced) {

	if (!BuiltInShader::materialUbo) {
		glGenBuffers(1, &BuiltInShader::materialUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::materialUbo);
		glBufferData(GL_UNIFORM_BUFFER, 52, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, BuiltInShader::materialBinding, BuiltInShader::materialUbo, 0, 52);
	}

	if (!BuiltInShader::viewUbo && instanced) {
		glGenBuffers(1, &BuiltInShader::viewUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::viewUbo);
		glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, BuiltInShader::viewBinding, BuiltInShader::viewUbo, 0, 64);
	}

	for (int i = 0; i < m_meshes.size(); i++) {
		if (m_meshes[i]->m_material.diffuseTexPath.empty()) {

			if (!shaderManager.checkAsset(instanced ? "diffuse_instance" : "diffuse")) {
				shaderManager.loadShaderFromString(instanced ? "diffuse_instance" : "diffuse", instanced ? DIFFUSE_INSTANCE_VS : DIFFUSE_VS, instanced ? DIFFUSE_INSTANCE_FS : DIFFUSE_FS);

				glUniformBlockBinding(shaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse")->m_program, glGetUniformBlockIndex(shaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse")->m_program, "u_material"), BuiltInShader::materialBinding);

				if (instanced) {
					glUniformBlockBinding(shaderManager.getAssetPointer("diffuse_instance")->m_program, glGetUniformBlockIndex(shaderManager.getAssetPointer("diffuse_instance")->m_program, "u_view"), BuiltInShader::viewBinding);
				}

			}
			m_shader[i] = shaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse");
			m_textures[i] = Texture();

		}else {
			if (!shaderManager.checkAsset(instanced ? "diffuse_texture_instance" : "diffuse_texture")) {
				shaderManager.loadShaderFromString(instanced ? "diffuse_texture_instance" : "diffuse_texture", instanced ? DIFFUSE_TEXTURE_INSTANCE_VS : DIFFUSE_TEXTURE_VS, instanced ? DIFFUSE_TEXTURE_INSTANCE_FS : DIFFUSE_TEXTURE_FS);

				glUniformBlockBinding(shaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->m_program, glGetUniformBlockIndex(shaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->m_program, "u_material"), BuiltInShader::materialBinding);

				if (instanced) {
					glUniformBlockBinding(shaderManager.getAssetPointer("diffuse_texture_instance")->m_program, glGetUniformBlockIndex(shaderManager.getAssetPointer("diffuse_texture_instance")->m_program, "u_view"), BuiltInShader::viewBinding);
				}

				glUseProgram(shaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->m_program);
				shaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->loadInt("u_texture", 0);
				glUseProgram(0);
			}

			std::string texture = m_meshes[i]->m_material.diffuseTexPath;
			int foundSlash = texture.find_last_of("/\\");

			int foundDot = texture.find_last_of(".");
			foundDot = (foundDot < 0 ? texture.length() : foundDot);
			foundDot = foundSlash < 0 ? foundDot : foundDot - 1;
			std::string textureName = texture.substr(foundSlash + 1, foundDot);
			if (!textureManager.checkAsset(textureName)) {
				textureManager.loadTexture(textureName, foundSlash < 0 ? m_modelDirectory + "/" + texture.substr(foundSlash + 1) : texture, true);
			}
			m_textures[i] = textureManager.get(textureName);
			m_shader[i] = shaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture");
		}
	}
}

void AssimpModel::CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& vao, unsigned int(&vbo)[5], unsigned int& ibo, unsigned int stride) {

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
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), &vertexBuffer[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//Texture Coordinates
	if (stride == 5 || stride == 8 || stride == 14) {

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), &vertexBuffer[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
	}

	//Normals
	if (stride == 6 || stride == 8 || stride == 14) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), &vertexBuffer[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)((stride == 8 || stride == 14) ? 5 * sizeof(float) : 3 * sizeof(float)));
	}

	//Tangents Bitangents
	if (stride == 14) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
		glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), &vertexBuffer[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(8 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
		glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), &vertexBuffer[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(11 * sizeof(float)));

	}

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int), &indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void AssimpModel::ReadAiMaterial(const aiMaterial* aiMaterial, Material& material) {
	float shininess = 0.0f;
	aiColor4D diffuse = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	aiColor4D ambient = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	aiColor4D specular = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

	if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
		//std::cout << "Diffuse: " << diffuse.r << "  " << diffuse.g << "  " << diffuse.b << "  " << diffuse.a << std::endl;
	}

	material.diffuse[0] = diffuse.r;
	material.diffuse[1] = diffuse.g;
	material.diffuse[2] = diffuse.b;
	material.diffuse[3] = diffuse.a;

	if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient)) {
		//std::cout << "Ambient: " << ambient.r << "  " << ambient.g << "  " << ambient.b << "  " << ambient.a << std::endl;
	}

	material.ambient[0] = ambient.r;
	material.ambient[1] = ambient.g;
	material.ambient[2] = ambient.b;
	material.ambient[3] = ambient.a;

	if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_SPECULAR, &specular)) {
		//std::cout << "Specular: " << specular.r << "  " << specular.g << "  " << specular.b << "  " << specular.a << std::endl;
	}

	material.specular[0] = specular.r;
	material.specular[1] = specular.g;
	material.specular[2] = specular.b;
	material.specular[3] = specular.a;

	if (AI_SUCCESS == aiGetMaterialFloat(aiMaterial, AI_MATKEY_SHININESS, &shininess)) {
		//std::cout << "Shininess: " << shininess << std::endl;
	}

	material.shininess = shininess;

	int numDiffuseTextures = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
	if (numDiffuseTextures > 0) {
		aiString name;
		aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), name);
		material.diffuseTexPath = name.data;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AssimpMesh::AssimpMesh(AssimpModel* model) {
	m_model = model;
	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
}

AssimpMesh::~AssimpMesh() {

}

void AssimpMesh::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void AssimpMesh::drawRawInstanced() {
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceCount);
	glBindVertexArray(0);
}

void AssimpMesh::updateMaterialUbo(unsigned int& ubo) {
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, &m_material.ambient);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, &m_material.diffuse);
	glBufferSubData(GL_UNIFORM_BUFFER, 32, 16, &m_material.specular);
	glBufferSubData(GL_UNIFORM_BUFFER, 48, 4, &m_material.shininess);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

std::vector<float>& AssimpMesh::getVertexBuffer() {
	return m_vertexBuffer;
}

std::vector<unsigned int>& AssimpMesh::getIndexBuffer() {
	return m_indexBuffer;
}

int AssimpMesh::getStride() {
	return m_stride;
}