#pragma once

#include <engine/ui/Widget.h>

class Surface : public Widget {

public:

	Surface();
	Surface(const Surface& rhs);
	Surface(Surface&& rhs) noexcept;
	virtual ~Surface();

	void setColor(const Vector4f& color);
	void setDrag(bool drag);

	void setScale(float sx, float sy) override;

private:
	
	void inputDefault(int mouseX, int mouseY, bool buttonLeft) override;
	void layoutDefault() override;
	void createDefault() override;

	Vector4f m_color;
	bool m_hasDrag;
	bool m_isDragged, m_firsDragged = false;
	int m_mouseX, m_mouseY;
};