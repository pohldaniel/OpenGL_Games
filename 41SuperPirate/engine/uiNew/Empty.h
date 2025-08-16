#pragma once
#include "Widget.h"

namespace ui
{
	class Empty : public Widget {

	public:

		Empty();
		Empty(const Empty& rhs);
		Empty(Empty&& rhs);
		virtual ~Empty();

	private:

		void drawDefault() override;
	};
}