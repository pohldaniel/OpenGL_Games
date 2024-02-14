#include "Background.h"
#include "Application.h"
#include "Globals.h"

std::unique_ptr<Shader> Background::s_shader = nullptr;

BackgroundLayer::BackgroundLayer(Texture* texture, unsigned int zoomX, float speed, float offset, Vector2f textureOffsetY) {
	init(texture, zoomX, speed, offset, textureOffsetY);
}

BackgroundLayer::BackgroundLayer(BackgroundLayer const& rhs) {
    m_texture = rhs.m_texture;
    m_zoomX = rhs.m_zoomX;
    m_speed = rhs.m_speed;
    m_offset = rhs.m_offset;
    m_textureRects = rhs.m_textureRects;
}

BackgroundLayer& BackgroundLayer::operator=(const BackgroundLayer& rhs) {
	m_texture = rhs.m_texture;
	m_zoomX = rhs.m_zoomX;
	m_speed = rhs.m_speed;
	m_offset = rhs.m_offset;
	m_textureRects = rhs.m_textureRects;
	return *this;
}

void BackgroundLayer::init(Texture* texture, unsigned int zoomX, float speed, float offset, Vector2f textureOffsetY) {
	m_zoomX = zoomX;
	m_texture = texture;
	m_offset = offset;
	m_speed = speed;

	for (unsigned int i = 0; i < m_zoomX; i++) {
		m_textureRects.push_back(Vector4f((static_cast<float>(i) / zoomX), textureOffsetY[0], (static_cast<float>(i+1) / zoomX), textureOffsetY[1]));
	}
	m_textureRects.push_back(Vector4f((0.0f / zoomX), textureOffsetY[0], (1.0f / zoomX), textureOffsetY[1]));
}

float BackgroundLayer::clampOffset(float input){
	if (input < 0){
		float mod = -input;
		mod = fmod(mod, 1.0f);
		mod = -mod + 1.0f;
		return mod;
	}
	return fmod(input, 1.0f);
}

void BackgroundLayer::draw(Shader* shader) {
	
	m_offset = clampOffset(m_offset);	
	float start = Application::Width * 2.0f * m_zoomX * m_offset;
	
	m_texture->bind(0);

	for (unsigned int i = 0; i < m_zoomX + 1; i++) {
		shader->loadMatrix("u_transform", Matrix4f::Translate(i * 2.0f - (start / Application::Width), 0.0f, 0.0f));
		//shader->loadVector("u_color", Vector4f(static_cast<float>(i + 1) / (m_zoomX + 1), static_cast<float>(i + 1) / (m_zoomX + 1), static_cast<float>(i + 1) / (m_zoomX + 1), 1.0f));
		shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		shader->loadVector("u_texRect", m_textureRects[i]);
		Globals::shapeManager.get("quad").drawRaw();
	}	
}

void BackgroundLayer::addOffset(float offset) {	
	m_offset = m_offset + offset * m_speed;
}

//////////////////////////////////////////////////////////////

Background::Background() : m_speed(1.0f) {
	if (!s_shader) {
		s_shader = std::unique_ptr<Shader>(new Shader(BACKGROUND_VERTEX, BACKGROUND_FRGAMENT, false));
		s_shader->use();
		s_shader->loadInt("u_texture", 0);
		s_shader->unuse();
	}
}

Background::Background(Background const& rhs) {
	m_layer = rhs.m_layer;	
}

Background& Background::operator=(const Background& rhs) {
	m_layer = rhs.m_layer;
	return *this;
}

void Background::addLayer(BackgroundLayer layer) {
	m_layer.push_back(layer);
}

void Background::setLayer(std::vector<BackgroundLayer>& layer) {
	m_layer = layer;
}

void Background::update(float dt) {
	addOffset(dt * m_speed);
}

void Background::draw() {
	glEnable(GL_BLEND);

	s_shader->use();
	for (auto&& layer : m_layer){
		layer.draw(s_shader.get());
	}
	s_shader->unuse();
	glDisable(GL_BLEND);
}

void Background::addOffset(float offset) {
	for (auto&& layer : m_layer) {
		layer.addOffset(offset);
	}
}

void Background::setSpeed(float speed) {
	m_speed = speed;
}