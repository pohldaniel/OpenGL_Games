#include "UiContext.h"
#include "Empty.h"

Empty::Empty() : Widget(), 
m_sliderPosX(0.0f), 
m_sliderPosY(0.0f),
m_isDragged(false),
m_mouseX(0),
m_mouseY(0),
m_dragMode(DragMode::NONE),
m_borderColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f)), 
m_sliderColor(Vector4f(0.2f, 0.45f, 0.85f, 1.0f)) {

}

Empty::Empty(const Empty& rhs) :
	Widget(rhs),
	m_sliderPosX(rhs.m_sliderPosX),
	m_sliderPosY(rhs.m_sliderPosY),
	m_isDragged(rhs.m_isDragged),
	m_mouseX(rhs.m_mouseX),
	m_mouseY(rhs.m_mouseY),
	m_dragMode(rhs.m_dragMode),
	m_sliderColor(rhs.m_sliderColor),	
    m_borderColor(rhs.m_borderColor) {

}

Empty::Empty(Empty&& rhs) noexcept :
	Widget(rhs),
	m_sliderPosX(rhs.m_sliderPosX),
	m_sliderPosY(rhs.m_sliderPosY),
	m_isDragged(rhs.m_isDragged),
	m_mouseX(rhs.m_mouseX),
	m_mouseY(rhs.m_mouseY),
	m_dragMode(rhs.m_dragMode),
	m_sliderColor(rhs.m_sliderColor),
	m_borderColor(rhs.m_borderColor) {
}

Empty::~Empty() {

}

bool Empty::OnInput(int mouseX, int mouseY, bool buttonLeft) {
	
	if (buttonLeft && !m_isDragged) {
		if (isOverSliderBottom(mouseX, mouseY)) {
			m_isDragged = true;
			m_dragMode = DragMode::HORIZONTAL;
			m_mouseX = mouseX;
			m_mouseY = mouseY;
		}else if (isOverSliderRight(mouseX, mouseY)) {
			m_isDragged = true;
			m_dragMode = DragMode::VERTICAL;
			m_mouseX = mouseX;
			m_mouseY = mouseY;
		}
	}else if (!buttonLeft && m_isDragged) {
		m_isDragged = false;
		m_dragMode = DragMode::NONE;
	}

	if (m_isDragged) {
		int deltaX = mouseX - m_mouseX;
		int deltaY = mouseY - m_mouseY;

		if (deltaX != 0 || deltaY != 0) {
			Vector2f parentScale = m_parent ? static_cast<Widget*>(m_parent)->getWorldScale() : Vector2f(1.0f, 1.0f);
			if (m_dragMode == DragMode::HORIZONTAL && deltaX != 0) {
				float sliderWidth = (uiContext.width * uiContext.width) / m_contentWidth;
				float maxSliderDeltaX = uiContext.width - sliderWidth;
				float maxContentDeltaX = m_contentWidth - uiContext.width;

				if (maxSliderDeltaX > 0.0f && maxContentDeltaX > 0.0f) {
					float newSliderPosX = m_sliderPosX + (static_cast<float>(deltaX) / parentScale[0]);
					newSliderPosX = std::clamp(newSliderPosX, 0.0f, maxSliderDeltaX);
					float scrollPercent = newSliderPosX / maxSliderDeltaX;
					float targetContentScrollX = scrollPercent * maxContentDeltaX;

					float translateDeltaX = targetContentScrollX - m_contentScrollX;

					if (translateDeltaX != 0.0f) {
						this->translate(-translateDeltaX, 0.0f);
						m_contentScrollX = targetContentScrollX;
					}

					m_sliderPosX = newSliderPosX;
				}
			}

			if (m_dragMode == DragMode::VERTICAL && deltaY != 0) {
				float sliderHeight = (uiContext.height * uiContext.height) / m_contentHeight;
				float maxSliderDeltaY = uiContext.height - sliderHeight;
				float maxContentDeltaY = m_contentHeight - uiContext.height;

				if (maxSliderDeltaY > 0.0f && maxContentDeltaY > 0.0f) {
					float newSliderPosY = m_sliderPosY + (static_cast<float>(deltaY) / parentScale[1]);
					newSliderPosY = std::clamp(newSliderPosY, 0.0f, maxSliderDeltaY);
					float scrollPercent = newSliderPosY / maxSliderDeltaY;
					float targetContentScrollY = scrollPercent * maxContentDeltaY;

					float translateDeltaY = targetContentScrollY - m_contentScrollY;

					if (translateDeltaY != 0.0f) {
						this->translate(0.0f, -translateDeltaY);
						m_contentScrollY = targetContentScrollY;
					}

					m_sliderPosY = newSliderPosY;
				}
			}
			
			m_mouseX = mouseX;
			m_mouseY = mouseY;
		}
	}
	return true;
}

void Empty::setWidth(float width, bool silent) {
	m_width = width;
	if (!silent)
		OnInvalidate();
}

void Empty::setHeight(float height, bool silent) {
	m_height = height;
	if (!silent) 
		OnInvalidate();
}

float Empty::getWidth() {
	return m_contentWidth;
}

float Empty::getContentWidth() {
	return m_contentWidth;
}

void Empty::setContentWidth(float contentWidth) {
	float mouseDeltaX = contentWidth - m_contentWidth;
	m_contentWidth = contentWidth;

	float maxContentDeltaX = m_contentWidth - uiContext.width;
	float targetScrollX = m_contentScrollX + mouseDeltaX;

	if (maxContentDeltaX > 0.0f) {
		targetScrollX = std::clamp(targetScrollX, 0.0f, maxContentDeltaX);
		float scrollPercent = targetScrollX / maxContentDeltaX;
		float sliderWidth = (uiContext.width * uiContext.width) / m_contentWidth;
		m_sliderPosX = scrollPercent * (uiContext.width - sliderWidth);
	}else {
		targetScrollX = 0.0f;
		m_sliderPosX = 0.0f;
	}

	float translateDeltaX = targetScrollX - m_contentScrollX;

	if (translateDeltaX != 0.0f) {
		this->translate(-translateDeltaX, 0.0f);
		m_contentScrollX = targetScrollX;
	}
}

float Empty::getContentHeight() {
	return m_contentHeight;
}

float Empty::getHeight() {
	return m_contentHeight;
}

void Empty::setContentHeight(float contentHeight) {
	float mouseDeltaY = contentHeight - m_contentHeight;
	m_contentHeight = contentHeight;

	float maxContentDeltaY = m_contentHeight - uiContext.height;
	float targetScrollY = m_contentScrollY + mouseDeltaY;

	if (maxContentDeltaY > 0.0f) {
		targetScrollY = std::clamp(targetScrollY, 0.0f, maxContentDeltaY);
		float scrollPercent = targetScrollY / maxContentDeltaY;
		float sliderHeight = (uiContext.height * uiContext.height) / m_contentHeight;
		m_sliderPosY = scrollPercent * (uiContext.height - sliderHeight);
	}else {
		targetScrollY = 0.0f;
		m_sliderPosY = 0.0f;
	}

	float translateDeltaY = targetScrollY - m_contentScrollY;
	if (translateDeltaY != 0.0f) {
		this->translate(0.0f, -translateDeltaY);
		m_contentScrollY = targetScrollY;
	}
}

void Empty::OnDraw() {
	
	UiInstance bottomBorderInst = {};
	std::memcpy(bottomBorderInst.transform, (Matrix4f::Translate(0.0f, uiContext.height - m_border / m_scale[1], 0.0f) * Matrix4f::Scale(uiContext.width, m_border / m_scale[1], 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(bottomBorderInst.color, m_borderColor.getData(), sizeof(Vector4f));

	bottomBorderInst.textureRect[0] = 0.0f;
	bottomBorderInst.textureRect[1] = 0.0f;
	bottomBorderInst.textureRect[2] = 1.0f;
	bottomBorderInst.textureRect[3] = 1.0f;
	bottomBorderInst.textureLayer = 0u;

	pushWidget(UiPipelineType::Standard, bottomBorderInst);

	UiInstance bottomSliderInst = {};
	std::memcpy(bottomSliderInst.transform, (Matrix4f::Translate(m_sliderPosX, uiContext.height - m_border / m_scale[1], 0.0f) * Matrix4f::Scale(uiContext.width * m_width / m_contentWidth, m_border / m_scale[1], 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(bottomSliderInst.color, m_sliderColor.getData(), sizeof(Vector4f));

	bottomSliderInst.textureRect[0] = 0.0f;
	bottomSliderInst.textureRect[1] = 0.0f;
	bottomSliderInst.textureRect[2] = 1.0f;
	bottomSliderInst.textureRect[3] = 1.0f;
	bottomSliderInst.textureLayer = 0u;

	pushWidget(UiPipelineType::Standard, bottomSliderInst);

	UiInstance rightBorderInst = {};
	std::memcpy(rightBorderInst.transform, (Matrix4f::Translate(uiContext.width - m_border / m_scale[0], 0.0f, 0.0f) * Matrix4f::Scale(m_border / m_scale[0], uiContext.height, 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(rightBorderInst.color, m_borderColor.getData(), sizeof(Vector4f));

	rightBorderInst.textureRect[0] = 0.0f;
	rightBorderInst.textureRect[1] = 0.0f;
	rightBorderInst.textureRect[2] = 1.0f;
	rightBorderInst.textureRect[3] = 1.0f;
	rightBorderInst.textureLayer = 0u;

	pushWidget(UiPipelineType::Standard, rightBorderInst);
	
	UiInstance  rightSliderInst = {};
	std::memcpy(rightSliderInst.transform, (Matrix4f::Translate(uiContext.width - m_border / m_scale[0], m_sliderPosY, 0.0f) * Matrix4f::Scale(m_border / m_scale[0], uiContext.height * m_height / m_contentHeight , 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(rightSliderInst.color, m_sliderColor.getData(), sizeof(Vector4f));

	rightSliderInst.textureRect[0] = 0.0f;
	rightSliderInst.textureRect[1] = 0.0f;
	rightSliderInst.textureRect[2] = 1.0f;
	rightSliderInst.textureRect[3] = 1.0f;
	rightSliderInst.textureLayer = 0u;

	pushWidget(UiPipelineType::Standard, rightSliderInst);
}

bool Empty::OnMouseOver(int mouseX, int mouseY) {
	return true;
}

bool Empty::isOverSliderBottom(int mouseX, int mouseY) {
	float sliderWidth = (m_width / m_contentWidth) * uiContext.width * m_scale[0];
	float height = m_height * m_scale[1];
	Vector2f position = getWorldPosition(true);

	return (mouseX >= m_sliderPosX && mouseX <= m_sliderPosX + sliderWidth) &&
           (mouseY >= uiContext.height - m_border && mouseY <= uiContext.height);
}

bool Empty::isOverSliderRight(int mouseX, int mouseY) {
	float sliderHeight = (m_height / m_contentHeight) * uiContext.height * m_scale[1];
	float width = m_width * m_scale[0];
	Vector2f position = getWorldPosition(true);

	return (mouseX >= uiContext.width - m_border && mouseX <= uiContext.width) &&
		   (mouseY >= m_sliderPosY && mouseY <= m_sliderPosY + sliderHeight);
}

void Empty::OnLayoutChanged() {
	if (!m_isLayoutDirty)
		return;

	if (!m_init) {
		Widget::OnLayoutChanged();
		m_init = true;
		m_contentHeight = m_height;
		m_height = uiContext.height;

		m_contentWidth = m_width;
		m_width = uiContext.width;		
	}
	//m_width = std::max(uiContext.width, m_width);
	//m_height = std::max(uiContext.height, m_height);
	//m_sliderPosX = 0.0f;
	//m_sliderPosY = 0.0f;
	//setPosition(0.0f, 0.0f);
	/*std::cout << "CONTENT SCROLL: " << m_contentScrollY << std::endl;
	for (const auto& childNode : getChildren()) {
		Widget* child = static_cast<Widget*>(childNode.get());
		Vector2f pos = child->getPosition();
		child->setPosition(Vector2f(pos[0], pos[1] - m_contentScrollY));

	}*/
	/*translate(0.0f, -m_contentScrollY);*/
	m_isLayoutDirty = false;
}

void Empty::OnMouseWheel(int mouseX, int mouseY, float delta) {

	Vector2f parentScale = m_parent ? static_cast<Widget*>(m_parent)->getWorldScale() : Vector2f(1.0f, 1.0f);
	float maxSliderDeltaY = std::max(m_border * m_scale[1], uiContext.height - (uiContext.height / m_height) * uiContext.height * m_scale[1]);
	float maxContentDeltaY = m_height - uiContext.height;

	if (maxSliderDeltaY > 0.0f && maxContentDeltaY > 0.0f) {
		float scrollFactor = (maxContentDeltaY) / maxSliderDeltaY;
		float prevSliderPos = m_sliderPosY;
		float scrollStep = 25.0f / parentScale[1];

		if (delta > 0.0f) {
			m_sliderPosY -= scrollStep;
		}else {
			m_sliderPosY += scrollStep;
		}
		if (m_sliderPosY < 0.0f) m_sliderPosY = 0.0f;
		if (m_sliderPosY > maxSliderDeltaY) m_sliderPosY = maxSliderDeltaY;
		float sliderDelta = m_sliderPosY - prevSliderPos;
		setPosition(getPosition() - Vector2f(0.0f, sliderDelta * scrollFactor));
	}
}

bool Empty::OnOverControl(int mouseX, int mouseY) {
	return isOverSliderBottom(mouseX, mouseY) || isOverSliderRight(mouseX, mouseY);
}