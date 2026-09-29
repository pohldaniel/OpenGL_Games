#include <iostream>
#include "Button.h"

Button::Button() : Widget(),
m_color(Vector4f::ONE), 
m_outlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f)), 
m_outlineColorHover(Vector4f(1.0f, 0.0f, 1.0f, 1.0f)),
m_thickness(0.0f),
m_isPressed(false),
m_wasPressed(false),
m_onClick(nullptr){
	
}

Button::Button(const Button& rhs) :
	Widget(rhs),
	m_color(rhs.m_color),
	m_outlineColorHover(rhs.m_outlineColorHover),
	m_thickness(rhs.m_thickness),
	m_isPressed(rhs.m_isPressed),
	m_wasPressed(rhs.m_wasPressed),
	m_onClick(rhs.m_onClick){
}

Button::Button(Button&& rhs) noexcept :
	Widget(rhs),
	m_color(rhs.m_color),
	m_outlineColorHover(rhs.m_outlineColorHover),
	m_thickness(rhs.m_thickness),
	m_isPressed(rhs.m_isPressed),
	m_wasPressed(rhs.m_wasPressed),
	m_onClick(std::move(rhs.m_onClick)) {
}

Button::~Button() {

}

void Button::setColor(const Vector4f& color) {
	m_color = color;
}

void Button::setOutlineColor(const Vector4f& color) {
	m_outlineColor = color;
}

void Button::setOutlineColorHover(const Vector4f& color) {
	m_outlineColorHover = color;
}

void Button::setOutlineThickness(float thickness) {
	m_thickness = thickness;
}

void Button::createDefault() {
	UiInstance buttonInst = {};
	std::memcpy(buttonInst.transform, getWorldTransformation().getData(), sizeof(Matrix4f));
	std::memcpy(buttonInst.color, m_color.getData(), sizeof(Vector4f));

	buttonInst.textureRect[0] = 0.0f;
	buttonInst.textureRect[1] = 0.0f;
	buttonInst.textureRect[2] = 1.0f;
	buttonInst.textureRect[3] = 1.0f;
	buttonInst.textureLayer = 0.0f;

	buttonInst.flipAndTile[0] = 0.0f;
	buttonInst.flipAndTile[1] = 0.0f;
	buttonInst.flipAndTile[2] = 0.0f;

	pushWidget(UiPipelineType::MaskWrite, buttonInst);

	Vector2f scale = getWorldScale();
	float xScaleOutline = (m_thickness) / (scale[0]);
	float yScaleOutline = (m_thickness) / (scale[1]);

	Matrix4f transformOutline = { 1.0f + xScaleOutline * 2.0f , 0.0f, 0.0f, 0.0f,
                                  0.0f, 1.0f + yScaleOutline * 2.0f , 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  -xScaleOutline,  -yScaleOutline , 0.0f, 1.0f};

	UiInstance outlineInst = {};
	std::memcpy(outlineInst.transform, (getWorldTransformation() * transformOutline).getData(), sizeof(Matrix4f));
	std::memcpy(outlineInst.color, m_outlineColor.getData(), sizeof(Vector4f));
	outlineInst.textureRect[0] = 0.0f;
	outlineInst.textureRect[1] = 0.0f;
	outlineInst.textureRect[2] = 1.0f;
	outlineInst.textureRect[3] = 1.0f;
	outlineInst.textureLayer = 0.0f;

	outlineInst.flipAndTile[0] = 0.0f;
	outlineInst.flipAndTile[1] = 0.0f;
	outlineInst.flipAndTile[2] = 0.0f;
	pushWidget(UiPipelineType::OutlineRead, outlineInst);
}

void Button::inputDefault(int mouseX, int mouseY, bool buttonLeft) {
	Vector2f position = getWorldPosition();
	Vector2f scale = getWorldScale();
	m_isPressed = false;

	if (mouseX > position[0] - m_thickness && mouseX < position[0] + scale[0] + m_thickness &&
		mouseY > position[1] - m_thickness && mouseY < position[1] + scale[1] + m_thickness) {
		m_outlineColor = m_outlineColorHover;
		m_isPressed = buttonLeft;
	}else {
		m_outlineColor = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
	}

	if (m_isPressed && !m_wasPressed && m_onClick) {
		m_onClick();
	}

	m_wasPressed = m_isPressed;
}

void Button::setOnClick(std::function<void()> fun) {
	m_onClick = fun;
}

void Button::layoutDefault() {
	if (!m_isLayoutDirty)
		return;

	if (!m_children.empty()) {
		Vector2f scale = m_parent ? static_cast<Widget*>(m_parent)->getWorldScale() : getScale();
		Vector2f position = getPosition();

		float width = 0.0f;
		float height = 0.0f;

		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			Widget* child = static_cast<Widget*>((*it).get());
			width += child->getWidth();
			height = std::max(height, child->getHeight());
		}
	
		setWidth(width + (m_paddingX * 2.0f), true);
		setHeight(height + (m_paddingY * 2.0f), true);
		setScale(m_width / scale[0], m_height / scale[1]);
		scale = getWorldScale();

		float posX = m_paddingX;
		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			Widget* child = static_cast<Widget*>((*it).get());
			float posY = (getHeight() - child->getHeight()) * 0.5f;
			child->setPosition(posX / scale[0], posY / scale[1]);
			posX += child->getWidth();
		}
	}else {
		
		Vector2f scale = m_parent ? static_cast<Widget*>(m_parent)->getWorldScale() : getScale();
		setWidth(m_scale[0] * scale[0] + m_paddingX * 2.0f, true);
		setHeight(m_scale[1] * scale[1] + m_paddingY * 2.0f, true);
		setScale(m_width / scale[0], m_height / scale[1]);

		//std::cout << " BUTTON: " << "WIDTH: " << m_width << " HEIGHT: " << m_height << "PADDING: " << m_padding << std::endl;
	}
	m_isLayoutDirty = false;
}

/*void Button::setPadding(float padding, bool silent) {
	Widget::setPadding(padding, silent);
	m_width = characterSet.getWidth(m_text) + m_padding * 2.0f;
	m_height = characterSet.lineHeight + m_padding * 2.0f;*/
