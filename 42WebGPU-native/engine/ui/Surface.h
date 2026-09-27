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

	void createDefault() override;
	void inputDefault(int mouseX, int mouseY, bool buttonLeft) override;

	Vector4f m_color;
};