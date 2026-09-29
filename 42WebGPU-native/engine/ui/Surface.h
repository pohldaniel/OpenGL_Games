#pragma once

#include <engine/ui/Widget.h>

class Surface : public Widget {

public:

	Surface();
	Surface(const Surface& rhs);
	Surface(Surface&& rhs) noexcept;
	virtual ~Surface();

	void setColor(const Vector4f& color);
	void setGap(float gap);
	void setDrag(bool drag);

private:
	
	void inputDefault(int mouseX, int mouseY, bool buttonLeft) override;
	void layoutDefault() override;
	void createDefault() override;

	Vector4f m_defaultColor;
	Vector4f m_color;
	float m_gap;
	bool m_hasDrag;
	bool m_isDragged = false, m_isResizing = false;
	int m_mouseX, m_mouseY;
};