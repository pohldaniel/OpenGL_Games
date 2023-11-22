#include <cstdlib>
#include <iostream>
#include <unordered_set>

#include <tolua++.h>
#include <lua.hpp>
#include "engine/Rect.h"
#include "Callindirection.h"
#include "Quest.h"
#include "Npc.h"
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
		if (lua_isnil(lState, -1) || !lua_istable(lState, -1)){
			std::cout << std::string("table ").append(tableName).append(" does not exist in lua as global table").c_str() << std::endl;
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
					}
					found = true;
					foundKey = lua_tostring(lState, -2);
				}
			}
			// pop the value from the stack
			lua_pop(lState, 1);
		}

		if (found) {
			// pop the last key from the stack
			lua_pop(lState, 1);

			// pop the table from the stack
			lua_pop(lState, 1);
		}else{
			std::cout << std::string("could not find searched value in table ").append(tableName).c_str() << std::endl;
		}

		return foundKey;
	}

	void incrementTableRects(const std::string &tableName) {
		lua_State *lState = getGlobalLuaState();
		lua_getglobal(lState, tableName.c_str());
		if (lua_isnil(lState, -1) || !lua_istable(lState, -1)) {
			std::cout << std::string("table ").append(tableName).append(" does not exist in lua as global table").c_str() << std::endl;
		}

		lua_pushnil(lState);

		// lua-stack layout now:
		// -1 nil
		// -2 table

		while (lua_next(lState, -2) != 0) {
			// lua-stack layout now:
			// -1 value
			// -2 key
			// -3 table
			if (lua_isuserdata(lState, -1)) {
				// tolua stores userdata in a strange way so we need to retrieve it as *(void**)
				void *curUserData = *(void**)lua_touserdata(lState, -1);
				((TextureRect*)curUserData)->frame++;
			}
			// pop the value from the stack
			lua_pop(lState, 1);
		}
	}

	std::string getEventHandlerName(const std::string &tableName, const CallIndirection* eventHandler) {
		lua_State *lState = getGlobalLuaState();
		lua_getglobal(lState, tableName.c_str());
		if (lua_isnil(lState, -1) || !lua_istable(lState, -1)) {
			std::cout << std::string("table ").append(tableName).append(" does not exist in lua as global table").c_str() << std::endl;
		}

		lua_pushnil(lState);
		std::string result = "";

		bool found = false;
		while (lua_next(lState, -2) != 0) {
			if (lua_isstring(lState, -2)) {
				size_t s_len = 0;
				const char *s = lua_tolstring(lState, -2, &s_len);
				std::string nameSubtable = std::string(s, s_len);
				lua_getfield(lState, -1, nameSubtable.c_str());
				while (lua_next(lState, -2) != 0) {
					if (lua_isuserdata(lState, -1)) {
						void *curUserData = *(void**)lua_touserdata(lState, -1);

						if (static_cast<CallIndirection*>(curUserData) == eventHandler) {
							if (lua_isstring(lState, -2)) {
								size_t s_len = 0;
								const char *s = lua_tolstring(lState, -2, &s_len);
								std::string nameHandler(s, s_len);
								result = tableName + "." + nameSubtable + "." + nameHandler;
								found = true;
							}

							found = true;
							break;
						}
					}
					lua_pop(lState, 1);
				}
				if (found) break;
			}

			lua_pop(lState, 1);
		}

		if (found) {
			lua_pop(lState, 1);
			lua_pop(lState, 1);
		}

		return result;
	}

	std::string getSpawnPointName(const std::string &tableName, const Npc* npc) {
		lua_State *lState = getGlobalLuaState();
		lua_getglobal(lState, tableName.c_str());
		if (lua_isnil(lState, -1) || !lua_istable(lState, -1)) {
			std::cout << std::string("table ").append(tableName).append(" does not exist in lua as global table").c_str() << std::endl;
		}

		lua_pushnil(lState);
		std::string result = "";

		bool found = false;
		while (lua_next(lState, -2) != 0) {
			if (lua_isstring(lState, -2)) {
				size_t s_len = 0;
				const char *s = lua_tolstring(lState, -2, &s_len);
				std::string nameSubtable = std::string(s, s_len);
				lua_getfield(lState, -1, nameSubtable.c_str());
				while (lua_next(lState, -2) != 0) {
					if (lua_isuserdata(lState, -1)) {
						void *curUserData = *(void**)lua_touserdata(lState, -1);

						if (static_cast<Npc*>(curUserData) == npc) {
							if (lua_isstring(lState, -2)) {
								size_t s_len = 0;
								const char *s = lua_tolstring(lState, -2, &s_len);
								std::string nameHandler(s, s_len);
								result = tableName + "." + nameSubtable + "." + nameHandler;
								found = true;
							}

							found = true;
							break;
						}
					}
					lua_pop(lState, 1);
				}
				if (found) break;
			}

			lua_pop(lState, 1);
		}

		if (found) {
			lua_pop(lState, 1);
			lua_pop(lState, 1);
		}
		
		return result;
	}

	std::string getPath(const std::string &tableName, const Quest *quest) {
		lua_State *lState = getGlobalLuaState();
		lua_getglobal(lState, tableName.c_str());
		if (lua_isnil(lState, -1) || !lua_istable(lState, -1)) {
			std::cout << std::string("table ").append(tableName).append(" does not exist in lua as global table").c_str() << std::endl;
		}

		lua_pushnil(lState);

		std::string nameSubtable;
		bool found = false;
		while (lua_next(lState, -2) != 0) {
			if (lua_isstring(lState, -2)) {
				size_t s_len = 0;
				const char *s = lua_tolstring(lState, -2, &s_len);
				nameSubtable = std::string(s, s_len);
				lua_getfield(lState, -1, nameSubtable.c_str());
				while (lua_next(lState, -2) != 0) {
					if (lua_isuserdata(lState, -1)) {
						void *curUserData = *(void**)lua_touserdata(lState, -1);
						if (static_cast<Quest*>(curUserData) == quest) {
							found = true;
							break;
						}
					}
					lua_pop(lState, 1);
				}
				if (found) break;
			}

			lua_pop(lState, 1);
		}

		if (found) {
			lua_pop(lState, 1);
			lua_pop(lState, 1);
		}

		found = false;
		std::string result = "";

		lua_getfield(lState, -1, nameSubtable.c_str());
		while (lua_next(lState, -2) != 0) {
			if (lua_isstring(lState, -1)) {
				size_t s_len = 0;
				const char *s = lua_tolstring(lState, -2, &s_len);
				std::string path(s, s_len);
				if (path.compare("path") == 0) {
					const char *s2 = lua_tolstring(lState, -1, &s_len);
					result = std::string(s2, s_len);
					found = true;
				}
			}

			lua_pop(lState, 1);
		}

		if (found) {
			lua_pop(lState, 1);
			lua_pop(lState, 1);
		}

		return result;
	}
}