#include <iostream>
#include <engine/Material.h>
#include "Md2ModelNew.h"

unsigned int Md2Model::StreamBufferCapacity = 8192 * 1024;

Md2Model::Md2Model() : 
	m_activeFrameIdx(0),
	currentAnimation(nullptr),
	m_speed(1.0f),
	m_activeFrame(0.0f),
	m_animationType(AnimationType::_STAND){
}

Md2Model::~Md2Model() {

}

void Md2Model::load(const char* path) {
	Utils::MD2IO md2Converter;

	md2Converter.loadMd2(path, true, { 0.0f, -90.0f, 0.0f }, { 0.5f, 0.5f, 0.5f }, m_interpolated, m_indexBuffer, m_animation);
	currentAnimation = &m_animation[m_animationType];

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	//glBufferData(GL_ARRAY_BUFFER, StreamBufferCapacity, NULL, GL_STREAM_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Utils::MD2IO::Vertex) * m_interpolated.size(), NULL, GL_STREAM_DRAW);
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(Utils::MD2IO::Vertex), (void*)0);

	//Texture Coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(Utils::MD2IO::Vertex), (void*)(3 * sizeof(float)));

	//Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, 0, sizeof(Utils::MD2IO::Vertex), (void*)(5 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

const void Md2Model::update(float dt) {
	// increment frame
	m_activeFrame += m_speed * dt * currentAnimation->fps;

	int len = static_cast<int>(currentAnimation->frames.size()) - 1;
	float lenf = static_cast<float>(len);

	// loop animation
	if (m_activeFrame >= lenf)
		m_activeFrame = std::modf(m_activeFrame, &m_activeFrame) + std::fmod(m_activeFrame, lenf + 1.0f);

	m_activeFrameIdx = static_cast<short>(m_activeFrame);
	short nextFrame = m_activeFrameIdx == len ? 0 : m_activeFrameIdx + 1;

	float lerp = m_activeFrame - floor(m_activeFrame);
	float oneMinusLerp = 1.0f - lerp;

	const Utils::MD2IO::Frame& frameA = currentAnimation->frames[m_activeFrameIdx];
	const Utils::MD2IO::Frame& frameB = currentAnimation->frames[nextFrame];
	
	std::transform(frameA.vertices.begin(), frameA.vertices.end(), frameB.vertices.begin(),
		m_interpolated.begin(), [lerp, oneMinusLerp](const Utils::MD2IO::Vertex& vert1, const Utils::MD2IO::Vertex& vert2) {
			return Utils::MD2IO::Vertex::Lerp(vert1, vert2, lerp, oneMinusLerp); 
		}
	);
}

const void Md2Model::draw(short textureIndex, short materialIndex) const {

	if (materialIndex >= 0)
		Material::GetMaterials()[materialIndex].updateMaterialUbo(BuiltInShader::materialUbo);

	if (textureIndex >= 0)
		Material::GetTextures()[textureIndex].bind();

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Utils::MD2IO::Vertex) * m_interpolated.size(), &m_interpolated[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	//glDrawElementsBaseVertex(GL_TRIANGLES, indexBuffer.size(), GL_UNSIGNED_INT, 0, drawOffset);
	glBindVertexArray(0);
}

const BoundingBox& Md2Model::getLocalBoundingBox() const {
	return currentAnimation->frames[m_activeFrameIdx].boundingBox;
}

void Md2Model::setAnimationType(AnimationType animationType) {
	m_animationType = animationType;
	currentAnimation = &m_animation[m_animationType];
	m_activeFrame = 0.0f;
}