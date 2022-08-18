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

bool AssimpModel::loadModel(const char* filename) {
	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile("res/models/dragon/dragon.obj", ASSIMP_LOAD_FLAGS);

	if (pScene) {
		std::cout << pScene->mNumMeshes << "  " << pScene->mNumMaterials << std::endl;
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
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void AssimpModel::CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& drawCount, unsigned int& vao, unsigned int(&vbo)[5], unsigned int& ibo, unsigned int stride) {

}