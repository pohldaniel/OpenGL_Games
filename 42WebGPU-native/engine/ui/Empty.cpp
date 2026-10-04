#include "UiContext.h"
#include "Empty.h"

Empty::Empty() : Widget(), 
m_sliderPosX(0.0f), 
m_sliderPosY(0.0f),
m_borderColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f)), 
m_sliderColor(Vector4f(0.2f, 0.45f, 0.85f, 1.0f)),
m_isDragged(false), 
m_mouseX(0),
m_mouseY(0),
m_dragMode(DragMode::NONE){

}

Empty::Empty(const Empty& rhs) :
	Widget(rhs),
	m_sliderPosX(rhs.m_sliderPosX),
	m_sliderPosY(rhs.m_sliderPosY),
    m_borderColor(rhs.m_borderColor),
	m_isDragged(rhs.m_isDragged),
	m_mouseX(rhs.m_mouseX),
	m_mouseY(rhs.m_mouseY),
    m_sliderColor(rhs.m_sliderColor),
	m_dragMode(rhs.m_dragMode) {
}

Empty::Empty(Empty&& rhs) noexcept :
	Widget(rhs),
	m_sliderPosX(rhs.m_sliderPosX),
	m_sliderPosY(rhs.m_sliderPosY),
	m_borderColor(rhs.m_borderColor),
	m_isDragged(rhs.m_isDragged),
	m_mouseX(rhs.m_mouseX),
	m_mouseY(rhs.m_mouseY),
	m_sliderColor(rhs.m_sliderColor),
	m_dragMode(rhs.m_dragMode) {
}

Empty::~Empty() {

}

bool Empty::inputTree(const int mouseX, const int mouseY, bool buttonLeft) {
	if (!OnMouseOver(mouseX, mouseY)) {
		return false;
	}

	if (isOverSliderBottom(mouseX, mouseY) || isOverSliderRight(mouseX, mouseY)) {
		if (buttonLeft) {
			Widget::ActiveWidget = this;
			m_hasFocus = true;
			pushToFront();
		}
		return OnInput(mouseX, mouseY, buttonLeft);
	}
	return Widget::inputTree(mouseX, mouseY, buttonLeft);
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
				float maxSliderDeltaX = std::max(m_border * m_scale[0], uiContext.width - (uiContext.width / m_width) * uiContext.width * m_scale[0]);
				float maxContentDeltaX = m_width - uiContext.width;

				if (maxSliderDeltaX > 0.0f && maxContentDeltaX > 0.0f) {
					float scrollFactor = (maxContentDeltaX) / maxSliderDeltaX;
					float prevSliderPos = m_sliderPosX;

					

					m_sliderPosX += static_cast<float>(deltaX) / parentScale[0];
					if (m_sliderPosX < 0.0f) m_sliderPosX = 0.0f;
					if (m_sliderPosX > maxSliderDeltaX) m_sliderPosX = maxSliderDeltaX;

					float sliderDelta = m_sliderPosX - prevSliderPos;
					setPosition(getPosition() - Vector2f(sliderDelta * scrollFactor, 0.0f));
				}
			}

			if (m_dragMode == DragMode::VERTICAL && deltaY != 0) {
				float maxSliderDeltaY = std::max(m_border * m_scale[1], uiContext.height - (uiContext.height / m_height) * uiContext.height * m_scale[1]);
				float maxContentDeltaY = m_height - uiContext.height;

				if (maxSliderDeltaY > 0.0f && maxContentDeltaY > 0.0f) {
					float scrollFactor = (maxContentDeltaY) / maxSliderDeltaY;
					float prevSliderPos = m_sliderPosY;

					m_sliderPosY += static_cast<float>(deltaY) / parentScale[1];
					if (m_sliderPosY < 0.0f) m_sliderPosY = 0.0f;
					if (m_sliderPosY > maxSliderDeltaY) m_sliderPosY = maxSliderDeltaY;

					float sliderDelta = m_sliderPosY - prevSliderPos;
					setPosition(getPosition() - Vector2f(0.0f, sliderDelta * scrollFactor));
				}
			}
			
			m_mouseX = mouseX;
			m_mouseY = mouseY;
		}
	}
	return true;
}

void Empty::setWidth(float width, bool silent) {
	float prevSliderDeltaX = std::max(m_border * m_scale[0], uiContext.width - (uiContext.width / m_width) * uiContext.width * m_scale[0]);
	float prevMaxContentDeltaX = m_width - uiContext.width;

	float newMaxSliderDeltaX = std::max(m_border * m_scale[0], uiContext.width - (uiContext.width / width) * uiContext.width * m_scale[0]);
	float newMaxContentDeltaX = width - uiContext.width;

	if (prevMaxContentDeltaX > 0.0f && newMaxSliderDeltaX > 0.0f && prevSliderDeltaX > 0.0f && newMaxContentDeltaX > 0.0f) {
		float oldScrollFactor = prevMaxContentDeltaX / prevSliderDeltaX;
		float newScrollFactor = newMaxContentDeltaX / newMaxSliderDeltaX;
		m_sliderPosX = (m_sliderPosX * oldScrollFactor) / newScrollFactor;

		if (m_sliderPosX < 0.0f) m_sliderPosX = 0.0f;
		if (m_sliderPosX > newMaxSliderDeltaX) m_sliderPosX = newMaxSliderDeltaX;
	}

	m_width = width;
	if (!silent)
		OnInvalidate();
}

void Empty::setHeight(float height, bool silent) {
	float prevSliderDeltaY = std::max(m_border * m_scale[1], uiContext.height - (uiContext.height / m_height) * uiContext.height * m_scale[1]);
	float prevMaxContentDeltaY = m_height - uiContext.height;

	float newMaxSliderDeltaY = std::max(m_border * m_scale[1], uiContext.height - (uiContext.height / height) * uiContext.height * m_scale[1]);
	float newMaxContentDeltaY = height - uiContext.height;

	if (prevMaxContentDeltaY > 0.0f && newMaxSliderDeltaY > 0.0f && prevSliderDeltaY > 0.0f && newMaxContentDeltaY > 0.0f) {
		float oldScrollFactor = prevMaxContentDeltaY / prevSliderDeltaY;
		float newScrollFactor = newMaxContentDeltaY / newMaxSliderDeltaY;
		m_sliderPosY = (m_sliderPosY * oldScrollFactor) / newScrollFactor;

		if (m_sliderPosY < 0.0f) m_sliderPosX = 0.0f;
		if (m_sliderPosY > newMaxSliderDeltaY) m_sliderPosY = newMaxSliderDeltaY;
	}

	m_height = height;
	if (!silent)
		OnInvalidate();
}

void Empty::OnDraw() {
	
	UiInstance bottomBorderInst = {};
	std::memcpy(bottomBorderInst.transform, (Matrix4f::Translate(0.0f, uiContext.height - m_border / m_scale[1], 0.0f) * Matrix4f::Scale(uiContext.width, m_border / m_scale[1], 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(bottomBorderInst.color, m_borderColor.getData(), sizeof(Vector4f));

	bottomBorderInst.textureRect[0] = 0.0f;
	bottomBorderInst.textureRect[1] = 0.0f;
	bottomBorderInst.textureRect[2] = 1.0f;
	bottomBorderInst.textureRect[3] = 1.0f;
	bottomBorderInst.textureLayer = 0.0f;

	bottomBorderInst.flipAndTile[0] = 0.0f;
	bottomBorderInst.flipAndTile[1] = 0.0f;
	bottomBorderInst.flipAndTile[2] = 0.0f;

	pushWidget(UiPipelineType::Standard, bottomBorderInst);

	UiInstance bottomSliderInst = {};
	std::memcpy(bottomSliderInst.transform, (Matrix4f::Translate(m_sliderPosX, uiContext.height - m_border / m_scale[1], 0.0f) * Matrix4f::Scale(uiContext.width * uiContext.width / m_width, m_border / m_scale[1], 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(bottomSliderInst.color, m_sliderColor.getData(), sizeof(Vector4f));

	bottomSliderInst.textureRect[0] = 0.0f;
	bottomSliderInst.textureRect[1] = 0.0f;
	bottomSliderInst.textureRect[2] = 1.0f;
	bottomSliderInst.textureRect[3] = 1.0f;
	bottomSliderInst.textureLayer = 0.0f;

	bottomSliderInst.flipAndTile[0] = 0.0f;
	bottomSliderInst.flipAndTile[1] = 0.0f;
	bottomSliderInst.flipAndTile[2] = 0.0f;

	pushWidget(UiPipelineType::Standard, bottomSliderInst);

	UiInstance rightBorderInst = {};
	std::memcpy(rightBorderInst.transform, (Matrix4f::Translate(uiContext.width - m_border / m_scale[0], 0.0f, 0.0f) * Matrix4f::Scale(m_border / m_scale[0], uiContext.height, 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(rightBorderInst.color, m_borderColor.getData(), sizeof(Vector4f));

	rightBorderInst.textureRect[0] = 0.0f;
	rightBorderInst.textureRect[1] = 0.0f;
	rightBorderInst.textureRect[2] = 1.0f;
	rightBorderInst.textureRect[3] = 1.0f;
	rightBorderInst.textureLayer = 0.0f;

	rightBorderInst.flipAndTile[0] = 0.0f;
	rightBorderInst.flipAndTile[1] = 0.0f;
	rightBorderInst.flipAndTile[2] = 0.0f;

	pushWidget(UiPipelineType::Standard, rightBorderInst);
	
	UiInstance  rightSliderInst = {};
	std::memcpy(rightSliderInst.transform, (Matrix4f::Translate(uiContext.width - m_border / m_scale[0], m_sliderPosY, 0.0f) * Matrix4f::Scale(m_border / m_scale[0], uiContext.height * uiContext.height / m_height, 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(rightSliderInst.color, m_sliderColor.getData(), sizeof(Vector4f));

	rightSliderInst.textureRect[0] = 0.0f;
	rightSliderInst.textureRect[1] = 0.0f;
	rightSliderInst.textureRect[2] = 1.0f;
	rightSliderInst.textureRect[3] = 1.0f;
	rightSliderInst.textureLayer = 0.0f;

	rightSliderInst.flipAndTile[0] = 0.0f;
	rightSliderInst.flipAndTile[1] = 0.0f;
	rightSliderInst.flipAndTile[2] = 0.0f;

	pushWidget(UiPipelineType::Standard, rightSliderInst);
}

bool Empty::OnMouseOver(int mouseX, int mouseY) {
	return true;
}

bool Empty::isOverSliderBottom(int mouseX, int mouseY) {
	float sliderWidth = (uiContext.width / m_width) * uiContext.width * m_scale[0];
	float height = m_height * m_scale[1];
	Vector2f position = getWorldPosition(true);

	return (mouseX >= m_sliderPosX && mouseX <= m_sliderPosX + sliderWidth) &&
           (mouseY >= uiContext.height - m_border && mouseY <= uiContext.height);
}

bool Empty::isOverSliderRight(int mouseX, int mouseY) {
	float sliderHeight = (uiContext.height / m_height) * uiContext.height * m_scale[1];
	float width = m_width * m_scale[0];
	Vector2f position = getWorldPosition(true);

	return (mouseX >= uiContext.width - m_border && mouseX <= uiContext.width) &&
		   (mouseY >= m_sliderPosY && mouseY <= m_sliderPosY + sliderHeight);
}

void Empty::OnLayoutChanged() {
	if (!m_isLayoutDirty)
		return;

	Widget::OnLayoutChanged();
	m_width = std::max(uiContext.width, m_width);
	m_height = std::max(uiContext.height, m_height);
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