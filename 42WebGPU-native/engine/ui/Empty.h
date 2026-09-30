#pragma once

#include <engine/ui/Widget.h>

class Empty : public Widget {

public:

	Empty();
	Empty(const Empty& rhs);
	Empty(Empty&& rhs) noexcept;
	virtual ~Empty();

private:
	
	bool inputDefault(int mouseX, int mouseY, bool buttonLeft) override;
	bool isMouseOverDefault(int mouseX, int mouseY) override;
	void createDefault() override;
};