#pragma once

#include <UI/WidgetMH.h>

class Empty : public WidgetMH {

public:

	Empty();
	Empty(const Empty& rhs);
	Empty(Empty&& rhs);
	virtual ~Empty();

private:

	void drawDefault() override;
};