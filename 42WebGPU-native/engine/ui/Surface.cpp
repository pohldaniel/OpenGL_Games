#include "Surface.h"
#include "Application.h"

Surface::Surface() : Widget(), m_color(Vector4f::ONE), m_defaultColor(Vector4f::ONE), m_isDragged(false), m_isResizing(false), m_mouseX(0), m_mouseY(0) {
	m_isMovable = true;
}

Surface::Surface(const Surface& rhs) :
	Widget(rhs),
	m_color(rhs.m_color),
	m_defaultColor(rhs.m_defaultColor),
	m_isDragged(rhs.m_isDragged),
	m_isResizing(rhs.m_isResizing),
	m_mouseX(rhs.m_mouseX),
	m_mouseY(rhs.m_mouseY) {
}

Surface::Surface(Surface&& rhs) noexcept :
	Widget(rhs),	
	m_color(rhs.m_color),
	m_defaultColor(rhs.m_defaultColor),
	m_isDragged(rhs.m_isDragged),
	m_isResizing(rhs.m_isResizing),
	m_mouseX(rhs.m_mouseX),
	m_mouseY(rhs.m_mouseY) {
}

Surface::~Surface() {

}

void Surface::setColor(const Vector4f& color) {
	m_color = color;
	m_defaultColor = color;
}

bool Surface::OnInput(int mouseX, int mouseY, bool buttonLeft) {

	float currentVisualWidth = m_width * m_scale[0];
	float currentVisualHeight = m_height * m_scale[1];

	Vector2f position = getWorldPosition(true);
	const float resizeBorder = 8.0f;

	bool isOverDragZone = (mouseX >= position[0] && mouseX <= position[0] + currentVisualWidth &&
		mouseY >= position[1] && mouseY <= position[1] + currentVisualHeight * 0.1f);

	bool isOverResizeZone = (mouseX >= position[0] + currentVisualWidth - resizeBorder && mouseX <= position[0] + currentVisualWidth)
		&& (mouseY >= position[1] + currentVisualHeight - resizeBorder && mouseY <= position[1] + currentVisualHeight);

	if (m_isResizing) {
		m_color = Vector4f(0.85f, 0.45f, 0.2f, 1.0f);
	}else if (isOverDragZone || m_isDragged) {
		m_color = Vector4f(0.2f, 0.45f, 0.85f, 1.0f);
	}else {
		m_color = m_defaultColor;
	}

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
	}else {
		m_isDragged = false;
		m_isResizing = false;
	}

	int deltaX = mouseX - m_mouseX;
	int deltaY = mouseY - m_mouseY;

	if (m_isDragged && (deltaX != 0 || deltaY != 0)) {
		Vector2f parentScale = m_parent ? static_cast<Widget*>(m_parent)->getWorldScale() : Vector2f(1.0f, 1.0f);

		setPosition(getPosition() + Vector2f(static_cast<float>(deltaX) / parentScale[0], static_cast<float>(deltaY) / parentScale[1]));

		m_mouseX = mouseX;
		m_mouseY = mouseY;
	}

	if (m_isResizing && (deltaX != 0 || deltaY != 0)) {
		float newVisualWidth = currentVisualWidth + static_cast<float>(deltaX);
		float newVisualHeight = currentVisualHeight + static_cast<float>(deltaY);

		const float minVisualWidth = 50.0f;
		const float minVisualHeight = 50.0f;

		float newScaleX = m_scale[0];
		float newScaleY = m_scale[1];

		if (newVisualWidth >= minVisualWidth) {
			newScaleX = newVisualWidth / m_width;
			m_mouseX = mouseX;
		}
		if (newVisualHeight >= minVisualHeight) {
			newScaleY = newVisualHeight / m_height;
			m_mouseY = mouseY;
		}
		scale(newScaleX / m_scale[0], newScaleY / m_scale[1]);
	}

	if (m_isDragged || m_isResizing || isOverDragZone || isOverResizeZone) {
		return true;
	}

	return true;
}

void Surface::OnDraw() {
	UiInstance uiInstance = {};

	Vector2f scale = getScale();
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
	pushWidget(UiPipelineType::Standard, uiInstance);
}

void Surface::OnReset() {
	Widget::OnReset();
	m_color = m_defaultColor;
}