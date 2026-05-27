#include "AssimpModel.h"

bool compareMaterial(Material const& s1, std::string const& s2) {
	return s1.m_name == s2;
}

AssimpModel::AssimpModel() {
	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
	m_hasMaterial = false;
	m_isStacked = false;

	m_numberOfTriangles = 0u;
	m_numberOfMeshes = 0u;
	m_stride = 0u;

	m_drawCount = 0u;
}

AssimpModel::AssimpModel(AssimpModel const& rhs) {
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_center = rhs.m_center;
	m_drawCount = rhs.m_drawCount;
}

AssimpModel::AssimpModel(AssimpModel&& rhs) noexcept {
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_center = rhs.m_center;
	m_drawCount = rhs.m_drawCount;
}

AssimpModel& AssimpModel::operator=(const AssimpModel& rhs) {
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_center = rhs.m_center;
	m_drawCount = rhs.m_drawCount;
	return *this;
}

AssimpModel& AssimpModel::operator=(AssimpModel&& rhs) noexcept {
	m_numberOfTriangles = rhs.m_numberOfTriangles;
	m_numberOfMeshes = rhs.m_numberOfMeshes;
	m_stride = rhs.m_stride;
	m_hasTextureCoords = rhs.m_hasTextureCoords;
	m_hasNormals = rhs.m_hasNormals;
	m_hasTangents = rhs.m_hasTangents;
	m_hasMaterial = rhs.m_hasMaterial;
	m_isStacked = rhs.m_isStacked;
	m_meshes = rhs.m_meshes;
	m_modelDirectory = rhs.m_modelDirectory;
	m_center = rhs.m_center;
	m_drawCount = rhs.m_drawCount;
	return *this;
}

AssimpModel::~AssimpModel() {
	cleanup();	
}

void AssimpModel::cleanup() {
	m_vertexBuffer.clear();
	m_vertexBuffer.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();

	for (Mesh* mesh : m_meshes) {
		delete mesh;
	}

	m_meshes.clear();
	m_meshes.shrink_to_fit();
}

const Vector3f &AssimpModel::getCenter() const {
	return m_center;
}

const unsigned int AssimpModel::getStride() const {
	return m_isStacked ? m_stride : m_meshes.back()->getStride();
}

const std::string& AssimpModel::getModelDirectory() {
	return m_modelDirectory;
}

const Mesh* AssimpModel::getMesh(unsigned short index) const {
	return m_meshes[index];
}

const std::vector<Mesh*>& AssimpModel::getMeshes() const {
	return m_meshes;
}

const std::vector<float>& AssimpModel::getVertexBuffer() const {
	return m_vertexBuffer;
}

const std::vector<unsigned int>& AssimpModel::getIndexBuffer() const {
	return m_indexBuffer;
}

const unsigned int AssimpModel::getNumberOfTriangles() const {
	return m_drawCount / 3;
}

void AssimpModel::generateNormals() {
	if (m_isStacked) {
		Model::GenerateNormals(m_vertexBuffer, m_indexBuffer, *this, m_hasNormals, m_stride, 0, m_meshes.size());
	}else {
		for (int j = 0; j < m_meshes.size(); j++) {
			Model::GenerateNormals(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, *this, m_meshes[j]->m_hasNormals, m_meshes[j]->m_stride, j, j + 1);
		}
	}
}

void AssimpModel::rewind() {
	if (m_isStacked) {
		Model::Rewind(m_vertexBuffer, m_indexBuffer, m_stride);
	}else {
		for (int j = 0; j < m_meshes.size(); j++) {
			Model::Rewind(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, m_meshes[j]->m_stride);
		}
	}
}

void AssimpModel::generateColors(ModelColor modelColor) {
	if (m_isStacked) {
		Model::GenerateColors(m_vertexBuffer, m_indexBuffer, m_stride, 0, m_meshes.size(), modelColor);
	}else {
		for (int j = 0; j < m_meshes.size(); j++) {
			Model::GenerateColors(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_indexBuffer, m_meshes[j]->m_stride, j, j + 1, modelColor);
		}
	}
}

void AssimpModel::packBuffer() {
	if (m_isStacked) {
		Model::PackBuffer(m_vertexBuffer, m_stride);
	}else {
		for (int j = 0; j < m_meshes.size(); j++) {
			Model::PackBuffer(m_meshes[j]->m_vertexBuffer, m_meshes[j]->m_stride);
		}
	}
}

void AssimpModel::loadModel(const char* filename, bool isStacked, bool generateNormals, bool generateTangents, bool flipYZ, bool flipWinding) {
	loadModelCpu(filename, Vector3f(0.0, 1.0, 0.0), 0.0, Vector3f(0.0, 0.0, 0.0), 1.0, isStacked, generateNormals, generateTangents, flipYZ, flipWinding);
}

void AssimpModel::loadModel(const char* filename, const Vector3f& axis, float degrees, const Vector3f& translate, float scale, bool isStacked, bool generateNormals, bool generateTangents, bool flipYZ, bool flipWinding) {
	loadModelCpu(filename, axis, degrees, translate, scale, isStacked, generateNormals, generateTangents, flipYZ, flipWinding);
}

void AssimpModel::loadModelCpu(const char* filename, bool isStacked, bool generateNormals, bool generateTangents, bool flipYZ, bool flipWinding) {
	loadModelCpu(filename, Vector3f(0.0, 1.0, 0.0), 0.0, Vector3f(0.0, 0.0, 0.0), 1.0, isStacked, generateNormals, generateTangents, flipYZ, flipWinding);
}

void AssimpModel::loadModelCpu(const char* _filename, const Vector3f& axis, float degrees, const Vector3f& translate, float scale, bool isStacked, bool generateNormals, bool generateTangents, bool flipYZ, bool flipWinding) {
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

	/*Assimp::Exporter Exporter;
	const aiExportFormatDesc* format = Exporter.GetExportFormatDescription(4);
	std::cout << format->fileExtension << std::endl;
	Exporter.Export(pScene, format->id, std::string("res/helmet.") + format->fileExtension);*/

	bool exportTangents = generateTangents;

	m_numberOfMeshes = pScene->mNumMeshes;
	m_isStacked = !(pScene->mNumMeshes == 1) && isStacked;

	float xmin = FLT_MAX; float ymin = FLT_MAX; float zmin = FLT_MAX;
	float xmax = -FLT_MAX; float ymax = -FLT_MAX; float zmax = -FLT_MAX;

	for (int j = 0; j < pScene->mNumMeshes; j++) {
		const aiMesh* aiMesh = pScene->mMeshes[j];
		m_meshes.push_back(new AssimpMesh(this));
		AssimpMesh* mesh = static_cast<AssimpMesh*>(m_meshes.back());

		const aiMaterial* aiMaterial = pScene->mMaterials[aiMesh->mMaterialIndex];

		if (aiMaterial->GetName().length != 0) {
			AssimpModel::ReadAiMaterial(aiMaterial, mesh->m_materialIndex, m_modelDirectory, aiMaterial->GetName().length == 0 ? "default" : aiMaterial->GetName().data);
		}
		m_isStacked ? m_hasTextureCoords = aiMesh->HasTextureCoords(0) : mesh->m_hasTextureCoords = aiMesh->HasTextureCoords(0);
		m_isStacked ? m_hasNormals = aiMesh->HasNormals() : mesh->m_hasNormals = aiMesh->HasNormals();
		m_isStacked ? m_hasTangents = aiMesh->HasTangentsAndBitangents() && exportTangents : mesh->m_hasTangents = aiMesh->HasTangentsAndBitangents() && exportTangents;

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
			rot.rotate(axis, degrees);
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
				vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].x); vertexBuffer.push_back(aiMesh->mTextureCoords[0][i].y);
			}

			if (m_hasNormals || mesh->m_hasNormals) {
				float normY = flipYZ ? aiMesh->mNormals[i].z : aiMesh->mNormals[i].y;
				float normZ = flipYZ ? aiMesh->mNormals[i].y : aiMesh->mNormals[i].z;

				Matrix4f rot;
				rot.rotate(axis, degrees);

				Vector3f normal = rot * Vector3f(aiMesh->mNormals[i].x, normY, normZ);
				vertexBuffer.push_back(normal[0]); vertexBuffer.push_back(normal[1]); vertexBuffer.push_back(normal[2]);
			}

			if (m_hasTangents || mesh->m_hasTangents) {

				float tangY = flipYZ ? aiMesh->mTangents[i].z : aiMesh->mTangents[i].y;
				float tangZ = flipYZ ? aiMesh->mTangents[i].y : aiMesh->mTangents[i].z;

				float bitangY = flipYZ ? aiMesh->mBitangents[i].z : aiMesh->mBitangents[i].y;
				float bitangZ = flipYZ ? aiMesh->mBitangents[i].y : aiMesh->mBitangents[i].z;

				Matrix4f rot;
				rot.rotate(axis, degrees);

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

		mesh->m_drawCount = aiMesh->mNumFaces * 3u;

		if (mesh->hasMaterial()) {
			std::vector<const aiTexture*> oldTextures;
			for (std::unordered_map<TextureSlot, std::string>::const_iterator it = mesh->getMaterial().getTextures().begin(); it != Material::GetMaterials().back().getTextures().end(); it++) {							
				const aiTexture* texture = pScene->GetEmbeddedTexture(it->second.c_str());				
				if (texture && std::find(oldTextures.begin(), oldTextures.end(), texture) == oldTextures.end()) {
					oldTextures.push_back(texture);
					mesh->m_embeddedTextures[it->first].second = texture->mWidth;
					mesh->m_embeddedTextures[it->first].first = (unsigned char*)malloc(texture->mWidth);
					std::memcpy(mesh->m_embeddedTextures[it->first].first, texture->pcData, texture->mWidth);
				}
			}
		}
	}
	m_center = Vector3f((xmax + xmin) * 0.5f, (ymax + ymin) * 0.5f, (zmax + zmin) * 0.5f);
}

std::string AssimpModel::GetTexturePath(std::string texPath, std::string modelDirectory) {
	int foundSlash = texPath.find_last_of("/\\");
	int foundDot = texPath.find_last_of(".");
	foundDot = (foundDot < 0 ? texPath.length() : foundDot);
	foundDot = foundSlash < 0 ? foundDot : foundDot - 1;
	return foundSlash < 0 ? modelDirectory + "/" + texPath.substr(foundSlash + 1) : texPath;
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
		material.m_name = mltName;
		
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
			material.addTexture(TextureSlot::TEXTURE_DIFFUSE, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));		
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_NORMALS);
		if (numTextures > 0) {			
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), name);
			material.addTexture(TextureSlot::TEXTURE_NORMAL, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_SPECULAR);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), name);
			material.addTexture(TextureSlot::TEXTURE_SPECULAR, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_AMBIENT);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT, 0), name);
			material.addTexture(TextureSlot::TEXTURE_AMBIENT, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_EMISSIVE);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_EMISSIVE, 0), name);
			material.addTexture(TextureSlot::TEXTURE_EMISSIVE, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_HEIGHT);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_HEIGHT, 0), name);
			material.addTexture(TextureSlot::TEXTURE_HEIGHT, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_SHININESS);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_SHININESS, 0), name);
			material.addTexture(TextureSlot::TEXTURE_SHININESS, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_OPACITY);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_OPACITY, 0), name);
			material.addTexture(TextureSlot::TEXTURE_OPACITY, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_DISPLACEMENT);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DISPLACEMENT, 0), name);
			material.addTexture(TextureSlot::TEXTURE_DISPLACEMENT, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_LIGHTMAP);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_LIGHTMAP, 0), name);
			material.addTexture(TextureSlot::TEXTURE_LIGHTMAP, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_REFLECTION);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_REFLECTION, 0), name);
			material.addTexture(TextureSlot::TEXTURE_REFLECTION, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}
		
		numTextures = aiMaterial->GetTextureCount(aiTextureType_BASE_COLOR);
		if (numTextures > 0 && !material.hasTexture(TEXTURE_DIFFUSE)) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_BASE_COLOR, 0), name);
			material.addTexture(TextureSlot::TEXTURE_BASE_COLOR, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_NORMAL_CAMERA);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMAL_CAMERA, 0), name);
			material.addTexture(TextureSlot::TEXTURE_NORMAL_CAMERA, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_EMISSION_COLOR);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_EMISSION_COLOR, 0), name);
			material.addTexture(TextureSlot::TEXTURE_EMISSION_COLOR, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_METALNESS);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_METALNESS, 0), name);
			material.addTexture(TextureSlot::TEXTURE_METALNESS, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE_ROUGHNESS, 0), name);
			material.addTexture(TextureSlot::TEXTURE_DIFFUSE_ROUGHNESS, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT_OCCLUSION, 0), name);
			material.addTexture(TextureSlot::TEXTURE_AMBIENT_OCCLUSION, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_SHEEN);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_SHEEN, 0), name);
			material.addTexture(TextureSlot::TEXTURE_SHEEN, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_CLEARCOAT);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_CLEARCOAT, 0), name);
			material.addTexture(TextureSlot::TEXTURE_CLEARCOAT, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

		numTextures = aiMaterial->GetTextureCount(aiTextureType_TRANSMISSION);
		if (numTextures > 0) {
			aiString name;
			aiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_TRANSMISSION, 0), name);
			material.addTexture(TextureSlot::TEXTURE_TRANSMISSION, name.data[0] == '*' ? name.data : GetTexturePath(name.data, modelDirectory));
		}

	}else {
		index = std::distance(Material::GetMaterials().begin(), it);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AssimpMesh::AssimpMesh(AssimpModel* model) {
	m_model = model;	
	
	m_materialIndex = -1;
	m_textureIndex = -1;
}

AssimpMesh::AssimpMesh(AssimpMesh const& rhs) : Mesh(rhs) {	
	m_model = rhs.m_model;
	
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
}

AssimpMesh::AssimpMesh(AssimpMesh&& rhs) noexcept : Mesh(rhs) {
	m_model = rhs.m_model;

	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
}

AssimpMesh& AssimpMesh::operator=(const AssimpMesh& rhs) {
	Mesh::operator=(rhs);
	m_model = rhs.m_model;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	return *this;
}

AssimpMesh& AssimpMesh::operator=(AssimpMesh&& rhs) noexcept {
	Mesh::operator=(rhs);
	m_model = rhs.m_model;
	m_materialIndex = rhs.m_materialIndex;
	m_textureIndex = rhs.m_textureIndex;
	return *this;
}

AssimpMesh::~AssimpMesh() {
	cleanup();	
}

void AssimpMesh::cleanup() {
	
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

const std::unordered_map<TextureSlot, std::pair<unsigned char*, unsigned int>>& AssimpMesh::getEmbeddedTextures() const {
	return m_embeddedTextures;
}

const void AssimpMesh::removeEmbeddedTexture(TextureSlot textureSlot) const {
	if (m_embeddedTextures.count(textureSlot)) {
		std::pair<unsigned char*, unsigned int>& texture = m_embeddedTextures.at(textureSlot);
		free(texture.first);
		m_embeddedTextures.erase(textureSlot);
	}
}

const bool AssimpMesh::hasMaterial() const {
	return m_materialIndex >= 0;
}