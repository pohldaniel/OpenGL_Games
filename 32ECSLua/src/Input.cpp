#include "Input.h"

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
	int key = (int)lua_tointeger(L, 1);
	//lua_pushboolean(L, IsKeyPressed(key));
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

int Input::lua_isMouseButtonPressed(lua_State* L)
{
	int button = (int)lua_tointeger(L, 1);
	//lua_pushboolean(L, IsMouseButtonPressed(button));
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
	//Vector2 pos = GetMousePosition();
	//lua_pushvector(L, { pos.x - GetScreenWidth() / 2, pos.y - GetScreenHeight() / 2, 0.0f });
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
