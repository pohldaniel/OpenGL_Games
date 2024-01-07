#include <engine/input/Mouse.h>
#include <engine/input/Keyboard.h>
#include <Components/TransformComp.h>
#include "Input.h"
#include "Application.h"

const std::unordered_map<std::string, int> Input::keys{
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

const std::unordered_map<std::string, int> Input::mouse{
	{ "LEFT", 0 },
	{ "RIGHT", 1 },
	{ "MIDDLE", 2 },
	{ "SIDE", 3 },
	{ "EXTRA", 4 },
	{ "FORWARD", 5 },
	{ "BACK", 6 },
};

const std::unordered_map<std::string, int>  Input::cursor = {
	{ "DEFAULT", 0 },
	{ "ARROW", 1 },
	{ "IBEAM", 2 },
	{ "CROSSHAIR", 3 },
	{ "POINTINGHAND", 4 },
	{ "SHOW", 5 },
	{ "HIDE", 6 },
};

Input::Input(){
}

Input::~Input(){
}

void Input::lua_openinput(lua_State* L, Input* input)
{
	lua_newtable(L);

	luaL_Reg methods[] = {
		{ "isKeyDown", lua_isKeyDown },
		{ "isKeyUp", lua_isKeyUp },
		{ "isKeyPressed", lua_isKeyPressed },
		{ "isKeyReleased", lua_isKeyReleased },
		{ "getKeyPressed", lua_getKeyPressed },
		{ "getCharPressed", lua_charPressed },
		{ "isMouseButtonDown", lua_isMouseButtonDown },
		{ "isMouseButtonUp", lua_isMouseButtonUp },
		{ "isMouseButtonPressed", lua_isMouseButtonPressed },
		{ "isMouseButtonReleased", lua_isMouseButtonReleased },
		{ "getMousePosition", lua_getMousePosition },
		{ "getMousePositionCenter", lua_getMousePositionCenter },
		{ "getMouseDelta", lua_getMouseDelta },
		{ "getMouseWheelMove", lua_getMouseWheelMove },
		{ "setMouseCursor", lua_setMouseCursor },
		{ NULL , NULL }
	};

	lua_pushlightuserdata(L, input);
	luaL_setfuncs(L, methods, 1);
	lua_setglobal(L, "input");

	lua_newtable(L);
	for (auto& element : keys)
	{
		lua_pushnumber(L, element.second);
		lua_setfield(L, -2, element.first.c_str());
	}
	lua_setglobal(L, "Keys");

	lua_newtable(L);
	for (auto& element : mouse)
	{
		lua_pushnumber(L, element.second);
		lua_setfield(L, -2, element.first.c_str());
	}
	lua_setglobal(L, "Mouse");

	lua_newtable(L);
	for (auto& element : cursor)
	{
		lua_pushnumber(L, element.second);
		lua_setfield(L, -2, element.first.c_str());
	}
	lua_setglobal(L, "Cursor");
}

int Input::lua_isKeyDown(lua_State* L)
{
	int key = (int)lua_tointeger(L, 1);
	//lua_pushboolean(L, IsKeyDown(key));
	return 1;
}

int Input::lua_isKeyUp(lua_State* L)
{
	int key = (int)lua_tointeger(L, 1);
	//lua_pushboolean(L, IsKeyUp(key));
	return 1;
}

int Input::lua_isKeyPressed(lua_State* L)
{
	Keyboard &keyboard = Keyboard::instance();
	int key = (int)lua_tointeger(L, 1);
	lua_pushboolean(L, keyboard.keyPressed(static_cast<Keyboard::Key>(key)));
	return 1;
}

int Input::lua_isKeyReleased(lua_State* L)
{
	int key = (int)lua_tointeger(L, 1);
	//lua_pushboolean(L, IsKeyReleased(key));
	return 1;
}

int Input::lua_getKeyPressed(lua_State* L)
{
	//lua_pushinteger(L, GetKeyPressed());
	return 1;
}

int Input::lua_charPressed(lua_State* L)
{
	//char c = GetCharPressed();
	//lua_pushlstring(L, &c, 1);
	return 1;
}

int Input::lua_isMouseButtonDown(lua_State* L)
{
	int button = (int)lua_tointeger(L, 1);
	//lua_pushboolean(L, IsMouseButtonDown(button));
	return 1;
}

int Input::lua_isMouseButtonUp(lua_State* L)
{
	int button = (int)lua_tointeger(L, 1);
	//lua_pushboolean(L, IsMouseButtonUp(button));
	return 1;
}

int Input::lua_isMouseButtonPressed(lua_State* L){
	Mouse &mouse = Mouse::instance();
	int button = (int)lua_tointeger(L, 1);	
	lua_pushboolean(L, mouse.buttonPressed(static_cast<Mouse::MouseButton>(button)));
	return 1;
}

int Input::lua_isMouseButtonReleased(lua_State* L)
{
	int button = (int)lua_tointeger(L, 1);
	//lua_pushboolean(L, IsMouseButtonReleased(button));
	return 1;
}

int Input::lua_getMousePosition(lua_State* L)
{
	//Vector2 pos = GetMousePosition();
	//lua_pushvector(L, { pos.x, pos.y, 0.0f });
	return 1;
}

int Input::lua_getMousePositionCenter(lua_State* L)
{
	Mouse &mouse = Mouse::instance();
	float mouseX = static_cast<float>(mouse.xPos());
	float mouseY = static_cast<float>(mouse.yPos());
	lua_pushvector(L, { mouseX - (Application::Width / 2), mouseY - (Application::Height / 2), 0.0f });
	return 1;
}

int Input::lua_getMouseDelta(lua_State* L)
{
	//Vector2 pos = GetMouseDelta();
	//lua_pushvector(L, { pos.x, pos.y, 0.0f });
	return 1;
}

int Input::lua_getMouseWheelMove(lua_State* L)
{
	//lua_pushnumber(L, GetMouseWheelMove());
	return 1;
}

int Input::lua_setMouseCursor(lua_State* L)
{
	/*int input = lua_tointeger(L, 1);
	if (input == cursor.at("SHOW"))
		ShowCursor();
	else if (input == cursor.at("HIDE"))
		HideCursor();
	else
		SetMouseCursor(input);*/

	return 0;
}
