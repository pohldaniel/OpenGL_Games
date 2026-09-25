#include "Surface.h"

Surface::Surface() : Widget(), m_color(Vector4f::ONE) {
	
}

Surface::Surface(const Surface& rhs) :
	Widget(rhs),
	m_color(rhs.m_color) {
}

Surface::Surface(Surface&& rhs) noexcept :
	Widget(rhs),
	m_color(std::move(rhs.m_color)) {
}

Surface::~Surface() {

}

void Surface::setColor(const Vector4f& color) {
	m_color = color;
}

void Surface::pushUiInstance(UiPipelineType type, const UiInstance& instance) {
	if (Batches.empty() || Batches.back().pipelineType != type) {
		UiBatch newBatch;
		newBatch.pipelineType = type;
		newBatch.startIndex = static_cast<uint32_t>(Instances.size());
		newBatch.instanceCount = 0;
		Batches.push_back(newBatch);
	}

	// Daten hinzufügen und den Zähler des aktuellen Batches erhöhen
	Instances.push_back(instance);
	Batches.back().instanceCount++;
}

void Surface::drawDefault() {
	UiInstance uiInstance = {};
	uiInstance.transform = getWorldTransformation();
	uiInstance.color = m_color;
	uiInstance.textureRect[0] = 0.0f;
	uiInstance.textureRect[1] = 0.0f;
	uiInstance.textureRect[2] = 1.0f;
	uiInstance.textureRect[3] = 1.0f;
	uiInstance.textureLayer = 0.0f;

	uiInstance.flipAndTile[0] = 0.0f;
	uiInstance.flipAndTile[1] = 0.0f;
	uiInstance.flipAndTile[2] = 0.0f;
	pushUiInstance(UiPipelineType::Standard, uiInstance);
	//Instances.push_back(uiInstance);
}