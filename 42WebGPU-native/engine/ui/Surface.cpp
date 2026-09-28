#include <iostream>
#include "Surface.h"
#include "Application.h"

Surface::Surface() : Widget(), m_color(Vector4f::ONE), m_hasDrag(false), m_isDragged(false){
	
}

Surface::Surface(const Surface& rhs) :
	Widget(rhs),
	m_color(rhs.m_color),
	m_hasDrag(rhs.m_hasDrag),
	m_isDragged(rhs.m_isDragged) {
}

Surface::Surface(Surface&& rhs) noexcept :
	Widget(rhs),
	m_color(rhs.m_color),
	m_hasDrag(rhs.m_hasDrag),
	m_isDragged(rhs.m_isDragged) {
}

Surface::~Surface() {

}

void Surface::setColor(const Vector4f& color) {
	m_color = color;
}

void Surface::setDrag(bool drag) {
	m_hasDrag = drag;
}

void Surface::inputDefault(int mouseX, int mouseY, bool buttonLeft) {
	float width = getWidth();
	float height = getHeight();

    /*Matrix4f inverseWorld = getWorldTransformation().Invert();
    Vector4f globalMouse(static_cast<float>(mouseX), static_cast<float>(mouseY), 0.0f, 1.0f);
    Vector4f localMouse = inverseWorld * globalMouse;
    float localX = localMouse[0];
    float localY = localMouse[1];*/

	Vector2f position = getWorldPosition(true);
    Vector2f scale = m_parent ? static_cast<Widget*>(m_parent)->getWorldScale() : getScale();

    bool isOverDragZone = (mouseX >= position[0] && mouseX <= position[0] + width &&
        mouseY >= position[1] && mouseY <= position[1] + height * 0.1f);

    if (isOverDragZone || m_isDragged) {
        m_color = Vector4f(0.2f, 0.45f, 0.85f, 1.0f);
    }else {
        m_color = Vector4f::ONE;
    }

    if (buttonLeft) {
        if (!m_isDragged && isOverDragZone) {
            m_isDragged = true;
            m_mouseX = mouseX;
            m_mouseY = mouseY;
        }
    }else {
        m_isDragged = false;
    }

    if (m_isDragged) {
        int deltaX = mouseX - m_mouseX;
        int deltaY = mouseY - m_mouseY;
        if (deltaX != 0 || deltaY != 0) {
            setPosition((position[0] + static_cast<float>(deltaX)) / scale[0], (position[1] + static_cast<float>(deltaY)) / scale[1]);
            m_mouseX = mouseX;
            m_mouseY = mouseY;
        }
    }
}

void Surface::layoutDefault() {

}

void Surface::createDefault() {
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
	pushWidget(UiPipelineType::Standard, uiInstance);
}

void Surface::setScale(float sx, float sy) {
	Vector2f scale = getWorldScale();
	setWidth(sx * scale[0]);
	setHeight(sy * scale[1]);

	Widget::setScale(sx, sy);
}