#include "Material.h"
#include "Md2Model.h"

Md2Model::Md2Model() : 
	m_activeFrameIdx(0),
	currentAnimation(nullptr),
	m_speed(1.0f),
	m_activeFrame(0.0f),
	m_animationType(AnimationType::STAND),
	m_loopAnimation(true),
	m_vao(0),
    m_ibo(0u),
	m_vbo{ 0u,0u } {
}

Md2Model::Md2Model(Md2Model const& rhs) : 
	m_animations(rhs.m_animations),
	m_indexBuffer(rhs.m_indexBuffer),
	m_interpolated(rhs.m_interpolated),
	m_texels(rhs.m_texels),
	m_activeFrameIdx(rhs.m_activeFrameIdx),
	currentAnimation(rhs.currentAnimation),
	m_activeFrame(rhs.m_activeFrame),
	m_speed(rhs.m_speed),
	m_animationType(rhs.m_animationType),
	m_vao(rhs.m_vao),
	m_ibo(rhs.m_ibo),
	m_vbo{ rhs.m_vbo[0], rhs.m_vbo[1]},
	m_markForDelete(false)
{
	
}

Md2Model::Md2Model(Md2Model&& rhs) :
	m_animations(std::move(rhs.m_animations)),
	m_indexBuffer(std::move(rhs.m_indexBuffer)),
	m_interpolated(std::move(rhs.m_interpolated)),
	m_texels(std::move(rhs.m_texels)),
	m_activeFrameIdx(std::move(rhs.m_activeFrameIdx)),
	currentAnimation(std::move(rhs.currentAnimation)),
	m_activeFrame(std::move(rhs.m_activeFrame)),
	m_speed(std::move(rhs.m_speed)),
	m_animationType(std::move(rhs.m_animationType)),
	m_vao(std::move(rhs.m_vao)),
	m_ibo(std::move(rhs.m_ibo)),
	m_vbo{ std::move(rhs.m_vbo[0]), std::move(rhs.m_vbo[1]) },
	m_markForDelete(false)
{

}

Md2Model& Md2Model::operator=(const Md2Model& rhs) {
	m_animations = rhs.m_animations;
	m_indexBuffer = rhs.m_indexBuffer;
	m_interpolated = rhs.m_interpolated;
	m_texels = rhs.m_texels;

	m_activeFrameIdx = rhs.m_activeFrameIdx;
	currentAnimation = rhs.currentAnimation;
	m_activeFrame = rhs.m_activeFrame;
	m_speed = rhs.m_speed;
	m_animationType = rhs.m_animationType;

	m_vao = rhs.m_vao;
	m_ibo = rhs.m_ibo;
	m_vbo[0] = rhs.m_vbo[0];
	m_vbo[1] = rhs.m_vbo[1];
	m_markForDelete = false;
	return *this;
}

Md2Model& Md2Model::operator=(Md2Model&& rhs) {
	m_animations = std::move(rhs.m_animations);
	m_indexBuffer = std::move(rhs.m_indexBuffer);
	m_interpolated = std::move(rhs.m_interpolated);
	m_texels = std::move(rhs.m_texels);

	m_activeFrameIdx = std::move(rhs.m_activeFrameIdx);
	currentAnimation = std::move(rhs.currentAnimation);
	m_activeFrame = std::move(rhs.m_activeFrame);
	m_speed = std::move(rhs.m_speed);
	m_animationType = std::move(rhs.m_animationType);

	m_vao = std::move(rhs.m_vao);
	m_ibo = std::move(rhs.m_ibo);
	m_vbo[0] = std::move(rhs.m_vbo[0]);
	m_vbo[1] = std::move(rhs.m_vbo[1]);
	m_markForDelete = false;
	return *this;
}

Md2Model::~Md2Model() {
	if (m_markForDelete) {
		cleanup();
	}

	/*m_texels.clear();
	m_texels.shrink_to_fit();
	m_interpolated.clear();
	m_interpolated.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
	m_animations.clear();
	m_animations.shrink_to_fit();*/
}

void Md2Model::cleanup() {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_vbo[0]) {
		glDeleteBuffers(1, &m_vbo[0]);
		m_vbo[0] = 0;
	}

	if (m_vbo[1]) {
		glDeleteBuffers(1, &m_vbo[1]);
		m_vbo[1] = 0;
	}

	if (m_ibo) {
		glDeleteBuffers(1, &m_ibo);
		m_ibo = 0;
	}
}

void Md2Model::markForDelete() {
	m_markForDelete = true;
}

void Md2Model::load(const char* path) {
	loadCpu(path);
	loadGpu();	
}

void Md2Model::loadCpu(const char* path) {
	Utils::MD2IO md2Converter;
	m_numVertices = md2Converter.loadMd2(path, true, { 0.0f, 25.0f, 0.0f }, { 0.0f, -90.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, m_interpolated, m_texels, m_indexBuffer, m_animations);
	currentAnimation = &m_animations[m_animationType];
}

void Md2Model::loadGpu() {
	glGenBuffers(1, &m_ibo);
	glGenBuffers(2, m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Utils::MD2IO::Vertex) * m_interpolated.size(), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(float), &m_texels[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//Texture Coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	//Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Utils::MD2IO::Vertex), (void*)0);

	//Normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Utils::MD2IO::Vertex), (void*)(3 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

const void Md2Model::update(float dt) const {

	if (m_animationType == AnimationType::NONE)
		return;

	m_activeFrame += m_speed * dt * currentAnimation->fps;

	int len = static_cast<int>(currentAnimation->frames.size()) - 1;
	float lenf = static_cast<float>(len);

	if (m_activeFrame >= lenf) {
		m_activeFrame = m_loopAnimation ? std::modf(m_activeFrame, &m_activeFrame) + std::fmod(m_activeFrame, lenf + 1.0f) : lenf;
	}

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

const void  Md2Model::updateBuffer(const std::vector<Utils::MD2IO::Vertex>& interpolated) const {
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Utils::MD2IO::Vertex) * interpolated.size(), &interpolated[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

const void Md2Model::draw(short textureIndex, short materialIndex) const {

	if (materialIndex >= 0)
		Material::GetMaterials()[materialIndex].updateMaterialUbo(BuiltInShader::materialUbo);

	if (textureIndex >= 0)
		Material::GetTextures()[textureIndex].bind();

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

const BoundingBox& Md2Model::getLocalBoundingBox() const {
	return currentAnimation->frames[m_activeFrameIdx].boundingBox;
}

void Md2Model::setAnimationType(AnimationType animationType, AnimationType animationTypeN) {
	m_animationType = animationType;
	m_activeFrame = 0.0f;
	if (m_animationType == AnimationType::NONE) {
		currentAnimation = &m_animations[animationTypeN];
		const Utils::MD2IO::Frame& frame = currentAnimation->frames[0];
		std::copy(frame.vertices.begin(), frame.vertices.end(), m_interpolated.begin());
		return;
	}
	currentAnimation = &m_animations[m_animationType];
}

const std::vector<Utils::MD2IO::Animation>& Md2Model::getAnimations() const {
	return m_animations;
}

const unsigned int Md2Model::getNumVertices() const {
	return m_numVertices;
}

void Md2Model::setSpeed(float speed) {
	m_speed = speed;
}

void Md2Model::setLoopAnimation(bool loopAnimation) {
	m_loopAnimation = loopAnimation;
}