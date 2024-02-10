#include <engine/Material.h>

#include "Skybox.h"
#include "Globals.h"

Skybox::Skybox() {
	transform.rotate(Vector3f(0.0f, 1.0f, 0.0f), 90.0f);
}

void Skybox::draw(const Camera& camera) {
	glFrontFace(GL_CW);
	auto shader = Globals::shaderManager.getAssetPointer("skybox");
	Matrix4f view = camera.getViewMatrix();
	view[3][0] = 0.0f; view[3][1] = 0.0f; view[3][2] = 0.0f;
	shader->use();
	shader->loadMatrix("u_projection", camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", view * transform);
	shader->loadInt("u_texture", 0);

	m_textureIndex >= 0 ? Material::GetTextures()[m_textureIndex].bind() : Texture::Unbind();
	Globals::shapeManager.get("cube").drawRaw();

	//Texture::Unbind(0u, GL_TEXTURE_CUBE_MAP);

	shader->unuse();

	glFrontFace(GL_CCW);
}

short Skybox::getTextureIndex() const {
	return m_textureIndex;
}

void Skybox::setTextureIndex(short index) const {
	m_textureIndex = index;
}