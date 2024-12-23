#include "Empty.h"

namespace ui
{
	Empty::Empty() : Widget() {

	}

	Empty::Empty(const Empty& rhs) :
		Widget(rhs) {
	}

	Empty::Empty(Empty&& rhs) :
		Widget(rhs) {
	}

	Empty::~Empty() {

	}

	void Empty::drawDefault() {

	}
}