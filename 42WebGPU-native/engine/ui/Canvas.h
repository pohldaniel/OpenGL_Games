#pragma once

#include <engine/ui/Widget.h>

class Canvas : public Widget {

public:

	Canvas();
	Canvas(const Canvas& rhs);
	Canvas(Canvas&& rhs) noexcept;
	virtual ~Canvas();

	void setColor(const Vector4f& color);
	void setLayer(uint32_t layer);

private:

	bool OnInput(int mouseX, int mouseY, bool buttonLeft) override;
	void OnDraw() override;
	void OnLayoutChanged() override;

	Vector4f m_color;
	uint32_t m_layer;
};