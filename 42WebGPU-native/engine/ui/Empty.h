#pragma once

#include <engine/ui/Widget.h>

class Empty : public Widget {

public:

	Empty();
	Empty(const Empty& rhs);
	Empty(Empty&& rhs) noexcept;
	virtual ~Empty();

private:

	void createDefault() override;
	void inputDefault(int mouseX, int mouseY, bool buttonLeft) override;
};