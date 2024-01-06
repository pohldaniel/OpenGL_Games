#pragma once

#include <Components/UIElement.h>

class UserInterface{
private:
	static int lua_hover(lua_State* L);
public:
	UserInterface();
	virtual ~UserInterface();

	static void lua_openui(lua_State* L, UserInterface* UI);
};
