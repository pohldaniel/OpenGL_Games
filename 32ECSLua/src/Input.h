#pragma once

#include <unordered_map>
#include <string>
#include "LuaHelper.h"

class Input {

private:
	inline static const std::unordered_map<std::string, int> keys{
		{ "NULL", 0 },
		{ "ZERO", 48 },
		{ "ONE", 49 },
		{ "TWO", 50 },
		{ "THREE", 51 },
		{ "FOUR", 52 },
		{ "FIVE", 53 },
		{ "SIX", 54 },
		{ "SEVEN", 55 },
		{ "EIGHT", 56 },
		{ "NINE", 57 },
		{ "A", 65 },
		{ "B", 66 },
		{ "C", 67 },
		{ "D", 68 },
		{ "E", 69 },
		{ "F", 70 },
		{ "G", 71 },
		{ "H", 72 },
		{ "I", 73 },
		{ "J", 74 },
		{ "K", 75 },
		{ "L", 76 },
		{ "M", 77 },
		{ "N", 78 },
		{ "O", 79 },
		{ "P", 80 },
		{ "Q", 81 },
		{ "R", 82 },
		{ "S", 83 },
		{ "T", 84 },
		{ "U", 85 },
		{ "V", 86 },
		{ "W", 87 },
		{ "X", 88 },
		{ "Y", 89 },
		{ "Z", 90 },
		{ "SPACE", 32 },
		{ "ESCAPE", 256 },
		{ "ENTER", 257 },
		{ "TAB", 258 },
		{ "BACKSPACE", 259 },
		{ "RIGHT", 262 },
		{ "LEFT", 263 },
		{ "DOWN", 264 },
		{ "UP", 265 },
		{ "LSHIFT", 340 },
		{ "LCONTROL", 341 },
		{ "LALT", 342 },
		{ "RSHIFT", 344 },
		{ "RCONTROL", 345 },
		{ "RALT", 346 },
		{ "KEYPAD0", 320 },
		{ "KEYPAD1", 321 },
		{ "KEYPAD2", 322 },
		{ "KEYPAD3", 323 },
		{ "KEYPAD4", 324 },
		{ "KEYPAD5", 325 },
		{ "KEYPAD6", 326 },
		{ "KEYPAD7", 327 },
		{ "KEYPAD8", 328 },
		{ "KEYPAD9", 329 },
	};
	inline static const std::unordered_map<std::string, int> mouse{
		{ "LEFT", 0 },
		{ "RIGHT", 1 },
		{ "MIDDLE", 2 },
		{ "SIDE", 3 },
		{ "EXTRA", 4 },
		{ "FORWARD", 5 },
		{ "BACK", 6 },
	};
	inline static const std::unordered_map<std::string, int> cursor{
		{ "DEFAULT", 0 },
		{ "ARROW", 1 },
		{ "IBEAM", 2 },
		{ "CROSSHAIR", 3 },
		{ "POINTINGHAND", 4 },
		{ "SHOW", 5 },
		{ "HIDE", 6 },
	};

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

