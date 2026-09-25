#pragma once

#include <engine/ui/Widget.h>

class Surface : public Widget {

public:

	Surface();
	Surface(const Surface& rhs);
	Surface(Surface&& rhs) noexcept;
	virtual ~Surface();

	void setDrawFunction(std::function<void()> fun);
	void setColor(const Vector4f& color);

private:

	void drawDefault() override;
	void pushUiInstance(UiPipelineType type, const UiInstance& instance);

	Vector4f m_color;
};