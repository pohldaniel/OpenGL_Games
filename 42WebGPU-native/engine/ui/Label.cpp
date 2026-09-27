#include <iostream>
#include "Label.h"

Label::Label(const CharacterSet& characterSet) : Widget(), characterSet(characterSet), m_color(Vector4f::ONE) {

}

Label::Label(const Label& rhs) :
	Widget(rhs),
	characterSet(rhs.characterSet),
	m_text(rhs.m_text),
	m_color(rhs.m_color) {

}

Label::Label(Label&& rhs) noexcept :
	Widget(rhs),
	characterSet(rhs.characterSet),
	m_text(std::move(rhs.m_text)),
	m_color(std::move(rhs.m_color)) {

}

Label::~Label() {

}

void Label::setColor(const Vector4f& textColor) {
	m_color = textColor;
}

void Label::setText(const std::string& text) {
	m_text = text;
	float padding = 5.0f;
	float widht = characterSet.getWidth(m_text) + padding * 2.0f;
	float height = characterSet.lineHeight + padding * 2.0f;

	Vector2f scale = getWorldScale();
	Object2D::setScale(widht / scale[0], height / scale[1]);

	if (m_parent) {
		Widget* parent = static_cast<Widget*>(m_parent);
		parent->scale(m_scale[0], m_scale[1]);
		Vector2f posP = parent->getPosition();
		Vector2f scaleP = parent->getWorldScale();

		float posY = posP[1] + (scaleP[1] + padding - characterSet.lineHeight) * 0.5f;

		Object2D::translate((padding) / scaleP[0], posY / scaleP[1]);		
	}
	OnTransformChanged();
}

void Label::createDefault() {
	Vector2f currentCursor = { 0.0f, 0.0f };
	Vector2f scale = getWorldScale();
	
	for (char c : m_text) {
		
		const Char& ch = characterSet.getCharacter(c);
		float dx = ch.pos[0] / scale[0];
		float dy = ch.pos[1] / scale[1];
		float gw = ch.size[0] / scale[0];
		float gh = ch.size[1] / scale[1];

		UiInstance uiInstance = {};
		Matrix4f glyphTransform = Matrix4f::Translate(currentCursor[0] + dx, 0.0f, 0.0f) * Matrix4f::Scale(gw, gh, 1.0f);

		std::memcpy(uiInstance.transform, (getWorldTransformation() * glyphTransform).getData(), sizeof(Matrix4f));
		std::memcpy(uiInstance.color, m_color.getData(), sizeof(Vector4f));

		uiInstance.textureRect[0] = ch.textureOffset[0];
		uiInstance.textureRect[1] = ch.textureOffset[1];
		uiInstance.textureRect[2] = ch.textureSize[0];
		uiInstance.textureRect[3] = ch.textureSize[1];
		uiInstance.textureLayer = static_cast<float>(characterSet.layer);
		pushWidget(UiPipelineType::Text, uiInstance);

		currentCursor[0] += ch.advance / scale[0];
	}
}

void Label::inputDefault(int mouseX, int mouseY, bool buttonLeft) {

}