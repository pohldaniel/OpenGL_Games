#include "Button.h"

Button::Button() : Widget(),
m_color(Vector4f::ONE), 
m_outlineColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f)), 
m_outlineColorHover(Vector4f(1.0f, 0.0f, 1.0f, 1.0f)),
m_thickness(0.0f),
m_isPressed(false),
m_wasPressed(false),
m_fun(nullptr){

}

Button::Button(const Button& rhs) :
	Widget(rhs),
	m_color(rhs.m_color),
	m_outlineColorHover(rhs.m_outlineColorHover),
	m_thickness(rhs.m_thickness),
	m_isPressed(rhs.m_isPressed),
	m_wasPressed(rhs.m_wasPressed),
	m_fun(rhs.m_fun){
}

Button::Button(Button&& rhs) noexcept :
	Widget(rhs),
	m_color(rhs.m_color),
	m_outlineColorHover(rhs.m_outlineColorHover),
	m_thickness(rhs.m_thickness),
	m_isPressed(rhs.m_isPressed),
	m_wasPressed(rhs.m_wasPressed),
	m_fun(std::move(rhs.m_fun)) {
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

void Button::drawDefault() {
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

	addWidget(UiPipelineType::MaskWrite, buttonInst);

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
	addWidget(UiPipelineType::OutlineRead, outlineInst);
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

	if (m_isPressed && !m_wasPressed && m_fun) {
		m_fun();
	}

	m_wasPressed = m_isPressed;
}

void Button::setFunction(std::function<void()> fun) {
	m_fun = fun;
}