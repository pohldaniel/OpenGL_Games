#include "Canvas.h"

Canvas::Canvas() : Widget(), m_color(Vector4f::ONE), m_layer(0u){
	
}

Canvas::Canvas(const Canvas& rhs) :
	Widget(rhs),
	m_color(rhs.m_color),
    m_layer(rhs.m_layer){
}

Canvas::Canvas(Canvas&& rhs) noexcept :
	Widget(rhs),
	m_color(rhs.m_color),
	m_layer(rhs.m_layer) {
}

Canvas::~Canvas() {

}

void Canvas::setColor(const Vector4f& color) {
	m_color = color;
}

void Canvas::setLayer(uint32_t layer) {
	m_layer = layer;
}

void Canvas::OnDraw() {

	UiInstance uiInstance = {};

	std::memcpy(uiInstance.transform, (getWorldTransformation() * Matrix4f::Scale(m_width, m_height, 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(uiInstance.color, m_color.getData(), sizeof(Vector4f));

	uiInstance.textureRect[0] = 0.0f;
	uiInstance.textureRect[1] = 0.0f;
	uiInstance.textureRect[2] = 1.0f;
	uiInstance.textureRect[3] = 1.0f;
	uiInstance.textureLayer = m_layer;

	pushWidget(UiPipelineType::Standard, uiInstance);
}

void Canvas::OnLayoutChanged() {
	if (!m_isLayoutDirty)
		return;
}

bool Canvas::OnInput(int mouseX, int mouseY, bool buttonLeft) {
	return true;
}