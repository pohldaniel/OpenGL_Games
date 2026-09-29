#pragma once
#include <functional>
#include <engine/ui/Widget.h>

class Button : public Widget {

public:

	Button();
	Button(const Button& rhs);
	Button(Button&& rhs) noexcept;
	virtual ~Button();

	void setColor(const Vector4f& color);
	void setOutlineColor(const Vector4f& color);
	void setOutlineColorHover(const Vector4f& color);
	void setOutlineThickness(float thickness);
	void setOnClick(std::function<void()> fun);

private:

	void inputDefault(int mouseX, int mouseY, bool buttonLeft) override;
	//void layoutDefault() override;
	void createDefault() override;

	Vector4f m_color;
	Vector4f m_outlineColor;
	Vector4f m_outlineColorHover;

	float m_thickness;
	bool m_isPressed;
	bool m_wasPressed;
	std::function<void()> m_onClick;
};