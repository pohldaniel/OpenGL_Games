#pragma once

#include <engine/ui/Widget.h>

class Empty : public Widget {

public:

	Empty();
	Empty(const Empty& rhs);
	Empty(Empty&& rhs) noexcept;
	virtual ~Empty();

private:
	
	bool OnInput(int mouseX, int mouseY, bool buttonLeft) override;
	void OnDraw() override;
	bool OnMouseOver(int mouseX, int mouseY) override;
	void OnLayoutChanged() override;

	bool isOverSlider(int mouseX, int mouseY);

	
	float m_sliderPosX;
	bool m_isDragged;
	int m_mouseX, m_mouseY;

	Vector4f m_borderColor;
	Vector4f m_sliderColor;
};