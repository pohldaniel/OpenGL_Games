

#include <cstdlib>
#include <iostream>

#include <tolua++.h>
#include <lua.hpp>

#include "Luainterfacegenerated.h"

namespace LuaFunctions{

	static const luaL_reg lualibs[] = {
		{ "",     luaopen_base },
		{ "io",   luaopen_io },
		{ "DawnInterface", tolua_Luainterface_open },
		{ NULL,         NULL }
	};

	static lua_State *globalLuaState = NULL;

	static void cleanupLuaState(){
		if (globalLuaState != NULL) {
			lua_close(globalLuaState);
			globalLuaState = NULL;
		}
	}

	lua_State* createAndInitNewLuaState(){
		// initialize lua script interpreter
		lua_State *luaState = lua_open();

		// open lua libs
		for (const luaL_reg *lib = lualibs; lib->func != NULL; lib++) {
			lua_pushcfunction(luaState, lib->func);
			lua_pushstring(luaState, lib->name);
			lua_call(luaState, 1, 0);
		}

		return luaState;
	}

	lua_State* getGlobalLuaState(){
		if (globalLuaState == NULL) {
			globalLuaState = createAndInitNewLuaState();
			atexit(cleanupLuaState);
		}
		return globalLuaState;
	}

	void executeLuaFile(std::string filename){
		lua_State *lState = getGlobalLuaState();
		int retcode = luaL_dofile(lState, filename.c_str());
		if (retcode != 0) {
			// there is some error in the lua code
			std::cout << "error in lua file: " << lua_tostring(lState, -1) << std::endl;
			abort();
		}
	}

	void executeLuaScript(std::string scripttext){
		lua_State *lState = getGlobalLuaState();
		int retcode = luaL_dostring(lState, scripttext.c_str());
		if (retcode != 0) {
			// there is some error in the lua code
			std::cout << "error in lua script: " << lua_tostring(lState, -1) << std::endl;
			abort();
		}
	}

	std::string getIDFromLuaTable(const std::string &tableName, const void *value){
		lua_State *lState = getGlobalLuaState();
		lua_getglobal(lState, tableName.c_str());
		if (lua_isnil(lState, -1) || !lua_istable(lState, -1))
		{
			std::cout << std::string("table ").append(tableName).append(" does not exist in lua as global table").c_str() << std::endl;
			//dawn_debug_fatal(std::string("table ").append(tableName).append(" does not exist in lua as global table"));
		}
		lua_pushnil(lState);

		// lua-stack layout now:
		// -1 nil
		// -2 table

		bool found = false;
		std::string foundKey = "";

		while (!found && lua_next(lState, -2)){
			// lua-stack layout now:
			// -1 value
			// -2 key
			// -3 table
			if (lua_isuserdata(lState, -1)){
				// tolua stores userdata in a strange way so we need to retrieve it as *(void**)
				const void *curUserData = *(void**)lua_touserdata(lState, -1);

				if (curUserData == value){
					// we have found the value
					// normally we should not convert the key directly to string since afterwards it might not
					// be usable for lua_next anymore if it was no string. But since we don't need lua_next afterwards
					// anymore it does not matter
					// still it should be a string in our case so we check that
					if (!lua_isstring(lState, -2)){
						std::cout << std::string("lua table ").append(tableName).append(" had a non-string key").c_str() << std::endl;
						//dawn_debug_fatal(std::string("lua table ").append(tableName).append(" had a non-string key"));
					}
					found = true;
					foundKey = lua_tostring(lState, -2);
				}
			}
			// pop the value from the stack
			lua_pop(lState, 1);
		}
		// pop the last key from the stack
		lua_pop(lState, 1);

		// pop the table from the stack
		lua_pop(lState, 1);

		if (!found){
			std::cout << std::string("could not find searched value in table ").append(tableName).c_str() << std::endl;
			//dawn_debug_fatal(std::string("could not find searched value in table ").append(tableName));
		}

		return foundKey;
	}
}