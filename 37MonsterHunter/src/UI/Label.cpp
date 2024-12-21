#include <engine/Fontrenderer.h>
#include "Label.h"
#include "Surface.h"

namespace ui
{
	Label::Label(const CharacterSet& characterSet) : Widget(), characterSet(characterSet), m_textColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f)), m_size(1.0f), m_label("label"), m_offsetX(0.0f), m_offsetY(0.0f) {

	}

	Label::Label(const Label& rhs) :
		Widget(rhs),
		characterSet(rhs.characterSet),
		m_label(rhs.m_label),
		m_textColor(rhs.m_textColor),
		m_size(rhs.m_size),
		m_offsetX(rhs.m_offsetX),
		m_offsetY(rhs.m_offsetY) {

	}

	Label::Label(Label&& rhs) :
		Widget(rhs),
		characterSet(std::move(rhs.characterSet)),
		m_label(std::move(rhs.m_label)),
		m_textColor(std::move(rhs.m_textColor)),
		m_size(std::move(rhs.m_size)),
		m_offsetX(std::move(rhs.m_offsetX)),
		m_offsetY(std::move(rhs.m_offsetY)) {

	}

	Label::~Label() {

	}

	void Label::setTextColor(const Vector4f& textColor) {
		m_textColor = textColor;
	}

	void Label::setLabel(const std::string& label) {
		m_label = label;
	}

	void Label::setSize(const float size) {
		m_size = size;
	}

	void Label::setOffsetX(const float offsetX) {
		m_offsetX = offsetX;
	}

	void Label::setOffsetY(const float offsetY) {
		m_offsetY = offsetY;
	}

	void Label::drawDefault() {
		characterSet.bind();

		if (m_offsetX || m_offsetY) {
			Fontrenderer::Get().addTextTransformed(characterSet, getWorldTransformationWithTranslation(m_offsetX, m_offsetY), m_label, m_textColor, m_size);
		}else {
			Fontrenderer::Get().addTextTransformed(characterSet, getWorldTransformation(), m_label, m_textColor, m_size);
		}
		Fontrenderer::Get().drawBuffer();
	}
	///////////////////////////////////////////////////////
	TextField::TextField(const CharacterSet& characterSet) : Label(characterSet), m_paddingX(10.0f), m_paddingY(10.0f), m_backgroundColor(Vector4f::ONE), m_edge(Edge::ALL), m_borderRadius(10.0f) {

	}

	TextField::TextField(const TextField& rhs) :
		Label(rhs),
		m_paddingX(rhs.m_paddingX),
		m_paddingY(rhs.m_paddingY),
		m_backgroundColor(rhs.m_backgroundColor),
		m_edge(rhs.m_edge),
		m_borderRadius(rhs.m_borderRadius)
	{

	}

	TextField::TextField(TextField&& rhs) :
		Label(rhs),
		m_paddingX(std::move(rhs.m_paddingX)),
		m_paddingY(std::move(rhs.m_paddingY)),
		m_backgroundColor(std::move(rhs.m_backgroundColor)),
		m_edge(std::move(rhs.m_edge)),
		m_borderRadius(std::move(rhs.m_borderRadius)) {

	}

	TextField::~TextField() {

	}

	void TextField::setBackgroundColor(const Vector4f& color) {
		m_backgroundColor = color;
	}

	void TextField::setEdge(Edge edge) {
		m_edge = edge;
	}

	void TextField::setBorderRadius(float borderRadius) {
		m_borderRadius = borderRadius;
	}

	void TextField::setPaddingX(const float paddingX) {
		m_paddingX = paddingX;
	}

	void TextField::setPaddingY(const float paddingY) {
		m_paddingY = paddingY;
	}

	void TextField::setShader(Shader* shader) {
		m_shader = shader;
		Sprite::setShader(m_shader);
	}

	void TextField::drawDefault() {

		float scaleX = characterSet.getWidth(m_label) * m_size + m_paddingX;
		float scaleY = static_cast<float>(characterSet.lineHeight) * m_size + m_paddingY;

		auto shader = m_shader ? m_shader : SurfaceShader.get();
		shader->use();
		shader->loadVector("u_dimensions", Vector2f(scaleX, scaleY));
		shader->loadFloat("u_radius", m_borderRadius);
		shader->loadUnsignedInt("u_edge", m_edge);
		drawTransformed(m_backgroundColor, getWorldTransformationWithScaleAndTranslation(scaleX, scaleY, m_offsetX, m_offsetY));

		characterSet.bind();
		if (m_offsetX || m_offsetY)
			Fontrenderer::Get().addTextTransformed(characterSet, getWorldTransformationWithTranslation(m_offsetX + 0.5f * m_paddingX, m_offsetY + 0.5f * m_paddingY), m_label, m_textColor, m_size);
		else
			Fontrenderer::Get().addTextTransformed(characterSet, getWorldTransformationWithTranslation(0.5f * m_paddingX, 0.5f * m_paddingY), m_label, m_textColor, m_size);

		Fontrenderer::Get().drawBuffer();
	}
}