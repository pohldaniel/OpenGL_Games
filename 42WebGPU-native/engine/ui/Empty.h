#pragma once

#include <engine/ui/Widget.h>

class Empty : public Widget {

public:

	Empty();
	Empty(const Empty& rhs);
	Empty(Empty&& rhs) noexcept;
	virtual ~Empty();

private:
	
	void inputDefault(int mouseX, int mouseY, bool buttonLeft) override;
	//void layoutDefault() override;
	void createDefault() override;
};