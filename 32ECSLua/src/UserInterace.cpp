#include <engine/input/Mouse.h>
#include "UserInterface.h"

int UserInterface::lua_hover(lua_State* L){
	UIElement elem = lua_touielement(L, 1);
	bool result = false;
	Mouse &mouse = Mouse::instance();
	float mouseX = static_cast<float>(mouse.xPos());
	float mouseY = static_cast<float>(mouse.yPos());

	if (mouseX >= elem.position[0] && mouseX <= elem.position[0] + elem.dimensions[0] &&
		mouseY >= elem.position[1] && mouseY <= elem.position[1] + elem.dimensions[1])
		result = true;

	lua_pushboolean(L, result);
	return 1;
}

UserInterface::UserInterface(){
}

UserInterface::~UserInterface(){
}

void UserInterface::lua_openui(lua_State* L, UserInterface* UI){
	lua_newtable(L);

	luaL_Reg methods[] = {
		{ "isHover", lua_hover },
		{ NULL , NULL }
	};

	lua_pushlightuserdata(L, UI);
	luaL_setfuncs(L, methods, 1);
	lua_setglobal(L, "UI");
}
