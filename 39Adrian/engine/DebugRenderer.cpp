#include "DebugRenderer.h"

DebugRenderer DebugRenderer::s_instance;
bool DebugRenderer::s_enabled = true;
std::unique_ptr<Shader> DebugRenderer::DebugShader = nullptr;

DebugRenderer::~DebugRenderer() {
	shutdown();
}

void DebugRenderer::init(size_t size) {
	DebugShader = std::unique_ptr<Shader>(new Shader(DEBUG_VERTEX, DEBUG_FRGAMENT, false));

	m_maxBoxes = size;
	m_maxVert = m_maxBoxes * 8;
	m_maxIndex = m_maxBoxes * 24;

	vertices = new DebugVertex[m_maxVert];
	indices = new unsigned int[m_maxIndex];

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_maxVert * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (const void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DebugVertex), (const void*)(3 * sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxIndex * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	verticesPtr = vertices;
	indicesPtr = indices;
}

void DebugRenderer::shutdown() {
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0u;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0u;
	}

	if (m_ibo) {
		glDeleteBuffers(1, &m_ibo);
		m_ibo = 0u;
	}

	//std::default_delete<Shader> DebugShader;
	DebugShader.reset();
}

void DebugRenderer::disable() {
	s_enabled = false;
}

void DebugRenderer::enable() {
	s_enabled = true;
}

void DebugRenderer::setEnable(bool enable) {
	s_enabled = enable;
}

void DebugRenderer::SetView(Camera* camera){
	
	if (!camera)
		return;

	view = camera->getViewMatrix();
	projection = camera->getPerspectiveMatrix();
}

void DebugRenderer::SetProjectionView(const Matrix4f& _projection, const Matrix4f& _view) {
	projection = _projection;
	view = _view;
}

void DebugRenderer::AddLine(const Vector3f& start, const Vector3f& end, unsigned int color){
	if (!s_enabled)
		return;

	if (m_maxIndex - indexCount < 2) {
		drawBuffer();
	}
	
	verticesPtr->position = start;
	verticesPtr->color = color;
	verticesPtr++;

	verticesPtr->position = end;
	verticesPtr->color = color;
	verticesPtr++;

	*indicesPtr = vertexCount;
	indicesPtr++;

	*indicesPtr = vertexCount + 1;
	indicesPtr++;

	vertexCount += 2;
	indexCount += 2;
}

void DebugRenderer::AddLine(const Vector3f& start, const Vector3f& end, const Vector4f& color) {
	AddLine(start, end, color.toUInt());
}

void DebugRenderer::AddBoundingBox(const BoundingBox& box, const Vector4f& color){
	if (!s_enabled)
		return;

	if (m_maxIndex - indexCount < 24) {
		drawBuffer();
	}
	
	unsigned int uintColor = color.toUInt();
	const Vector3f& min = box.min;
	const Vector3f& max = box.max;

	verticesPtr->position = min;
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = { max[0], min[1], min[2] };
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = { max[0], max[1], min[2] };
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = { min[0], max[1], min[2] };
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = { min[0], min[1], max[2] };
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = { max[0], min[1], max[2] };
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = { min[0], max[1], max[2] };
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = max;
	verticesPtr->color = uintColor;
	verticesPtr++;


	*indicesPtr = vertexCount;
	*(indicesPtr + 1) = vertexCount + 1;

	*(indicesPtr + 2) = vertexCount + 1;
	*(indicesPtr + 3) = vertexCount + 2;

	*(indicesPtr + 4) = vertexCount + 2;
	*(indicesPtr + 5) = vertexCount + 3;

	*(indicesPtr + 6) = vertexCount + 3;
	*(indicesPtr + 7) = vertexCount;

	*(indicesPtr + 8) = vertexCount + 4;
	*(indicesPtr + 9) = vertexCount + 5;

	*(indicesPtr + 10) = vertexCount + 5;
	*(indicesPtr + 11) = vertexCount + 7;

	*(indicesPtr + 12) = vertexCount + 7;
	*(indicesPtr + 13) = vertexCount + 6;

	*(indicesPtr + 14) = vertexCount + 6;
	*(indicesPtr + 15) = vertexCount + 4;

	*(indicesPtr + 16) = vertexCount + 0;
	*(indicesPtr + 17) = vertexCount + 4;

	*(indicesPtr + 18) = vertexCount + 1;
	*(indicesPtr + 19) = vertexCount + 5;

	*(indicesPtr + 20) = vertexCount + 2;
	*(indicesPtr + 21) = vertexCount + 7;

	*(indicesPtr + 22) = vertexCount + 3;
	*(indicesPtr + 23) = vertexCount + 6;

	indicesPtr += 24;

	vertexCount += 8;
	indexCount += 24;
}

void DebugRenderer::AddBoundingBox(const BoundingBox& box, const Matrix4f& transform, const Vector4f& color){
	if (!s_enabled)
		return;

	if (m_maxIndex - indexCount < 24) {
		drawBuffer();
	}

	unsigned uintColor = color.toUInt();
	const Vector3f& min = box.min;
	const Vector3f& max = box.max;

	verticesPtr->position = transform ^ min;
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = transform ^ Vector3f(max[0], min[1], min[2]);
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = transform ^ Vector3f(max[0], max[1], min[2]);
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = transform ^ Vector3f(min[0], max[1], min[2]);
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = transform ^ Vector3f(min[0], min[1], max[2]);
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = transform ^ Vector3f(max[0], min[1], max[2]);
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = transform ^ Vector3f(min[0], max[1], max[2]);
	verticesPtr->color = uintColor;
	verticesPtr++;

	verticesPtr->position = transform ^ max;
	verticesPtr->color = uintColor;
	verticesPtr++;


	*indicesPtr = vertexCount;
	*(indicesPtr + 1) = vertexCount + 1;

	*(indicesPtr + 2) = vertexCount + 1;
	*(indicesPtr + 3) = vertexCount + 2;

	*(indicesPtr + 4) = vertexCount + 2;
	*(indicesPtr + 5) = vertexCount + 3;

	*(indicesPtr + 6) = vertexCount + 3;
	*(indicesPtr + 7) = vertexCount;

	*(indicesPtr + 8) = vertexCount + 4;
	*(indicesPtr + 9) = vertexCount + 5;

	*(indicesPtr + 10) = vertexCount + 5;
	*(indicesPtr + 11) = vertexCount + 7;

	*(indicesPtr + 12) = vertexCount + 7;
	*(indicesPtr + 13) = vertexCount + 6;

	*(indicesPtr + 14) = vertexCount + 6;
	*(indicesPtr + 15) = vertexCount + 4;

	*(indicesPtr + 16) = vertexCount + 0;
	*(indicesPtr + 17) = vertexCount + 4;

	*(indicesPtr + 18) = vertexCount + 1;
	*(indicesPtr + 19) = vertexCount + 5;

	*(indicesPtr + 20) = vertexCount + 2;
	*(indicesPtr + 21) = vertexCount + 7;

	*(indicesPtr + 22) = vertexCount + 3;
	*(indicesPtr + 23) = vertexCount + 6;

	indicesPtr += 24;

	vertexCount += 8;
	indexCount += 24;
}

void DebugRenderer::AddCylinder(const Vector3f& position, float radius, float height, const Vector4f& color){
	Vector3f heightVec(0, height, 0);
	Vector3f offsetXVec(radius, 0, 0);
	Vector3f offsetZVec(0, 0, radius);

	for (float i = 0.0f; i < 360.0f; i += 45.0f){
		float theta1 = i * PI_ON_180;
		float theta2 = (i + 45.0f) * PI_ON_180;
		float phi = 90.0f * PI_ON_180;
		Vector3f p1 = position + Vector3f(radius * std::sinf(theta1) * std::sinf(phi), radius * std::cosf(phi), radius * std::cosf(theta1) * std::sinf(phi));
		Vector3f p2 = position + Vector3f(radius * std::sinf(theta2) * std::sinf(phi), radius * std::cosf(phi), radius * std::cosf(theta2) * std::sinf(phi));
		AddLine(p1, p2, color);
		AddLine(p1 + heightVec, p2 + heightVec, color);
	}

	AddLine(position + offsetXVec, position + heightVec + offsetXVec, color);
	AddLine(position - offsetXVec, position + heightVec - offsetXVec, color);
	AddLine(position + offsetZVec, position + heightVec + offsetZVec, color);
	AddLine(position - offsetZVec, position + heightVec - offsetZVec, color);
}

void DebugRenderer::AddSkeleton(BoneNode**& bones, unsigned short numBones, const Vector4f& color) {
	for (size_t i = 0; i < numBones; ++i){

		const BoneNode* bone = bones[i];
		if (!bone->isRootBone()) {
			Vector3f pos1 = bone->getWorldPosition();
			Vector3f pos2 = static_cast<const BaseNode*>(bone->getParent())->getWorldPosition();

			AddLine(pos1, pos2, color);
		}
	}

}

void DebugRenderer::AddNode(SceneNodeLC* node, float scale){
	if (!node)
		return;

	Vector3f start = node->getWorldPosition();
	Quaternion rotation = node->getWorldOrientation();
	
	AddLine(start, start + Quaternion::Rotate(rotation, scale * Vector3f::RIGHT), Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	AddLine(start, start + Quaternion::Rotate(rotation, scale * Vector3f::UP), Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	AddLine(start, start + Quaternion::Rotate(rotation, scale * Vector3f::FORWARD), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
}

void DebugRenderer::AddSphere(const Vector3f& center, float radius, const Vector4f& color) {
	unsigned uintColor = color.toUInt();
	for (float j = 0.0f; j < 180.0f; j += 45.0f){
		for (float i = 0.0f; i < 360.0f; i += 45.0f){

			verticesPtr->position = center + Vector3f(radius * std::sinf(i * PI_ON_180) * std::sinf(j * PI_ON_180), radius * std::cosf(j * PI_ON_180), radius * std::cosf(i * PI_ON_180) * std::sinf(j * PI_ON_180));
			verticesPtr->color = uintColor;
			verticesPtr++;

			verticesPtr->position = center + Vector3f(radius * std::sinf((i + 45.0f) * PI_ON_180) * std::sinf(j * PI_ON_180), radius * std::cosf(j * PI_ON_180), radius * std::cosf((i + 45.0f) * PI_ON_180) * std::sinf(j * PI_ON_180));
			verticesPtr->color = uintColor;
			verticesPtr++;

			verticesPtr->position = center + Vector3f(radius * std::sinf(i * PI_ON_180) * std::sinf((j + 45.0f) * PI_ON_180), radius * std::cosf((j + 45.0f) * PI_ON_180), radius * std::cosf(i * PI_ON_180) * std::sinf((j + 45.0f) * PI_ON_180));
			verticesPtr->color = uintColor;
			verticesPtr++;

			verticesPtr->position = center + Vector3f(radius * std::sinf((i + 45.0f) * PI_ON_180) * std::sinf((j + 45.0f) * PI_ON_180), radius * std::cosf((j + 45.0f) * PI_ON_180), radius * std::cosf((i + 45.0f) * PI_ON_180) * std::sinf((j + 45.0f) * PI_ON_180));
			verticesPtr->color = uintColor;
			verticesPtr++;

			*indicesPtr = vertexCount;
			*(indicesPtr + 1) = vertexCount + 1;

			*(indicesPtr + 2) = vertexCount + 2;
			*(indicesPtr + 3) = vertexCount + 3;

			*(indicesPtr + 4) = vertexCount;
			*(indicesPtr + 5) = vertexCount + 2;

			*(indicesPtr + 6) = vertexCount + 1;
			*(indicesPtr + 7) = vertexCount + 3;

			indicesPtr += 8;

			vertexCount += 4;
			indexCount += 8;
		}
	}
}

void DebugRenderer::drawBuffer() {
	if (!s_enabled)
		return;

	glBindVertexArray(m_vao);
	GLsizeiptr size = (uint8_t*)verticesPtr - (uint8_t*)vertices;
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
	

	size = (uint8_t*)indicesPtr - (uint8_t*)indices;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, indices);
		
	DebugShader->use();
	DebugShader->loadMatrix("u_vp", projection * view );
	glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr);

	DebugShader->unuse();
	glBindVertexArray(0);

	vertexCount = 0;
	indexCount = 0;
	verticesPtr = vertices;
	indicesPtr = indices;
}

DebugRenderer& DebugRenderer::Get() {
	return s_instance;
}

bool& DebugRenderer::Enabled() {
	return s_enabled;
}