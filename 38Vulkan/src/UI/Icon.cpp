#include <engine/Fontrenderer.h>
#include <engine/Spritesheet.h>
#include "Icon.h"

Icon::Icon(const TextureRect& textureRect) : WidgetMH(), textureRect(textureRect), m_draw(nullptr), m_color(Vector4f::ONE), m_spriteSheet(0) {

}

Icon::Icon(const Icon& rhs) :
	WidgetMH(rhs),
	m_draw(rhs.m_draw),
	m_color(rhs.m_color),
	textureRect(rhs.textureRect),
	m_spriteSheet(rhs.m_spriteSheet) {
}

Icon::Icon(Icon&& rhs) :
	WidgetMH(rhs),
	m_draw(std::move(rhs.m_draw)),
	m_color(rhs.m_color),
	textureRect(std::move(rhs.textureRect)),
	m_spriteSheet(rhs.m_spriteSheet) {
}

Icon::~Icon() {

}

void Icon::setDrawFunction(std::function<void()> fun) {
	m_draw = fun;
}

void Icon::setColor(const Vector4f& color) {
	m_color = color;
}

void Icon::setShader(Shader* shader) {
	m_shader = shader;
	Sprite::setShader(m_shader);
}

void Icon::setSpriteSheet(const unsigned int& spriteSheet) {
	m_spriteSheet = spriteSheet;
}

void Icon::draw() {
	if (m_draw) {
		return m_draw();
	}
	drawDefault();
}

void Icon::drawDefault() {
	Spritesheet::Bind(m_spriteSheet);
	draw2(textureRect, m_color, getWorldTransformation());
}
////////////////////////////////////////////////////////////////
IconAnimated::IconAnimated(const std::vector<TextureRect>& textureRects) : WidgetMH(), textureRects(textureRects), m_draw(nullptr), m_color(Vector4f::ONE), m_spriteSheet(0), m_currentFrame(0){

}

IconAnimated::IconAnimated(const IconAnimated& rhs) :
	WidgetMH(rhs),
	m_draw(rhs.m_draw),
	m_color(rhs.m_color),
	textureRects(rhs.textureRects), 
	m_spriteSheet(rhs.m_spriteSheet),
	m_currentFrame(rhs.m_currentFrame) {
}

IconAnimated::IconAnimated(IconAnimated&& rhs) :
	WidgetMH(rhs),
	m_draw(std::move(rhs.m_draw)),
	m_color(rhs.m_color),
    textureRects(std::move(rhs.textureRects)),
	m_spriteSheet(rhs.m_spriteSheet),
	m_currentFrame(rhs.m_currentFrame) {
}

IconAnimated::~IconAnimated() {

}

void IconAnimated::setDrawFunction(std::function<void()> fun) {
	m_draw = fun;
}

void IconAnimated::setColor(const Vector4f& color) {
	m_color = color;
}

void IconAnimated::setShader(Shader* shader) {
	m_shader = shader;
	Sprite::setShader(m_shader);
}

void IconAnimated::setSpriteSheet(const unsigned int& spriteSheet) {
	m_spriteSheet = spriteSheet;
}

void IconAnimated::setCurrentFrame(const size_t currentFrame) {
	m_currentFrame = currentFrame;
}

void IconAnimated::draw() {
	if (m_draw) {
		return m_draw();
	}
	drawDefault();	
}

void IconAnimated::drawDefault() {
	Spritesheet::Bind(m_spriteSheet);
	const TextureRect& rect = textureRects[m_currentFrame];
	draw2(rect, m_color, getWorldTransformation());
}