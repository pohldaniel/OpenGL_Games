#include <entt/entt.hpp>

#include "i-debug-draw.hpp"
//#include "audio/i-audio.hpp"
#include "i-random.hpp"
#include "i-helper.hpp"

// Is init in the constructor of Game class in core folder
struct locator {
	using debugDraw = entt::ServiceLocator<IDebugDraw>;
	//using audio = entt::ServiceLocator<IAudio>;
	using random = entt::ServiceLocator<IRandom>;
	using helper = entt::ServiceLocator<IHelper>;
};
