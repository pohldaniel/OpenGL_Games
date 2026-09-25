#include "Button.h"

Button::Button() : Widget(), m_color(Vector4f::ONE), m_outlineColor(Vector4f(0.0f, 0.0f, 1.0f, 1.0f)){

}

Button::Button(const Button& rhs) :
	Widget(rhs),
	m_color(rhs.m_color) {
}

Button::Button(Button&& rhs) noexcept :
	Widget(rhs),
	m_color(std::move(rhs.m_color)) {
}

Button::~Button() {

}

void Button::setColor(const Vector4f& color) {
	m_color = color;
}

void Button::setOutlineColor(const Vector4f& color) {
	m_outlineColor = color;
}

void Button::pushUiInstance(UiPipelineType type, const UiInstance& instance) {
	if (Batches.empty() || Batches.back().pipelineType != type) {
		UiBatch newBatch;
		newBatch.pipelineType = type;
		newBatch.startIndex = static_cast<uint32_t>(Instances.size());
		newBatch.instanceCount = 0;
		Batches.push_back(newBatch);
	}
	Instances.push_back(instance);
	Batches.back().instanceCount++;
}

void Button::drawDefault() {
	UiInstance buttonInst = {};
	buttonInst.transform = getWorldTransformation();
	buttonInst.color = m_color;
	buttonInst.textureRect[0] = 0.0f;
	buttonInst.textureRect[1] = 0.0f;
	buttonInst.textureRect[2] = 1.0f;
	buttonInst.textureRect[3] = 1.0f;
	buttonInst.textureLayer = 0.0f;

	buttonInst.flipAndTile[0] = 0.0f;
	buttonInst.flipAndTile[1] = 0.0f;
	buttonInst.flipAndTile[2] = 0.0f;

	pushUiInstance(UiPipelineType::MaskWrite, buttonInst);

	UiInstance outlineInst = {};
	outlineInst.transform = getWorldTransformation() * Matrix4f::Scale(1.2f, 1.2f, 1.2f);
	outlineInst.color = m_outlineColor;
	outlineInst.textureRect[0] = 0.0f;
	outlineInst.textureRect[1] = 0.0f;
	outlineInst.textureRect[2] = 1.0f;
	outlineInst.textureRect[3] = 1.0f;
	outlineInst.textureLayer = 0.0f;

	outlineInst.flipAndTile[0] = 0.0f;
	outlineInst.flipAndTile[1] = 0.0f;
	outlineInst.flipAndTile[2] = 0.0f;
	pushUiInstance(UiPipelineType::OutlineRead, outlineInst);
}