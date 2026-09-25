#pragma once

#include <engine/ui/Widget.h>

class Button : public Widget {

public:

	Button();
	Button(const Button& rhs);
	Button(Button&& rhs) noexcept;
	virtual ~Button();

	void setDrawFunction(std::function<void()> fun);
	void setColor(const Vector4f& color);
	void setOutlineColor(const Vector4f& color);

private:

	void drawDefault() override;
	void pushUiInstance(UiPipelineType type, const UiInstance& instance);
	Vector4f m_color;
	Vector4f m_outlineColor;
};