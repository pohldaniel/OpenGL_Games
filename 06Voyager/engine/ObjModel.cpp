
#define CONVHULL_3D_USE_SINGLE_PRECISION
#define CONVHULL_3D_ENABLE
#include "..\convhull\convhull_3d.h"

#include "ObjModel.h"

Model::Model() : m_size(0), m_stride(0), m_offset(0), m_numberOfBytes(0) {
	m_numberOfMeshes = 0;
	m_numberOfTriangles = 0;
	m_mltPath = "";
	m_modelDirectory = "";
	m_hasMaterial = false;
	m_center;
	m_modelMatrix = Matrix4f::IDENTITY;

	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;

	m_hasAABB = false;
	m_hasBoundingSphere = false;
	m_hasConvexHull = false;
}

Model::~Model() {
	
}

void Model::setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz) {
	modelMatrix.setRotPos(axis, degrees, dx, dy, dz);
}

void Model::setRotXYZPos(const Vector3f &axisX, float degreesX,
	const Vector3f &axisY, float degreesY,
	const Vector3f &axisZ, float degreesZ,
	float dx, float dy, float dz) {

	modelMatrix.setRotXYZPos(axisX, degreesX,
		axisY, degreesY,
		axisZ, degreesZ,
		dx, dy, dz);
}

void Model::rotate(const Vector3f &axis, float degrees) {
	modelMatrix.rotate(axis, degrees);
}

void Model::translate(float dx, float dy, float dz) {
	modelMatrix.translate(dx, dy, dz);
}

void Model::scale(float a, float b, float c) {
	modelMatrix.scale(a, b, c);
}

const Matrix4f &Model::getTransformationMatrix() const {
	return modelMatrix.getTransformationMatrix();
}

const Matrix4f &Model::getInvTransformationMatrix() const {
	return modelMatrix.getInvTransformationMatrix();
}

const Vector3f &Model::getCenter() const {
	return m_center;
}

std::string Model::getMltPath() {
	return m_mltPath;
}

std::string Model::getModelDirectory() {
	return m_modelDirectory;
}

bool Model::loadObject(const char* filename, bool createHull) {
	return loadObject(filename, Vector3f(0.0, 1.0, 0.0), 0.0, Vector3f(0.0, 0.0, 0.0), 1.0, createHull);
}

bool compare(const std::array<int, 10> &i_lhs, const std::array<int, 10> &i_rhs) {
	return i_lhs[9] < i_rhs[9];
}

bool Model::loadObject(const char* a_filename, Vector3f& rotate, float degree, Vector3f& translate, float scale, bool createHull) {
	
	std::string filename(a_filename);

	const size_t index = filename.rfind('/');

	if (std::string::npos != index) {
		m_modelDirectory = filename.substr(0, index);
	}

	std::vector<std::array<int, 10>> face;

	std::vector<float> vertexCoords;
	std::vector<float> normalCoords;
	std::vector<float> textureCoords;

	std::map<std::string, int> name;

	int countMesh = 0;
	int assign = 0;
	int countFacesWithTexture = 0;

	float xmin = FLT_MAX; float ymin = FLT_MAX; float zmin = FLT_MAX;
	float xmax = -FLT_MAX; float ymax = -FLT_MAX; float zmax = -FLT_MAX;

	char buffer[250];

	FILE * pFile = fopen(a_filename, "r");
	if (pFile == NULL) {
		std::cout << "File not found" << std::endl;
		return false;
	}

	while (fscanf(pFile, "%s", buffer) != EOF) {

		switch (buffer[0]) {

		case '#': {

			fgets(buffer, sizeof(buffer), pFile);
			break;

		}case 'm': {

			fgets(buffer, sizeof(buffer), pFile);
			sscanf(buffer, "%s %s", buffer, buffer);
			m_mltPath = buffer;

			m_hasMaterial = true;
			break;

		}case 'v': {

			switch (buffer[1]) {

			case '\0': {

				float tmpx, tmpy, tmpz;
				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%f %f %f", &tmpx, &tmpy, &tmpz);

				tmpx = tmpx * scale + translate[0];
				tmpy = tmpy * scale + translate[1];
				tmpz = tmpz * scale + translate[2];

				vertexCoords.push_back(tmpx);
				vertexCoords.push_back(tmpy);
				vertexCoords.push_back(tmpz);

				xmin = (std::min)(tmpx, xmin);
				ymin = (std::min)(tmpy, ymin);
				zmin = (std::min)(tmpz, zmin);

				xmax = (std::max)(tmpx, xmax);
				ymax = (std::max)(tmpy, ymax);
				zmax = (std::max)(tmpz, zmax);
				break;

			}case 't': {

				float tmpu, tmpv;
				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%f %f", &tmpu, &tmpv);

				textureCoords.push_back(tmpu);
				textureCoords.push_back(tmpv);
				break;

			}case 'n': {

				float tmpx, tmpy, tmpz;
				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%f %f %f", &tmpx, &tmpy, &tmpz);

				normalCoords.push_back(tmpx);
				normalCoords.push_back(tmpy);
				normalCoords.push_back(tmpz);
				break;

			}default: {

				break;
			}
			}
			break;

		}case 'u': {

			if (m_hasMaterial) {

				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%s %s", buffer, buffer);

				std::map<std::string, int >::const_iterator iter = name.find(buffer);

				if (iter == name.end()) {
					// mlt name not found
					countMesh++;
					assign = countMesh;

					name[buffer] = countMesh;

				}
				else {
					// mlt name found
					assign = iter->second;
				}
			}
			break;

		}case 'g': {
			if (!m_hasMaterial) {

				fgets(buffer, sizeof(buffer), pFile);
				sscanf(buffer, "%s", buffer);

				countMesh++;
				assign = countMesh;
				name[buffer] = countMesh;
			}
			break;

		}case 'f': {

			int a, b, c, n1, n2, n3, t1, t2, t3;
			fgets(buffer, sizeof(buffer), pFile);

			if (!textureCoords.empty() && !normalCoords.empty()) {
				sscanf(buffer, "%d/%d/%d %d/%d/%d %d/%d/%d ", &a, &t1, &n1, &b, &t2, &n2, &c, &t3, &n3);
				face.push_back({ { a, b, c, t1, t2, t3, n1, n2, n3, assign } });

			}else if (!normalCoords.empty()) {
				sscanf(buffer, "%d//%d %d//%d %d//%d", &a, &n1, &b, &n2, &c, &n3);
				face.push_back({ { a, b, c, 0, 0, 0, n1, n2, n3, assign } });

			}else if (!textureCoords.empty()) {
				sscanf(buffer, "%d/%d %d/%d %d/%d", &a, &t1, &b, &t2, &c, &t3);
				face.push_back({ { a, b, c, t1, t2, t3, 0, 0, 0, assign } });

			}else {
				sscanf(buffer, "%d %d %d", &a, &b, &c);
				face.push_back({ { a, b, c, 0, 0, 0, 0, 0, 0, assign } });
			}
			break;

		}default: {

			break;
		}

		}//end switch
	}// end while
	fclose(pFile);

	aabb.position = Vector3f(xmin, ymin, zmin);
	aabb.size = Vector3f(xmax, ymax, zmax) - Vector3f(xmin, ymin, zmin);

	//unfortunately convhull_3.h doesn't works with the indexed/reduced vertexbuffer and so I have to go for a flag driven approach
	if(createHull)
		convexHull.createBuffer(vertexCoords);
	
	if (convexHull.m_vertexBuffer.size() > 0)
		m_hasConvexHull = true;

	std::sort(face.begin(), face.end(), compare);
	std::map<int, int> dup;

	for (int i = 0; i < face.size(); i++) {
		dup[face[i][9]]++;
	}

	std::map<int, int>::const_iterator iterDup = dup.begin();
	for (iterDup; iterDup != dup.end(); iterDup++) {

		if (name.empty()) {
			m_mesh.push_back(new Mesh(iterDup->second, this));
		}else {

			std::map<std::string, int >::const_iterator iterName = name.begin();
			for (iterName; iterName != name.end(); iterName++) {

				if (iterDup->first == iterName->second) {
					m_mesh.push_back(new Mesh("newmtl " + iterName->first, iterDup->second, this));
					if (m_mesh.size() > 1) {
						m_mesh[m_mesh.size() - 1]->m_triangleOffset = m_mesh[m_mesh.size() - 2]->m_numberOfTriangles + m_mesh[m_mesh.size() - 2]->m_triangleOffset;
					}
				}
			}
		}
	}

	m_numberOfMeshes = m_mesh.size();
	dup.clear();
	name.clear();

	IndexBufferCreator indexBufferCreator;
	indexBufferCreator.positionCoordsIn = vertexCoords;
	indexBufferCreator.normalCoordsIn = normalCoords;
	indexBufferCreator.textureCoordsIn = textureCoords;
	
	for (int j = 0; j < m_numberOfMeshes; j++) {
		std::vector<std::array<int, 10>>::const_iterator first = face.begin() + m_mesh[j]->m_triangleOffset;
		std::vector<std::array<int, 10>>::const_iterator last = face.begin() + (m_mesh[j]->m_triangleOffset + m_mesh[j]->m_numberOfTriangles);
		std::vector<std::array<int, 10>> subFace(first, last);
		indexBufferCreator.face = subFace;
		indexBufferCreator.createIndexBuffer();

		m_mesh[j]->m_indexBuffer = indexBufferCreator.indexBufferOut;
		m_mesh[j]->m_vertexBuffer = indexBufferCreator.vertexBufferOut;

		if (!textureCoords.empty() && !normalCoords.empty()) {
			m_hasTextureCoords = true; m_hasNormals = true;
			m_mesh[j]->m_hasTextureCoords = true; m_mesh[j]->m_hasNormals = true;
			m_mesh[j]->m_stride = 8;
		}else if (!normalCoords.empty()) {
			m_hasNormals = true;
			m_mesh[j]->m_hasNormals = true;
			m_mesh[j]->m_stride = 6;
		}else if (!textureCoords.empty()) {
			m_hasTextureCoords = true;
			m_mesh[j]->m_hasTextureCoords = true;
			m_mesh[j]->m_stride = 5;
		}else {
			m_mesh[j]->m_stride = 3;
		}

		if (m_hasMaterial) {
			m_mesh[j]->readMaterial();
		}else {
			m_mesh[j]->m_material.shader = Globals::shaderManager.getAssetPointer("texture");
		}

		m_mesh[j]->createBuffer();

		indexBufferCreator.indexBufferOut.clear();
		indexBufferCreator.indexBufferOut.shrink_to_fit();
		indexBufferCreator.vertexBufferOut.clear();
		indexBufferCreator.vertexBufferOut.shrink_to_fit();
	}

	indexBufferCreator.positionCoordsIn.clear();
	indexBufferCreator.positionCoordsIn.shrink_to_fit();
	indexBufferCreator.normalCoordsIn.clear();
	indexBufferCreator.normalCoordsIn.shrink_to_fit();
	indexBufferCreator.textureCoordsIn.clear();
	indexBufferCreator.textureCoordsIn.shrink_to_fit();
}

void Model::drawRaw() {
	for (int j = 0; j < m_numberOfMeshes; j++) {
		m_mesh[j]->drawRaw();
	}
}

void Model::drawAABB() {
	aabb.drawRaw();
}

void Model::drawSphere() {
	boundingSphere.drawRaw();
}

void Model::drawHull() {
	convexHull.drawRaw();
}

void Model::createAABB() {
	aabb.createBuffer(*this);
}

void Model::createSphere() {
	boundingSphere.createBuffer(*this);
}

void Model::createInstances(std::vector<Matrix4f> modelMTX) {
	for (int j = 0; j < m_numberOfMeshes; j++) {
		m_mesh[j]->createInstances(modelMTX);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Mesh::Mesh(std::string mltName, int numberTriangles, Model* model) : m_stride(0), m_triangleOffset(0) {
	m_numberOfTriangles = numberTriangles;
	m_mltName = mltName;
	m_model = model;

	m_material.diffuseTexPath = "";
	m_material.bumpMapPath = "";
	m_material.displacementMapPath = "";

	m_numBuffers = 5;

	m_positions.clear();
	m_texels.clear();
	m_normals.clear();
	m_tangents.clear();
	m_bitangents.clear();
	m_vertexBuffer.clear();
	m_indexBuffer.clear();

	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
}

Mesh::Mesh(int numberTriangles, Model* model) : m_stride(0), m_triangleOffset(0) {
	m_numberOfTriangles = numberTriangles;
	m_model = model;
	
	m_material.diffuseTexPath = "";
	m_material.bumpMapPath = "";
	m_material.displacementMapPath = "";

	m_numBuffers = 5;

	m_positions.clear();
	m_texels.clear();
	m_normals.clear();
	m_tangents.clear();
	m_bitangents.clear();
	m_vertexBuffer.clear();
	m_indexBuffer.clear();

	m_hasTextureCoords = false;
	m_hasNormals = false;
	m_hasTangents = false;
}

Mesh::Material Mesh::getMaterial() {
	return m_material;
}

void Mesh::setMaterial(const Vector3f &ambient, const Vector3f &diffuse, const Vector3f &specular, float shininess) {
	m_material.ambient[0] = ambient[0];
	m_material.ambient[1] = ambient[1];
	m_material.ambient[2] = ambient[2];

	m_material.diffuse[0] = diffuse[0];
	m_material.diffuse[1] = diffuse[1];
	m_material.diffuse[2] = diffuse[2];

	m_material.specular[0] = specular[0];
	m_material.specular[1] = specular[1];
	m_material.specular[2] = specular[2];

	m_material.shininess = shininess;
}

bool Mesh::readMaterial() {
	std::vector<std::string*>lines;
	int start = -1;
	int end = -1;

	std::ifstream in((m_model->getModelDirectory() + "/" + m_model->getMltPath()).c_str());

	if (!in.is_open()) {
		std::cout << "mlt file not found" << std::endl;
		return false;
	}

	std::string line;
	while (getline(in, line)) {
		lines.push_back(new std::string(line));

	}
	in.close();

	for (int i = 0; i < lines.size(); i++) {

		if (strcmp((*lines[i]).c_str(), m_mltName.c_str()) == 0) {
			start = i;
			continue;
		}

		if ((*lines[i]).find("newmtl") != std::string::npos && start > 0) {
			end = i;
			break;
		}else {
			end = lines.size();
		}

	}

	if (start < 0 || end < 0) return false;

	for (int i = start; i < end; i++) {

		if ((*lines[i])[0] == '#') {

			continue;

		}else if ((*lines[i])[0] == 'N' && (*lines[i])[1] == 's') {
			int tmp;
			sscanf(lines[i]->c_str(), "Ns %i", &tmp);
			m_material.shininess = tmp;

		}else if ((*lines[i])[0] == 'K' && (*lines[i])[1] == 'a') {
			float tmpx, tmpy, tmpz;
			sscanf(lines[i]->c_str(), "Ka %f %f %f", &tmpx, &tmpy, &tmpz);

			m_material.ambient[0] = tmpx;
			m_material.ambient[1] = tmpy;
			m_material.ambient[2] = tmpz;
			m_material.ambient[3] = 0.0f;

		}else if ((*lines[i])[0] == 'K' && (*lines[i])[1] == 'd') {
			float tmpx, tmpy, tmpz;
			sscanf(lines[i]->c_str(), "Kd %f %f %f", &tmpx, &tmpy, &tmpz);

			m_material.diffuse[0] = tmpx;
			m_material.diffuse[1] = tmpy;
			m_material.diffuse[2] = tmpz;
			m_material.diffuse[3] = 0.0f;

		}else if ((*lines[i])[0] == 'K' && (*lines[i])[1] == 's') {
			float tmpx, tmpy, tmpz;
			sscanf(lines[i]->c_str(), "Ks %f %f %f", &tmpx, &tmpy, &tmpz);

			m_material.specular[0] = tmpx;
			m_material.specular[1] = tmpy;
			m_material.specular[2] = tmpz;
			m_material.specular[3] = 0.0f;

		}else if ((*lines[i])[0] == 'm') {

			char identifierBuffer[20], valueBuffer[250];;
			sscanf(lines[i]->c_str(), "%s %s", identifierBuffer, valueBuffer);

			if (strstr(identifierBuffer, "map_Kd") != 0) {
				m_material.diffuseTexPath = valueBuffer;
			}else if (strstr(identifierBuffer, "map_bump") != 0) {
				m_material.bumpMapPath = valueBuffer;

			}else if (strstr(identifierBuffer, "map_disp") != 0) {
				m_material.displacementMapPath = valueBuffer;
			}
		}
	}

	for (int i = 0; i < lines.size(); i++) {
		delete lines[i];
	}
}

void Mesh::createBuffer() {
	m_positions.clear();
	m_texels.clear();
	m_normals.clear();
	m_tangents.clear();
	m_bitangents.clear();

	if (!m_indexBuffer.empty()) {
		for (int i = 0; i < m_vertexBuffer.size(); i = i + m_stride) {

			if (m_stride == 14) {
				Vector3f position = Vector3f(m_vertexBuffer[i], m_vertexBuffer[i + 1], m_vertexBuffer[i + 2]);
				Vector2f texCoord = Vector2f(m_vertexBuffer[i + 3], m_vertexBuffer[i + 4]);
				Vector3f normal = Vector3f(m_vertexBuffer[i + 5], m_vertexBuffer[i + 6], m_vertexBuffer[i + 7]);
				Vector3f tangent = Vector3f(m_vertexBuffer[i + 8], m_vertexBuffer[i + 9], m_vertexBuffer[i + 10]);
				Vector3f bitangent = Vector3f(m_vertexBuffer[i + 11], m_vertexBuffer[i + 12], m_vertexBuffer[i + 13]);

				m_positions.push_back(position);
				m_texels.push_back(texCoord);
				m_normals.push_back(normal);
				m_tangents.push_back(tangent);
				m_bitangents.push_back(bitangent);

			} else if (m_stride == 8) {
				Vector3f position = Vector3f(m_vertexBuffer[i], m_vertexBuffer[i + 1], m_vertexBuffer[i + 2]);
				Vector2f texCoord = Vector2f(m_vertexBuffer[i + 3], m_vertexBuffer[i + 4]);
				Vector3f normal = Vector3f(m_vertexBuffer[i + 5], m_vertexBuffer[i + 6], m_vertexBuffer[i + 7]);

				m_positions.push_back(position);
				m_texels.push_back(texCoord);
				m_normals.push_back(normal);

			}else if (m_stride == 6) {
				Vector3f position = Vector3f(m_vertexBuffer[i], m_vertexBuffer[i + 1], m_vertexBuffer[i + 2]);
				Vector3f normal = Vector3f(m_vertexBuffer[i + 3], m_vertexBuffer[i + 4], m_vertexBuffer[i + 5]);

				m_positions.push_back(position);
				m_normals.push_back(normal);

			}else if (m_stride == 5) {
				Vector3f position = Vector3f(m_vertexBuffer[i], m_vertexBuffer[i + 1], m_vertexBuffer[i + 2]);
				Vector2f texCoord = Vector2f(m_vertexBuffer[i + 3], m_vertexBuffer[i + 4]);

				m_positions.push_back(position);
				m_texels.push_back(texCoord);

			}else if (m_stride == 3) {

				Vector3f position = Vector3f(m_vertexBuffer[i], m_vertexBuffer[i + 1], m_vertexBuffer[i + 2]);
				m_positions.push_back(position);
			}
		}
	}

	m_drawCount = m_indexBuffer.size();

	if(m_vao) 
		glDeleteVertexArrays(1, &m_vao);

	if(m_vbo[0])
		glDeleteBuffers(1, &m_vbo[0]);

	if (m_vbo[1])
		glDeleteBuffers(1, &m_vbo[1]);

	if (m_vbo[2])
		glDeleteBuffers(1, &m_vbo[2]);

	if (m_vbo[3])
		glDeleteBuffers(1, &m_vbo[3]);

	if (m_vbo[4])
		glDeleteBuffers(1, &m_vbo[4]);

	if (m_ibo)
		glDeleteBuffers(1, &m_ibo);


	glGenBuffers(m_numBuffers, m_vbo);
	glGenBuffers(1, &m_ibo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	if (m_model->m_hasTextureCoords) {
		
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(m_texels[0]), &m_texels[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//Normals
	if (m_model->m_hasNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), &m_normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (m_hasTangents) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[3]);
		glBufferData(GL_ARRAY_BUFFER, m_tangents.size() * sizeof(m_tangents[0]), &m_tangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[4]);
		glBufferData(GL_ARRAY_BUFFER, m_bitangents.size() * sizeof(m_bitangents[0]), &m_bitangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);

	}

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mesh::createInstances(std::vector<Matrix4f> modelMTX){
	m_instanceCount = modelMTX.size();
	
	glGenBuffers(1, &m_vbo2);

	glBindVertexArray(m_vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo2);
	glBufferData(GL_ARRAY_BUFFER, modelMTX.size() * sizeof(GLfloat) * 4 * 4, modelMTX[0][0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	
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

void Mesh::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::generateNormals() {

	if (m_hasNormals) { return; }

	std::vector<float> tmpVertex;

	const unsigned int *pTriangle = 0;
	float *pVertex0 = 0;
	float *pVertex1 = 0;
	float *pVertex2 = 0;
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float normal[3] = { 0.0f, 0.0f, 0.0f };
	float length = 0.0f;
	int stride = m_model->m_hasTextureCoords ? 5 : 3;
	int vertexLength = m_model->m_hasTextureCoords ? 8 : 6;
	int vertexOffset = m_model->m_hasTextureCoords ? 2 : 0;

	int totalTriangles = m_numberOfTriangles;

	for (int i = 0; i < m_vertexBuffer.size(); i++) {
		tmpVertex.push_back(m_vertexBuffer[i]);
		if ((i + 1) % stride == 0) {

			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
		}
	}

	for (int i = 0; i < totalTriangles; i++) {

		pTriangle = &m_indexBuffer[i * 3];

		pVertex0 = &m_vertexBuffer[pTriangle[0] * stride];
		pVertex1 = &m_vertexBuffer[pTriangle[1] * stride];
		pVertex2 = &m_vertexBuffer[pTriangle[2] * stride];

		// Calculate triangle face normal.
		edge1[0] = pVertex1[0] - pVertex0[0];
		edge1[1] = pVertex1[1] - pVertex0[1];
		edge1[2] = pVertex1[2] - pVertex0[2];

		edge2[0] = pVertex2[0] - pVertex0[0];
		edge2[1] = pVertex2[1] - pVertex0[1];
		edge2[2] = pVertex2[2] - pVertex0[2];

		normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
		normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
		normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

		// Accumulate the normals.
		tmpVertex[pTriangle[0] * vertexLength + 3 + vertexOffset] = tmpVertex[pTriangle[0] * vertexLength + 3 + vertexOffset] + normal[0];
		tmpVertex[pTriangle[0] * vertexLength + 4 + vertexOffset] = tmpVertex[pTriangle[0] * vertexLength + 4 + vertexOffset] + normal[1];
		tmpVertex[pTriangle[0] * vertexLength + 5 + vertexOffset] = tmpVertex[pTriangle[0] * vertexLength + 5 + vertexOffset] + normal[2];

		tmpVertex[pTriangle[1] * vertexLength + 3 + vertexOffset] = tmpVertex[pTriangle[1] * vertexLength + 3 + vertexOffset] + normal[0];
		tmpVertex[pTriangle[1] * vertexLength + 4 + vertexOffset] = tmpVertex[pTriangle[1] * vertexLength + 4 + vertexOffset] + normal[1];
		tmpVertex[pTriangle[1] * vertexLength + 5 + vertexOffset] = tmpVertex[pTriangle[1] * vertexLength + 5 + vertexOffset] + normal[2];

		tmpVertex[pTriangle[2] * vertexLength + 3 + vertexOffset] = tmpVertex[pTriangle[2] * vertexLength + 3 + vertexOffset] + normal[0];
		tmpVertex[pTriangle[2] * vertexLength + 4 + vertexOffset] = tmpVertex[pTriangle[2] * vertexLength + 4 + vertexOffset] + normal[1];
		tmpVertex[pTriangle[2] * vertexLength + 5 + vertexOffset] = tmpVertex[pTriangle[2] * vertexLength + 5 + vertexOffset] + normal[2];
	}

	// Normalize the vertex normals.
	for (int i = 0; i < tmpVertex.size(); i = i + vertexLength) {

		float length = 1.0f / sqrtf(tmpVertex[i + 3 + vertexOffset] * tmpVertex[i + 3 + vertexOffset] +
			tmpVertex[i + 4 + vertexOffset] * tmpVertex[i + 4 + vertexOffset] +
			tmpVertex[i + 5 + vertexOffset] * tmpVertex[i + 5 + vertexOffset]);

		tmpVertex[i + 3 + vertexOffset] *= length;
		tmpVertex[i + 4 + vertexOffset] *= length;
		tmpVertex[i + 5 + vertexOffset] *= length;
	}

	m_vertexBuffer.clear();
	copy(tmpVertex.begin(), tmpVertex.end(), back_inserter(m_vertexBuffer));
	tmpVertex.clear();
	
	m_stride = m_model->m_hasTextureCoords ? 8 : 6;
	m_hasNormals = true;
	createBuffer();
}

void Mesh::generateTangents() {

	if (m_hasTangents) { return; }
	if (!m_hasTextureCoords) { std::cout << "TextureCoords needed!" << std::endl; return; }
	if (!m_hasNormals) {
		generateNormals();
	}
	
	std::vector<float> tmpVertex;
	const unsigned int *pTriangle = 0;
	float *pVertex0 = 0;
	float *pVertex1 = 0;
	float *pVertex2 = 0;
	float edge1[3] = { 0.0f, 0.0f, 0.0f };
	float edge2[3] = { 0.0f, 0.0f, 0.0f };
	float texEdge1[2] = { 0.0f, 0.0f };
	float texEdge2[2] = { 0.0f, 0.0f };
	float tangent[3] = { 0.0f, 0.0f, 0.0f };
	float bitangent[3] = { 0.0f, 0.0f, 0.0f };
	float det = 0.0f;
	float nDotT = 0.0f;
	float bDotB = 0.0f;
	float length = 0.0f;


	int totalTriangles = m_numberOfTriangles;

	for (int i = 0; i < m_vertexBuffer.size(); i++) {

		tmpVertex.push_back(m_vertexBuffer[i]);

		if ((i + 1) % m_stride == 0) {

			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
			tmpVertex.push_back(0.0);
		}
	}	

	// Calculate the vertex tangents and bitangents.
	for (int i = 0; i < totalTriangles; ++i) {

		pTriangle = &m_indexBuffer[i * 3];

		pVertex0 = &m_vertexBuffer[pTriangle[0] * m_stride];
		pVertex1 = &m_vertexBuffer[pTriangle[1] * m_stride];
		pVertex2 = &m_vertexBuffer[pTriangle[2] * m_stride];

		// Calculate the triangle face tangent and bitangent.

		edge1[0] = pVertex1[0] - pVertex0[0];
		edge1[1] = pVertex1[1] - pVertex0[1];
		edge1[2] = pVertex1[2] - pVertex0[2];

		edge2[0] = pVertex2[0] - pVertex0[0];
		edge2[1] = pVertex2[1] - pVertex0[1];
		edge2[2] = pVertex2[2] - pVertex0[2];

		texEdge1[0] = pVertex1[3] - pVertex0[3];
		texEdge1[1] = pVertex1[4] - pVertex0[4];

		texEdge2[0] = pVertex2[3] - pVertex0[3];
		texEdge2[1] = pVertex2[4] - pVertex0[4];

		det = texEdge1[0] * texEdge2[1] - texEdge2[0] * texEdge1[1];

		if (fabs(det) < 1e-6f) {

			tangent[0] = 1.0f;
			tangent[1] = 0.0f;
			tangent[2] = 0.0f;

			bitangent[0] = 0.0f;
			bitangent[1] = 1.0f;
			bitangent[2] = 0.0f;

		}else {

			det = 1.0f / det;

			tangent[0] = (texEdge2[1] * edge1[0] - texEdge1[1] * edge2[0]) * det;
			tangent[1] = (texEdge2[1] * edge1[1] - texEdge1[1] * edge2[1]) * det;
			tangent[2] = (texEdge2[1] * edge1[2] - texEdge1[1] * edge2[2]) * det;

			bitangent[0] = (-texEdge2[0] * edge1[0] + texEdge1[0] * edge2[0]) * det;
			bitangent[1] = (-texEdge2[0] * edge1[1] + texEdge1[0] * edge2[1]) * det;
			bitangent[2] = (-texEdge2[0] * edge1[2] + texEdge1[0] * edge2[2]) * det;
		}


		// Accumulate the tangents and bitangents.
		tmpVertex[pTriangle[0] * 14 + 8] = tmpVertex[pTriangle[0] * 14 + 8] + tangent[0];
		tmpVertex[pTriangle[0] * 14 + 9] = tmpVertex[pTriangle[0] * 14 + 9] + tangent[1];
		tmpVertex[pTriangle[0] * 14 + 10] = tmpVertex[pTriangle[0] * 14 + 10] + tangent[2];

		tmpVertex[pTriangle[0] * 14 + 11] = tmpVertex[pTriangle[0] * 14 + 11] + bitangent[0];
		tmpVertex[pTriangle[0] * 14 + 12] = tmpVertex[pTriangle[0] * 14 + 12] + bitangent[1];
		tmpVertex[pTriangle[0] * 14 + 13] = tmpVertex[pTriangle[0] * 14 + 13] + bitangent[2];

		tmpVertex[pTriangle[1] * 14 + 8] = tmpVertex[pTriangle[1] * 14 + 8] + tangent[0];
		tmpVertex[pTriangle[1] * 14 + 9] = tmpVertex[pTriangle[1] * 14 + 9] + tangent[1];
		tmpVertex[pTriangle[1] * 14 + 10] = tmpVertex[pTriangle[1] * 14 + 10] + tangent[2];

		tmpVertex[pTriangle[1] * 14 + 11] = tmpVertex[pTriangle[1] * 14 + 11] + bitangent[0];
		tmpVertex[pTriangle[1] * 14 + 12] = tmpVertex[pTriangle[1] * 14 + 12] + bitangent[1];
		tmpVertex[pTriangle[1] * 14 + 13] = tmpVertex[pTriangle[1] * 14 + 13] + bitangent[2];

		tmpVertex[pTriangle[2] * 14 + 8] = tmpVertex[pTriangle[2] * 14 + 8] + tangent[0];
		tmpVertex[pTriangle[2] * 14 + 9] = tmpVertex[pTriangle[2] * 14 + 9] + tangent[1];
		tmpVertex[pTriangle[2] * 14 + 10] = tmpVertex[pTriangle[2] * 14 + 10] + tangent[2];

		tmpVertex[pTriangle[2] * 14 + 11] = tmpVertex[pTriangle[2] * 14 + 11] + bitangent[0];
		tmpVertex[pTriangle[2] * 14 + 12] = tmpVertex[pTriangle[2] * 14 + 12] + bitangent[1];
		tmpVertex[pTriangle[2] * 14 + 13] = tmpVertex[pTriangle[2] * 14 + 13] + bitangent[2];
	}

	// Orthogonalize and normalize the vertex tangents.
	for (int i = 0; i < tmpVertex.size(); i = i + 14) {

		pVertex0 = &tmpVertex[i];

		// Gram-Schmidt orthogonalize tangent with normal.

		nDotT = pVertex0[5] * pVertex0[8] +
			pVertex0[6] * pVertex0[9] +
			pVertex0[7] * pVertex0[10];

		pVertex0[8] -= pVertex0[5] * nDotT;
		pVertex0[9] -= pVertex0[6] * nDotT;
		pVertex0[10] -= pVertex0[7] * nDotT;

		// Normalize the tangent.

		length = 1.0f / sqrtf(pVertex0[8] * pVertex0[8] +
			pVertex0[9] * pVertex0[9] +
			pVertex0[10] * pVertex0[10]);

		pVertex0[8] *= length;
		pVertex0[9] *= length;
		pVertex0[10] *= length;

		bitangent[0] = (pVertex0[6] * pVertex0[10]) -
			(pVertex0[7] * pVertex0[9]);
		bitangent[1] = (pVertex0[7] * pVertex0[8]) -
			(pVertex0[5] * pVertex0[10]);
		bitangent[2] = (pVertex0[5] * pVertex0[9]) -
			(pVertex0[6] * pVertex0[8]);

		bDotB = bitangent[0] * pVertex0[11] +
			bitangent[1] * pVertex0[12] +
			bitangent[2] * pVertex0[13];

		// Calculate handedness
		if (bDotB < 0.0f) {
			pVertex0[11] = -bitangent[0];
			pVertex0[12] = -bitangent[1];
			pVertex0[13] = -bitangent[2];

		}else {

			pVertex0[11] = bitangent[0];
			pVertex0[12] = bitangent[1];
			pVertex0[13] = bitangent[2];
		}

		//pVertex0[11] = bitangent[0];
		//pVertex0[12] = bitangent[1];
		//pVertex0[13] = bitangent[2];

		//std::cout << bitangent[0] << "  " << bitangent[1] << "  " << bitangent[2] << std::endl;
	}

	m_vertexBuffer.clear();
	copy(tmpVertex.begin(), tmpVertex.end(), back_inserter(m_vertexBuffer));
	tmpVertex.clear();
	m_stride = 14;
	m_hasTangents = true;
	createBuffer();
}

Mesh::~Mesh() {}
////////////////////////////////////////////////////////////////////////////////
void IndexBufferCreator::createIndexBuffer() {

	indexBufferOut.resize(face.size() * 3);
	if (!textureCoordsIn.empty() && !normalCoordsIn.empty()) {
		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[3] - 1) * 2], textureCoordsIn[((face[i])[3] - 1) * 2 + 1],
			normalCoordsIn[((face[i])[6] - 1) * 3], normalCoordsIn[((face[i])[6] - 1) * 3 + 1], normalCoordsIn[((face[i])[6] - 1) * 3 + 2] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 8);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3], positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[4] - 1) * 2], textureCoordsIn[((face[i])[4] - 1) * 2 + 1],
			normalCoordsIn[((face[i])[7] - 1) * 3], normalCoordsIn[((face[i])[7] - 1) * 3 + 1], normalCoordsIn[((face[i])[7] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 8);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
			textureCoordsIn[((face[i])[5] - 1) * 2], textureCoordsIn[((face[i])[5] - 1) * 2 + 1],
			normalCoordsIn[((face[i])[8] - 1) * 3], normalCoordsIn[((face[i])[8] - 1) * 3 + 1], normalCoordsIn[((face[i])[8] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 8);
		}

	}else if (!normalCoordsIn.empty()) {

		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2],
				normalCoordsIn[((face[i])[6] - 1) * 3], normalCoordsIn[((face[i])[6] - 1) * 3 + 1], normalCoordsIn[((face[i])[6] - 1) * 3 + 2] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 6);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3], positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2],
				normalCoordsIn[((face[i])[7] - 1) * 3], normalCoordsIn[((face[i])[7] - 1) * 3 + 1], normalCoordsIn[((face[i])[7] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 6);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
				normalCoordsIn[((face[i])[8] - 1) * 3], normalCoordsIn[((face[i])[8] - 1) * 3 + 1], normalCoordsIn[((face[i])[8] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 6);
		}

	}else if (!textureCoordsIn.empty()) {

		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[3] - 1) * 2], textureCoordsIn[((face[i])[3] - 1) * 2 + 1] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 5);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3],positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[4] - 1) * 2], textureCoordsIn[((face[i])[4] - 1) * 2 + 1] };
			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 5);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2],
				textureCoordsIn[((face[i])[5] - 1) * 2], textureCoordsIn[((face[i])[5] - 1) * 2 + 1] };
			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 5);
		}

	}else {

		for (int i = 0; i < face.size(); i++) {

			float vertex1[] = { positionCoordsIn[((face[i])[0] - 1) * 3], positionCoordsIn[((face[i])[0] - 1) * 3 + 1], positionCoordsIn[((face[i])[0] - 1) * 3 + 2] };
			indexBufferOut[i * 3] = addVertex(((face[i])[0] - 1), &vertex1[0], 3);

			float vertex2[] = { positionCoordsIn[((face[i])[1] - 1) * 3], positionCoordsIn[((face[i])[1] - 1) * 3 + 1], positionCoordsIn[((face[i])[1] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 1] = addVertex(((face[i])[1] - 1), &vertex2[0], 3);

			float vertex3[] = { positionCoordsIn[((face[i])[2] - 1) * 3], positionCoordsIn[((face[i])[2] - 1) * 3 + 1], positionCoordsIn[((face[i])[2] - 1) * 3 + 2] };
			indexBufferOut[i * 3 + 2] = addVertex(((face[i])[2] - 1), &vertex3[0], 3);
		}
	}
	std::map<int, std::vector<int>>().swap(m_vertexCache);
	face.clear();
	face.shrink_to_fit();
}

int IndexBufferCreator::addVertex(int hash, const float *pVertex, int stride) {

	int index = -1;
	std::map<int, std::vector<int> >::const_iterator iter = m_vertexCache.find(hash);

	if (iter == m_vertexCache.end()) {
		// Vertex hash doesn't exist in the cache.
		index = static_cast<int>(vertexBufferOut.size() / stride);
		vertexBufferOut.insert(vertexBufferOut.end(), pVertex, pVertex + stride);
		m_vertexCache.insert(std::make_pair(hash, std::vector<int>(1, index)));
	}else {
		// One or more vertices have been hashed to this entry in the cache.
		const std::vector<int> &vertices = iter->second;
		const float *pCachedVertex = 0;
		bool found = false;

		for (std::vector<int>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
			index = *i;
			pCachedVertex = &vertexBufferOut[index * stride];

			if (memcmp(pCachedVertex, pVertex, sizeof(float)* stride) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			index = static_cast<int>(vertexBufferOut.size() / stride);
			vertexBufferOut.insert(vertexBufferOut.end(), pVertex, pVertex + stride);
			m_vertexCache[hash].push_back(index);
		}
	}
	return index;
}

void BoundingBox::createBuffer(Model& model) {

	m_vertexBuffer.push_back(position[0]); m_vertexBuffer.push_back(position[1]); m_vertexBuffer.push_back(position[2]);	
	m_vertexBuffer.push_back(position[0] + size[0]); m_vertexBuffer.push_back(position[1]); m_vertexBuffer.push_back(position[2]);	
	m_vertexBuffer.push_back(position[0] + size[0]); m_vertexBuffer.push_back(position[1] + size[1]); m_vertexBuffer.push_back(position[2]);	
	m_vertexBuffer.push_back(position[0]); m_vertexBuffer.push_back(position[1] + size[1]); m_vertexBuffer.push_back(position[2]);	
	m_vertexBuffer.push_back(position[0]); m_vertexBuffer.push_back(position[1]); m_vertexBuffer.push_back(position[2] + size[2]);	
	m_vertexBuffer.push_back(position[0] + size[0]); m_vertexBuffer.push_back(position[1]); m_vertexBuffer.push_back(position[2] + size[2]);	
	m_vertexBuffer.push_back(position[0] + size[0]); m_vertexBuffer.push_back(position[1] + size[1]); m_vertexBuffer.push_back(position[2] + size[2]);	
	m_vertexBuffer.push_back(position[0]); m_vertexBuffer.push_back(position[1] + size[1]); m_vertexBuffer.push_back(position[2] + size[2]);
	
	m_indexBuffer.push_back(0); m_indexBuffer.push_back(1); m_indexBuffer.push_back(2);
	m_indexBuffer.push_back(2); m_indexBuffer.push_back(3); m_indexBuffer.push_back(0);

	m_indexBuffer.push_back(1); m_indexBuffer.push_back(5); m_indexBuffer.push_back(6);
	m_indexBuffer.push_back(6); m_indexBuffer.push_back(2); m_indexBuffer.push_back(1);

	m_indexBuffer.push_back(7); m_indexBuffer.push_back(6); m_indexBuffer.push_back(5);
	m_indexBuffer.push_back(5); m_indexBuffer.push_back(4); m_indexBuffer.push_back(7);

	m_indexBuffer.push_back(4); m_indexBuffer.push_back(0); m_indexBuffer.push_back(3);
	m_indexBuffer.push_back(3); m_indexBuffer.push_back(7); m_indexBuffer.push_back(4);

	m_indexBuffer.push_back(4); m_indexBuffer.push_back(5); m_indexBuffer.push_back(1);
	m_indexBuffer.push_back(1); m_indexBuffer.push_back(0); m_indexBuffer.push_back(4);

	m_indexBuffer.push_back(3); m_indexBuffer.push_back(2); m_indexBuffer.push_back(6);
	m_indexBuffer.push_back(6); m_indexBuffer.push_back(7); m_indexBuffer.push_back(3);

	short stride = 3; short offset = 0;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.size() * sizeof(float), &m_vertexBuffer[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	model.m_hasAABB = true;
	
}

void BoundingBox::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void BoundingSphere::createBuffer(Model& model) {

	std::vector<float> vertices;
	for (unsigned int j = 0; j < model.getMeshes().size(); j++) {
		for (unsigned int i = 0; i < model.getMeshes()[j]->m_vertexBuffer.size(); i = i + model.getMeshes()[j]->m_stride) {
			vertices.push_back(model.getMeshes()[j]->m_vertexBuffer[i]);
			vertices.push_back(model.getMeshes()[j]->m_vertexBuffer[i + 1]);
			vertices.push_back(model.getMeshes()[j]->m_vertexBuffer[i + 2]);
		}
	}

	int numberOfVertices = vertices.size() / 3;

	float** ap = new float*[numberOfVertices];
	for (int i = 0; i < numberOfVertices; ++i) {
		float* p = new float[3];
		for (int dim = 0; dim < 3; ++dim) {
			p[dim] = vertices[i * 3 + dim];
		}
		ap[i] = p;
	}

	vertices.clear();
	vertices.shrink_to_fit();

	Miniball::Miniball<Miniball::CoordAccessor<float* const*, const float*>> mb(3, ap, ap + numberOfVertices);
	m_radius = sqrtf(mb.squared_radius());
	m_position = Vector3f(mb.center()[0], mb.center()[1], mb.center()[2]);
	
	for (int i = 0; i < numberOfVertices; ++i)
		delete[] ap[i];
	delete[] ap;

	float uAngleStep = (2.0f * PI) / float(m_uResolution);
	float vAngleStep = PI / float(m_vResolution);

	float vSegmentAngle;
	for (unsigned int i = 0; i <= m_vResolution; i++) {

		vSegmentAngle = i * vAngleStep;
		float cosVSegment = cosf(vSegmentAngle);
		float sinVSegment = sinf(vSegmentAngle);

		for (int j = 0; j <= m_uResolution; j++) {

			float uSegmentAngle = j * uAngleStep;

			float cosUSegment = cosf(uSegmentAngle);
			float sinUSegment = sinf(uSegmentAngle);

			// Calculate vertex position on the surface of a sphere
			float x = m_radius * sinVSegment * cosUSegment + m_position[0];
			float y = m_radius * cosVSegment + m_position[1];
			float z = m_radius * sinVSegment * sinUSegment + m_position[2];


			m_vertexBuffer.push_back(x); m_vertexBuffer.push_back(y); m_vertexBuffer.push_back(z);
		}
	}

	//north pole
	for (unsigned int j = 0; j < m_uResolution; j++) {
		m_indexBuffer.push_back(0);
		m_indexBuffer.push_back((m_uResolution + 1) + j + 1);
		m_indexBuffer.push_back((m_uResolution + 1) + j);
	}

	for (unsigned int i = 1; i < m_vResolution - 1; i++) {

		int k1 = i * (m_uResolution + 1);
		int k2 = k1 + (m_uResolution + 1);

		for (unsigned int j = 0; j < m_uResolution; j++) {

			m_indexBuffer.push_back(k1 + j + 1);
			m_indexBuffer.push_back(k2 + j);
			m_indexBuffer.push_back(k1 + j);

			m_indexBuffer.push_back(k2 + j + 1);
			m_indexBuffer.push_back(k2 + j);
			m_indexBuffer.push_back(k1 + j + 1);
		}
	}

	//south pole
	for (unsigned int j = 0; j < m_uResolution; j++) {

		m_indexBuffer.push_back((m_vResolution - 1) * (m_uResolution + 1) + j);
		m_indexBuffer.push_back((m_vResolution - 1) * (m_uResolution + 1) + j + 1);
		m_indexBuffer.push_back(m_vResolution * (m_uResolution + 1));

	}

	short stride = 3; short offset = 0;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.size() * sizeof(float), &m_vertexBuffer[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	model.m_hasBoundingSphere = true;
}

void BoundingSphere::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ConvexHull::createBuffer(std::vector<float>& vertexBuffer) {
	int numberOfVertices = vertexBuffer.size() / 3;

	ch_vertex* vertices;
	vertices = (ch_vertex*)malloc(numberOfVertices * sizeof(ch_vertex));
	for (unsigned int i = 0; i < numberOfVertices; i++) {
		vertices[i].x = vertexBuffer[i * 3 + 0];
		vertices[i].y = vertexBuffer[i * 3 + 1];
		vertices[i].z = vertexBuffer[i * 3 + 2];
	}

	int* faceIndices = NULL;
	int nFaces;

	convhull_3d_build(vertices, numberOfVertices, &faceIndices, &nFaces);

	for (int i = 0; i < nFaces; i++) {
		m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 0]].x); m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 0]].y); m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 0]].z);
		m_indexBuffer.push_back(i * 3 + 0);

		m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 1]].x); m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 1]].y); m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 1]].z);
		m_indexBuffer.push_back(i * 3 + 1);

		m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 2]].x); m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 2]].y); m_vertexBuffer.push_back(vertices[faceIndices[i * 3 + 2]].z);
		m_indexBuffer.push_back(i * 3 + 2);
	}

	free(vertices);
	free(faceIndices);

	short stride = 3; short offset = 0;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.size() * sizeof(float), &m_vertexBuffer[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ConvexHull::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}