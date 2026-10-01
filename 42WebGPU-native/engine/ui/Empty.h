#pragma once

#include <engine/ui/Widget.h>

class Empty : public Widget {

public:

	Empty();
	Empty(const Empty& rhs);
	Empty(Empty&& rhs) noexcept;
	virtual ~Empty();

private:
	
	bool OnMouseOver(int mouseX, int mouseY) override;
	void OnDraw() override;
};