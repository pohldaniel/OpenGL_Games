#include "Background.h"
#include "Application.h"
#include "Globals.h"

void Background::init(Texture* texture, unsigned int zoomX, unsigned int zoomY, float offsetX, float offsetY) {
	m_zoomX = zoomX;

	for (int i = 0; i < m_zoomX; i++) {
		m_textureRects.push_back(Vector4f((static_cast<float>(i) / zoomX), 0.333f, static_cast<float>(i+1) / zoomX, 0.666f));
	}
	m_textureRects.push_back(Vector4f((0.0f / zoomX), 0.333f, 1.0f / zoomX, 0.666f));
}

void Background::draw() {

	float start_x = m_offset;
	while (start_x > Application::Width * 2.0f * m_zoomX) {
		start_x -= Application::Width * 2.0f * m_zoomX;
	}

	auto quad = Globals::shaderManager.getAssetPointer("quad_back");
	quad->use();
	Globals::textureManager.get("bg_layer_2").bind(0);

	for (int i = 0; i < m_zoomX + 1; i++) {
		quad->loadMatrix("u_transform", Matrix4f::Translate(i * 2.0f -(start_x / Application::Width), 0.0f, 0.0f));
		//quad->loadVector("u_color", Vector4f(static_cast<float>(i + 1) / (m_zoomX + 1), static_cast<float>(i + 1) / (m_zoomX + 1), static_cast<float>(i + 1) / (m_zoomX + 1), 1.0f));
		quad->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		quad->loadVector("u_texRect", m_textureRects[i]);
		Globals::shapeManager.get("quad").drawRaw();
	}

	quad->unuse();
}

void Background::setOffset(float offset) {
	m_offset = offset;
}