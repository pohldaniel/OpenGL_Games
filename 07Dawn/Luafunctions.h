#pragma once

#include <string>
class CallIndirection;
class Quest;
class Npc;
struct lua_State;

namespace LuaFunctions {

	lua_State *getGlobalLuaState();
	void executeLuaFile(std::string filename);
	void executeLuaScript(std::string scripttext);
	std::string getIDFromLuaTable(const std::string &tableName, const void *value);
	void incrementTableRects(const std::string &tableName);
	std::string getEventHandlerName(const std::string &tableName, const CallIndirection *eventHandler);
	std::string getSpawnPointName(const std::string &tableName, const Npc *npc);
	std::string getPath(const std::string &tableName, const Quest *quest);
}