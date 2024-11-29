#include "Bar.h"
#include <engine/TileSet.h>
#include "Globals.h"
#include "Surface.h"

BarUI::BarUI(const TextureRect& textureRect) : WidgetMH(), textureRect(textureRect), m_color(Vector4f::ONE), m_bgColor(0.0f, 0.0f, 0.0f, 1.0f),  m_height(5.0f), m_radius(5.0f), m_width(200.0f), m_value(0.0f), m_maxValue(200.0f){
	
}

BarUI::BarUI(const BarUI& rhs) :
	WidgetMH(rhs),
	textureRect(rhs.textureRect),
	m_bgColor(rhs.m_bgColor),
	m_color(rhs.m_color),
	m_height(rhs.m_height),
	m_width(rhs.m_width),
	m_value(rhs.m_value),
	m_maxValue(rhs.m_maxValue){
}

BarUI::BarUI(BarUI&& rhs) :
	WidgetMH(rhs),
	textureRect(std::move(rhs.textureRect)),
	m_bgColor(std::move(rhs.m_bgColor)),
	m_color(std::move(rhs.m_color)),
	m_height(std::move(rhs.m_height)),
	m_width(std::move(rhs.m_width)),
	m_value(std::move(rhs.m_value)),
	m_maxValue(std::move(rhs.m_maxValue)){
}

BarUI::~BarUI() {

}

void BarUI::setValue(const float value) {
	m_value = value;
}

void BarUI::setMaxValue(const float maxValue) {
	m_maxValue = maxValue;
}

void BarUI::setWidth(const float width) {
	m_width = width;
}

void BarUI::setHeight(const float height) {
	m_height = height;
}

void BarUI::setRadius(const float radius) {
	m_radius = radius;
}

void BarUI::setColor(const Vector4f& color) {
	m_color = color;
}

void BarUI::setBgColor(const Vector4f& bgColor) {
	m_bgColor = bgColor;
}

void BarUI::drawDefault() {
	float ratio = m_width / m_maxValue;
	float progress = std::max(0.0f, std::min(m_width, m_value * ratio));

	if (m_radius != 0.0f) {
	
		Sprite::setShader(Globals::shaderManager.getAssetPointer("list"));
		auto shader = Sprite::getShader();
		shader->use();
		shader->loadFloat("u_radius", m_radius);
		shader->loadUnsignedInt("u_edge", Edge::ALL);

		shader->loadVector("u_dimensions", Vector2f(m_width, m_height));
		Sprite::draw2(m_bgColor, getWorldTransformationWithScale(m_width, m_height));

		shader->loadVector("u_dimensions", Vector2f(progress, m_height));
		Sprite::draw2(m_color, getWorldTransformationWithScale(progress, m_height));
		
	}else {

		if (m_height == 5.0f) {

			const TextureRect& bgRect = TileSetManager::Get().getTileSet("bars").getTextureRects()[static_cast<int>(m_width)];
			const TextureRect& rect = TileSetManager::Get().getTileSet("bars").getTextureRects()[static_cast<int>(progress)];
			TileSetManager::Get().getTileSet("bars").bind();
			//std::cout << static_cast<int>(m_width) << "  " << static_cast<int>(progress) << std::endl;

			Sprite::draw2(bgRect, m_bgColor, getWorldTransformationWithScale(m_width, m_height));
			Sprite::draw2(rect, m_color, getWorldTransformationWithScale(progress, m_height));

		}else {
			TileSetManager::Get().getTileSet("monster_icon").bind();
			Sprite::resetShader();
			Sprite::draw2(textureRect, m_bgColor, getWorldTransformationWithScale(m_width, m_height));
			Sprite::draw2(textureRect, m_color, getWorldTransformationWithScale(progress, m_height));
		}
	}
}