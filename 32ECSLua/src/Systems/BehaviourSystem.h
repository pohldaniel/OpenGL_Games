#pragma once

#include <Systems/System.h>
#include <Components/Behaviour.h>

#include "LuaHelper.h"

class BehaviourSystem : public System{

public:
	BehaviourSystem(lua_State* L): L(L) {}

	bool update(entt::registry& reg, float deltaTime) final{
		auto view = reg.view<Behaviour>();

		view.each([&](const Behaviour& script){
			lua_rawgeti(L, LUA_REGISTRYINDEX, script.luaRef);
			if (luaL_dofile(L, ("Scripts/Behaviour/" + std::string(script.path)).c_str()) != LUA_OK)
				LuaHelper::dumpError(L);
			else{
				lua_getfield(L, -1, "update"); // Get new update function
				lua_setfield(L, -3, "update"); // Set instance update function to the new one
				lua_pop(L, 1);
			}

			lua_getfield(L, -1, "update");
			if (lua_type(L, -1) == LUA_TNIL){
				lua_pop(L, 1);
				return;
			}

			lua_pushvalue(L, -2);
			lua_pushnumber(L, deltaTime);

			if (lua_pcall(L, 2, 0, 0) != LUA_OK)
				LuaHelper::dumpError(L);
			else
				lua_pop(L, 1);
		});

		return false;
	}

private:
	lua_State* L;
};