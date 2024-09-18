#include "Bar.h"
#include <engine/TileSet.h>

BarUI::BarUI(const TextureRect& textureRect) : WidgetMH(), textureRect(textureRect), m_draw(nullptr){

}

BarUI::BarUI(const BarUI& rhs) :
	WidgetMH(rhs),
	textureRect(rhs.textureRect),
	m_draw(rhs.m_draw){

}

BarUI& BarUI::operator=(const BarUI& rhs) {
	WidgetMH::operator=(rhs);
	m_draw = rhs.m_draw;
	return *this;
}

BarUI::BarUI(BarUI&& rhs) :
	WidgetMH(rhs),
	textureRect(std::move(rhs.textureRect)),
	m_draw(std::move(rhs.m_draw)){
}

BarUI& BarUI::operator=(BarUI&& rhs) {
	WidgetMH::operator=(rhs);
	m_draw = std::move(rhs.m_draw);
	return *this;
}

BarUI::~BarUI() {

}

void BarUI::draw() {
	if (m_draw) {
		return m_draw();
	}
	drawDefault();
}

void BarUI::setDrawFunction(std::function<void()> fun) {
	m_draw = fun;
}

void BarUI::drawDefault() {
	TileSetManager::Get().getTileSet("monster").bind();
	//Sprite::draw(textureRect);

	Sprite::draw2(textureRect, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), getWorldTransformation());
}