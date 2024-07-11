#include "Sprite.h"
#include "Globals.h"
std::unique_ptr<Shader> Sprite::SpriteShader = nullptr;
Matrix4f Sprite::Orthographic;
unsigned int Sprite::Vao = 0u;
unsigned int Sprite::Vbo = 0u;

void Sprite::draw(const TextureRect& rect, const Vector4f& color) {	
	SpriteShader->use();
	SpriteShader->loadMatrix("u_transform", Orthographic * getTransformationSOP());
	SpriteShader->loadVector("u_color", color);
	SpriteShader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
	SpriteShader->loadInt("u_layer", rect.frame);
	DrawQuad();
}

void Sprite::draw(const Vector4f& color) {
	SpriteShader->use();
	SpriteShader->loadMatrix("u_transform", Orthographic * getTransformationSOP());
	SpriteShader->loadVector("u_color", color);
	SpriteShader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	SpriteShader->loadInt("u_layer", 0u);
	DrawQuad();
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

void Sprite::setTextureUnit(int unit) {
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