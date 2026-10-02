#include "UiContext.h"
#include "Empty.h"

Empty::Empty() : Widget(), 
m_sliderPosX(0.0f), 
m_borderColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f)), 
m_sliderColor(Vector4f(0.2f, 0.45f, 0.85f, 1.0f)),
m_isDragged(false), 
m_mouseX(0),
m_mouseY(0){

}

Empty::Empty(const Empty& rhs) :
	Widget(rhs),
	m_sliderPosX(rhs.m_sliderPosX),
    m_borderColor(rhs.m_borderColor),
	m_isDragged(rhs.m_isDragged),
	m_mouseX(rhs.m_mouseX),
	m_mouseY(rhs.m_mouseY),
    m_sliderColor(rhs.m_sliderColor) {
}

Empty::Empty(Empty&& rhs) noexcept :
	Widget(rhs),
	m_sliderPosX(rhs.m_sliderPosX),
	m_borderColor(rhs.m_borderColor),
	m_isDragged(rhs.m_isDragged),
	m_mouseX(rhs.m_mouseX),
	m_mouseY(rhs.m_mouseY),
	m_sliderColor(rhs.m_sliderColor) {
}

Empty::~Empty() {

}

bool Empty::OnInput(int mouseX, int mouseY, bool buttonLeft) {
	if(buttonLeft && !m_isDragged && isOverSlider(mouseX, mouseY)) {	
		m_isDragged = true;
		m_mouseX = mouseX;
		m_mouseY = mouseY;
	}else if (!buttonLeft && m_isDragged) {
		m_isDragged = false;
	}
	
	if (m_isDragged) {
		int deltaX = mouseX - m_mouseX;
		int deltaY = mouseY - m_mouseY;

		if (deltaX != 0 || deltaY != 0) {
			Vector2f parentScale = m_parent ? static_cast<Widget*>(m_parent)->getWorldScale() : Vector2f(1.0f, 1.0f);

			float maxSliderDelta = std::max(10.0f * m_scale[0], uiContext.width - (uiContext.width / m_width) * uiContext.width * m_scale[0]);
			float maxContentDelta = m_width - uiContext.width;
			if (maxSliderDelta > 0.0f && maxContentDelta > 0.0f) {

				float scrollFactor = (maxContentDelta) / maxSliderDelta;
				float prevSliderPos = m_sliderPosX;

				m_sliderPosX += static_cast<float>(deltaX) / parentScale[0];
				if (m_sliderPosX < 0.0f) m_sliderPosX = 0.0f;
				if (m_sliderPosX > maxSliderDelta) m_sliderPosX = maxSliderDelta;

				float sliderDelta = m_sliderPosX - prevSliderPos;
				m_mouseX = mouseX;
				m_mouseY = mouseY;

				setPosition(getPosition() - Vector2f(sliderDelta * scrollFactor, 0.0f));
			}
		}
	}
	return true;
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

	float scale = uiContext.width / m_width;
	UiInstance sliderInst = {};
	std::memcpy(sliderInst.transform, (Matrix4f::Translate(m_sliderPosX, uiContext.height - m_border / m_scale[1], 0.0f) * Matrix4f::Scale(scale * uiContext.width, m_border / m_scale[1], 1.0f)).getData(), sizeof(Matrix4f));
	std::memcpy(sliderInst.color, m_sliderColor.getData(), sizeof(Vector4f));

	sliderInst.textureRect[0] = 0.0f;
	sliderInst.textureRect[1] = 0.0f;
	sliderInst.textureRect[2] = 1.0f;
	sliderInst.textureRect[3] = 1.0f;
	sliderInst.textureLayer = 0.0f;

	sliderInst.flipAndTile[0] = 0.0f;
	sliderInst.flipAndTile[1] = 0.0f;
	sliderInst.flipAndTile[2] = 0.0f;

	pushWidget(UiPipelineType::Standard, sliderInst);
}

bool Empty::OnMouseOver(int mouseX, int mouseY) {
	return true;
}

bool Empty::isOverSlider(int mouseX, int mouseY) {

	float widthSlider = (uiContext.width / m_width) * uiContext.width * m_scale[0];
	float height = m_height * m_scale[1];

	Vector2f position = getWorldPosition(true);

	return (mouseX >= m_sliderPosX && mouseX <= m_sliderPosX + widthSlider) &&
           (mouseY >= uiContext.height - m_border && mouseY <= uiContext.height);
}

void Empty::OnLayoutChanged() {
	if (!m_isLayoutDirty)
		return;

	Widget::OnLayoutChanged();

	m_width = std::max(uiContext.width, m_width);
	m_height = std::max(uiContext.height, m_height);
}