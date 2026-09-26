#include "Surface.h"

Surface::Surface() : Widget(), m_color(Vector4f::ONE) {
	
}

Surface::Surface(const Surface& rhs) :
	Widget(rhs),
	m_color(rhs.m_color) {
}

Surface::Surface(Surface&& rhs) noexcept :
	Widget(rhs),
	m_color(std::move(rhs.m_color)) {
}

Surface::~Surface() {

}

void Surface::setColor(const Vector4f& color) {
	m_color = color;
}

void Surface::drawDefault() {
	UiInstance uiInstance = {};
	std::memcpy(uiInstance.transform, getWorldTransformation().getData(), sizeof(Matrix4f));
	std::memcpy(uiInstance.color, m_color.getData(), sizeof(Vector4f));

	uiInstance.textureRect[0] = 0.0f;
	uiInstance.textureRect[1] = 0.0f;
	uiInstance.textureRect[2] = 1.0f;
	uiInstance.textureRect[3] = 1.0f;
	uiInstance.textureLayer = 0.0f;

	uiInstance.flipAndTile[0] = 0.0f;
	uiInstance.flipAndTile[1] = 0.0f;
	uiInstance.flipAndTile[2] = 0.0f;
	addWidget(UiPipelineType::Standard, uiInstance);
}

void Surface::inputDefault(int mouseX, int mouseY, bool buttonLeft) {

}