#pragma once

#include "LuaHelper.h"
#include "TransformComp.h"

struct UIElement{

	bool visibleBG;
	std::string text;
	int fontSize;
	Vector2f position;
	Vector2f dimensions;
	Vector3f colour;

	UIElement() : visibleBG(true), fontSize(36), position({ 0.0f, 0.0f }), dimensions({ 100.0f, 100.0f }), colour(1.0f){}
};

static UIElement lua_touielement(lua_State* L, int index)
{
	UIElement elem;
	if (!lua_istable(L, index)) {
		std::cout << "Error: not table" << std::endl;
		elem.dimensions = { 0.0f, 0.0f };
		return elem;
	}
	Vector3f vec;

	lua_getfield(L, index, "visibleBG");
	elem.visibleBG = lua_toboolean(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "fontSize");
	elem.fontSize = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "text");
	elem.text = lua_tostring(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, index, "position");
	vec = lua_tovector(L, -1);
	elem.position = { vec[0], vec[1] };
	lua_pop(L, 1);

	lua_getfield(L, index, "dimensions");
	vec = lua_tovector(L, -1);
	elem.dimensions = { vec[0], vec[1] };
	lua_pop(L, 1);

	lua_getfield(L, index, "colour");
	vec = lua_tovector(L, -1);
	elem.colour[0] = vec[0];
	elem.colour[1] = vec[1];
	elem.colour[2] = vec[2];
	lua_pop(L, 1);

	return elem;
}

static void lua_pushuielement(lua_State* L, const UIElement& element)
{
	lua_newtable(L);

	lua_pushboolean(L, element.visibleBG);
	lua_setfield(L, -2, "visibleBG");

	lua_pushinteger(L, element.fontSize);
	lua_setfield(L, -2, "fontSize");

	lua_pushstring(L, element.text.c_str());
	lua_setfield(L, -2, "text");

	lua_pushvector(L, { element.position[0], element.position[1], 0.0f });
	lua_setfield(L, -2, "position");

	lua_pushvector(L, { element.dimensions[0], element.dimensions[1], 0.0f });
	lua_setfield(L, -2, "dimensions");

	Vector3f col;
	col[0] = element.colour[0];
	col[1] = element.colour[1];
	col[2] = element.colour[2];
	lua_pushvector(L, col);
	lua_setfield(L, -2, "colour");
}