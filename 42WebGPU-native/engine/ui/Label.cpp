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
	m_width = characterSet.getWidth(m_text) + m_padding * 2.0f;
	m_height = characterSet.lineHeight + m_padding * 2.0f;
	OnInvalidate();
}

void Label::layoutDefault() {
	if (!m_isLayoutDirty)
		return;

	Vector2f worldScale = getWorldScale();
	setScale(m_width / worldScale[0], m_height / worldScale[1]);
	m_isLayoutDirty = false;
}

void Label::createDefault() {
	Vector2f currentCursor = { 0.0f, 0.0f };
	Vector2f scale = getWorldScale();
	
	for (char c : m_text) {
		
		const Char& ch = characterSet.getCharacter(c);
		float dx = (m_padding + ch.pos[0]) / scale[0];
		float dy = m_padding / scale[1];
		float gw = ch.size[0] / scale[0];
		float gh = ch.size[1] / scale[1];

		UiInstance uiInstance = {};
		Matrix4f glyphTransform = Matrix4f::Translate(currentCursor[0] + dx, dy, 0.0f) * Matrix4f::Scale(gw, gh, 1.0f);

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

void Label::setPadding(float padding, bool silent) {
	Widget::setPadding(padding, silent);
	m_width = characterSet.getWidth(m_text) + m_padding * 2.0f;
	m_height = characterSet.lineHeight + m_padding * 2.0f;
}