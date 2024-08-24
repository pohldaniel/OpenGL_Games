#include <engine/Fontrenderer.h>
#include "Label.h"

Label::Label(const CharacterSet& characterSet) : WidgetMH(), characterSet(characterSet), m_draw(nullptr), m_color(Vector4f::ONE), m_size(1.0f), m_label("label"), m_offsetX(0.0f), m_offsetY(0.0f) {

}

Label::Label(const Label& rhs) : 
	WidgetMH(rhs), 
	characterSet(rhs.characterSet), 
	m_draw(rhs.m_draw), 
	m_label(rhs.m_label), 
	m_color(rhs.m_color), 
	m_size(rhs.m_size),
	m_offsetX(rhs.m_offsetX),
	m_offsetY(rhs.m_offsetY) {

}

Label& Label::operator=(const Label& rhs) {
	WidgetMH::operator=(rhs);
	m_draw = rhs.m_draw;
	m_label = rhs.m_label;
	m_color = rhs.m_color;
	m_size = rhs.m_size;
	m_offsetX = rhs.m_offsetX;
	m_offsetY = rhs.m_offsetY;
	return *this;
}

Label::Label(Label&& rhs) : 
	WidgetMH(rhs), 
	characterSet(std::move(rhs.characterSet)), 
	m_draw(std::move(rhs.m_draw)), 
	m_label(rhs.m_label), 
	m_color(rhs.m_color), 
	m_size(rhs.m_size),
	m_offsetX(rhs.m_offsetX),
	m_offsetY(rhs.m_offsetY) {

}

Label& Label::operator=(Label&& rhs) {
	WidgetMH::operator=(rhs);
	m_draw = std::move(rhs.m_draw);
	m_label = rhs.m_label;
	m_color = rhs.m_color;
	m_size = rhs.m_size;
	m_offsetX = rhs.m_offsetX;
	m_offsetX = rhs.m_offsetX;
	return *this;
}

Label::~Label() {

}

void Label::draw() {
	if (m_draw) {
		return m_draw();
	}
	drawDefault();
}

void Label::setDrawFunction(std::function<void()> fun) {
	m_draw = fun;
}

void Label::setColor(const Vector4f& color) {
	m_color = color;
}

void Label::setLabel(const std::string& label) {
	m_label = label;
}

void Label::setSize(const float size) {
	m_size = size;
}

void Label::setOffsetX(const float offsetX) {
	m_offsetX = offsetX;
}

void Label::setOffsetY(const float offsetY) {
	m_offsetY = offsetY;
}

void Label::drawDefault() {
	characterSet.bind();
	if (m_offsetX || m_offsetY) {
		Fontrenderer::Get().addTextTransformed(characterSet, getWorldTransformationWithTranslation(m_offsetX, m_offsetY, 0.0f), m_label, m_color, m_size);
	}else {
		Fontrenderer::Get().addTextTransformed(characterSet, getWorldTransformation(), m_label, m_color, m_size);
	}
	Fontrenderer::Get().drawBuffer();
}