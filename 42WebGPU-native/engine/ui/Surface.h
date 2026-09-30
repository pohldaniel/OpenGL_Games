#pragma once

#include <engine/ui/Widget.h>

class Surface : public Widget {

public:

	Surface();
	Surface(const Surface& rhs);
	Surface(Surface&& rhs) noexcept;
	virtual ~Surface();

	void setColor(const Vector4f& color);

private:
	
	bool inputDefault(int mouseX, int mouseY, bool buttonLeft) override;
	void createDefault() override;
	void resetDefault() override;

	Vector4f m_defaultColor;
	Vector4f m_color;

	bool m_isDragged;
	bool m_isResizing;
	int m_mouseX, m_mouseY;
};