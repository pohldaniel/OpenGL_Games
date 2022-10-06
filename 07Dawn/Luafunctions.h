#pragma once

#include <string>

struct lua_State;

namespace LuaFunctions{

	lua_State *getGlobalLuaState();
	void executeLuaFile(std::string filename);
	void executeLuaScript(std::string scripttext);
	std::string getIDFromLuaTable(const std::string &tableName, const void *value);
}