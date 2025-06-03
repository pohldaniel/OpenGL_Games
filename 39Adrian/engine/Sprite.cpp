#include "Sprite.h"
#include "Globals.h"

std::unique_ptr<Shader> Sprite::SpriteShader = nullptr;
Matrix4f Sprite::Orthographic;
unsigned int Sprite::Vao = 0u;
unsigned int Sprite::Vbo = 0u;

Sprite::Sprite() : m_shader(nullptr) {

}

Sprite::Sprite(Sprite const& rhs) : Object2D(rhs), m_shader(rhs.m_shader) {

}

Sprite& Sprite::operator=(const Sprite& rhs) {
	Object2D::operator=(rhs);	
	m_shader = rhs.m_shader;
	return *this;
}

Sprite::Sprite(Sprite&& rhs) : Object2D(rhs), m_shader(std::move(rhs.m_shader)) {
	
}

Sprite& Sprite::operator=(Sprite&& rhs) {
	Object2D::operator=(rhs);
	m_shader = rhs.m_shader;
	return *this;
}

Sprite::~Sprite() {

}

void Sprite::draw(const TextureRect& rect, const Vector4f& color, bool flipped, float tileX, float tileY) {
	auto shader = m_shader ? m_shader : SpriteShader.get();
	
	float offsetX = tileX != 1.0f ? 0.0f : rect.textureOffsetX;
	float offsetY = tileY != 1.0f ? 0.0f : rect.textureOffsetY;
	float width = tileX != 1.0f ? 1.0f : rect.textureWidth;
	float height = tileY != 1.0f ? 1.0f : rect.textureHeight;
	
	float texelPosX = tileX != 1.0f ? rect.textureOffsetX : 0.0f;
	float texelPosY = tileY != 1.0f ? rect.textureOffsetY : 0.0f;
	float texelSizeX = tileX != 1.0f ? rect.textureWidth : 1.0f;
	float texelSizeY = tileY != 1.0f ? rect.textureHeight : 1.0f;

	shader->use();
	shader->loadMatrix("u_transform", Orthographic * getTransformationSOP());
	shader->loadVector("u_color", color);
	shader->loadVector("u_texRect", flipped ? Vector4f(offsetX + width, offsetY, offsetX, offsetY + height) : Vector4f(offsetX, offsetY, offsetX + width, offsetY + height));
	shader->loadVector("u_texelTrans", Vector4f(texelPosX, texelPosY, texelSizeX, texelSizeY));
	shader->loadVector("u_tileFactor", Vector2f(tileX, tileY));
	shader->loadInt("u_layer", rect.frame);
	DrawQuad();
}

void Sprite::draw(const Vector4f& color, bool flipped, float tileX, float tileY) {
	auto shader = m_shader ? m_shader : SpriteShader.get();

	shader->use();
	shader->loadMatrix("u_transform", Orthographic * getTransformationSOP());
	shader->loadVector("u_color", color);
	shader->loadVector("u_texRect", flipped ? Vector4f(1.0f * tileX, 0.0f, 0.0f, 1.0f * tileY) : Vector4f(0.0f, 0.0f, 1.0f * tileX, 1.0f * tileY));
	shader->loadInt("u_layer", 0u);
	DrawQuad();
}

void Sprite::drawTransformed(const TextureRect& rect, const Vector4f& color, const Matrix4f& worldTransformation, bool flipped, float tileX, float tileY) {
	auto shader = m_shader ? m_shader : SpriteShader.get();

	float offsetX = tileX != 1.0f ? 0.0f : rect.textureOffsetX;
	float offsetY = tileY != 1.0f ? 0.0f : rect.textureOffsetY;
	float width = tileX != 1.0f ? 1.0f : rect.textureWidth;
	float height = tileY != 1.0f ? 1.0f : rect.textureHeight;

	float texelPosX = tileX != 1.0f ? rect.textureOffsetX : 0.0f;
	float texelPosY = tileY != 1.0f ? rect.textureOffsetY : 0.0f;
	float texelSizeX = tileX != 1.0f ? rect.textureWidth : 1.0f;
	float texelSizeY = tileY != 1.0f ? rect.textureHeight : 1.0f;

	shader->use();
	shader->loadMatrix("u_transform", Orthographic * worldTransformation);
	shader->loadVector("u_color", color);
	shader->loadVector("u_texRect", flipped ? Vector4f(offsetX + width, offsetY, offsetX, offsetY + height) : Vector4f(offsetX, offsetY, offsetX + width, offsetY + height));
	shader->loadVector("u_texelTrans", Vector4f(texelPosX, texelPosY, texelSizeX, texelSizeY));
	shader->loadVector("u_tileFactor", Vector2f(tileX, tileY));
	shader->loadInt("u_layer", rect.frame);
	DrawQuad();
}

void Sprite::drawTransformed(const Vector4f& color, const Matrix4f& worldTransformation, bool flipped, float tileX, float tileY) {
	auto shader = m_shader ? m_shader : SpriteShader.get();

	shader->use();
	shader->loadMatrix("u_transform", Orthographic * worldTransformation);
	shader->loadVector("u_color", color);
	shader->loadVector("u_texRect", flipped ? Vector4f(1.0f * tileX, 0.0f, 0.0f, 1.0f * tileY) : Vector4f(0.0f, 0.0f, 1.0f * tileX, 1.0f * tileY));
	shader->loadInt("u_layer", 0u);
	DrawQuad();
}

void Sprite::setTextureUnit(unsigned int unit) {
	if (m_shader) {
		m_shader->use();
		m_shader->loadInt("u_texture", unit);
		m_shader->unuse();
	}
}

void Sprite::Resize(unsigned int width, unsigned int height) {
	Orthographic.orthographic(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height), -1.0f, 1.0f);
}

void Sprite::Init(unsigned int width, unsigned int height) {

	SpriteShader = std::unique_ptr<Shader>(new Shader(SPRITE_VERTEX, SPRITE_FRGAMENT, false));


	float data[] = {
		0.0, 0.0, 0.0f, 0.0f, 0.0f,
		0.0, 1.0, 0.0f, 0.0f, 1.0f,
		1.0, 1.0, 0.0f, 1.0f, 1.0f,
		1.0, 0.0, 0.0f, 1.0f, 0.0f
	};

	const GLushort index[] = {
		0, 2, 1,
		0, 3, 2
	};

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &Vbo);

	glGenVertexArrays(1, &Vao);
	glBindVertexArray(Vao);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);

	glBindBuffer(GL_ARRAY_BUFFER, Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Resize(width, height);
}

void Sprite::CleanUp() {
	if (Vao) {
		glDeleteVertexArrays(1, &Vao);
		Vao = 0;
	}

	if (Vbo) {
		glDeleteBuffers(1, &Vbo);
		Vbo = 0;
	}
}

void Sprite::DrawQuad() {
	glBindVertexArray(Vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

void Sprite::SetTextureUnit(unsigned int unit) {
	SpriteShader->use();
	SpriteShader->loadInt("u_texture", unit);
	SpriteShader->unuse();
}

Shader* Sprite::GetShader() {
	return SpriteShader.get();
}

void Sprite::UnuseShader() {
	SpriteShader->unuse();
}

void Sprite::setShader(Shader* shader) {
	m_shader = shader;
}

Shader* Sprite::getShader() {
	return m_shader;
}

void Sprite::resetShader() {
	m_shader = nullptr;
}