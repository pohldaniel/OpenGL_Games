#include <iostream>
#include "Surface.h"
#include "Application.h"

Surface::Surface() : Widget(), m_color(Vector4f::ONE), m_defaultColor(Vector4f::ONE), m_gap(0.0f), m_hasDrag(false), m_isDragged(false){
	
}

Surface::Surface(const Surface& rhs) :
	Widget(rhs),
	m_color(rhs.m_color),
	m_defaultColor(rhs.m_defaultColor),
	m_gap(rhs.m_gap),
	m_hasDrag(rhs.m_hasDrag),
	m_isDragged(rhs.m_isDragged) {
}

Surface::Surface(Surface&& rhs) noexcept :
	Widget(rhs),	
	m_color(rhs.m_color),
	m_defaultColor(rhs.m_defaultColor),
	m_gap(rhs.m_gap),
	m_hasDrag(rhs.m_hasDrag),
	m_isDragged(rhs.m_isDragged) {
}

Surface::~Surface() {

}

void Surface::setColor(const Vector4f& color) {
	m_color = color;
	m_defaultColor = color;
}

void Surface::setDrag(bool drag) {
	m_hasDrag = drag;
}

void Surface::inputDefault(int mouseX, int mouseY, bool buttonLeft) {
	float width = getWidth();
	float height = getHeight();

	Vector2f position = getWorldPosition(true);
    Vector2f scale = m_parent ? static_cast<Widget*>(m_parent)->getWorldScale() : getScale();
	const float resizeBorder = 8.0f;

    bool isOverDragZone = (mouseX >= position[0] && mouseX <= position[0] + width &&
        mouseY >= position[1] && mouseY <= position[1] + height * 0.1f);

	bool isOverResizeZone = (mouseX >= position[0] + width - resizeBorder && mouseX <= position[0] + width)
		&& (mouseY >= position[1] + height - resizeBorder && mouseY <= position[1] + height);

	std::cout << "SCALE 1: " << m_scale[0] << "  " << m_scale[1] << std::endl;

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
    if (m_isDragged) {
        
        if (deltaX != 0 || deltaY != 0) {
            setPosition((position[0] + static_cast<float>(deltaX)) / scale[0], (position[1] + static_cast<float>(deltaY)) / scale[1]);
            m_mouseX = mouseX;
            m_mouseY = mouseY;
        }
    }

	if (m_isResizing && (deltaX != 0 || deltaY != 0)) {
		float newWidth = width + (static_cast<float>(deltaX));
		float newHeight = height + (static_cast<float>(deltaY) );
		const float minWidth = 50.0f;
		const float minHeight = 50.0f;

		if (newWidth >= minWidth) {
			m_mouseX = mouseX;
		}
		if (newHeight >= minHeight) {
			m_mouseY = mouseY;
		}
		setScale(newWidth / scale[0], newHeight / scale[1]);
	}

	std::cout << "SCALE 2: " << m_scale[0] << "  " << m_scale[1] << std::endl;
}

void Surface::layoutDefault() {
    if (!m_isLayoutDirty)
        return;

	if (!m_children.empty()) {
		Vector2f scale = getWorldScale(true);
		Vector2f position = getPosition();
		Vector2f localScale = getScale();
		float prevWidth = localScale[0] * scale[0];
		float prevHeight = localScale[1] * scale[1];
		float width = 0.0f;
		float height = 0.0f;
		
		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			Widget* child = static_cast<Widget*>((*it).get());
			width += child->getWidth() + m_gap;
			height = std::max(height, child->getHeight());
		} 

		setWidth((width + (m_paddingX * 2.0f)), true);
		setHeight((height + (m_paddingY * 2.0f)) / localScale[1], true);
		
		setScale((m_width * localScale[0]) / scale[0], (m_height * localScale[1]) / scale[1]);

		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			Widget* child = static_cast<Widget*>((*it).get());	
			child->scale(prevWidth / (m_width * localScale[0]), prevHeight / (m_height * localScale[1]));
		}

		scale = getWorldScale(true);

		float posX = m_paddingX;
		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			Widget* child = static_cast<Widget*>((*it).get());
			float posY = (getHeight() - child->getHeight()) * 0.5f;
			child->setPosition(posX / scale[0], posY / scale[1]);
			posX += child->getWidth() + m_gap;
		}
	}
	m_isLayoutDirty = false;
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

void Surface::setGap(float gap) {
	m_gap = gap;
}