#include "AssimpModel.h"

AssetManager<Shader> AssimpModel::ShaderManager;

bool compareMaterial(Material const& s1, std::string const& s2) {
	return s1.name == s2;
}

AssimpModel::AssimpModel() {
	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_hasMaterial = false;
	m_hasAABB = false;
	m_hasBoundingSphere = false;
	m_hasConvexHull = false;
	m_isStacked = false;
	m_markForDelete = false;

	m_numberOfVertices = 0u;
	m_numberOfTriangles = 0u;
	m_numberOfMeshes = 0u;
	m_stride = 0u;

	m_drawCount = 0u;
	m_instanceCount = 0u;
	m_vao = 0u;
	m_vbo = 0u;
	m_ibo = 0u;
	m_vboInstances = 0u;
	
	m_transform.reset();
}

AssimpModel::AssimpModel(AssimpModel const& rhs) {
	m_numberOfVertices = rhs.m_numberOfVertices;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_hasAABB = rhs.m_hasAABB;
	m_hasBoundingSphere = rhs.m_hasBoundingSphere;
	m_hasConvexHull = rhs.m_hasConvexHull;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_center = rhs.m_center;
	m_aabb = rhs.m_aabb;
	m_shader = rhs.m_shader;
	m_transform = rhs.m_transform;
	m_drawCount = rhs.m_drawCount;
	m_instanceCount = rhs.m_instanceCount;
	m_instances = rhs.m_instances;
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;

	//m_vertexBuffer = rhs.m_vertexBuffer;
    //m_indexBuffer = rhs.m_indexBuffer;

	m_markForDelete = false;
}

AssimpModel::AssimpModel(AssimpModel&& rhs) {
	m_numberOfVertices = rhs.m_numberOfVertices;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_hasAABB = rhs.m_hasAABB;
	m_hasBoundingSphere = rhs.m_hasBoundingSphere;
	m_hasConvexHull = rhs.m_hasConvexHull;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_center = rhs.m_center;
	m_aabb = rhs.m_aabb;
	m_shader = rhs.m_shader;
	m_transform = rhs.m_transform;
	m_drawCount = rhs.m_drawCount;
	m_instanceCount = rhs.m_instanceCount;
	m_instances = rhs.m_instances;
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;

	//m_vertexBuffer = rhs.m_vertexBuffer;
    //m_indexBuffer = rhs.m_indexBuffer;

	m_markForDelete = false;
}

AssimpModel& AssimpModel::operator=(const AssimpModel& rhs) {
	m_numberOfVertices = rhs.m_numberOfVertices;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_hasAABB = rhs.m_hasAABB;
	m_hasBoundingSphere = rhs.m_hasBoundingSphere;
	m_hasConvexHull = rhs.m_hasConvexHull;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_center = rhs.m_center;
	m_aabb = rhs.m_aabb;
	m_shader = rhs.m_shader;
	m_transform = rhs.m_transform;
	m_drawCount = rhs.m_drawCount;
	m_instanceCount = rhs.m_instanceCount;
	m_instances = rhs.m_instances;
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;

	//m_vertexBuffer = rhs.m_vertexBuffer;
	//m_indexBuffer = rhs.m_indexBuffer;

	m_markForDelete = false;
	return *this;
}

AssimpModel& AssimpModel::operator=(AssimpModel&& rhs) {
	m_numberOfVertices = rhs.m_numberOfVertices;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_hasAABB = rhs.m_hasAABB;
	m_hasBoundingSphere = rhs.m_hasBoundingSphere;
	m_hasConvexHull = rhs.m_hasConvexHull;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_center = rhs.m_center;
	m_aabb = rhs.m_aabb;
	m_shader = rhs.m_shader;
	m_transform = rhs.m_transform;
	m_drawCount = rhs.m_drawCount;
	m_instanceCount = rhs.m_instanceCount;
	m_instances = rhs.m_instances;
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;

	//m_vertexBuffer = rhs.m_vertexBuffer;
   //m_indexBuffer = rhs.m_indexBuffer;

	m_markForDelete = false;
	return *this;
}

AssimpModel::~AssimpModel() {
	if (m_markForDelete) {
		cleanup();
	}
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

	for (auto mesh : m_meshes) {
		delete mesh;
	}

	m_meshes.clear();
	m_meshes.shrink_to_fit();

	m_shader.clear();
	m_aabb.cleanup();
}

void AssimpModel::markForDelete() {
	m_markForDelete = true;
	for(auto&& mesh : m_meshes) {
		mesh->markForDelete();
	}
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

std::vector<AssimpMesh*>& AssimpModel::getMeshes() {
	return m_meshes;
}

std::vector<float>& AssimpModel::getVertexBuffer() {
	return m_vertexBuffer;
}

std::vector<unsigned int>& AssimpModel::getIndexBuffer() {
	return m_indexBuffer;
}

const AssimpMesh* AssimpModel::getMesh(unsigned short index) const {
	return m_meshes[index];
}

const BoundingBox& AssimpModel::getAABB() const {
	return m_aabb;
}

void AssimpModel::loadModel(const char* filename, bool isStacked, bool generateNormals, bool generateTangents, bool flipYZ, bool flipWinding) {
	loadModelCpu(filename, Vector3f(0.0, 1.0, 0.0), 0.0, Vector3f(0.0, 0.0, 0.0), 1.0, isStacked, generateNormals, generateTangents, flipYZ, flipWinding);
	loadModelGpu();
}

void AssimpModel::loadModel(const char* filename, Vector3f& axis, float degree, Vector3f& translate, float scale, bool isStacked, bool generateNormals, bool generateTangents, bool flipYZ, bool flipWinding) {
	loadModelCpu(filename, axis, degree, translate, scale, isStacked, generateNormals, generateTangents, flipYZ, flipWinding);
	loadModelGpu();
}

void AssimpModel::loadModelCpu(const char* filename, bool isStacked, bool generateNormals, bool generateTangents, bool flipYZ, bool flipWinding) {
	loadModelCpu(filename, Vector3f(0.0, 1.0, 0.0), 0.0, Vector3f(0.0, 0.0, 0.0), 1.0, isStacked, generateNormals, generateTangents, flipYZ, flipWinding);
}

void AssimpModel::loadModelCpu(const char* _filename, Vector3f& axis, float degree, Vector3f& translate, float scale, bool isStacked, bool generateNormals, bool generateTangents, bool flipYZ, bool flipWinding) {
	std::string filename(_filename);

	const size_t index = filename.rfind('/');

	if (std::string::npos != index) {
		m_modelDirectory = filename.substr(0, index);
	}

	Assimp::Importer Importer;
	unsigned int flag = ASSIMP_LOAD_FLAGS;
	flag = generateNormals ? flag | aiProcess_GenSmoothNormals : flag;
	flag = generateTangents ? flag | aiProcess_CalcTangentSpace : flag;
	flag = flipWinding ? flag | aiProcess_FlipWindingOrder : flag;

	const aiScene* pScene = Importer.ReadFile(_filename, flag);

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

		if (aiMaterial->GetName().length != 0) {
			AssimpModel::ReadAiMaterial(aiMaterial, mesh->m_materialIndex, m_modelDirectory, aiMaterial->GetName().length == 0 ? "default" : aiMaterial->GetName().data);
		}
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
			float posX = aiMesh->mVertices[i].x;
			float posY = flipYZ ? aiMesh->mVertices[i].z : aiMesh->mVertices[i].y;
			float posZ = flipYZ ? aiMesh->mVertices[i].y : aiMesh->mVertices[i].z;

			Matrix4f rot;
			rot.rotate(axis, degree);
			Vector3f pos = rot * Vector3f(posX, posY, posZ);

			posX = pos[0] * scale + translate[0];
			posY = pos[1] * scale + translate[1];
			posZ = pos[2] * scale + translate[2];

			xmin = (std::min)(posX, xmin);
			ymin = (std::min)(posY, ymin);
			zmin = (std::min)(posZ, zmin);

			xmax = (std::max)(posX, xmax);
			ymax = (std::max)(posY, ymax);
			zmax = (std::max)(posZ, zmax);

			vertexBuffer.push_back(posX); vertexBuffer.push_back(posY); vertexBuffer.push_back(posZ);

			if (m_hasTextureCoords || mesh->m_hasTextureCoords) {
				//std::cout << "Tex Coord: " << aiMesh->mTextureCoords[0][i].x << "  " << aiMesh->mTextureCoords[0][i].y << std::endl << std::endl;
				vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].x); vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].y);
			}

			if (m_hasNormals || mesh->m_hasNormals) {
				float normY = flipYZ ? aiMesh->mNormals[i].z : aiMesh->mNormals[i].y;
				float normZ = flipYZ ? aiMesh->mNormals[i].y : aiMesh->mNormals[i].z;

				Matrix4f rot;
				rot.rotate(axis, degree);

				Vector3f normal = rot * Vector3f(aiMesh->mNormals[i].x, normY, normZ);
				vertexBuffer.push_back(normal[0]); vertexBuffer.push_back(normal[1]); vertexBuffer.push_back(normal[2]);
			}

			if (m_hasTangents || mesh->m_hasTangents) {

				float tangY = flipYZ ? aiMesh->mTangents[i].z : aiMesh->mTangents[i].y;
				float tangZ = flipYZ ? aiMesh->mTangents[i].y : aiMesh->mTangents[i].z;

				float bitangY = flipYZ ? aiMesh->mBitangents[i].z : aiMesh->mBitangents[i].y;
				float bitangZ = flipYZ ? aiMesh->mBitangents[i].y : aiMesh->mBitangents[i].z;

				Matrix4f rot;
				rot.rotate(axis, degree);

				Vector3f tangent = rot * Vector3f(aiMesh->mTangents[i].x, tangY, tangZ);
				Vector3f bitangent = rot * Vector3f(aiMesh->mBitangents[i].x, bitangY, bitangZ);

				vertexBuffer.push_back(tangent[0]); vertexBuffer.push_back(tangent[1]); vertexBuffer.push_back(tangent[2]);
				vertexBuffer.push_back(bitangent[0]); vertexBuffer.push_back(bitangent[1]); vertexBuffer.push_back(bitangent[2]);
			}

		}

		for (unsigned int t = 0; t < aiMesh->mNumFaces; ++t) {
			const aiFace* face = &aiMesh->mFaces[t];
			indexBuffer.push_back(face->mIndices[0]);
			indexBuffer.push_back(face->mIndices[1]);
			indexBuffer.push_back(face->mIndices[2]);
		}

		mesh->m_drawCount = aiMesh->mNumFaces * 3;
	}

	m_center = Vector3f((xmax + xmin) * 0.5f, (ymax + ymin) * 0.5f, (zmax + zmin) * 0.5f);
	m_aabb.min = Vector3f(xmin, ymin, zmin);
	m_aabb.max = Vector3f(xmax, ymax, zmax);
}


void AssimpModel::loadModelGpu() {
	if (m_isStacked) {
		AssimpModel::CreateBuffer(m_vertexBuffer, m_indexBuffer, m_vao, m_vbo, m_ibo, m_stride);
		//m_vertexBuffer.clear();
		//m_vertexBuffer.shrink_to_fit();
		//m_indexBuffer.clear();
		//m_indexBuffer.shrink_to_fit();
	}else{
		for (auto&& mesh : m_meshes) {
			AssimpModel::CreateBuffer(mesh->m_vertexBuffer, mesh->m_indexBuffer, mesh->m_vao,mesh->m_vbo, mesh->m_ibo, mesh->m_stride);
			//mesh->m_vertexBuffer.clear(); 
			//mesh->m_vertexBuffer.shrink_to_fit();
			//mesh->m_indexBuffer.clear();
			//mesh->m_indexBuffer.shrink_to_fit();
		}
	}
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

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);	

	}else {

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

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
	}else {

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

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
	}else {
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

void AssimpModel::drawRaw() const{
	for (int j = 0; j < m_numberOfMeshes; j++) {
		m_meshes[j]->drawRaw();
	}
}

void AssimpModel::drawRawInstanced() const{
	for (int j = 0; j < m_numberOfMeshes; j++) {
		m_meshes[j]->drawRawInstanced();
	}
}

void  AssimpModel::drawRawStacked() const{
	glBindVertexArray(m_vao);
	for (auto&& mesh : m_meshes) {
		if(mesh->m_materialIndex >= 0)
			Material::GetMaterials()[mesh->m_materialIndex].updateMaterialUbo(BuiltInShader::materialUbo);

		mesh->m_textureIndex >= 0 ? Material::GetTextures()[mesh->m_textureIndex].bind() : Texture::Unbind();

		glDrawElementsBaseVertex(GL_TRIANGLES, mesh->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->m_baseIndex), mesh->m_baseVertex);
	}
	glBindVertexArray(0);
}

void AssimpModel::drawRawInstancedStacked() const{
	glBindVertexArray(m_vao);
	for (auto&& mesh : m_meshes) {
		if (mesh->m_materialIndex >= 0)
			Material::GetMaterials()[mesh->m_materialIndex].updateMaterialUbo(BuiltInShader::materialUbo);

		mesh->m_textureIndex >= 0 ? Material::GetTextures()[mesh->m_textureIndex].bind() : Texture::Unbind();

		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, mesh->m_drawCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->m_baseIndex), m_instanceCount, mesh->m_baseVertex, 0);
	}
	glBindVertexArray(0);
}

void AssimpModel::draw(const Camera& camera) const{
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
	//Texture::Unbind();
}

void AssimpModel::draw(const Camera& camera, unsigned short meshIndex) const {
	Material& material = Material::GetMaterials()[m_meshes[meshIndex]->m_materialIndex];
	material.updateMaterialUbo(BuiltInShader::materialUbo);
	if (!m_shader[meshIndex]->inUse()) {
		m_shader[meshIndex]->use();

		m_shader[meshIndex]->loadMatrix("u_projection", camera.getPerspectiveMatrix());
		m_shader[meshIndex]->loadMatrix("u_view", camera.getViewMatrix());
		m_shader[meshIndex]->loadMatrix("u_model", m_transform.getTransformationMatrix());
	}

	material.bind();
	m_meshes[meshIndex]->drawRaw();

	unuseAllShader();
	//Texture::Unbind();
}

void AssimpModel::drawInstanced(const Camera& camera) const{
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
	//Texture::Unbind();
}

void AssimpModel::drawStacked(const Camera& camera) const{
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
	//Texture::Unbind();
	glBindVertexArray(0);
}

void AssimpModel::drawInstancedStacked(const Camera& camera) const{
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

void AssimpModel::unuseAllShader() const{
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
		glBufferData(GL_UNIFORM_BUFFER, 56, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, BuiltInShader::materialBinding, BuiltInShader::materialUbo, 0, 56);
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

void AssimpModel::initShader(AssetManager<Shader>& shaderManager, bool instanced) {

	if (!BuiltInShader::materialUbo) {
		glGenBuffers(1, &BuiltInShader::materialUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::materialUbo);
		glBufferData(GL_UNIFORM_BUFFER, 56, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, BuiltInShader::materialBinding, BuiltInShader::materialUbo, 0, 56);
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
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

void AssimpModel::ReadAiMaterial(const aiMaterial* aiMaterial, short& index, std::string modelDirectory, std::string mltName) {
	//skip assimp default material
	if(mltName == "DefaultMaterial" || mltName == "default")
		return;

	std::vector<Material>::iterator it = std::find_if(Material::GetMaterials().begin(), Material::GetMaterials().end(), std::bind(compareMaterial, std::placeholders::_1, mltName));
	if (it == Material::GetMaterials().end()) {

		Material::GetMaterials().resize(Material::GetMaterials().size() + 1);
		index = Material::GetMaterials().size() - 1;
		Material& material = Material::GetMaterials().back();
		material.name = mltName;
		
		float shininess = 0.0f;
		aiColor4D diffuse = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		aiColor4D ambient = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		aiColor4D specular = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

		if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) { }
		material.setDiffuse({ diffuse.r , diffuse.g , diffuse.b , diffuse.a });

		if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient)) { }
		material.setAmbient({ ambient.r , ambient.g , ambient.b , ambient.a });

		if (AI_SUCCESS == aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_SPECULAR, &specular)) { }
		material.setSpecular({ specular.r , specular.g , specular.b , specular.a });

		if (AI_SUCCESS == aiGetMaterialFloat(aiMaterial, AI_MATKEY_SHININESS, &shininess)) { }

		material.setShininess(shininess);

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
	}else {
		index = std::distance(Material::GetMaterials().begin(), it);
	}
}

void AssimpModel::CleanupShader() {
	ShaderManager.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AssimpMesh::AssimpMesh(AssimpModel* model) {
	m_model = model;
	
	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_markForDelete = false;

	m_numberOfTriangles = 0u;
	m_stride = 0u;
	m_baseVertex = 0u;
	m_baseIndex = 0u;

	m_vao = 0u;
	m_vbo = 0u;
	m_vboInstances = 0u;
	m_ibo = 0u;

	m_drawCount = 0u;
	m_instanceCount = 0u;
	m_materialIndex = -1;
	m_textureIndex = -1;
}

AssimpMesh::AssimpMesh(AssimpMesh const& rhs) {
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;
	m_stride = rhs.m_stride;
	m_model = rhs.m_model;
	m_drawCount = rhs.m_drawCount;
	m_instanceCount = rhs.m_instanceCount;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_baseVertex = rhs.m_baseVertex;
	m_baseIndex = rhs.m_baseIndex;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	m_markForDelete = false;

	//m_vertexBuffer = rhs.m_vertexBuffer;
    //m_indexBuffer = rhs.m_indexBuffer;
}

AssimpMesh::AssimpMesh(AssimpMesh&& rhs) {
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;
	m_stride = rhs.m_stride;
	m_model = rhs.m_model;
	m_drawCount = rhs.m_drawCount;
	m_instanceCount = rhs.m_instanceCount;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_baseVertex = rhs.m_baseVertex;
	m_baseIndex = rhs.m_baseIndex;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	m_markForDelete = false;

	//m_vertexBuffer = rhs.m_vertexBuffer;
    //m_indexBuffer = rhs.m_indexBuffer;
}

AssimpMesh& AssimpMesh::operator=(const AssimpMesh& rhs) {
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;
	m_stride = rhs.m_stride;
	m_model = rhs.m_model;
	m_drawCount = rhs.m_drawCount;
	m_instanceCount = rhs.m_instanceCount;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_baseVertex = rhs.m_baseVertex;
	m_baseIndex = rhs.m_baseIndex;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	m_markForDelete = false;

	//m_vertexBuffer = rhs.m_vertexBuffer;
    //m_indexBuffer = rhs.m_indexBuffer;

	return *this;
}

AssimpMesh& AssimpMesh::operator=(AssimpMesh&& rhs) {
	m_vao = rhs.m_vao;
	m_vbo = rhs.m_vbo;
	m_ibo = rhs.m_ibo;
	m_vboInstances = rhs.m_vboInstances;
	m_stride = rhs.m_stride;
	m_model = rhs.m_model;
	m_drawCount = rhs.m_drawCount;
	m_instanceCount = rhs.m_instanceCount;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_baseVertex = rhs.m_baseVertex;
	m_baseIndex = rhs.m_baseIndex;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	m_markForDelete = false;

	//m_vertexBuffer = rhs.m_vertexBuffer;
    //m_indexBuffer = rhs.m_indexBuffer;

	return *this;
}

AssimpMesh::~AssimpMesh() {
	if (m_markForDelete) {
		cleanup();
	}
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


void AssimpMesh::markForDelete() {
	m_markForDelete = true;
}

void AssimpMesh::drawRaw() const{

	if (m_materialIndex >= 0)
		Material::GetMaterials()[m_materialIndex].bind();

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void AssimpMesh::drawRawInstanced() const{

	if (m_materialIndex >= 0) 
		Material::GetMaterials()[m_materialIndex].bind();

	if (m_textureIndex >= 0)
		Material::GetTextures()[m_textureIndex].bind();

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

short AssimpMesh::getMaterialIndex() const {
	return m_materialIndex;
}

void AssimpMesh::setMaterialIndex(short index) const {
	m_materialIndex = index;
}

short AssimpMesh::getTextureIndex()const {
	return m_textureIndex;
}

void AssimpMesh::setTextureIndex(short index) const {
	m_textureIndex = index;
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

	}else {
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

		glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AssimpMesh::updateInstances(std::vector<Matrix4f>& modelMTX) {
	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferSubData(GL_ARRAY_BUFFER, 0, modelMTX.size() * sizeof(GLfloat) * 4 * 4, modelMTX[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}