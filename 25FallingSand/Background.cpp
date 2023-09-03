#include "Background.h"
#include "Application.h"
#include "Globals.h"

void BackgroundLayer::init(Texture* texture, unsigned int zoomX, float offset, Vector2f textureOffsetY) {
	m_zoomX = zoomX;
	m_texture = texture;
	m_imageWidth = static_cast<float>(m_texture->getWidth());
	m_offset = offset;
	m_store = offset;
	for (unsigned int i = 0; i < m_zoomX; i++) {
		m_textureRects.push_back(Vector4f((static_cast<float>(i) / zoomX), textureOffsetY[0], (static_cast<float>(i+1) / zoomX), textureOffsetY[1]));
	}
	m_textureRects.push_back(Vector4f((0.0f / zoomX), textureOffsetY[0], (1.0f / zoomX), textureOffsetY[1]));
}

float BackgroundLayer::clampOffset(float input){
	// move input to range 0.. Application::Width * 2.0f * m_zoomX
	if (input < 0){
		// fmod only supports positive numbers. Thus we have
		// to emulate negative numbers
		float mod = -input;
		mod = fmod(mod, 1.0f);
		mod = -mod + 1.0f;
		return mod;
	}
	return fmod(input, 1.0f);
}

void BackgroundLayer::draw() {
	
	m_offset = clampOffset(m_offset);
	
	float start = Application::Width * 2.0f * m_zoomX * m_offset;

	auto quad = Globals::shaderManager.getAssetPointer("quad_back");
	quad->use();
	m_texture->bind(0);

	for (unsigned int i = 0; i < m_zoomX + 1; i++) {
		quad->loadMatrix("u_transform", Matrix4f::Translate(i * 2.0f - (start / Application::Width), 0.0f, 0.0f));
		//quad->loadVector("u_color", Vector4f(static_cast<float>(i + 1) / (m_zoomX + 1), static_cast<float>(i + 1) / (m_zoomX + 1), static_cast<float>(i + 1) / (m_zoomX + 1), 1.0f));
		quad->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		quad->loadVector("u_texRect", m_textureRects[i]);
		Globals::shapeManager.get("quad").drawRaw();
	}

	quad->unuse();
}

void BackgroundLayer::addOffset(float offset) {	
	m_offset = m_offset + offset;
}