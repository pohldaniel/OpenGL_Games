#pragma once

#include <engine/Vector.h>
#include "LuaHelper.h"

struct TransformComp {
	Vector3f position;
	Vector3f rotation;
	Vector3f scale;

	TransformComp() : position(0.0f), rotation(0.0f), scale(1.0f){
	}
};

static Vector3f lua_tovector(lua_State* L, int index){
	Vector3f vec;
	// Sanity check
	if (!lua_istable(L, index)) {
		std::cout << "Error: not table" << std::endl;
		return vec;
	}

	lua_getfield(L, index, "x");
	vec[0] = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "y");
	vec[1] = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "z");
	vec[2] = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);

	return vec;
}

static void lua_pushvector(lua_State* L, const Vector3f& vec){
	lua_newtable(L);

	lua_pushnumber(L, vec[0]);
	lua_setfield(L, -2, "x");

	lua_pushnumber(L, vec[1]);
	lua_setfield(L, -2, "y");

	lua_pushnumber(L, vec[2]);
	lua_setfield(L, -2, "z");

	lua_getglobal(L, "vector");
	lua_setmetatable(L, -2);
}

static TransformComp lua_totransform(lua_State* L, int index){
	TransformComp transform;
	// Sanity check
	if (!lua_istable(L, index)) {
		std::cout << "Error: not table" << std::endl;
		return transform;
	}

	lua_getfield(L, index, "position");
	transform.position = lua_tovector(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "rotation");
	transform.rotation = lua_tovector(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "scale");
	transform.scale = lua_tovector(L, -1);
	lua_pop(L, 1);

	return transform;
}

static void lua_pushtransform(lua_State* L, const TransformComp& transform){
	lua_newtable(L);

	lua_pushvector(L, transform.position);
	lua_setfield(L, -2, "position");

	lua_pushvector(L, transform.rotation);
	lua_setfield(L, -2, "rotation");

	lua_pushvector(L, transform.scale);
	lua_setfield(L, -2, "scale");
}