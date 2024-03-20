#include <engine/Material.h>

#include "Skybox.h"
#include "Globals.h"

Skybox::Skybox() {
	transform.identity();
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

	Globals::textureManager.get("dessert").bind();
	Globals::shapeManager.get("cube").drawRaw();



	shader->unuse();

	glFrontFace(GL_CCW);
}

short Skybox::getTextureIndex() const {
	return m_textureIndex;
}

void Skybox::setTextureIndex(short index) const {
	m_textureIndex = index;
}