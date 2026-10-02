#include "Surface.h"
#include "Empty.h"

Surface::Surface() : Widget(), m_color(Vector4f::ONE), m_dragColor(Vector4f(0.2f, 0.45f, 0.85f, 1.0f)), m_gripColor(Vector4f(0.85f, 0.45f, 0.2f, 1.0f)), m_isDragged(false), m_isResizing(false), m_mouseX(0), m_mouseY(0), m_controlSize(10.0f) {
	m_isMovable = true;
}

Surface::Surface(const Surface& rhs) :
	Widget(rhs),
	m_color(rhs.m_color),
	m_dragColor(rhs.m_dragColor),
	m_gripColor(rhs.m_gripColor),
	m_isDragged(rhs.m_isDragged),
	m_isResizing(rhs.m_isResizing),
	m_mouseX(rhs.m_mouseX),
	m_mouseY(rhs.m_mouseY),
	m_controlSize(rhs.m_controlSize) {
}

Surface::Surface(Surface&& rhs) noexcept :
	Widget(rhs),	
	m_color(rhs.m_color),
	m_dragColor(rhs.m_dragColor),
	m_gripColor(rhs.m_gripColor),
	m_isDragged(rhs.m_isDragged),
	m_isResizing(rhs.m_isResizing),
	m_mouseX(rhs.m_mouseX),
	m_mouseY(rhs.m_mouseY),
	m_controlSize(rhs.m_controlSize) {
}

Surface::~Surface() {

}

void Surface::setColor(const Vector4f& color) {
	m_color = color;
}
#include <iostream>
bool Surface::OnInput(int mouseX, int mouseY, bool buttonLeft) {
	float currentVisualWidth = m_width * m_scale[0];
	float currentVisualHeight = m_height * m_scale[1];

	Vector2f position = getWorldPosition(true);

	bool isOverDragZone = (mouseX >= position[0] && mouseX <= position[0] + currentVisualWidth &&
		mouseY >= position[1] && mouseY <= position[1] + m_controlSize);

	bool isOverResizeZone = (mouseX >= position[0] + currentVisualWidth - m_controlSize && mouseX <= position[0] + currentVisualWidth)
		&& (mouseY >= position[1] + currentVisualHeight - m_controlSize && mouseY <= position[1] + currentVisualHeight);

	if (buttonLeft) {
		if (!m_isDragged && !m_isResizing) {
			if (isOverResizeZone) {
				m_isResizing = true;
				m_mouseX = mouseX;
				m_mouseY = mouseY;
			}
			else if (isOverDragZone) {
				m_isDragged = true;
				m_mouseX = mouseX;
				m_mouseY = mouseY;
			}
		}
	}
	else {
		m_isDragged = false;
		m_isResizing = false;
	}

	int deltaX = mouseX - m_mouseX;
	int deltaY = mouseY - m_mouseY;

	if (m_isDragged && (deltaX != 0 || deltaY != 0)) {
		Vector2f parentScale = m_parent ? static_cast<Widget*>(m_parent)->getWorldScale() : Vector2f(1.0f, 1.0f);
		Vector2f nextLocalPos = getPosition() + Vector2f(static_cast<float>(deltaX) / parentScale[0], static_cast<float>(deltaY) / parentScale[1]);

		if (nextLocalPos[0] < 0.0f) nextLocalPos[0] = 0.0f;
		if (nextLocalPos[1] < 0.0f) nextLocalPos[1] = 0.0f;

		setPosition(nextLocalPos);

		m_mouseX = mouseX;
		m_mouseY = mouseY;

		if (m_parent) {
			Widget* parentWidget = static_cast<Widget*>(m_parent);

			float widgetRightEdge = nextLocalPos[0] + m_width * m_scale[0];
			float widgetBottomEdge = nextLocalPos[1] + m_height * m_scale[1];

			if (widgetRightEdge > parentWidget->getWidth()) {
				parentWidget->setWidth(widgetRightEdge, true);
			}
			if (widgetBottomEdge > parentWidget->getHeight()) {
				parentWidget->setHeight(widgetBottomEdge, true);
			}
		}
	}

	if (m_isResizing && (deltaX != 0 || deltaY != 0)) {
		float newVisualWidth = currentVisualWidth + static_cast<float>(deltaX);
		float newVisualHeight = currentVisualHeight + static_cast<float>(deltaY);

		const float minVisualWidth = 50.0f;
		const float minVisualHeight = 50.0f;

		if (newVisualWidth < minVisualWidth) newVisualWidth = minVisualWidth;
		if (newVisualHeight < minVisualHeight) newVisualHeight = minVisualHeight;

		float newScaleX = newVisualWidth / m_width;
		float newScaleY = newVisualHeight / m_height;

		scale(newScaleX / m_scale[0], newScaleY / m_scale[1]);

		m_mouseX = mouseX;
		m_mouseY = mouseY;

		if (m_parent) {
			Widget* parentWidget = static_cast<Widget*>(m_parent);
			Vector2f localPos = getPosition();

			float widgetRightEdge = localPos[0] + m_width * newScaleX;
			float widgetBottomEdge = localPos[1] + m_height * newScaleY;

			if (widgetRightEdge > parentWidget->getWidth()) {
				parentWidget->setWidth(widgetRightEdge, true);
			}
			if (widgetBottomEdge > parentWidget->getHeight()) {
				parentWidget->setHeight(widgetBottomEdge, true);
			}
		}
	}

	return true;
}

void Surface::OnDraw() {

	UiInstance uiInstance = {};

	std::memcpy(uiInstance.transform, (getWorldTransformation() * Matrix4f::Scale(m_width, m_height, 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(uiInstance.color, m_color.getData(), sizeof(Vector4f));

	uiInstance.textureRect[0] = 0.0f;
	uiInstance.textureRect[1] = 0.0f;
	uiInstance.textureRect[2] = 1.0f;
	uiInstance.textureRect[3] = 1.0f;
	uiInstance.textureLayer = 0.0f;

	uiInstance.flipAndTile[0] = 0.0f;
	uiInstance.flipAndTile[1] = 0.0f;
	uiInstance.flipAndTile[2] = 0.0f;
	pushWidget(UiPipelineType::Clear, uiInstance);

	UiInstance dragInst = {};
	std::memcpy(dragInst.transform, (getWorldTransformation() * Matrix4f::Scale(m_width, m_controlSize / m_scale[1], 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(dragInst.color, m_dragColor.getData(), sizeof(Vector4f));

	dragInst.textureRect[0] = 0.0f;
	dragInst.textureRect[1] = 0.0f;
	dragInst.textureRect[2] = 1.0f;
	dragInst.textureRect[3] = 1.0f;
	dragInst.textureLayer = 0.0f;

	dragInst.flipAndTile[0] = 0.0f;
	dragInst.flipAndTile[1] = 0.0f;
	dragInst.flipAndTile[2] = 0.0f;

	pushWidget(UiPipelineType::Standard, dragInst);

	UiInstance gripInst = {};
	std::memcpy(gripInst.transform, (getWorldTransformation() * Matrix4f::Translate(m_width - m_controlSize / m_scale[0], m_height - m_controlSize / m_scale[1], 0.0f) * Matrix4f::Scale(m_controlSize / m_scale[0], m_controlSize / m_scale[1], 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(gripInst.color, m_gripColor.getData(), sizeof(Vector4f));

	gripInst.textureRect[0] = 0.0f;
	gripInst.textureRect[1] = 0.0f;
	gripInst.textureRect[2] = 1.0f;
	gripInst.textureRect[3] = 1.0f;
	gripInst.textureLayer = 0.0f;

	gripInst.flipAndTile[0] = 0.0f;
	gripInst.flipAndTile[1] = 0.0f;
	gripInst.flipAndTile[2] = 0.0f;

	pushWidget(UiPipelineType::Standard, gripInst);
}