#pragma once

#include <cstdint>

namespace evnt {
	struct ExitTrigger {
		ExitTrigger(int entity1, int entity2) : entity1(entity1), entity2(entity2) {}

		std::uint32_t entity1;
		std::uint32_t entity2;
	};
}
