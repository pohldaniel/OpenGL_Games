#include <entt/entt.hpp>

#include <Services/IDebugDraw.h>
#include <Services/IRandom.h>
#include <Services/IHelper.h>

// Is init in the constructor of Game class in core folder
struct locator {
	using debugDraw = entt::ServiceLocator<IDebugDraw>;
	//using audio = entt::ServiceLocator<IAudio>;
	using random = entt::ServiceLocator<IRandom>;
	using helper = entt::ServiceLocator<IHelper>;
};
