#pragma once

#include <unordered_map>
#include <string>
#include "LuaHelper.h"

class Input {

private:
	
	static const std::unordered_map<std::string, int> keys;
	static const std::unordered_map<std::string, int> mouse;
	static const std::unordered_map<std::string, int> cursor;

	// Lua Wrappers
	static int lua_isKeyDown(lua_State* L);
	static int lua_isKeyUp(lua_State* L);
	static int lua_isKeyPressed(lua_State* L);
	static int lua_isKeyReleased(lua_State* L);
	static int lua_getKeyPressed(lua_State* L);
	static int lua_charPressed(lua_State* L);

	static int lua_isMouseButtonDown(lua_State* L);
	static int lua_isMouseButtonUp(lua_State* L);
	static int lua_isMouseButtonPressed(lua_State* L);
	static int lua_isMouseButtonReleased(lua_State* L);
	static int lua_getMousePosition(lua_State* L);
	static int lua_getMousePositionCenter(lua_State* L);
	static int lua_getMouseDelta(lua_State* L);
	static int lua_getMouseWheelMove(lua_State* L);
	static int lua_setMouseCursor(lua_State* L);

public:
	Input();
	virtual ~Input();

	static void lua_openinput(lua_State* L, Input* input);
};

