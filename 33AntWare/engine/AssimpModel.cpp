#include "AssimpModel.h"

AssetManager<Shader> AssimpModel::ShaderManager;

bool compareMaterial(Material const& s1, std::string const& s2) {
	return s1.name == s2;
}

AssimpModel::AssimpModel() {
	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_transform.reset();
}

AssimpModel::~AssimpModel() {
	cleanup();
}

void AssimpModel::cleanup() {
	if (m_vao)
		glDeleteVertexArrays(1, &m_vao);

	if (m_vbo)
		glDeleteBuffers(1, &m_vbo);

	if (m_ibo)
		glDeleteBuffers(1, &m_ibo);

	if (m_vboInstances)
		glDeleteBuffers(1, &m_vboInstances);

	m_vertexBuffer.clear();
	m_vertexBuffer.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
	m_instances.clear();
	m_instances.shrink_to_fit();

	m_meshes.clear();
	m_meshes.shrink_to_fit();

	m_shader.clear();
	m_aabb.cleanup();
}

void AssimpModel::setPosition(float x, float y, float z) {
	m_transform.setPosition(x, y, z);
}

void AssimpModel::rotate(const Vector3f &axis, float degrees) {
	m_transform.rotate(axis, degrees);
}

void AssimpModel::rotate(float pitch, float yaw, float roll) {
	m_transform.rotate(pitch, yaw, roll);
}

void AssimpModel::translate(float dx, float dy, float dz) {
	m_transform.translate(dx, dy, dz);
}

void AssimpModel::scale(float sx, float sy, float sz) {
	m_transform.scale(sx, sy, sz);
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

const AssimpMesh* AssimpModel::getMesh(unsigned short index) const {
	return m_meshes[index];
}

BoundingBox& AssimpModel::getAABB() {
	return m_aabb;
}

bool AssimpModel::loadModel(const char* a_filename, bool isStacked, bool generateTangents, bool flipYZ, bool flipWinding) {

	std::string filename(a_filename);

	const size_t index = filename.rfind('/');

	if (std::string::npos != index) {
		m_modelDirectory = filename.substr(0, index);
	}

	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile(a_filename, (generateTangents && flipWinding) ? (ASSIMP_LOAD_FLAGS | aiProcess_CalcTangentSpace) | aiProcess_FlipWindingOrder :
														   generateTangents ? ASSIMP_LOAD_FLAGS | aiProcess_CalcTangentSpace :  
														   flipWinding ? ASSIMP_LOAD_FLAGS | aiProcess_FlipWindingOrder :	
														   ASSIMP_LOAD_FLAGS);

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

		AssimpModel::ReadAiMaterial(aiMaterial, mesh->m_materialIndex, m_modelDirectory);

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

			if (!flipYZ) {
				vertexBuffer.push_back(aiMesh->mVertices[i].x); vertexBuffer.push_back(aiMesh->mVertices[i].y); vertexBuffer.push_back(aiMesh->mVertices[i].z);
			}
			else {
				vertexBuffer.push_back(aiMesh->mVertices[i].x); vertexBuffer.push_back(aiMesh->mVertices[i].z); vertexBuffer.push_back(aiMesh->mVertices[i].y);
			}

			if (m_hasTextureCoords || mesh->m_hasTextureCoords) {
				vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].x); vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].y);
			}

			if (m_hasNormals || mesh->m_hasNormals) {
				if (!flipYZ) {
					vertexBuffer.push_back(aiMesh->mNormals[i].x); vertexBuffer.push_back(aiMesh->mNormals[i].y); vertexBuffer.push_back(aiMesh->mNormals[i].z);
				}
				else {
					vertexBuffer.push_back(aiMesh->mNormals[i].x); vertexBuffer.push_back(aiMesh->mNormals[i].z); vertexBuffer.push_back(aiMesh->mNormals[i].y);
				}
			}

			if (m_hasTangents || mesh->m_hasTangents) {

				if (!flipYZ) {
					vertexBuffer.push_back(aiMesh->mTangents[i].x); vertexBuffer.push_back(aiMesh->mTangents[i].y); vertexBuffer.push_back(aiMesh->mTangents[i].z);
					vertexBuffer.push_back(aiMesh->mBitangents[i].x); vertexBuffer.push_back(aiMesh->mBitangents[i].y); vertexBuffer.push_back(aiMesh->mBitangents[i].z);
				}
				else {
					vertexBuffer.push_back(aiMesh->mTangents[i].x); vertexBuffer.push_back(aiMesh->mTangents[i].z); vertexBuffer.push_back(aiMesh->mTangents[i].y);
					vertexBuffer.push_back(aiMesh->mBitangents[i].x); vertexBuffer.push_back(aiMesh->mBitangents[i].z); vertexBuffer.push_back(aiMesh->mBitangents[i].y);
				}
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

	m_aabb.position = Vector3f(xmin, ymin, zmin);
	m_aabb.size = Vector3f(xmax, ymax, zmax) - Vector3f(xmin, ymin, zmin);
	return true;
}

void AssimpModel::addInstances(const std::vector<Matrix4f>& modelMTX) {
	m_instances.insert(m_instances.end(), modelMTX.begin(), modelMTX.end());
	m_instanceCount = m_instances.size();

	if (m_isStacked) {
		m_instanceCount = m_instances.size();

		glGenBuffers(1, &m_vboInstances);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(GLfloat) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);

		glBindVertexArray(0);
	}
	else {

		for (int j = 0; j < m_numberOfMeshes; j++) {
			m_meshes[j]->addInstance(*this);
		}
	}
}

void AssimpModel::addInstance(const Matrix4f& modelMTX) {
	m_instances.push_back(modelMTX);
	m_instanceCount = m_instances.size();

	if (m_isStacked) {
		m_instanceCount = m_instances.size();

		glGenBuffers(1, &m_vboInstances);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(GLfloat) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);

		glBindVertexArray(0);
	}
	else {

		for (int j = 0; j < m_numberOfMeshes; j++) {
			m_meshes[j]->addInstance(*this);
		}
	}
}

void AssimpModel::createInstancesDynamic(unsigned int numberOfInstances) {

	if (m_isStacked) {
		m_instanceCount = numberOfInstances;
		glGenBuffers(1, &m_vboInstances);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instanceCount * sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);


		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);

		glBindVertexArray(0);
	}
	else {
		for (int j = 0; j < m_numberOfMeshes; j++) {
			m_meshes[j]->createInstancesDynamic(numberOfInstances);
		}
	}
}

void AssimpModel::updateInstances(std::vector<Matrix4f>& modelMTX) {

	if (m_isStacked) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferSubData(GL_ARRAY_BUFFER, 0, modelMTX.size() * sizeof(GLfloat) * 4 * 4, modelMTX[0][0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else {
		for (int j = 0; j < m_numberOfMeshes; j++) {
			m_meshes[j]->updateInstances(modelMTX);
		}
	}
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

void AssimpModel::draw(const Camera& camera) {
	for (int i = 0; i < m_meshes.size(); i++) {
		Material& material = Material::GetMaterials()[m_meshes[i]->m_materialIndex];
		material.updateMaterialUbo(BuiltInShader::materialUbo);
		if (!m_shader[i]->inUse()) {
			m_shader[i]->use();
			m_shader[i]->loadMatrix("u_projection", camera.getPerspectiveMatrix());
			m_shader[i]->loadMatrix("u_view", camera.getViewMatrix());
			m_shader[i]->loadMatrix("u_model", m_transform.getTransformationMatrix());
		}
		material.bind();
		m_meshes[i]->drawRaw();
	}
	unuseAllShader();
	Texture::Unbind();
}

void AssimpModel::drawInstanced(const Camera& camera) {
	for (int i = 0; i < m_meshes.size(); i++) {
		Material& material = Material::GetMaterials()[m_meshes[i]->m_materialIndex];
		material.updateMaterialUbo(BuiltInShader::materialUbo);

		if (!m_shader[i]->inUse()) {
			m_shader[i]->use();
			m_shader[i]->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		}
		material.bind();
		m_meshes[i]->drawRawInstanced();

	}
	unuseAllShader();
	Texture::Unbind();
}

void AssimpModel::drawStacked(const Camera& camera) {
	glBindVertexArray(m_vao);
	for (int i = 0; i < m_meshes.size(); i++) {
		Material& material = Material::GetMaterials()[m_meshes[i]->m_materialIndex];
		material.updateMaterialUbo(BuiltInShader::materialUbo);

		if (!m_shader[i]->inUse()) {
			m_shader[i]->use();
			m_shader[i]->loadMatrix("u_projection", camera.getPerspectiveMatrix());
			m_shader[i]->loadMatrix("u_view", camera.getViewMatrix());
			m_shader[i]->loadMatrix("u_model", m_transform.getTransformationMatrix());
		}
		material.bind();
		glDrawElementsBaseVertex(GL_TRIANGLES, m_meshes[i]->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[i]->m_baseIndex), m_meshes[i]->m_baseVertex);
	}
	unuseAllShader();
	Texture::Unbind();
	glBindVertexArray(0);
}

void AssimpModel::drawInstancedStacked(const Camera& camera) {
	glBindVertexArray(m_vao);
	for (int i = 0; i < m_meshes.size(); i++) {
		Material& material = Material::GetMaterials()[m_meshes[i]->m_materialIndex];
		material.updateMaterialUbo(BuiltInShader::materialUbo);

		if (!m_shader[i]->inUse()) {
			m_shader[i]->use();
			m_shader[i]->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		}
		material.bind();
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, m_meshes[i]->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_meshes[i]->m_baseIndex), m_instanceCount, m_meshes[i]->m_baseVertex, 0);
	}
	unuseAllShader();
	Texture::Unbind();
	glBindVertexArray(0);
}

void AssimpModel::unuseAllShader() {
	for (Shader* shader : m_shader) {
		if (shader->inUse()) {
			shader->unuse();
		}
	}
}

void AssimpModel::drawAABB() {
	m_aabb.drawRaw();
}

void AssimpModel::createAABB() {
	m_aabb.createBuffer();
	m_hasAABB = true;
}

void AssimpModel::initShader(bool instanced) {

	if (!BuiltInShader::materialUbo) {
		glGenBuffers(1, &BuiltInShader::materialUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::materialUbo);
		glBufferData(GL_UNIFORM_BUFFER, 52, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, BuiltInShader::materialBinding, BuiltInShader::materialUbo, 0, 52);
	}

	if (!BuiltInShader::viewUbo && instanced) {
		glGenBuffers(1, &BuiltInShader::materialUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::materialUbo);
		glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, BuiltInShader::viewBinding, BuiltInShader::viewUbo, 0, 52);
	}

	for (int i = 0; i < m_meshes.size(); i++) {
		Material& material = Material::GetMaterials()[m_meshes[i]->getMaterialIndex()];

		if (material.textures.size() > 0) {
			if (!ShaderManager.checkAsset(instanced ? "diffuse_texture_instance" : "diffuse_texture")) {
				ShaderManager.loadShaderFromString(instanced ? "diffuse_texture_instance" : "diffuse_texture", instanced ? DIFFUSE_TEXTURE_INSTANCE_VS : DIFFUSE_TEXTURE_VS, instanced ? DIFFUSE_TEXTURE_INSTANCE_FS : DIFFUSE_TEXTURE_FS);

				glUniformBlockBinding(ShaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->m_program, glGetUniformBlockIndex(ShaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->m_program, "u_material"), BuiltInShader::materialBinding);

				if (instanced) {
					glUniformBlockBinding(ShaderManager.getAssetPointer("diffuse_texture_instance")->m_program, glGetUniformBlockIndex(ShaderManager.getAssetPointer("diffuse_texture_instance")->m_program, "u_view"), BuiltInShader::viewBinding);
				}
			}
			m_shader.push_back(ShaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture"));
		}else {
			if (!ShaderManager.checkAsset(instanced ? "diffuse_instance" : "diffuse")) {
				ShaderManager.loadShaderFromString(instanced ? "diffuse_instance" : "diffuse", instanced ? DIFFUSE_INSTANCE_VS : DIFFUSE_VS, instanced ? DIFFUSE_INSTANCE_FS : DIFFUSE_FS);

				glUniformBlockBinding(ShaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse")->m_program, glGetUniformBlockIndex(ShaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse")->m_program, "u_material"), BuiltInShader::materialBinding);

				if (instanced) {
					glUniformBlockBinding(ShaderManager.getAssetPointer("diffuse_instance")->m_program, glGetUniformBlockIndex(ShaderManager.getAssetPointer("diffuse_instance")->m_program, "u_view"), BuiltInShader::viewBinding);
				}
			}
			m_shader.push_back(ShaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse"));
		}
	}
}

void AssimpModel::initShader(AssetManager<Shader>& shaderManager, bool instanced) {

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
		Material& material = Material::GetMaterials()[m_meshes[i]->getMaterialIndex()];

		if (material.textures.size() > 0) {

			if (!ShaderManager.checkAsset(instanced ? "diffuse_texture_instance" : "diffuse_texture")) {
				ShaderManager.loadShaderFromString(instanced ? "diffuse_texture_instance" : "diffuse_texture", instanced ? DIFFUSE_TEXTURE_INSTANCE_VS : DIFFUSE_TEXTURE_VS, instanced ? DIFFUSE_TEXTURE_INSTANCE_FS : DIFFUSE_TEXTURE_FS);

				glUniformBlockBinding(ShaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->m_program, glGetUniformBlockIndex(ShaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture")->m_program, "u_material"), BuiltInShader::materialBinding);

				if (instanced) {
					glUniformBlockBinding(ShaderManager.getAssetPointer("diffuse_texture_instance")->m_program, glGetUniformBlockIndex(ShaderManager.getAssetPointer("diffuse_texture_instance")->m_program, "u_view"), BuiltInShader::viewBinding);
				}
			}
			m_shader.push_back(ShaderManager.getAssetPointer(instanced ? "diffuse_texture_instance" : "diffuse_texture"));
		}else {

			if (!ShaderManager.checkAsset(instanced ? "diffuse_instance" : "diffuse")) {
				ShaderManager.loadShaderFromString(instanced ? "diffuse_instance" : "diffuse", instanced ? DIFFUSE_INSTANCE_VS : DIFFUSE_VS, instanced ? DIFFUSE_INSTANCE_FS : DIFFUSE_FS);

				glUniformBlockBinding(ShaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse")->m_program, glGetUniformBlockIndex(ShaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse")->m_program, "u_material"), BuiltInShader::materialBinding);

				if (instanced) {
					glUniformBlockBinding(ShaderManager.getAssetPointer("diffuse_instance")->m_program, glGetUniformBlockIndex(ShaderManager.getAssetPointer("diffuse_instance")->m_program, "u_view"), BuiltInShader::viewBinding);
				}
			}
			m_shader.push_back(ShaderManager.getAssetPointer(instanced ? "diffuse_instance" : "diffuse"));
		}
	}
}

void AssimpModel::CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& vao, unsigned int vbo, unsigned int& ibo, unsigned int stride) {

	if (vao)
		glDeleteVertexArrays(1, &vao);

	if (vbo)
		glDeleteBuffers(1, &vbo);

	if (ibo)
		glDeleteBuffers(1, &ibo);

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Positions
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
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

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int), &indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}


std::string AssimpModel::GetTexturePath(std::string texPath, std::string modelDirectory) {

	int foundSlash = texPath.find_last_of("/\\");

	int foundDot = texPath.find_last_of(".");
	foundDot = (foundDot < 0 ? texPath.length() : foundDot);
	foundDot = foundSlash < 0 ? foundDot : foundDot - 1;
	std::string textureName = texPath.substr(foundSlash + 1, foundDot);

	return textureName, foundSlash < 0 ? modelDirectory + "/" + texPath.substr(foundSlash + 1) : texPath;
}

void AssimpModel::ReadAiMaterial(const aiMaterial* aiMaterial, short& index, std::string modelDirectory) {
	std::vector<Material>::iterator it = std::find_if(Material::GetMaterials().begin(), Material::GetMaterials().end(), std::bind(compareMaterial, std::placeholders::_1, modelDirectory));
	if (it == Material::GetMaterials().end()) {

		Material::GetMaterials().resize(Material::GetMaterials().size() + 1);
		index = Material::GetMaterials().size() - 1;
		Material& material = Material::GetMaterials().back();
		material.name = modelDirectory;

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

		int numTextures = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), name);
			material.textures[0] = Texture();
			material.textures[0].loadFromFile(GetTexturePath(name.data, modelDirectory), true);
			material.textures[0].setFilter(GL_LINEAR_MIPMAP_LINEAR);
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_NORMALS);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), name);
			material.textures[1] = Texture();
			material.textures[1].loadFromFile(GetTexturePath(name.data, modelDirectory), true);
			material.textures[1].setFilter(GL_LINEAR_MIPMAP_LINEAR);
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_SPECULAR);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), name);
			material.textures[2] = Texture();
			material.textures[2].loadFromFile(GetTexturePath(name.data, modelDirectory), true);
			material.textures[2].setFilter(GL_LINEAR_MIPMAP_LINEAR);
		}
	}
	else {
		index = std::distance(Material::GetMaterials().begin(), it);
	}
}

void AssimpModel::Cleanup() {
	ShaderManager.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AssimpMesh::AssimpMesh(AssimpModel* model) {
	m_model = model;
	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
}

AssimpMesh::~AssimpMesh() {
	cleanup();
}

void AssimpMesh::cleanup() {
	if (m_vao)
		glDeleteVertexArrays(1, &m_vao);

	if (m_vbo)
		glDeleteBuffers(1, &m_vbo);

	if (m_ibo)
		glDeleteBuffers(1, &m_ibo);

	if (m_vboInstances)
		glDeleteBuffers(1, &m_vboInstances);

	m_vertexBuffer.clear();
	m_vertexBuffer.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
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

std::vector<float>& AssimpMesh::getVertexBuffer() {
	return m_vertexBuffer;
}

std::vector<unsigned int>& AssimpMesh::getIndexBuffer() {
	return m_indexBuffer;
}

unsigned int AssimpMesh::getStride() {
	return m_stride;
}

short AssimpMesh::getMaterialIndex() {
	return m_materialIndex;
}

const Material& AssimpMesh::getMaterial() const {
	return Material::GetMaterials()[m_materialIndex];
}

void AssimpMesh::addInstance(const AssimpModel& model) {

	const std::vector<Matrix4f>& instances = model.m_instances;
	m_instanceCount = instances.size();

	if (m_vboInstances) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(float) * 4 * 4, instances[0][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else {
		glGenBuffers(1, &m_vboInstances);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(float) * 4 * 4, instances[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);

		glBindVertexArray(0);
	}
}

void AssimpMesh::createInstancesDynamic(unsigned int numberOfInstances) {
	m_instanceCount = numberOfInstances;

	glGenBuffers(1, &m_vboInstances);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferData(GL_ARRAY_BUFFER, m_instanceCount * sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));

	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);

	glBindVertexArray(0);
}

void AssimpMesh::updateInstances(std::vector<Matrix4f>& modelMTX) {
	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferSubData(GL_ARRAY_BUFFER, 0, modelMTX.size() * sizeof(GLfloat) * 4 * 4, modelMTX[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}