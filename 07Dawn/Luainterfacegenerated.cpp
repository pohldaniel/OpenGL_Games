/*
** Lua binding: Luainterface
** Generated automatically by tolua++-1.0.92 on Tue Dec 27 22:14:10 2022.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_Luainterface_open(lua_State* tolua_S);

#include "TilesetManager.h"
#include "Zone.h"
#include "Character.h"
#include "TextureManager.h"
#include "Npc.h"
#include "Player.h"
#include "InteractionRegion.h"
#include "InteractionPoint.h"
#include "TextWindow.h"
#include "Enums.h"
#include "Actions.h"
#include "Spells.h"
#include "Item.h"
#include "Shop.h"
#include "Quest.h"
#include "CallIndirection.h"
#include "Shop.h"
#include "Constants.h"

/* function to register type */
static void tolua_reg_types(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "GeneralRayDamageSpell");
	tolua_usertype(tolua_S, "TextureManager");
	tolua_usertype(tolua_S, "Action");
	tolua_usertype(tolua_S, "CharacterType");
	tolua_usertype(tolua_S, "ShopCanvas");
	tolua_usertype(tolua_S, "GeneralBoltDamageSpell");
	tolua_usertype(tolua_S, "Npc");
	tolua_usertype(tolua_S, "TextureRect");
	tolua_usertype(tolua_S, "MeleeDamageAction");
	tolua_usertype(tolua_S, "Character");
	tolua_usertype(tolua_S, "Item");
	tolua_usertype(tolua_S, "CallIndirection");
	tolua_usertype(tolua_S, "Shop");
	tolua_usertype(tolua_S, "TextWindow");
	tolua_usertype(tolua_S, "Quest");
	tolua_usertype(tolua_S, "RangedDamageAction");
	tolua_usertype(tolua_S, "LuaCallIndirection");
	tolua_usertype(tolua_S, "Spell");
	tolua_usertype(tolua_S, "GeneralDamageSpell");
	tolua_usertype(tolua_S, "TileSet");
	tolua_usertype(tolua_S, "InteractionPoint");
	tolua_usertype(tolua_S, "GeneralHealingSpell");
	tolua_usertype(tolua_S, "GeneralAreaDamageSpell");
	tolua_usertype(tolua_S, "TextureAtlasCreator");
	tolua_usertype(tolua_S, "GeneralBuffSpell");
	tolua_usertype(tolua_S, "Player");
	tolua_usertype(tolua_S, "ConfigurableAction");
	tolua_usertype(tolua_S, "ConfigurableSpell");
	tolua_usertype(tolua_S, "AdjacencyEquivalenceClass");
	tolua_usertype(tolua_S, "SpellActionBase");
	tolua_usertype(tolua_S, "Zone");
	tolua_usertype(tolua_S, "InteractionRegion");
}

/* function: Globals::useDisplaymode */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Globals_useDisplaymode00
static int tolua_Luainterface_Globals_useDisplaymode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isboolean(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		bool flag = ((bool)tolua_toboolean(tolua_S, 1, 0));
		{
			Globals::useDisplaymode(flag);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'useDisplaymode'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: Globals::isFullscreen */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Globals_isFullscreen00
static int tolua_Luainterface_Globals_isFullscreen00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isboolean(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		bool flag = ((bool)tolua_toboolean(tolua_S, 1, 0));
		{
			Globals::isFullscreen(flag);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'isFullscreen'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: Globals::setSoundVolume */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Globals_setSoundVolume00
static int tolua_Luainterface_Globals_setSoundVolume00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnumber(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		float soundVolume = ((float)tolua_tonumber(tolua_S, 1, 0));
		{
			Globals::setSoundVolume(soundVolume);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSoundVolume'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: Globals::setMusicVolume */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Globals_setMusicVolume00
static int tolua_Luainterface_Globals_setMusicVolume00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnumber(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		float musicVolume = ((float)tolua_tonumber(tolua_S, 1, 0));
		{
			Globals::setMusicVolume(musicVolume);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMusicVolume'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: Globals::setResolution */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Globals_setResolution00
static int tolua_Luainterface_Globals_setResolution00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnumber(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		unsigned int width = ((unsigned int)tolua_tonumber(tolua_S, 1, 0));
		unsigned int height = ((unsigned int)tolua_tonumber(tolua_S, 2, 0));
		{
			Globals::setResolution(width, height);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setResolution'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::loadimage */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_loadimage00
static int tolua_Luainterface_DawnInterface_loadimage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string file = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		{
			TextureRect& tolua_ret = (TextureRect&)DawnInterface::loadimage(file);
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "TextureRect");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'loadimage'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::enterZone */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_enterZone00
static int tolua_Luainterface_DawnInterface_enterZone00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string zoneName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		int enterX = ((int)tolua_tonumber(tolua_S, 2, 0));
		int enterY = ((int)tolua_tonumber(tolua_S, 3, 0));
		{
			DawnInterface::enterZone(zoneName, enterX, enterY);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'enterZone'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::setCurrentZone */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_setCurrentZone00
static int tolua_Luainterface_DawnInterface_setCurrentZone00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string zoneName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		{
			DawnInterface::setCurrentZone(zoneName);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCurrentZone'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::createNewMobType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_createNewMobType00
static int tolua_Luainterface_DawnInterface_createNewMobType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string typeID = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		{
			CharacterType& tolua_ret = (CharacterType&)DawnInterface::createNewMobType(typeID);
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "CharacterType");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'createNewMobType'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::addMobSpawnPoint */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_addMobSpawnPoint00
static int tolua_Luainterface_DawnInterface_addMobSpawnPoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 6, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 7, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 8, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string mobID = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		std::string name = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
		int x_pos = ((int)tolua_tonumber(tolua_S, 3, 0));
		int y_pos = ((int)tolua_tonumber(tolua_S, 4, 0));
		int respawn_rate = ((int)tolua_tonumber(tolua_S, 5, 0));
		int do_respawn = ((int)tolua_tonumber(tolua_S, 6, 0));
		Enums::Attitude attitude = ((Enums::Attitude) (int)  tolua_tonumber(tolua_S, 7, 0));
		{
			Npc* tolua_ret = (Npc*)DawnInterface::addMobSpawnPoint(mobID, name, x_pos, y_pos, respawn_rate, do_respawn, attitude);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "Npc");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addMobSpawnPoint'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::removeMobSpawnPoint */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_removeMobSpawnPoint00
static int tolua_Luainterface_DawnInterface_removeMobSpawnPoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Npc", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Npc* mobSpawnPoint = ((Npc*)tolua_tousertype(tolua_S, 1, 0));
		{
			DawnInterface::removeMobSpawnPoint(mobSpawnPoint);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'removeMobSpawnPoint'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::addInteractionRegion */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_addInteractionRegion00
static int tolua_Luainterface_DawnInterface_addInteractionRegion00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			InteractionRegion& tolua_ret = (InteractionRegion&)DawnInterface::addInteractionRegion();
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "InteractionRegion");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addInteractionRegion'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::removeInteractionRegion */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_removeInteractionRegion00
static int tolua_Luainterface_DawnInterface_removeInteractionRegion00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "InteractionRegion", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		InteractionRegion* regionToRemove = ((InteractionRegion*)tolua_tousertype(tolua_S, 1, 0));
		{
			DawnInterface::removeInteractionRegion(regionToRemove);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'removeInteractionRegion'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::addInteractionPoint */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_addInteractionPoint00
static int tolua_Luainterface_DawnInterface_addInteractionPoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			InteractionPoint& tolua_ret = (InteractionPoint&)DawnInterface::addInteractionPoint();
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "InteractionPoint");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addInteractionPoint'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::addCharacterInteractionPoint */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_addCharacterInteractionPoint00
static int tolua_Luainterface_DawnInterface_addCharacterInteractionPoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Character", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Character* character = ((Character*)tolua_tousertype(tolua_S, 1, 0));
		{
			InteractionPoint& tolua_ret = (InteractionPoint&)DawnInterface::addCharacterInteractionPoint(character);
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "InteractionPoint");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addCharacterInteractionPoint'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::removeInteractionPoint */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_removeInteractionPoint00
static int tolua_Luainterface_DawnInterface_removeInteractionPoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "InteractionPoint", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		InteractionPoint* pointToRemove = ((InteractionPoint*)tolua_tousertype(tolua_S, 1, 0));
		{
			DawnInterface::removeInteractionPoint(pointToRemove);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'removeInteractionPoint'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getItemReferenceRestore */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getItemReferenceRestore00
static int tolua_Luainterface_DawnInterface_getItemReferenceRestore00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isuserdata(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string varName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		void* ignore = ((void*)tolua_touserdata(tolua_S, 2, 0));
		{
			std::string tolua_ret = (std::string)  DawnInterface::getItemReferenceRestore(varName, ignore);
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'getItemReferenceRestore'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getItemReferenceRestore */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getItemReferenceRestore01
static int tolua_Luainterface_DawnInterface_getItemReferenceRestore01(lua_State* tolua_S)
{
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "Quest", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
	{
		std::string varName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		Quest* quest = ((Quest*)tolua_tousertype(tolua_S, 2, 0));
		{
			std::string tolua_ret = (std::string)  DawnInterface::getItemReferenceRestore(varName, quest);
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
tolua_lerror:
	return tolua_Luainterface_DawnInterface_getItemReferenceRestore00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getItemReferenceRestore */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getItemReferenceRestore02
static int tolua_Luainterface_DawnInterface_getItemReferenceRestore02(lua_State* tolua_S)
{
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "Shop", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
	{
		std::string varName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		Shop* shop = ((Shop*)tolua_tousertype(tolua_S, 2, 0));
		{
			std::string tolua_ret = (std::string)  DawnInterface::getItemReferenceRestore(varName, shop);
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
tolua_lerror:
	return tolua_Luainterface_DawnInterface_getItemReferenceRestore01(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getItemReferenceRestore */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getItemReferenceRestore03
static int tolua_Luainterface_DawnInterface_getItemReferenceRestore03(lua_State* tolua_S)
{
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "CallIndirection", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
	{
		std::string varName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		CallIndirection* eventHandler = ((CallIndirection*)tolua_tousertype(tolua_S, 2, 0));
		{
			std::string tolua_ret = (std::string)  DawnInterface::getItemReferenceRestore(varName, eventHandler);
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
tolua_lerror:
	return tolua_Luainterface_DawnInterface_getItemReferenceRestore02(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getItemReferenceRestore */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getItemReferenceRestore04
static int tolua_Luainterface_DawnInterface_getItemReferenceRestore04(lua_State* tolua_S)
{
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "InteractionPoint", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
	{
		std::string varName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		InteractionPoint* interactionPoint = ((InteractionPoint*)tolua_tousertype(tolua_S, 2, 0));
		{
			std::string tolua_ret = (std::string)  DawnInterface::getItemReferenceRestore(varName, interactionPoint);
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
tolua_lerror:
	return tolua_Luainterface_DawnInterface_getItemReferenceRestore03(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getItemReferenceRestore */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getItemReferenceRestore05
static int tolua_Luainterface_DawnInterface_getItemReferenceRestore05(lua_State* tolua_S)
{
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "InteractionRegion", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
	{
		std::string varName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		InteractionRegion* interactionRegion = ((InteractionRegion*)tolua_tousertype(tolua_S, 2, 0));
		{
			std::string tolua_ret = (std::string)  DawnInterface::getItemReferenceRestore(varName, interactionRegion);
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
tolua_lerror:
	return tolua_Luainterface_DawnInterface_getItemReferenceRestore04(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getItemReferenceRestore */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getItemReferenceRestore06
static int tolua_Luainterface_DawnInterface_getItemReferenceRestore06(lua_State* tolua_S)
{
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "Npc", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
	{
		std::string varName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		Npc* npc = ((Npc*)tolua_tousertype(tolua_S, 2, 0));
		{
			std::string tolua_ret = (std::string)  DawnInterface::getItemReferenceRestore(varName, npc);
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
tolua_lerror:
	return tolua_Luainterface_DawnInterface_getItemReferenceRestore05(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::storeGroundloot */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_storeGroundloot00
static int tolua_Luainterface_DawnInterface_storeGroundloot00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			std::string tolua_ret = (std::string)  DawnInterface::storeGroundloot();
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'storeGroundloot'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getSpellbookSaveText */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getSpellbookSaveText00
static int tolua_Luainterface_DawnInterface_getSpellbookSaveText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			std::string tolua_ret = (std::string)  DawnInterface::getSpellbookSaveText();
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'getSpellbookSaveText'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getActionbarSaveText */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getActionbarSaveText00
static int tolua_Luainterface_DawnInterface_getActionbarSaveText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			std::string tolua_ret = (std::string)  DawnInterface::getActionbarSaveText();
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'getActionbarSaveText'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getReenterCurrentZoneText */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getReenterCurrentZoneText00
static int tolua_Luainterface_DawnInterface_getReenterCurrentZoneText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			std::string tolua_ret = (std::string)  DawnInterface::getReenterCurrentZoneText();
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'getReenterCurrentZoneText'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::restoreActionBar */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_restoreActionBar00
static int tolua_Luainterface_DawnInterface_restoreActionBar00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnumber(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		int buttonNr = ((int)tolua_tonumber(tolua_S, 1, 0));
		SpellActionBase* action = ((SpellActionBase*)tolua_tousertype(tolua_S, 2, 0));
		{
			DawnInterface::restoreActionBar(buttonNr, action);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'restoreActionBar'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::restoreGroundLootItem */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_restoreGroundLootItem00
static int tolua_Luainterface_DawnInterface_restoreGroundLootItem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* item = ((Item*)tolua_tousertype(tolua_S, 1, 0));
		int xPos = ((int)tolua_tonumber(tolua_S, 2, 0));
		int yPos = ((int)tolua_tonumber(tolua_S, 3, 0));
		{
			DawnInterface::restoreGroundLootItem(item, xPos, yPos);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'restoreGroundLootItem'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::restoreGroundGold */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_restoreGroundGold00
static int tolua_Luainterface_DawnInterface_restoreGroundGold00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnumber(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		int amount = ((int)tolua_tonumber(tolua_S, 1, 0));
		int xPos = ((int)tolua_tonumber(tolua_S, 2, 0));
		int yPos = ((int)tolua_tonumber(tolua_S, 3, 0));
		{
			DawnInterface::restoreGroundGold(amount, xPos, yPos);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'restoreGroundGold'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::createTextWindow */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_createTextWindow00
static int tolua_Luainterface_DawnInterface_createTextWindow00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			TextWindow* tolua_ret = (TextWindow*)DawnInterface::createTextWindow();
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "TextWindow");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'createTextWindow'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::createGeneralRayDamageSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_createGeneralRayDamageSpell00
static int tolua_Luainterface_DawnInterface_createGeneralRayDamageSpell00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			GeneralRayDamageSpell* tolua_ret = (GeneralRayDamageSpell*)DawnInterface::createGeneralRayDamageSpell();
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "GeneralRayDamageSpell");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'createGeneralRayDamageSpell'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::createGeneralAreaDamageSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_createGeneralAreaDamageSpell00
static int tolua_Luainterface_DawnInterface_createGeneralAreaDamageSpell00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			GeneralAreaDamageSpell* tolua_ret = (GeneralAreaDamageSpell*)DawnInterface::createGeneralAreaDamageSpell();
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "GeneralAreaDamageSpell");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'createGeneralAreaDamageSpell'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::createGeneralBoltDamageSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_createGeneralBoltDamageSpell00
static int tolua_Luainterface_DawnInterface_createGeneralBoltDamageSpell00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			GeneralBoltDamageSpell* tolua_ret = (GeneralBoltDamageSpell*)DawnInterface::createGeneralBoltDamageSpell();
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "GeneralBoltDamageSpell");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'createGeneralBoltDamageSpell'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::createGeneralHealingSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_createGeneralHealingSpell00
static int tolua_Luainterface_DawnInterface_createGeneralHealingSpell00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			GeneralHealingSpell* tolua_ret = (GeneralHealingSpell*)DawnInterface::createGeneralHealingSpell();
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "GeneralHealingSpell");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'createGeneralHealingSpell'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::createGeneralBuffSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_createGeneralBuffSpell00
static int tolua_Luainterface_DawnInterface_createGeneralBuffSpell00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			GeneralBuffSpell* tolua_ret = (GeneralBuffSpell*)DawnInterface::createGeneralBuffSpell();
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "GeneralBuffSpell");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'createGeneralBuffSpell'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::createMeleeDamageAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_createMeleeDamageAction00
static int tolua_Luainterface_DawnInterface_createMeleeDamageAction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			MeleeDamageAction* tolua_ret = (MeleeDamageAction*)DawnInterface::createMeleeDamageAction();
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "MeleeDamageAction");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'createMeleeDamageAction'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::createRangedDamageAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_createRangedDamageAction00
static int tolua_Luainterface_DawnInterface_createRangedDamageAction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			RangedDamageAction* tolua_ret = (RangedDamageAction*)DawnInterface::createRangedDamageAction();
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "RangedDamageAction");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'createRangedDamageAction'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::copySpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_copySpell00
static int tolua_Luainterface_DawnInterface_copySpell00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralRayDamageSpell", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralRayDamageSpell* other = ((GeneralRayDamageSpell*)tolua_tousertype(tolua_S, 1, 0));
		{
			GeneralRayDamageSpell* tolua_ret = (GeneralRayDamageSpell*)DawnInterface::copySpell(other);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "GeneralRayDamageSpell");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'copySpell'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::copySpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_copySpell01
static int tolua_Luainterface_DawnInterface_copySpell01(lua_State* tolua_S)
{
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralAreaDamageSpell", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
	{
		GeneralAreaDamageSpell* other = ((GeneralAreaDamageSpell*)tolua_tousertype(tolua_S, 1, 0));
		{
			GeneralAreaDamageSpell* tolua_ret = (GeneralAreaDamageSpell*)DawnInterface::copySpell(other);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "GeneralAreaDamageSpell");
		}
	}
	return 1;
tolua_lerror:
	return tolua_Luainterface_DawnInterface_copySpell00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::copySpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_copySpell02
static int tolua_Luainterface_DawnInterface_copySpell02(lua_State* tolua_S)
{
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralBoltDamageSpell", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
	{
		GeneralBoltDamageSpell* other = ((GeneralBoltDamageSpell*)tolua_tousertype(tolua_S, 1, 0));
		{
			GeneralBoltDamageSpell* tolua_ret = (GeneralBoltDamageSpell*)DawnInterface::copySpell(other);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "GeneralBoltDamageSpell");
		}
	}
	return 1;
tolua_lerror:
	return tolua_Luainterface_DawnInterface_copySpell01(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::copySpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_copySpell03
static int tolua_Luainterface_DawnInterface_copySpell03(lua_State* tolua_S)
{
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralHealingSpell", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
	{
		GeneralHealingSpell* other = ((GeneralHealingSpell*)tolua_tousertype(tolua_S, 1, 0));
		{
			GeneralHealingSpell* tolua_ret = (GeneralHealingSpell*)DawnInterface::copySpell(other);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "GeneralHealingSpell");
		}
	}
	return 1;
tolua_lerror:
	return tolua_Luainterface_DawnInterface_copySpell02(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::copySpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_copySpell04
static int tolua_Luainterface_DawnInterface_copySpell04(lua_State* tolua_S)
{
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralBuffSpell", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
	{
		GeneralBuffSpell* other = ((GeneralBuffSpell*)tolua_tousertype(tolua_S, 1, 0));
		{
			GeneralBuffSpell* tolua_ret = (GeneralBuffSpell*)DawnInterface::copySpell(other);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "GeneralBuffSpell");
		}
	}
	return 1;
tolua_lerror:
	return tolua_Luainterface_DawnInterface_copySpell03(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::copySpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_copySpell05
static int tolua_Luainterface_DawnInterface_copySpell05(lua_State* tolua_S)
{
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "MeleeDamageAction", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
	{
		MeleeDamageAction* other = ((MeleeDamageAction*)tolua_tousertype(tolua_S, 1, 0));
		{
			MeleeDamageAction* tolua_ret = (MeleeDamageAction*)DawnInterface::copySpell(other);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "MeleeDamageAction");
		}
	}
	return 1;
tolua_lerror:
	return tolua_Luainterface_DawnInterface_copySpell04(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::copySpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_copySpell06
static int tolua_Luainterface_DawnInterface_copySpell06(lua_State* tolua_S)
{
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "RangedDamageAction", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
	{
		RangedDamageAction* other = ((RangedDamageAction*)tolua_tousertype(tolua_S, 1, 0));
		{
			RangedDamageAction* tolua_ret = (RangedDamageAction*)DawnInterface::copySpell(other);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "RangedDamageAction");
		}
	}
	return 1;
tolua_lerror:
	return tolua_Luainterface_DawnInterface_copySpell05(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::inscribeSpellInPlayerSpellbook */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_inscribeSpellInPlayerSpellbook00
static int tolua_Luainterface_DawnInterface_inscribeSpellInPlayerSpellbook00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* inscribedSpell = ((SpellActionBase*)tolua_tousertype(tolua_S, 1, 0));
		{
			DawnInterface::inscribeSpellInPlayerSpellbook(inscribedSpell);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'inscribeSpellInPlayerSpellbook'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::reloadSpellsFromPlayer */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_reloadSpellsFromPlayer00
static int tolua_Luainterface_DawnInterface_reloadSpellsFromPlayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			DawnInterface::reloadSpellsFromPlayer();
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'reloadSpellsFromPlayer'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getInventorySaveText */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getInventorySaveText00
static int tolua_Luainterface_DawnInterface_getInventorySaveText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			std::string tolua_ret = (std::string)  DawnInterface::getInventorySaveText();
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'getInventorySaveText'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::restoreItemInBackpack */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_restoreItemInBackpack00
static int tolua_Luainterface_DawnInterface_restoreItemInBackpack00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 1, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 5, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* item = ((Item*)tolua_tousertype(tolua_S, 1, 0));
		int inventoryPosX = ((int)tolua_tonumber(tolua_S, 2, 0));
		int inventoryPosY = ((int)tolua_tonumber(tolua_S, 3, 0));
		int stackSize = ((int)tolua_tonumber(tolua_S, 4, 1));
		{
			DawnInterface::restoreItemInBackpack(item, inventoryPosX, inventoryPosY, stackSize);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'restoreItemInBackpack'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::restoreWieldItem */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_restoreWieldItem00
static int tolua_Luainterface_DawnInterface_restoreWieldItem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnumber(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "Item", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		int slot = ((int)tolua_tonumber(tolua_S, 1, 0));
		Item* item = ((Item*)tolua_tousertype(tolua_S, 2, 0));
		{
			DawnInterface::restoreWieldItem(slot, item);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'restoreWieldItem'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::createNewItem */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_createNewItem00
static int tolua_Luainterface_DawnInterface_createNewItem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 6, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 7, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 8, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 9, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 10, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string name = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		int sizeX = ((int)tolua_tonumber(tolua_S, 2, 0));
		int sizeY = ((int)tolua_tonumber(tolua_S, 3, 0));
		std::string symbolFile = ((std::string)  tolua_tocppstring(tolua_S, 4, 0));
		Enums::ItemQuality itemQuality = ((Enums::ItemQuality) (int)  tolua_tonumber(tolua_S, 5, 0));
		EquipPosition::EquipPosition equipPosition = ((EquipPosition::EquipPosition) (int)  tolua_tonumber(tolua_S, 6, 0));
		Enums::ItemType itemType = ((Enums::ItemType) (int)  tolua_tonumber(tolua_S, 7, 0));
		Enums::ArmorType armorType = ((Enums::ArmorType) (int)  tolua_tonumber(tolua_S, 8, 0));
		Enums::WeaponType weaponType = ((Enums::WeaponType) (int)  tolua_tonumber(tolua_S, 9, 0));
		{
			Item* tolua_ret = (Item*)DawnInterface::createNewItem(name, sizeX, sizeY, symbolFile, itemQuality, equipPosition, itemType, armorType, weaponType);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "Item");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'createNewItem'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getPlayer */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getPlayer00
static int tolua_Luainterface_DawnInterface_getPlayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			Player& tolua_ret = (Player&)DawnInterface::getPlayer();
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "Player");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'getPlayer'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::getCurrentZone */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_getCurrentZone00
static int tolua_Luainterface_DawnInterface_getCurrentZone00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			Zone& tolua_ret = (Zone&)DawnInterface::getCurrentZone();
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "Zone");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'getCurrentZone'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::giveItemToPlayer */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_giveItemToPlayer00
static int tolua_Luainterface_DawnInterface_giveItemToPlayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* item = ((Item*)tolua_tousertype(tolua_S, 1, 0));
		{
			DawnInterface::giveItemToPlayer(item);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'giveItemToPlayer'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::addShop */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_addShop00
static int tolua_Luainterface_DawnInterface_addShop00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isboolean(tolua_S, 2, 1, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string name = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		bool replace = ((bool)tolua_toboolean(tolua_S, 2, false));
		{
			Shop& tolua_ret = (Shop&)DawnInterface::addShop(name, replace);
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "Shop");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addShop'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::addQuest */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_addQuest00
static int tolua_Luainterface_DawnInterface_addQuest00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string questName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		std::string questDescription = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
		{
			Quest* tolua_ret = (Quest*)DawnInterface::addQuest(questName, questDescription);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "Quest");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addQuest'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::createEventHandler */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_createEventHandler00
static int tolua_Luainterface_DawnInterface_createEventHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			LuaCallIndirection* tolua_ret = (LuaCallIndirection*)DawnInterface::createEventHandler();
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "LuaCallIndirection");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'createEventHandler'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::isSavingAllowed */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_isSavingAllowed00
static int tolua_Luainterface_DawnInterface_isSavingAllowed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnoobj(tolua_S, 1, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			bool tolua_ret = (bool)DawnInterface::isSavingAllowed();
			tolua_pushboolean(tolua_S, (bool)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'isSavingAllowed'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::setSavingAllowed */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_setSavingAllowed00
static int tolua_Luainterface_DawnInterface_setSavingAllowed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isboolean(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		bool savingAllowed = ((bool)tolua_toboolean(tolua_S, 1, 0));
		{
			DawnInterface::setSavingAllowed(savingAllowed);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSavingAllowed'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::restoreCharacterReference */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_restoreCharacterReference00
static int tolua_Luainterface_DawnInterface_restoreCharacterReference00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string zoneName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		int posInArray = ((int)tolua_tonumber(tolua_S, 2, 0));
		{
			Character* tolua_ret = (Character*)DawnInterface::restoreCharacterReference(zoneName, posInArray);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "Character");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'restoreCharacterReference'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::restoreInteractionPointReference */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_restoreInteractionPointReference00
static int tolua_Luainterface_DawnInterface_restoreInteractionPointReference00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string zoneName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		int posInArray = ((int)tolua_tonumber(tolua_S, 2, 0));
		{
			InteractionPoint* tolua_ret = (InteractionPoint*)DawnInterface::restoreInteractionPointReference(zoneName, posInArray);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "InteractionPoint");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'restoreInteractionPointReference'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::restoreInteractionRegionReference */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_restoreInteractionRegionReference00
static int tolua_Luainterface_DawnInterface_restoreInteractionRegionReference00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string zoneName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		int posInArray = ((int)tolua_tonumber(tolua_S, 2, 0));
		{
			InteractionRegion* tolua_ret = (InteractionRegion*)DawnInterface::restoreInteractionRegionReference(zoneName, posInArray);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "InteractionRegion");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'restoreInteractionRegionReference'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DawnInterface::restoreEventHandlerReference */
#ifndef TOLUA_DISABLE_tolua_Luainterface_DawnInterface_restoreEventHandlerReference00
static int tolua_Luainterface_DawnInterface_restoreEventHandlerReference00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_iscppstring(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string zoneName = ((std::string)  tolua_tocppstring(tolua_S, 1, 0));
		int posInArray = ((int)tolua_tonumber(tolua_S, 2, 0));
		{
			CallIndirection* tolua_ret = (CallIndirection*)DawnInterface::restoreEventHandlerReference(zoneName, posInArray);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "CallIndirection");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'restoreEventHandlerReference'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EditorInterface::getTileSet */
#ifndef TOLUA_DISABLE_tolua_Luainterface_EditorInterface_getTileSet00
static int tolua_Luainterface_EditorInterface_getTileSet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnumber(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Enums::TileClassificationType tileType = ((Enums::TileClassificationType) (int)  tolua_tonumber(tolua_S, 1, 0));
		{
			TileSet& tolua_ret = (TileSet&)EditorInterface::getTileSet(tileType);
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "TileSet");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'getTileSet'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EditorInterface::addGroundTile */
#ifndef TOLUA_DISABLE_tolua_Luainterface_EditorInterface_addGroundTile00
static int tolua_Luainterface_EditorInterface_addGroundTile00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnumber(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		int posX = ((int)tolua_tonumber(tolua_S, 1, 0));
		int posY = ((int)tolua_tonumber(tolua_S, 2, 0));
		int tile = ((int)tolua_tonumber(tolua_S, 3, 0));
		{
			EditorInterface::addGroundTile(posX, posY, tile);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addGroundTile'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EditorInterface::addEnvironment */
#ifndef TOLUA_DISABLE_tolua_Luainterface_EditorInterface_addEnvironment00
static int tolua_Luainterface_EditorInterface_addEnvironment00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnumber(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 5, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		int posX = ((int)tolua_tonumber(tolua_S, 1, 0));
		int posY = ((int)tolua_tonumber(tolua_S, 2, 0));
		int posZ = ((int)tolua_tonumber(tolua_S, 3, 0));
		int tile = ((int)tolua_tonumber(tolua_S, 4, 0));
		{
			EditorInterface::addEnvironment(posX, posY, posZ, tile);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addEnvironment'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EditorInterface::adjustLastRGBA */
#ifndef TOLUA_DISABLE_tolua_Luainterface_EditorInterface_adjustLastRGBA00
static int tolua_Luainterface_EditorInterface_adjustLastRGBA00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnumber(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 5, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		float red = ((float)tolua_tonumber(tolua_S, 1, 0));
		float green = ((float)tolua_tonumber(tolua_S, 2, 0));
		float blue = ((float)tolua_tonumber(tolua_S, 3, 0));
		float alpha = ((float)tolua_tonumber(tolua_S, 4, 0));
		{
			EditorInterface::adjustLastRGBA(red, green, blue, alpha);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'adjustLastRGBA'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EditorInterface::adjustLastSize */
#ifndef TOLUA_DISABLE_tolua_Luainterface_EditorInterface_adjustLastSize00
static int tolua_Luainterface_EditorInterface_adjustLastSize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnumber(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		float width = ((float)tolua_tonumber(tolua_S, 1, 0));
		float height = ((float)tolua_tonumber(tolua_S, 2, 0));
		{
			EditorInterface::adjustLastSize(width, height);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'adjustLastSize'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EditorInterface::addCollisionRect */
#ifndef TOLUA_DISABLE_tolua_Luainterface_EditorInterface_addCollisionRect00
static int tolua_Luainterface_EditorInterface_addCollisionRect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isnumber(tolua_S, 1, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 5, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		int lrx = ((int)tolua_tonumber(tolua_S, 1, 0));
		int lry = ((int)tolua_tonumber(tolua_S, 2, 0));
		int width = ((int)tolua_tonumber(tolua_S, 3, 0));
		int height = ((int)tolua_tonumber(tolua_S, 4, 0));
		{
			EditorInterface::addCollisionRect(lrx, lry, width, height);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addCollisionRect'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addEquivalentTile of class  AdjacencyEquivalenceClass */
#ifndef TOLUA_DISABLE_tolua_Luainterface_AdjacencyEquivalenceClass_addEquivalentTile00
static int tolua_Luainterface_AdjacencyEquivalenceClass_addEquivalentTile00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "AdjacencyEquivalenceClass", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 5, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		AdjacencyEquivalenceClass* self = (AdjacencyEquivalenceClass*)tolua_tousertype(tolua_S, 1, 0);
		int tile = ((int)tolua_tonumber(tolua_S, 2, 0));
		int offsetX = ((int)tolua_tonumber(tolua_S, 3, 0));
		int offsetY = ((int)tolua_tonumber(tolua_S, 4, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addEquivalentTile'", NULL);
#endif
		{
			self->addEquivalentTile(tile, offsetX, offsetY);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addEquivalentTile'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addTile of class  TileSet */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TileSet_addTile00
static int tolua_Luainterface_TileSet_addTile00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TileSet", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TileSet* self = (TileSet*)tolua_tousertype(tolua_S, 1, 0);
		std::string filename = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addTile'", NULL);
#endif
		{
			unsigned int tolua_ret = (unsigned int)self->addTile(filename);
			tolua_pushnumber(tolua_S, (lua_Number)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addTile'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addTileWithCollisionBox of class  TileSet */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TileSet_addTileWithCollisionBox00
static int tolua_Luainterface_TileSet_addTileWithCollisionBox00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TileSet", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 6, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 7, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TileSet* self = (TileSet*)tolua_tousertype(tolua_S, 1, 0);
		std::string filename = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
		int cbx = ((int)tolua_tonumber(tolua_S, 3, 0));
		int cby = ((int)tolua_tonumber(tolua_S, 4, 0));
		int cbw = ((int)tolua_tonumber(tolua_S, 5, 0));
		int cbh = ((int)tolua_tonumber(tolua_S, 6, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addTileWithCollisionBox'", NULL);
#endif
		{
			unsigned int tolua_ret = (unsigned int)self->addTileWithCollisionBox(filename, cbx, cby, cbw, cbh);
			tolua_pushnumber(tolua_S, (lua_Number)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addTileWithCollisionBox'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addAdjacency of class  TileSet */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TileSet_addAdjacency00
static int tolua_Luainterface_TileSet_addAdjacency00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TileSet", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 6, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 7, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TileSet* self = (TileSet*)tolua_tousertype(tolua_S, 1, 0);
		unsigned int tile1 = ((unsigned int)tolua_tonumber(tolua_S, 2, 0));
		Enums::AdjacencyType adjacencyType = ((Enums::AdjacencyType) (int)  tolua_tonumber(tolua_S, 3, 0));
		unsigned int tile2 = ((unsigned int)tolua_tonumber(tolua_S, 4, 0));
		int offsetX = ((int)tolua_tonumber(tolua_S, 5, 0));
		int offsetY = ((int)tolua_tonumber(tolua_S, 6, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addAdjacency'", NULL);
#endif
		{
			self->addAdjacency(tile1, adjacencyType, tile2, offsetX, offsetY);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addAdjacency'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: createAdjacencyEquivalenceClass of class  TileSet */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TileSet_createAdjacencyEquivalenceClass00
static int tolua_Luainterface_TileSet_createAdjacencyEquivalenceClass00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TileSet", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TileSet* self = (TileSet*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'createAdjacencyEquivalenceClass'", NULL);
#endif
		{
			AdjacencyEquivalenceClass* tolua_ret = (AdjacencyEquivalenceClass*)self->createAdjacencyEquivalenceClass();
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "AdjacencyEquivalenceClass");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'createAdjacencyEquivalenceClass'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addEquivalenceAdjacency of class  TileSet */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TileSet_addEquivalenceAdjacency00
static int tolua_Luainterface_TileSet_addEquivalenceAdjacency00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TileSet", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "AdjacencyEquivalenceClass", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 4, "AdjacencyEquivalenceClass", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 6, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 7, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TileSet* self = (TileSet*)tolua_tousertype(tolua_S, 1, 0);
		AdjacencyEquivalenceClass* class1 = ((AdjacencyEquivalenceClass*)tolua_tousertype(tolua_S, 2, 0));
		Enums::AdjacencyType adjacencyType = ((Enums::AdjacencyType) (int)  tolua_tonumber(tolua_S, 3, 0));
		AdjacencyEquivalenceClass* class2 = ((AdjacencyEquivalenceClass*)tolua_tousertype(tolua_S, 4, 0));
		int offsetX = ((int)tolua_tonumber(tolua_S, 5, 0));
		int offsetY = ((int)tolua_tonumber(tolua_S, 6, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addEquivalenceAdjacency'", NULL);
#endif
		{
			self->addEquivalenceAdjacency(class1, adjacencyType, class2, offsetX, offsetY);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addEquivalenceAdjacency'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: printTileSet of class  TileSet */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TileSet_printTileSet00
static int tolua_Luainterface_TileSet_printTileSet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "const TileSet", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		const TileSet* self = (const TileSet*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'printTileSet'", NULL);
#endif
		{
			self->printTileSet();
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'printTileSet'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addMoveTexture of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_addMoveTexture00
static int tolua_Luainterface_CharacterType_addMoveTexture00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 5, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 6, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 7, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 8, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 9, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 10, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 11, 1, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 12, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		Enums::ActivityType activity = ((Enums::ActivityType) (int)  tolua_tonumber(tolua_S, 2, 0));
		Enums::Direction direction = ((Enums::Direction) (int)  tolua_tonumber(tolua_S, 3, 0));
		int index = ((int)tolua_tonumber(tolua_S, 4, 0));
		std::string filename = ((std::string)  tolua_tocppstring(tolua_S, 5, 0));
		unsigned int maxWidth = ((unsigned int)tolua_tonumber(tolua_S, 6, 0));
		unsigned int maxHeight = ((unsigned int)tolua_tonumber(tolua_S, 7, 0));
		int paddingLeft = ((int)tolua_tonumber(tolua_S, 8, 0));
		int paddingRight = ((int)tolua_tonumber(tolua_S, 9, 0));
		int paddingTop = ((int)tolua_tonumber(tolua_S, 10, 0));
		int paddingBottom = ((int)tolua_tonumber(tolua_S, 11, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addMoveTexture'", NULL);
#endif
		{
			self->addMoveTexture(activity, direction, index, filename, maxWidth, maxHeight, paddingLeft, paddingRight, paddingTop, paddingBottom);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addMoveTexture'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: calcNumMoveTexturesPerDirection of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_calcNumMoveTexturesPerDirection00
static int tolua_Luainterface_CharacterType_calcNumMoveTexturesPerDirection00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'calcNumMoveTexturesPerDirection'", NULL);
#endif
		{
			self->calcNumMoveTexturesPerDirection();
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'calcNumMoveTexturesPerDirection'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: baseOnType of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_baseOnType00
static int tolua_Luainterface_CharacterType_baseOnType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		std::string name = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'baseOnType'", NULL);
#endif
		{
			self->baseOnType(name);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'baseOnType'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: inscribeSpellInSpellbook of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_inscribeSpellInSpellbook00
static int tolua_Luainterface_CharacterType_inscribeSpellInSpellbook00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		SpellActionBase* spell = ((SpellActionBase*)tolua_tousertype(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'inscribeSpellInSpellbook'", NULL);
#endif
		{
			self->inscribeSpellInSpellbook(spell);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'inscribeSpellInSpellbook'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addItemToLootTable of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_addItemToLootTable00
static int tolua_Luainterface_CharacterType_addItemToLootTable00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "Item", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		Item* item = ((Item*)tolua_tousertype(tolua_S, 2, 0));
		double dropChance = ((double)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addItemToLootTable'", NULL);
#endif
		{
			self->addItemToLootTable(item, dropChance);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addItemToLootTable'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setCoinDrop of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setCoinDrop00
static int tolua_Luainterface_CharacterType_setCoinDrop00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 5, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		unsigned int minCoinDrop = ((unsigned int)tolua_tonumber(tolua_S, 2, 0));
		unsigned int maxCoinDrop = ((unsigned int)tolua_tonumber(tolua_S, 3, 0));
		double dropChance = ((double)tolua_tonumber(tolua_S, 4, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setCoinDrop'", NULL);
#endif
		{
			self->setCoinDrop(minCoinDrop, maxCoinDrop, dropChance);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCoinDrop'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setStrength of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setStrength00
static int tolua_Luainterface_CharacterType_setStrength00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newStrength = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setStrength'", NULL);
#endif
		{
			self->setStrength(newStrength);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setStrength'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDexterity of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setDexterity00
static int tolua_Luainterface_CharacterType_setDexterity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newDexterity = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setDexterity'", NULL);
#endif
		{
			self->setDexterity(newDexterity);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setDexterity'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setVitality of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setVitality00
static int tolua_Luainterface_CharacterType_setVitality00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newVitality = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setVitality'", NULL);
#endif
		{
			self->setVitality(newVitality);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setVitality'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setIntellect of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setIntellect00
static int tolua_Luainterface_CharacterType_setIntellect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newIntellect = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setIntellect'", NULL);
#endif
		{
			self->setIntellect(newIntellect);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setIntellect'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setWisdom of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setWisdom00
static int tolua_Luainterface_CharacterType_setWisdom00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newWisdom = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setWisdom'", NULL);
#endif
		{
			self->setWisdom(newWisdom);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setWisdom'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaxHealth of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setMaxHealth00
static int tolua_Luainterface_CharacterType_setMaxHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newMaxHealth = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMaxHealth'", NULL);
#endif
		{
			self->setMaxHealth(newMaxHealth);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMaxHealth'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaxMana of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setMaxMana00
static int tolua_Luainterface_CharacterType_setMaxMana00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newMaxMana = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMaxMana'", NULL);
#endif
		{
			self->setMaxMana(newMaxMana);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMaxMana'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaxFatigue of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setMaxFatigue00
static int tolua_Luainterface_CharacterType_setMaxFatigue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newMaxFatigue = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMaxFatigue'", NULL);
#endif
		{
			self->setMaxFatigue(newMaxFatigue);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMaxFatigue'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setHealthRegen of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setHealthRegen00
static int tolua_Luainterface_CharacterType_setHealthRegen00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newHealthRegen = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setHealthRegen'", NULL);
#endif
		{
			self->setHealthRegen(newHealthRegen);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setHealthRegen'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setManaRegen of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setManaRegen00
static int tolua_Luainterface_CharacterType_setManaRegen00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newManaRegen = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setManaRegen'", NULL);
#endif
		{
			self->setManaRegen(newManaRegen);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setManaRegen'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setFatigueRegen of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setFatigueRegen00
static int tolua_Luainterface_CharacterType_setFatigueRegen00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newFatigueRegen = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setFatigueRegen'", NULL);
#endif
		{
			self->setFatigueRegen(newFatigueRegen);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setFatigueRegen'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMinDamage of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setMinDamage00
static int tolua_Luainterface_CharacterType_setMinDamage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newMinDamage = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMinDamage'", NULL);
#endif
		{
			self->setMinDamage(newMinDamage);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMinDamage'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaxDamage of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setMaxDamage00
static int tolua_Luainterface_CharacterType_setMaxDamage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newMaxDamage = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMaxDamage'", NULL);
#endif
		{
			self->setMaxDamage(newMaxDamage);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMaxDamage'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setArmor of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setArmor00
static int tolua_Luainterface_CharacterType_setArmor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newArmor = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setArmor'", NULL);
#endif
		{
			self->setArmor(newArmor);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setArmor'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDamageModifierPoints of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setDamageModifierPoints00
static int tolua_Luainterface_CharacterType_setDamageModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newDamageModifierPoints = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setDamageModifierPoints'", NULL);
#endif
		{
			self->setDamageModifierPoints(newDamageModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setDamageModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setHitModifierPoints of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setHitModifierPoints00
static int tolua_Luainterface_CharacterType_setHitModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newHitModifierPoints = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setHitModifierPoints'", NULL);
#endif
		{
			self->setHitModifierPoints(newHitModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setHitModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setEvadeModifierPoints of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setEvadeModifierPoints00
static int tolua_Luainterface_CharacterType_setEvadeModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newEvadeModifierPoints = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setEvadeModifierPoints'", NULL);
#endif
		{
			self->setEvadeModifierPoints(newEvadeModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setEvadeModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setName of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setName00
static int tolua_Luainterface_CharacterType_setName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		std::string newName = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setName'", NULL);
#endif
		{
			self->setName(newName);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setName'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setWanderRadius of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setWanderRadius00
static int tolua_Luainterface_CharacterType_setWanderRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newWanderRadius = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setWanderRadius'", NULL);
#endif
		{
			self->setWanderRadius(newWanderRadius);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setWanderRadius'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setLevel of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setLevel00
static int tolua_Luainterface_CharacterType_setLevel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int newLevel = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setLevel'", NULL);
#endif
		{
			self->setLevel(newLevel);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setLevel'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setClass of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setClass00
static int tolua_Luainterface_CharacterType_setClass00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		Enums::CharacterClass characterClass = ((Enums::CharacterClass) (int)  tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setClass'", NULL);
#endif
		{
			self->setClass(characterClass);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setClass'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setExperienceValue of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_setExperienceValue00
static int tolua_Luainterface_CharacterType_setExperienceValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int experienceValue = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setExperienceValue'", NULL);
#endif
		{
			self->setExperienceValue(experienceValue);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setExperienceValue'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: modifyStrength of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_modifyStrength00
static int tolua_Luainterface_CharacterType_modifyStrength00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int strengthModifier = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'modifyStrength'", NULL);
#endif
		{
			self->modifyStrength(strengthModifier);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'modifyStrength'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: modifyDexterity of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_modifyDexterity00
static int tolua_Luainterface_CharacterType_modifyDexterity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int dexterityModifier = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'modifyDexterity'", NULL);
#endif
		{
			self->modifyDexterity(dexterityModifier);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'modifyDexterity'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: modifyVitality of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_modifyVitality00
static int tolua_Luainterface_CharacterType_modifyVitality00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int vitalityModifier = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'modifyVitality'", NULL);
#endif
		{
			self->modifyVitality(vitalityModifier);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'modifyVitality'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: modifyIntellect of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_modifyIntellect00
static int tolua_Luainterface_CharacterType_modifyIntellect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int intellectModifier = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'modifyIntellect'", NULL);
#endif
		{
			self->modifyIntellect(intellectModifier);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'modifyIntellect'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: modifyWisdom of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_modifyWisdom00
static int tolua_Luainterface_CharacterType_modifyWisdom00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int wisdomModifier = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'modifyWisdom'", NULL);
#endif
		{
			self->modifyWisdom(wisdomModifier);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'modifyWisdom'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: modifyMaxHealth of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_modifyMaxHealth00
static int tolua_Luainterface_CharacterType_modifyMaxHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int maxHealthModifier = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'modifyMaxHealth'", NULL);
#endif
		{
			self->modifyMaxHealth(maxHealthModifier);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'modifyMaxHealth'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: modifyMaxFatigue of class  CharacterType */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CharacterType_modifyMaxFatigue00
static int tolua_Luainterface_CharacterType_modifyMaxFatigue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CharacterType", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CharacterType* self = (CharacterType*)tolua_tousertype(tolua_S, 1, 0);
		int maxFatigueModifier = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'modifyMaxFatigue'", NULL);
#endif
		{
			self->modifyMaxFatigue(maxFatigueModifier);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'modifyMaxFatigue'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Get of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_Get00
static int tolua_Luainterface_Player_Get00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertable(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			Player& tolua_ret = (Player&)Player::Get();
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "Player");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'Get'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setBoundingBox of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setBoundingBox00
static int tolua_Luainterface_Player_setBoundingBox00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 6, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		int bbx = ((int)tolua_tonumber(tolua_S, 2, 0));
		int bby = ((int)tolua_tonumber(tolua_S, 3, 0));
		int bbw = ((int)tolua_tonumber(tolua_S, 4, 0));
		int bbh = ((int)tolua_tonumber(tolua_S, 5, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setBoundingBox'", NULL);
#endif
		{
			self->setBoundingBox(bbx, bby, bbw, bbh);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setBoundingBox'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setClass of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setClass00
static int tolua_Luainterface_Player_setClass00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		Enums::CharacterClass characterClass = ((Enums::CharacterClass) (int)  tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setClass'", NULL);
#endif
		{
			self->setClass(characterClass);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setClass'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setName of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setName00
static int tolua_Luainterface_Player_setName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		std::string name = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setName'", NULL);
#endif
		{
			self->setName(name);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setName'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setStrength of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setStrength00
static int tolua_Luainterface_Player_setStrength00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newStrength = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setStrength'", NULL);
#endif
		{
			self->setStrength(newStrength);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setStrength'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDexterity of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setDexterity00
static int tolua_Luainterface_Player_setDexterity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newDexterity = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setDexterity'", NULL);
#endif
		{
			self->setDexterity(newDexterity);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setDexterity'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setVitality of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setVitality00
static int tolua_Luainterface_Player_setVitality00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newVitality = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setVitality'", NULL);
#endif
		{
			self->setVitality(newVitality);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setVitality'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setIntellect of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setIntellect00
static int tolua_Luainterface_Player_setIntellect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newIntellect = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setIntellect'", NULL);
#endif
		{
			self->setIntellect(newIntellect);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setIntellect'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setWisdom of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setWisdom00
static int tolua_Luainterface_Player_setWisdom00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newWisdom = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setWisdom'", NULL);
#endif
		{
			self->setWisdom(newWisdom);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setWisdom'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaxHealth of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setMaxHealth00
static int tolua_Luainterface_Player_setMaxHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newMaxHealth = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMaxHealth'", NULL);
#endif
		{
			self->setMaxHealth(newMaxHealth);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMaxHealth'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaxMana of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setMaxMana00
static int tolua_Luainterface_Player_setMaxMana00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newMaxMana = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMaxMana'", NULL);
#endif
		{
			self->setMaxMana(newMaxMana);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMaxMana'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaxFatigue of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setMaxFatigue00
static int tolua_Luainterface_Player_setMaxFatigue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newMaxFatigue = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMaxFatigue'", NULL);
#endif
		{
			self->setMaxFatigue(newMaxFatigue);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMaxFatigue'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setHealthRegen of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setHealthRegen00
static int tolua_Luainterface_Player_setHealthRegen00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newHealthRegen = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setHealthRegen'", NULL);
#endif
		{
			self->setHealthRegen(newHealthRegen);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setHealthRegen'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setManaRegen of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setManaRegen00
static int tolua_Luainterface_Player_setManaRegen00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newManaRegen = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setManaRegen'", NULL);
#endif
		{
			self->setManaRegen(newManaRegen);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setManaRegen'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setFatigueRegen of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setFatigueRegen00
static int tolua_Luainterface_Player_setFatigueRegen00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newFatigueRegen = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setFatigueRegen'", NULL);
#endif
		{
			self->setFatigueRegen(newFatigueRegen);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setFatigueRegen'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMinDamage of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setMinDamage00
static int tolua_Luainterface_Player_setMinDamage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newMinDamage = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMinDamage'", NULL);
#endif
		{
			self->setMinDamage(newMinDamage);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMinDamage'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaxDamage of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setMaxDamage00
static int tolua_Luainterface_Player_setMaxDamage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newMaxDamage = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMaxDamage'", NULL);
#endif
		{
			self->setMaxDamage(newMaxDamage);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMaxDamage'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setArmor of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setArmor00
static int tolua_Luainterface_Player_setArmor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newArmor = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setArmor'", NULL);
#endif
		{
			self->setArmor(newArmor);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setArmor'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDamageModifierPoints of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setDamageModifierPoints00
static int tolua_Luainterface_Player_setDamageModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newDamageModifierPoints = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setDamageModifierPoints'", NULL);
#endif
		{
			self->setDamageModifierPoints(newDamageModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setDamageModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setHitModifierPoints of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setHitModifierPoints00
static int tolua_Luainterface_Player_setHitModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newHitModifierPoints = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setHitModifierPoints'", NULL);
#endif
		{
			self->setHitModifierPoints(newHitModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setHitModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setEvadeModifierPoints of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setEvadeModifierPoints00
static int tolua_Luainterface_Player_setEvadeModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newEvadeModifierPoints = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setEvadeModifierPoints'", NULL);
#endif
		{
			self->setEvadeModifierPoints(newEvadeModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setEvadeModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setParryModifierPoints of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setParryModifierPoints00
static int tolua_Luainterface_Player_setParryModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newParryModifierPoints = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setParryModifierPoints'", NULL);
#endif
		{
			self->setParryModifierPoints(newParryModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setParryModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setBlockModifierPoints of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setBlockModifierPoints00
static int tolua_Luainterface_Player_setBlockModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newBlockModifierPoints = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setBlockModifierPoints'", NULL);
#endif
		{
			self->setBlockModifierPoints(newBlockModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setBlockModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMeleeCriticalModifierPoints of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setMeleeCriticalModifierPoints00
static int tolua_Luainterface_Player_setMeleeCriticalModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newMeleeCriticalModifierPoints = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMeleeCriticalModifierPoints'", NULL);
#endif
		{
			self->setMeleeCriticalModifierPoints(newMeleeCriticalModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMeleeCriticalModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setResistElementModifierPoints of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setResistElementModifierPoints00
static int tolua_Luainterface_Player_setResistElementModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		Enums::ElementType elementType = ((Enums::ElementType) (int)  tolua_tonumber(tolua_S, 2, 0));
		unsigned short newResistElementModifierPoints = ((unsigned short)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setResistElementModifierPoints'", NULL);
#endif
		{
			self->setResistElementModifierPoints(elementType, newResistElementModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setResistElementModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setResistAllModifierPoints of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setResistAllModifierPoints00
static int tolua_Luainterface_Player_setResistAllModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newResistAllModifierPoints = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setResistAllModifierPoints'", NULL);
#endif
		{
			self->setResistAllModifierPoints(newResistAllModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setResistAllModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSpellEffectElementModifierPoints of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setSpellEffectElementModifierPoints00
static int tolua_Luainterface_Player_setSpellEffectElementModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		Enums::ElementType elementType = ((Enums::ElementType) (int)  tolua_tonumber(tolua_S, 2, 0));
		unsigned short newSpellEffectElementModifierPoints = ((unsigned short)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setSpellEffectElementModifierPoints'", NULL);
#endif
		{
			self->setSpellEffectElementModifierPoints(elementType, newSpellEffectElementModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSpellEffectElementModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSpellEffectAllModifierPoints of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setSpellEffectAllModifierPoints00
static int tolua_Luainterface_Player_setSpellEffectAllModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newSpellEffectAllModifierPoints = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setSpellEffectAllModifierPoints'", NULL);
#endif
		{
			self->setSpellEffectAllModifierPoints(newSpellEffectAllModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSpellEffectAllModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSpellCriticalModifierPoints of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setSpellCriticalModifierPoints00
static int tolua_Luainterface_Player_setSpellCriticalModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newSpellCriticalModifierPoints = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setSpellCriticalModifierPoints'", NULL);
#endif
		{
			self->setSpellCriticalModifierPoints(newSpellCriticalModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSpellCriticalModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setLevel of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setLevel00
static int tolua_Luainterface_Player_setLevel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newLevel = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setLevel'", NULL);
#endif
		{
			self->setLevel(newLevel);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setLevel'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setExperience of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setExperience00
static int tolua_Luainterface_Player_setExperience00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned long experience = ((unsigned long)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setExperience'", NULL);
#endif
		{
			self->setExperience(experience);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setExperience'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: giveCoins of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_giveCoins00
static int tolua_Luainterface_Player_giveCoins00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned int amountOfCoins = ((unsigned int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'giveCoins'", NULL);
#endif
		{
			self->giveCoins(amountOfCoins);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'giveCoins'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setCoins of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setCoins00
static int tolua_Luainterface_Player_setCoins00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned int amountOfCoins = ((unsigned int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setCoins'", NULL);
#endif
		{
			self->setCoins(amountOfCoins);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCoins'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setPosition of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setPosition00
static int tolua_Luainterface_Player_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		int xpos = ((int)tolua_tonumber(tolua_S, 2, 0));
		int ypos = ((int)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setPosition'", NULL);
#endif
		{
			self->setPosition(xpos, ypos);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setPosition'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setCharacterType of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setCharacterType00
static int tolua_Luainterface_Player_setCharacterType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		std::string characterType = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setCharacterType'", NULL);
#endif
		{
			self->setCharacterType(characterType);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCharacterType'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_init00
static int tolua_Luainterface_Player_init00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'init'", NULL);
#endif
		{
			self->init();
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'init'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getClass of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_getClass00
static int tolua_Luainterface_Player_getClass00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'getClass'", NULL);
#endif
		{
			Enums::CharacterClass tolua_ret = (Enums::CharacterClass)  self->getClass();
			tolua_pushnumber(tolua_S, (lua_Number)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'getClass'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getSaveText of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_getSaveText00
static int tolua_Luainterface_Player_getSaveText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'getSaveText'", NULL);
#endif
		{
			std::string tolua_ret = (std::string)  self->getSaveText();
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'getSaveText'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setCurrentHealth of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setCurrentHealth00
static int tolua_Luainterface_Player_setCurrentHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short currentHealth = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setCurrentHealth'", NULL);
#endif
		{
			self->setCurrentHealth(currentHealth);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCurrentHealth'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setCurrentMana of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setCurrentMana00
static int tolua_Luainterface_Player_setCurrentMana00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short currentMana = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setCurrentMana'", NULL);
#endif
		{
			self->setCurrentMana(currentMana);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCurrentMana'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setCurrentFatigue of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_setCurrentFatigue00
static int tolua_Luainterface_Player_setCurrentFatigue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Player", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Player* self = (Player*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short currentFatigue = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setCurrentFatigue'", NULL);
#endif
		{
			self->setCurrentFatigue(currentFatigue);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCurrentFatigue'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setAttitude of class  Npc */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Npc_setAttitude00
static int tolua_Luainterface_Npc_setAttitude00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Npc", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Npc* self = (Npc*)tolua_tousertype(tolua_S, 1, 0);
		Enums::Attitude attitude = ((Enums::Attitude) (int)  tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setAttitude'", NULL);
#endif
		{
			self->setAttitude(attitude);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setAttitude'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addOnDieEventHandler of class  Npc */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Npc_addOnDieEventHandler00
static int tolua_Luainterface_Npc_addOnDieEventHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Npc", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "CallIndirection", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Npc* self = (Npc*)tolua_tousertype(tolua_S, 1, 0);
		CallIndirection* eventHandler = ((CallIndirection*)tolua_tousertype(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addOnDieEventHandler'", NULL);
#endif
		{
			self->addOnDieEventHandler(eventHandler);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addOnDieEventHandler'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: removeOnDieEventHandler of class  Npc */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Npc_removeOnDieEventHandler00
static int tolua_Luainterface_Npc_removeOnDieEventHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Npc", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "CallIndirection", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Npc* self = (Npc*)tolua_tousertype(tolua_S, 1, 0);
		CallIndirection* eventHandler = ((CallIndirection*)tolua_tousertype(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'removeOnDieEventHandler'", NULL);
#endif
		{
			self->removeOnDieEventHandler(eventHandler);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'removeOnDieEventHandler'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setWanderRadius of class  Npc */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Npc_setWanderRadius00
static int tolua_Luainterface_Npc_setWanderRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Npc", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Npc* self = (Npc*)tolua_tousertype(tolua_S, 1, 0);
		unsigned short newWanderRadius = ((unsigned short)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setWanderRadius'", NULL);
#endif
		{
			self->setWanderRadius(newWanderRadius);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setWanderRadius'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setPosition of class  InteractionPoint */
#ifndef TOLUA_DISABLE_tolua_Luainterface_InteractionPoint_setPosition00
static int tolua_Luainterface_InteractionPoint_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "InteractionPoint", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 6, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		InteractionPoint* self = (InteractionPoint*)tolua_tousertype(tolua_S, 1, 0);
		int posX = ((int)tolua_tonumber(tolua_S, 2, 0));
		int posY = ((int)tolua_tonumber(tolua_S, 3, 0));
		int width = ((int)tolua_tonumber(tolua_S, 4, 0));
		int height = ((int)tolua_tonumber(tolua_S, 5, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setPosition'", NULL);
#endif
		{
			self->setPosition(posX, posY, width, height);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setPosition'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setInteractionType of class  InteractionPoint */
#ifndef TOLUA_DISABLE_tolua_Luainterface_InteractionPoint_setInteractionType00
static int tolua_Luainterface_InteractionPoint_setInteractionType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "InteractionPoint", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		InteractionPoint* self = (InteractionPoint*)tolua_tousertype(tolua_S, 1, 0);
		Enums::InteractionType interactionType = ((Enums::InteractionType) (int)  tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setInteractionType'", NULL);
#endif
		{
			self->setInteractionType(interactionType);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setInteractionType'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setBackgroundTexture of class  InteractionPoint */
#ifndef TOLUA_DISABLE_tolua_Luainterface_InteractionPoint_setBackgroundTexture00
static int tolua_Luainterface_InteractionPoint_setBackgroundTexture00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "InteractionPoint", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isboolean(tolua_S, 3, 1, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		InteractionPoint* self = (InteractionPoint*)tolua_tousertype(tolua_S, 1, 0);
		std::string texturename = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
		bool transparent = ((bool)tolua_toboolean(tolua_S, 3, false));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setBackgroundTexture'", NULL);
#endif
		{
			self->setBackgroundTexture(texturename, transparent);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setBackgroundTexture'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setInteractionCode of class  InteractionPoint */
#ifndef TOLUA_DISABLE_tolua_Luainterface_InteractionPoint_setInteractionCode00
static int tolua_Luainterface_InteractionPoint_setInteractionCode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "InteractionPoint", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		InteractionPoint* self = (InteractionPoint*)tolua_tousertype(tolua_S, 1, 0);
		std::string interactionCode = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setInteractionCode'", NULL);
#endif
		{
			self->setInteractionCode(interactionCode);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setInteractionCode'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setPosition of class  InteractionRegion */
#ifndef TOLUA_DISABLE_tolua_Luainterface_InteractionRegion_setPosition00
static int tolua_Luainterface_InteractionRegion_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "InteractionRegion", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 6, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		InteractionRegion* self = (InteractionRegion*)tolua_tousertype(tolua_S, 1, 0);
		int left = ((int)tolua_tonumber(tolua_S, 2, 0));
		int bottom = ((int)tolua_tonumber(tolua_S, 3, 0));
		int width = ((int)tolua_tonumber(tolua_S, 4, 0));
		int height = ((int)tolua_tonumber(tolua_S, 5, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setPosition'", NULL);
#endif
		{
			self->setPosition(left, bottom, width, height);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setPosition'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setOnEnterText of class  InteractionRegion */
#ifndef TOLUA_DISABLE_tolua_Luainterface_InteractionRegion_setOnEnterText00
static int tolua_Luainterface_InteractionRegion_setOnEnterText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "InteractionRegion", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		InteractionRegion* self = (InteractionRegion*)tolua_tousertype(tolua_S, 1, 0);
		std::string enterCode = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setOnEnterText'", NULL);
#endif
		{
			self->setOnEnterText(enterCode);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setOnEnterText'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setOnLeaveText of class  InteractionRegion */
#ifndef TOLUA_DISABLE_tolua_Luainterface_InteractionRegion_setOnLeaveText00
static int tolua_Luainterface_InteractionRegion_setOnLeaveText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "InteractionRegion", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		InteractionRegion* self = (InteractionRegion*)tolua_tousertype(tolua_S, 1, 0);
		std::string leaveCode = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setOnLeaveText'", NULL);
#endif
		{
			self->setOnLeaveText(leaveCode);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setOnLeaveText'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Get of class  TextureAtlasCreator */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextureAtlasCreator_Get00
static int tolua_Luainterface_TextureAtlasCreator_Get00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertable(tolua_S, 1, "TextureAtlasCreator", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			TextureAtlasCreator& tolua_ret = (TextureAtlasCreator&)TextureAtlasCreator::Get();
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "TextureAtlasCreator");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'Get'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init of class  TextureAtlasCreator */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextureAtlasCreator_init00
static int tolua_Luainterface_TextureAtlasCreator_init00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TextureAtlasCreator", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 1, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 5, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TextureAtlasCreator* self = (TextureAtlasCreator*)tolua_tousertype(tolua_S, 1, 0);
		std::string name = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
		unsigned int width = ((unsigned int)tolua_tonumber(tolua_S, 3, 1024u));
		unsigned int height = ((unsigned int)tolua_tonumber(tolua_S, 4, 1024u));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'init'", NULL);
#endif
		{
			self->init(name, width, height);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'init'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addFrame of class  TextureAtlasCreator */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextureAtlasCreator_addFrame00
static int tolua_Luainterface_TextureAtlasCreator_addFrame00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TextureAtlasCreator", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TextureAtlasCreator* self = (TextureAtlasCreator*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addFrame'", NULL);
#endif
		{
			self->addFrame();
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addFrame'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getAtlas of class  TextureAtlasCreator */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextureAtlasCreator_getAtlas00
static int tolua_Luainterface_TextureAtlasCreator_getAtlas00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TextureAtlasCreator", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TextureAtlasCreator* self = (TextureAtlasCreator*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'getAtlas'", NULL);
#endif
		{
			unsigned int tolua_ret = (unsigned int)self->getAtlas();
			tolua_pushnumber(tolua_S, (lua_Number)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'getAtlas'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: resetLine of class  TextureAtlasCreator */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextureAtlasCreator_resetLine00
static int tolua_Luainterface_TextureAtlasCreator_resetLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TextureAtlasCreator", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TextureAtlasCreator* self = (TextureAtlasCreator*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'resetLine'", NULL);
#endif
		{
			self->resetLine();
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'resetLine'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getName of class  TextureAtlasCreator */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextureAtlasCreator_getName00
static int tolua_Luainterface_TextureAtlasCreator_getName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TextureAtlasCreator", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TextureAtlasCreator* self = (TextureAtlasCreator*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'getName'", NULL);
#endif
		{
			std::string tolua_ret = (std::string)  self->getName();
			tolua_pushcppstring(tolua_S, (const char*)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'getName'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetTextureAtlas of class  TextureManager */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextureManager_SetTextureAtlas00
static int tolua_Luainterface_TextureManager_SetTextureAtlas00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertable(tolua_S, 1, "TextureManager", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		std::string name = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
		unsigned int value = ((unsigned int)tolua_tonumber(tolua_S, 3, 0));
		{
			TextureManager::SetTextureAtlas(name, value);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'SetTextureAtlas'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: loadZone of class  Zone */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Zone_loadZone00
static int tolua_Luainterface_Zone_loadZone00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Zone", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Zone* self = (Zone*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'loadZone'", NULL);
#endif
		{
			self->loadZone();
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'loadZone'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setInit of class  Zone */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Zone_setInit00
static int tolua_Luainterface_Zone_setInit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Zone", 0, &tolua_err) ||
		!tolua_isboolean(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Zone* self = (Zone*)tolua_tousertype(tolua_S, 1, 0);
		bool flag = ((bool)tolua_toboolean(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setInit'", NULL);
#endif
		{
			self->setInit(flag);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setInit'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addAdditionalSpellOnTarget of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_addAdditionalSpellOnTarget00
static int tolua_Luainterface_SpellActionBase_addAdditionalSpellOnTarget00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		SpellActionBase* spell = ((SpellActionBase*)tolua_tousertype(tolua_S, 2, 0));
		double chanceToExecute = ((double)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addAdditionalSpellOnTarget'", NULL);
#endif
		{
			self->addAdditionalSpellOnTarget(spell, chanceToExecute);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addAdditionalSpellOnTarget'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addAdditionalSpellOnCreator of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_addAdditionalSpellOnCreator00
static int tolua_Luainterface_SpellActionBase_addAdditionalSpellOnCreator00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		SpellActionBase* spell = ((SpellActionBase*)tolua_tousertype(tolua_S, 2, 0));
		double chanceToExecute = ((double)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addAdditionalSpellOnCreator'", NULL);
#endif
		{
			self->addAdditionalSpellOnCreator(spell, chanceToExecute);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addAdditionalSpellOnCreator'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setRequiredClass of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_setRequiredClass00
static int tolua_Luainterface_SpellActionBase_setRequiredClass00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		Enums::CharacterClass requiredClass = ((Enums::CharacterClass) (int)  tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setRequiredClass'", NULL);
#endif
		{
			self->setRequiredClass(requiredClass);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setRequiredClass'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setRequiredLevel of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_setRequiredLevel00
static int tolua_Luainterface_SpellActionBase_setRequiredLevel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		int requiredLevel = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setRequiredLevel'", NULL);
#endif
		{
			self->setRequiredLevel(requiredLevel);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setRequiredLevel'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addRequiredWeapon of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_addRequiredWeapon00
static int tolua_Luainterface_SpellActionBase_addRequiredWeapon00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		Enums::WeaponType weaponType = ((Enums::WeaponType) (int)  tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addRequiredWeapon'", NULL);
#endif
		{
			self->addRequiredWeapon(weaponType);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addRequiredWeapon'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSoundSpellCasting of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_setSoundSpellCasting00
static int tolua_Luainterface_SpellActionBase_setSoundSpellCasting00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		std::string soundSpellCasting = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setSoundSpellCasting'", NULL);
#endif
		{
			self->setSoundSpellCasting(soundSpellCasting);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSoundSpellCasting'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSoundSpellStart of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_setSoundSpellStart00
static int tolua_Luainterface_SpellActionBase_setSoundSpellStart00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		std::string soundSpellStart = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setSoundSpellStart'", NULL);
#endif
		{
			self->setSoundSpellStart(soundSpellStart);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSoundSpellStart'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSoundSpellHit of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_setSoundSpellHit00
static int tolua_Luainterface_SpellActionBase_setSoundSpellHit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		std::string soundSpellHit = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setSoundSpellHit'", NULL);
#endif
		{
			self->setSoundSpellHit(soundSpellHit);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSoundSpellHit'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setRank of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_setRank00
static int tolua_Luainterface_SpellActionBase_setRank00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		int rank = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setRank'", NULL);
#endif
		{
			self->setRank(rank);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setRank'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setInstant of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_setInstant00
static int tolua_Luainterface_SpellActionBase_setInstant00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isboolean(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		bool instant = ((bool)tolua_toboolean(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setInstant'", NULL);
#endif
		{
			self->setInstant(instant);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setInstant'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setCharacterState of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_setCharacterState00
static int tolua_Luainterface_SpellActionBase_setCharacterState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 1, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		Enums::CharacterStates characterState = ((Enums::CharacterStates) (int)  tolua_tonumber(tolua_S, 2, 0));
		float value = ((float)tolua_tonumber(tolua_S, 3, 1.0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setCharacterState'", NULL);
#endif
		{
			self->setCharacterState(characterState, value);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCharacterState'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSymbolTextureRect of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_setSymbolTextureRect00
static int tolua_Luainterface_SpellActionBase_setSymbolTextureRect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		(tolua_isvaluenil(tolua_S, 2, &tolua_err) || !tolua_isusertype(tolua_S, 2, "TextureRect", 0, &tolua_err)) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		TextureRect* textureRect = ((TextureRect*)tolua_tousertype(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setSymbolTextureRect'", NULL);
#endif
		{
			self->setSymbolTextureRect(*textureRect);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSymbolTextureRect'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setNeedTarget of class  SpellActionBase */
#ifndef TOLUA_DISABLE_tolua_Luainterface_SpellActionBase_setNeedTarget00
static int tolua_Luainterface_SpellActionBase_setNeedTarget00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isboolean(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		SpellActionBase* self = (SpellActionBase*)tolua_tousertype(tolua_S, 1, 0);
		bool needTarget = ((bool)tolua_toboolean(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setNeedTarget'", NULL);
#endif
		{
			self->setNeedTarget(needTarget);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setNeedTarget'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setCastTime of class  ConfigurableSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ConfigurableSpell_setCastTime00
static int tolua_Luainterface_ConfigurableSpell_setCastTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ConfigurableSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ConfigurableSpell* self = (ConfigurableSpell*)tolua_tousertype(tolua_S, 1, 0);
		int newCastTime = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setCastTime'", NULL);
#endif
		{
			self->setCastTime(newCastTime);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCastTime'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setCooldown of class  ConfigurableSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ConfigurableSpell_setCooldown00
static int tolua_Luainterface_ConfigurableSpell_setCooldown00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ConfigurableSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ConfigurableSpell* self = (ConfigurableSpell*)tolua_tousertype(tolua_S, 1, 0);
		int newCooldown = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setCooldown'", NULL);
#endif
		{
			self->setCooldown(newCooldown);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCooldown'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSpellCost of class  ConfigurableSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ConfigurableSpell_setSpellCost00
static int tolua_Luainterface_ConfigurableSpell_setSpellCost00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ConfigurableSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ConfigurableSpell* self = (ConfigurableSpell*)tolua_tousertype(tolua_S, 1, 0);
		int spellCost = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setSpellCost'", NULL);
#endif
		{
			self->setSpellCost(spellCost);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSpellCost'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setName of class  ConfigurableSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ConfigurableSpell_setName00
static int tolua_Luainterface_ConfigurableSpell_setName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ConfigurableSpell", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ConfigurableSpell* self = (ConfigurableSpell*)tolua_tousertype(tolua_S, 1, 0);
		std::string newName = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setName'", NULL);
#endif
		{
			self->setName(newName);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setName'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setInfo of class  ConfigurableSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ConfigurableSpell_setInfo00
static int tolua_Luainterface_ConfigurableSpell_setInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ConfigurableSpell", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ConfigurableSpell* self = (ConfigurableSpell*)tolua_tousertype(tolua_S, 1, 0);
		std::string newInfo = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setInfo'", NULL);
#endif
		{
			self->setInfo(newInfo);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setInfo'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setRange of class  ConfigurableSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ConfigurableSpell_setRange00
static int tolua_Luainterface_ConfigurableSpell_setRange00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ConfigurableSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ConfigurableSpell* self = (ConfigurableSpell*)tolua_tousertype(tolua_S, 1, 0);
		int minRange = ((int)tolua_tonumber(tolua_S, 2, 0));
		int maxRange = ((int)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setRange'", NULL);
#endif
		{
			self->setRange(minRange, maxRange);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setRange'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setCastTime of class  ConfigurableAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ConfigurableAction_setCastTime00
static int tolua_Luainterface_ConfigurableAction_setCastTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ConfigurableAction", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ConfigurableAction* self = (ConfigurableAction*)tolua_tousertype(tolua_S, 1, 0);
		int newCastTime = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setCastTime'", NULL);
#endif
		{
			self->setCastTime(newCastTime);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCastTime'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setCooldown of class  ConfigurableAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ConfigurableAction_setCooldown00
static int tolua_Luainterface_ConfigurableAction_setCooldown00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ConfigurableAction", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ConfigurableAction* self = (ConfigurableAction*)tolua_tousertype(tolua_S, 1, 0);
		int newCooldown = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setCooldown'", NULL);
#endif
		{
			self->setCooldown(newCooldown);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCooldown'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSpellCost of class  ConfigurableAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ConfigurableAction_setSpellCost00
static int tolua_Luainterface_ConfigurableAction_setSpellCost00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ConfigurableAction", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ConfigurableAction* self = (ConfigurableAction*)tolua_tousertype(tolua_S, 1, 0);
		int spellCost = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setSpellCost'", NULL);
#endif
		{
			self->setSpellCost(spellCost);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSpellCost'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setName of class  ConfigurableAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ConfigurableAction_setName00
static int tolua_Luainterface_ConfigurableAction_setName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ConfigurableAction", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ConfigurableAction* self = (ConfigurableAction*)tolua_tousertype(tolua_S, 1, 0);
		std::string newName = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setName'", NULL);
#endif
		{
			self->setName(newName);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setName'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setInfo of class  ConfigurableAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ConfigurableAction_setInfo00
static int tolua_Luainterface_ConfigurableAction_setInfo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ConfigurableAction", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ConfigurableAction* self = (ConfigurableAction*)tolua_tousertype(tolua_S, 1, 0);
		std::string newInfo = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setInfo'", NULL);
#endif
		{
			self->setInfo(newInfo);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setInfo'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setRange of class  ConfigurableAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ConfigurableAction_setRange00
static int tolua_Luainterface_ConfigurableAction_setRange00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ConfigurableAction", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ConfigurableAction* self = (ConfigurableAction*)tolua_tousertype(tolua_S, 1, 0);
		int minRange = ((int)tolua_tonumber(tolua_S, 2, 0));
		int maxRange = ((int)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setRange'", NULL);
#endif
		{
			self->setRange(minRange, maxRange);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setRange'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDirectDamage of class  GeneralDamageSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralDamageSpell_setDirectDamage00
static int tolua_Luainterface_GeneralDamageSpell_setDirectDamage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralDamageSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 5, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralDamageSpell* self = (GeneralDamageSpell*)tolua_tousertype(tolua_S, 1, 0);
		int newMinDirectDamage = ((int)tolua_tonumber(tolua_S, 2, 0));
		int newMaxDirectDamage = ((int)tolua_tonumber(tolua_S, 3, 0));
		Enums::ElementType newElementDirect = ((Enums::ElementType) (int)  tolua_tonumber(tolua_S, 4, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setDirectDamage'", NULL);
#endif
		{
			self->setDirectDamage(newMinDirectDamage, newMaxDirectDamage, newElementDirect);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setDirectDamage'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setContinuousDamage of class  GeneralDamageSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralDamageSpell_setContinuousDamage00
static int tolua_Luainterface_GeneralDamageSpell_setContinuousDamage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralDamageSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 6, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralDamageSpell* self = (GeneralDamageSpell*)tolua_tousertype(tolua_S, 1, 0);
		double newMinContDamagePerSec = ((double)tolua_tonumber(tolua_S, 2, 0));
		double newMaxContDamagePerSec = ((double)tolua_tonumber(tolua_S, 3, 0));
		int newContDamageTime = ((int)tolua_tonumber(tolua_S, 4, 0));
		Enums::ElementType newContDamageElement = ((Enums::ElementType) (int)  tolua_tonumber(tolua_S, 5, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setContinuousDamage'", NULL);
#endif
		{
			self->setContinuousDamage(newMinContDamagePerSec, newMaxContDamagePerSec, newContDamageTime, newContDamageElement);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setContinuousDamage'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addAnimationFrame of class  GeneralDamageSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralDamageSpell_addAnimationFrame00
static int tolua_Luainterface_GeneralDamageSpell_addAnimationFrame00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralDamageSpell", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 5, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 6, 1, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 7, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralDamageSpell* self = (GeneralDamageSpell*)tolua_tousertype(tolua_S, 1, 0);
		std::string file = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
		int paddingLeft = ((int)tolua_tonumber(tolua_S, 3, 0));
		int paddingRight = ((int)tolua_tonumber(tolua_S, 4, 0));
		int paddingTop = ((int)tolua_tonumber(tolua_S, 5, 0));
		int paddingBottom = ((int)tolua_tonumber(tolua_S, 6, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addAnimationFrame'", NULL);
#endif
		{
			self->addAnimationFrame(file, paddingLeft, paddingRight, paddingTop, paddingBottom);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addAnimationFrame'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setRadius of class  GeneralAreaDamageSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralAreaDamageSpell_setRadius00
static int tolua_Luainterface_GeneralAreaDamageSpell_setRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralAreaDamageSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralAreaDamageSpell* self = (GeneralAreaDamageSpell*)tolua_tousertype(tolua_S, 1, 0);
		int newRadius = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setRadius'", NULL);
#endif
		{
			self->setRadius(newRadius);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setRadius'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMoveSpeed of class  GeneralBoltDamageSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralBoltDamageSpell_setMoveSpeed00
static int tolua_Luainterface_GeneralBoltDamageSpell_setMoveSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralBoltDamageSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralBoltDamageSpell* self = (GeneralBoltDamageSpell*)tolua_tousertype(tolua_S, 1, 0);
		int newMoveSpeed = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMoveSpeed'", NULL);
#endif
		{
			self->setMoveSpeed(newMoveSpeed);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMoveSpeed'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setExpireTime of class  GeneralBoltDamageSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralBoltDamageSpell_setExpireTime00
static int tolua_Luainterface_GeneralBoltDamageSpell_setExpireTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralBoltDamageSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralBoltDamageSpell* self = (GeneralBoltDamageSpell*)tolua_tousertype(tolua_S, 1, 0);
		int newExpireTime = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setExpireTime'", NULL);
#endif
		{
			self->setExpireTime(newExpireTime);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setExpireTime'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setEffectType of class  GeneralHealingSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralHealingSpell_setEffectType00
static int tolua_Luainterface_GeneralHealingSpell_setEffectType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralHealingSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralHealingSpell* self = (GeneralHealingSpell*)tolua_tousertype(tolua_S, 1, 0);
		Enums::EffectType newEffectType = ((Enums::EffectType) (int)  tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setEffectType'", NULL);
#endif
		{
			self->setEffectType(newEffectType);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setEffectType'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDirectHealing of class  GeneralHealingSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralHealingSpell_setDirectHealing00
static int tolua_Luainterface_GeneralHealingSpell_setDirectHealing00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralHealingSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 5, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralHealingSpell* self = (GeneralHealingSpell*)tolua_tousertype(tolua_S, 1, 0);
		int healEffectMin = ((int)tolua_tonumber(tolua_S, 2, 0));
		int healEffectMax = ((int)tolua_tonumber(tolua_S, 3, 0));
		Enums::ElementType healEffectElement = ((Enums::ElementType) (int)  tolua_tonumber(tolua_S, 4, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setDirectHealing'", NULL);
#endif
		{
			self->setDirectHealing(healEffectMin, healEffectMax, healEffectElement);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setDirectHealing'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setContinuousHealing of class  GeneralHealingSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralHealingSpell_setContinuousHealing00
static int tolua_Luainterface_GeneralHealingSpell_setContinuousHealing00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralHealingSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 5, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 6, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralHealingSpell* self = (GeneralHealingSpell*)tolua_tousertype(tolua_S, 1, 0);
		double minContinuousHealingPerSecond = ((double)tolua_tonumber(tolua_S, 2, 0));
		double maxContinuousHealingPerSecond = ((double)tolua_tonumber(tolua_S, 3, 0));
		int continuousHealingTime = ((int)tolua_tonumber(tolua_S, 4, 0));
		Enums::ElementType elementContinuous = ((Enums::ElementType) (int)  tolua_tonumber(tolua_S, 5, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setContinuousHealing'", NULL);
#endif
		{
			self->setContinuousHealing(minContinuousHealingPerSecond, maxContinuousHealingPerSecond, continuousHealingTime, elementContinuous);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setContinuousHealing'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setEffectType of class  GeneralBuffSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralBuffSpell_setEffectType00
static int tolua_Luainterface_GeneralBuffSpell_setEffectType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralBuffSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralBuffSpell* self = (GeneralBuffSpell*)tolua_tousertype(tolua_S, 1, 0);
		Enums::EffectType newEffectType = ((Enums::EffectType) (int)  tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setEffectType'", NULL);
#endif
		{
			self->setEffectType(newEffectType);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setEffectType'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDuration of class  GeneralBuffSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralBuffSpell_setDuration00
static int tolua_Luainterface_GeneralBuffSpell_setDuration00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralBuffSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralBuffSpell* self = (GeneralBuffSpell*)tolua_tousertype(tolua_S, 1, 0);
		int newDuration = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setDuration'", NULL);
#endif
		{
			self->setDuration(newDuration);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setDuration'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setStats of class  GeneralBuffSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralBuffSpell_setStats00
static int tolua_Luainterface_GeneralBuffSpell_setStats00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralBuffSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralBuffSpell* self = (GeneralBuffSpell*)tolua_tousertype(tolua_S, 1, 0);
		Enums::StatsType statsType = ((Enums::StatsType) (int)  tolua_tonumber(tolua_S, 2, 0));
		int amount = ((int)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setStats'", NULL);
#endif
		{
			self->setStats(statsType, amount);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setStats'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setResistElementModifierPoints of class  GeneralBuffSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralBuffSpell_setResistElementModifierPoints00
static int tolua_Luainterface_GeneralBuffSpell_setResistElementModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralBuffSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralBuffSpell* self = (GeneralBuffSpell*)tolua_tousertype(tolua_S, 1, 0);
		Enums::ElementType elementType = ((Enums::ElementType) (int)  tolua_tonumber(tolua_S, 2, 0));
		int resistModifierPoints = ((int)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setResistElementModifierPoints'", NULL);
#endif
		{
			self->setResistElementModifierPoints(elementType, resistModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setResistElementModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSpellEffectElementModifierPoints of class  GeneralBuffSpell */
#ifndef TOLUA_DISABLE_tolua_Luainterface_GeneralBuffSpell_setSpellEffectElementModifierPoints00
static int tolua_Luainterface_GeneralBuffSpell_setSpellEffectElementModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "GeneralBuffSpell", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		GeneralBuffSpell* self = (GeneralBuffSpell*)tolua_tousertype(tolua_S, 1, 0);
		Enums::ElementType elementType = ((Enums::ElementType) (int)  tolua_tonumber(tolua_S, 2, 0));
		int spellEffectElementModifierPoints = ((int)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setSpellEffectElementModifierPoints'", NULL);
#endif
		{
			self->setSpellEffectElementModifierPoints(elementType, spellEffectElementModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSpellEffectElementModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDamageBonus of class  MeleeDamageAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_MeleeDamageAction_setDamageBonus00
static int tolua_Luainterface_MeleeDamageAction_setDamageBonus00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "MeleeDamageAction", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		MeleeDamageAction* self = (MeleeDamageAction*)tolua_tousertype(tolua_S, 1, 0);
		double damageBonus = ((double)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setDamageBonus'", NULL);
#endif
		{
			self->setDamageBonus(damageBonus);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setDamageBonus'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDamageBonus of class  RangedDamageAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_RangedDamageAction_setDamageBonus00
static int tolua_Luainterface_RangedDamageAction_setDamageBonus00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "RangedDamageAction", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		RangedDamageAction* self = (RangedDamageAction*)tolua_tousertype(tolua_S, 1, 0);
		double damageBonus = ((double)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setDamageBonus'", NULL);
#endif
		{
			self->setDamageBonus(damageBonus);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setDamageBonus'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMoveSpeed of class  RangedDamageAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_RangedDamageAction_setMoveSpeed00
static int tolua_Luainterface_RangedDamageAction_setMoveSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "RangedDamageAction", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		RangedDamageAction* self = (RangedDamageAction*)tolua_tousertype(tolua_S, 1, 0);
		int newMoveSpeed = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMoveSpeed'", NULL);
#endif
		{
			self->setMoveSpeed(newMoveSpeed);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMoveSpeed'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setExpireTime of class  RangedDamageAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_RangedDamageAction_setExpireTime00
static int tolua_Luainterface_RangedDamageAction_setExpireTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "RangedDamageAction", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		RangedDamageAction* self = (RangedDamageAction*)tolua_tousertype(tolua_S, 1, 0);
		int newExpireTime = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setExpireTime'", NULL);
#endif
		{
			self->setExpireTime(newExpireTime);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setExpireTime'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addAnimationFrame of class  RangedDamageAction */
#ifndef TOLUA_DISABLE_tolua_Luainterface_RangedDamageAction_addAnimationFrame00
static int tolua_Luainterface_RangedDamageAction_addAnimationFrame00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "RangedDamageAction", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 5, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 6, 1, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 7, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		RangedDamageAction* self = (RangedDamageAction*)tolua_tousertype(tolua_S, 1, 0);
		std::string file = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
		int paddingLeft = ((int)tolua_tonumber(tolua_S, 3, 0));
		int paddingRight = ((int)tolua_tonumber(tolua_S, 4, 0));
		int paddingTop = ((int)tolua_tonumber(tolua_S, 5, 0));
		int paddingBottom = ((int)tolua_tonumber(tolua_S, 6, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addAnimationFrame'", NULL);
#endif
		{
			self->addAnimationFrame(file, paddingLeft, paddingRight, paddingTop, paddingBottom);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addAnimationFrame'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDescription of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_setDescription00
static int tolua_Luainterface_Item_setDescription00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		std::string description = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setDescription'", NULL);
#endif
		{
			self->setDescription(description);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setDescription'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setStats of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_setStats00
static int tolua_Luainterface_Item_setStats00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		Enums::StatsType statsType = ((Enums::StatsType) (int)  tolua_tonumber(tolua_S, 2, 0));
		int amount = ((int)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setStats'", NULL);
#endif
		{
			self->setStats(statsType, amount);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setStats'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setResistElementModifierPoints of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_setResistElementModifierPoints00
static int tolua_Luainterface_Item_setResistElementModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		Enums::ElementType elementType = ((Enums::ElementType) (int)  tolua_tonumber(tolua_S, 2, 0));
		int resistModifierPoints = ((int)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setResistElementModifierPoints'", NULL);
#endif
		{
			self->setResistElementModifierPoints(elementType, resistModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setResistElementModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSpellEffectElementModifierPoints of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_setSpellEffectElementModifierPoints00
static int tolua_Luainterface_Item_setSpellEffectElementModifierPoints00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		Enums::ElementType elementType = ((Enums::ElementType) (int)  tolua_tonumber(tolua_S, 2, 0));
		int spellEffectElementModifierPoints = ((int)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setSpellEffectElementModifierPoints'", NULL);
#endif
		{
			self->setSpellEffectElementModifierPoints(elementType, spellEffectElementModifierPoints);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSpellEffectElementModifierPoints'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMinDamage of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_setMinDamage00
static int tolua_Luainterface_Item_setMinDamage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		int minDamage = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMinDamage'", NULL);
#endif
		{
			self->setMinDamage(minDamage);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMinDamage'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaxDamage of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_setMaxDamage00
static int tolua_Luainterface_Item_setMaxDamage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		int maxDamage = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMaxDamage'", NULL);
#endif
		{
			self->setMaxDamage(maxDamage);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMaxDamage'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setRequiredLevel of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_setRequiredLevel00
static int tolua_Luainterface_Item_setRequiredLevel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		int requiredLevel = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setRequiredLevel'", NULL);
#endif
		{
			self->setRequiredLevel(requiredLevel);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setRequiredLevel'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaxStackSize of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_setMaxStackSize00
static int tolua_Luainterface_Item_setMaxStackSize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		int maxStackSize = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMaxStackSize'", NULL);
#endif
		{
			self->setMaxStackSize(maxStackSize);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMaxStackSize'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setValue of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_setValue00
static int tolua_Luainterface_Item_setValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		int newValue = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setValue'", NULL);
#endif
		{
			self->setValue(newValue);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setValue'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSpell of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_setSpell00
static int tolua_Luainterface_Item_setSpell00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "Spell", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		Spell* newSpell = ((Spell*)tolua_tousertype(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setSpell'", NULL);
#endif
		{
			self->setSpell(newSpell);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setSpell'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setItemTextureRect of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_setItemTextureRect00
static int tolua_Luainterface_Item_setItemTextureRect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		(tolua_isvaluenil(tolua_S, 2, &tolua_err) || !tolua_isusertype(tolua_S, 2, "TextureRect", 0, &tolua_err)) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		TextureRect* textureRect = ((TextureRect*)tolua_tousertype(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setItemTextureRect'", NULL);
#endif
		{
			self->setItemTextureRect(*textureRect);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setItemTextureRect'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addTriggerSpellOnSelf of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_addTriggerSpellOnSelf00
static int tolua_Luainterface_Item_addTriggerSpellOnSelf00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 5, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		SpellActionBase* spellToTrigger = ((SpellActionBase*)tolua_tousertype(tolua_S, 2, 0));
		float chanceToTrigger = ((float)tolua_tonumber(tolua_S, 3, 0));
		Enums::TriggerType triggerType = ((Enums::TriggerType) (int)  tolua_tonumber(tolua_S, 4, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addTriggerSpellOnSelf'", NULL);
#endif
		{
			self->addTriggerSpellOnSelf(spellToTrigger, chanceToTrigger, triggerType);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addTriggerSpellOnSelf'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addTriggerSpellOnTarget of class  Item */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Item_addTriggerSpellOnTarget00
static int tolua_Luainterface_Item_addTriggerSpellOnTarget00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Item", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "SpellActionBase", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 5, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Item* self = (Item*)tolua_tousertype(tolua_S, 1, 0);
		SpellActionBase* spellToTrigger = ((SpellActionBase*)tolua_tousertype(tolua_S, 2, 0));
		float chanceToTrigger = ((float)tolua_tonumber(tolua_S, 3, 0));
		Enums::TriggerType triggerType = ((Enums::TriggerType) (int)  tolua_tonumber(tolua_S, 4, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addTriggerSpellOnTarget'", NULL);
#endif
		{
			self->addTriggerSpellOnTarget(spellToTrigger, chanceToTrigger, triggerType);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addTriggerSpellOnTarget'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addItem of class  Shop */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Shop_addItem00
static int tolua_Luainterface_Shop_addItem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Shop", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "Item", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Shop* self = (Shop*)tolua_tousertype(tolua_S, 1, 0);
		Item* item = ((Item*)tolua_tousertype(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addItem'", NULL);
#endif
		{
			self->addItem(item);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addItem'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: loadShopkeeperInventory of class  Shop */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Shop_loadShopkeeperInventory00
static int tolua_Luainterface_Shop_loadShopkeeperInventory00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Shop", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Shop* self = (Shop*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'loadShopkeeperInventory'", NULL);
#endif
		{
			self->loadShopkeeperInventory();
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'loadShopkeeperInventory'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setText of class  TextWindow */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextWindow_setText00
static int tolua_Luainterface_TextWindow_setText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TextWindow", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TextWindow* self = (TextWindow*)tolua_tousertype(tolua_S, 1, 0);
		std::string text = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setText'", NULL);
#endif
		{
			self->setText(text);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setText'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setAutocloseTime of class  TextWindow */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextWindow_setAutocloseTime00
static int tolua_Luainterface_TextWindow_setAutocloseTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TextWindow", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TextWindow* self = (TextWindow*)tolua_tousertype(tolua_S, 1, 0);
		int autocloseTime = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setAutocloseTime'", NULL);
#endif
		{
			self->setAutocloseTime(autocloseTime);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setAutocloseTime'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setPosition of class  TextWindow */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextWindow_setPosition00
static int tolua_Luainterface_TextWindow_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TextWindow", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 5, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TextWindow* self = (TextWindow*)tolua_tousertype(tolua_S, 1, 0);
		Enums::PositionType tolua_var_1 = ((Enums::PositionType) (int)  tolua_tonumber(tolua_S, 2, 0));
		int x = ((int)tolua_tonumber(tolua_S, 3, 0));
		int y = ((int)tolua_tonumber(tolua_S, 4, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setPosition'", NULL);
#endif
		{
			self->setPosition(tolua_var_1, x, y);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setPosition'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: center of class  TextWindow */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextWindow_center00
static int tolua_Luainterface_TextWindow_center00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TextWindow", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TextWindow* self = (TextWindow*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'center'", NULL);
#endif
		{
			self->center();
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'center'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setOnCloseText of class  TextWindow */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextWindow_setOnCloseText00
static int tolua_Luainterface_TextWindow_setOnCloseText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TextWindow", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TextWindow* self = (TextWindow*)tolua_tousertype(tolua_S, 1, 0);
		std::string onCloseText = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setOnCloseText'", NULL);
#endif
		{
			self->setOnCloseText(onCloseText);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setOnCloseText'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addRequiredItemForCompletion of class  Quest */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Quest_addRequiredItemForCompletion00
static int tolua_Luainterface_Quest_addRequiredItemForCompletion00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Quest", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "Item", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Quest* self = (Quest*)tolua_tousertype(tolua_S, 1, 0);
		Item* requiredItem = ((Item*)tolua_tousertype(tolua_S, 2, 0));
		int quantity = ((int)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addRequiredItemForCompletion'", NULL);
#endif
		{
			self->addRequiredItemForCompletion(requiredItem, quantity);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addRequiredItemForCompletion'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDescription of class  Quest */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Quest_setDescription00
static int tolua_Luainterface_Quest_setDescription00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Quest", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Quest* self = (Quest*)tolua_tousertype(tolua_S, 1, 0);
		std::string tolua_var_2 = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setDescription'", NULL);
#endif
		{
			self->setDescription(tolua_var_2);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setDescription'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setExperienceReward of class  Quest */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Quest_setExperienceReward00
static int tolua_Luainterface_Quest_setExperienceReward00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Quest", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Quest* self = (Quest*)tolua_tousertype(tolua_S, 1, 0);
		int experienceReward = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setExperienceReward'", NULL);
#endif
		{
			self->setExperienceReward(experienceReward);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setExperienceReward'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setCoinReward of class  Quest */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Quest_setCoinReward00
static int tolua_Luainterface_Quest_setCoinReward00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Quest", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Quest* self = (Quest*)tolua_tousertype(tolua_S, 1, 0);
		int coinReward = ((int)tolua_tonumber(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setCoinReward'", NULL);
#endif
		{
			self->setCoinReward(coinReward);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setCoinReward'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setItemReward of class  Quest */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Quest_setItemReward00
static int tolua_Luainterface_Quest_setItemReward00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Quest", 0, &tolua_err) ||
		!tolua_isusertype(tolua_S, 2, "Item", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Quest* self = (Quest*)tolua_tousertype(tolua_S, 1, 0);
		Item* itemReward = ((Item*)tolua_tousertype(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setItemReward'", NULL);
#endif
		{
			self->setItemReward(itemReward);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setItemReward'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: finishQuest of class  Quest */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Quest_finishQuest00
static int tolua_Luainterface_Quest_finishQuest00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "Quest", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Quest* self = (Quest*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'finishQuest'", NULL);
#endif
		{
			bool tolua_ret = (bool)self->finishQuest();
			tolua_pushboolean(tolua_S, (bool)tolua_ret);
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'finishQuest'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setExecuteText of class  LuaCallIndirection */
#ifndef TOLUA_DISABLE_tolua_Luainterface_LuaCallIndirection_setExecuteText00
static int tolua_Luainterface_LuaCallIndirection_setExecuteText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "LuaCallIndirection", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		LuaCallIndirection* self = (LuaCallIndirection*)tolua_tousertype(tolua_S, 1, 0);
		std::string text = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setExecuteText'", NULL);
#endif
		{
			self->setExecuteText(text);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setExecuteText'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Get of class  ShopCanvas */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ShopCanvas_Get00
static int tolua_Luainterface_ShopCanvas_Get00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertable(tolua_S, 1, "ShopCanvas", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		{
			ShopCanvas& tolua_ret = (ShopCanvas&)ShopCanvas::Get();
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "ShopCanvas");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'Get'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: activate of class  ShopCanvas */
#ifndef TOLUA_DISABLE_tolua_Luainterface_ShopCanvas_activate00
static int tolua_Luainterface_ShopCanvas_activate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "ShopCanvas", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		ShopCanvas* self = (ShopCanvas*)tolua_tousertype(tolua_S, 1, 0);
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'activate'", NULL);
#endif
		{
			self->activate();
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'activate'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_Luainterface_open(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	tolua_reg_types(tolua_S);
	tolua_module(tolua_S, NULL, 0);
	tolua_beginmodule(tolua_S, NULL);
	tolua_module(tolua_S, "Enums", 0);
	tolua_beginmodule(tolua_S, "Enums");
	tolua_constant(tolua_S, "FLOOR", Enums::FLOOR);
	tolua_constant(tolua_S, "ENVIRONMENT", Enums::ENVIRONMENT);
	tolua_constant(tolua_S, "SHADOW", Enums::SHADOW);
	tolua_constant(tolua_S, "COLLISION", Enums::COLLISION);
	tolua_constant(tolua_S, "NPC", Enums::NPC);
	tolua_constant(tolua_S, "COUNT", Enums::COUNT);
	tolua_constant(tolua_S, "RIGHT", Enums::RIGHT);
	tolua_constant(tolua_S, "LEFT", Enums::LEFT);
	tolua_constant(tolua_S, "TOP", Enums::TOP);
	tolua_constant(tolua_S, "BOTTOM", Enums::BOTTOM);
	tolua_constant(tolua_S, "FRIENDLY", Enums::FRIENDLY);
	tolua_constant(tolua_S, "NEUTRAL", Enums::NEUTRAL);
	tolua_constant(tolua_S, "HOSTILE", Enums::HOSTILE);
	tolua_constant(tolua_S, "NOCLASS", Enums::NOCLASS);
	tolua_constant(tolua_S, "ANYCLASS", Enums::ANYCLASS);
	tolua_constant(tolua_S, "Liche", Enums::Liche);
	tolua_constant(tolua_S, "Ranger", Enums::Ranger);
	tolua_constant(tolua_S, "Warrior", Enums::Warrior);
	tolua_constant(tolua_S, "Walking", Enums::Walking);
	tolua_constant(tolua_S, "Casting", Enums::Casting);
	tolua_constant(tolua_S, "Attacking", Enums::Attacking);
	tolua_constant(tolua_S, "Shooting", Enums::Shooting);
	tolua_constant(tolua_S, "Dying", Enums::Dying);
	tolua_constant(tolua_S, "Count", Enums::Count);
	tolua_constant(tolua_S, "STOP", Enums::STOP);
	tolua_constant(tolua_S, "N", Enums::N);
	tolua_constant(tolua_S, "NE", Enums::NE);
	tolua_constant(tolua_S, "E", Enums::E);
	tolua_constant(tolua_S, "SE", Enums::SE);
	tolua_constant(tolua_S, "S", Enums::S);
	tolua_constant(tolua_S, "SW", Enums::SW);
	tolua_constant(tolua_S, "W", Enums::W);
	tolua_constant(tolua_S, "NW", Enums::NW);
	tolua_constant(tolua_S, "Quest", Enums::Quest);
	tolua_constant(tolua_S, "Shop", Enums::Shop);
	tolua_constant(tolua_S, "Zone", Enums::Zone);
	tolua_constant(tolua_S, "SingleTargetSpell", Enums::SingleTargetSpell);
	tolua_constant(tolua_S, "SelfAffectingSpell", Enums::SelfAffectingSpell);
	tolua_constant(tolua_S, "AreaTargetSpell", Enums::AreaTargetSpell);
	tolua_constant(tolua_S, "Fire", Enums::Fire);
	tolua_constant(tolua_S, "Water", Enums::Water);
	tolua_constant(tolua_S, "Air", Enums::Air);
	tolua_constant(tolua_S, "Earth", Enums::Earth);
	tolua_constant(tolua_S, "Light", Enums::Light);
	tolua_constant(tolua_S, "Dark", Enums::Dark);
	tolua_constant(tolua_S, "CountET", Enums::CountET);
	tolua_constant(tolua_S, "Dexterity", Enums::Dexterity);
	tolua_constant(tolua_S, "Intellect", Enums::Intellect);
	tolua_constant(tolua_S, "Strength", Enums::Strength);
	tolua_constant(tolua_S, "Vitality", Enums::Vitality);
	tolua_constant(tolua_S, "Wisdom", Enums::Wisdom);
	tolua_constant(tolua_S, "Health", Enums::Health);
	tolua_constant(tolua_S, "Mana", Enums::Mana);
	tolua_constant(tolua_S, "Fatigue", Enums::Fatigue);
	tolua_constant(tolua_S, "Armor", Enums::Armor);
	tolua_constant(tolua_S, "DamageModifier", Enums::DamageModifier);
	tolua_constant(tolua_S, "HitModifier", Enums::HitModifier);
	tolua_constant(tolua_S, "EvadeModifier", Enums::EvadeModifier);
	tolua_constant(tolua_S, "ParryModifier", Enums::ParryModifier);
	tolua_constant(tolua_S, "BlockModifier", Enums::BlockModifier);
	tolua_constant(tolua_S, "MeleeCritical", Enums::MeleeCritical);
	tolua_constant(tolua_S, "SpellCritical", Enums::SpellCritical);
	tolua_constant(tolua_S, "ResistAll", Enums::ResistAll);
	tolua_constant(tolua_S, "SpellEffectAll", Enums::SpellEffectAll);
	tolua_constant(tolua_S, "HealthRegen", Enums::HealthRegen);
	tolua_constant(tolua_S, "ManaRegen", Enums::ManaRegen);
	tolua_constant(tolua_S, "FatigueRegen", Enums::FatigueRegen);
	tolua_constant(tolua_S, "CountST", Enums::CountST);
	tolua_constant(tolua_S, "Channeling", Enums::Channeling);
	tolua_constant(tolua_S, "Charmed", Enums::Charmed);
	tolua_constant(tolua_S, "Confused", Enums::Confused);
	tolua_constant(tolua_S, "Feared", Enums::Feared);
	tolua_constant(tolua_S, "Invisible", Enums::Invisible);
	tolua_constant(tolua_S, "Mesmerized", Enums::Mesmerized);
	tolua_constant(tolua_S, "Movementspeed", Enums::Movementspeed);
	tolua_constant(tolua_S, "SeeInvisible", Enums::SeeInvisible);
	tolua_constant(tolua_S, "SeeSneaking", Enums::SeeSneaking);
	tolua_constant(tolua_S, "Sneaking", Enums::Sneaking);
	tolua_constant(tolua_S, "Stunned", Enums::Stunned);
	tolua_constant(tolua_S, "NOEFFECT", Enums::NOEFFECT);
	tolua_constant(tolua_S, "NO_WEAPON", Enums::NO_WEAPON);
	tolua_constant(tolua_S, "ONEHAND_SWORD", Enums::ONEHAND_SWORD);
	tolua_constant(tolua_S, "TWOHAND_SWORD", Enums::TWOHAND_SWORD);
	tolua_constant(tolua_S, "DAGGER", Enums::DAGGER);
	tolua_constant(tolua_S, "STAFF", Enums::STAFF);
	tolua_constant(tolua_S, "ONEHAND_CLUB", Enums::ONEHAND_CLUB);
	tolua_constant(tolua_S, "TWOHAND_CLUB", Enums::TWOHAND_CLUB);
	tolua_constant(tolua_S, "WAND", Enums::WAND);
	tolua_constant(tolua_S, "SPELLBOOK", Enums::SPELLBOOK);
	tolua_constant(tolua_S, "ONEHAND_MACE", Enums::ONEHAND_MACE);
	tolua_constant(tolua_S, "TWOHAND_MACE", Enums::TWOHAND_MACE);
	tolua_constant(tolua_S, "ONEHAND_AXE", Enums::ONEHAND_AXE);
	tolua_constant(tolua_S, "TWOHAND_AXE", Enums::TWOHAND_AXE);
	tolua_constant(tolua_S, "BOW", Enums::BOW);
	tolua_constant(tolua_S, "SLINGSHOT", Enums::SLINGSHOT);
	tolua_constant(tolua_S, "CROSSBOW", Enums::CROSSBOW);
	tolua_constant(tolua_S, "SHIELD", Enums::SHIELD);
	tolua_constant(tolua_S, "COUNTWT", Enums::COUNTWT);
	tolua_constant(tolua_S, "QUESTITEM", Enums::QUESTITEM);
	tolua_constant(tolua_S, "MISCELLANEOUS", Enums::MISCELLANEOUS);
	tolua_constant(tolua_S, "ARMOR", Enums::ARMOR);
	tolua_constant(tolua_S, "WEAPON", Enums::WEAPON);
	tolua_constant(tolua_S, "JEWELRY", Enums::JEWELRY);
	tolua_constant(tolua_S, "SCROLL", Enums::SCROLL);
	tolua_constant(tolua_S, "POTION", Enums::POTION);
	tolua_constant(tolua_S, "FOOD", Enums::FOOD);
	tolua_constant(tolua_S, "DRINK", Enums::DRINK);
	tolua_constant(tolua_S, "NEWSPELL", Enums::NEWSPELL);
	tolua_constant(tolua_S, "COUNT", Enums::COUNT);
	tolua_constant(tolua_S, "NO_ARMOR", Enums::NO_ARMOR);
	tolua_constant(tolua_S, "CLOTH", Enums::CLOTH);
	tolua_constant(tolua_S, "LEATHER", Enums::LEATHER);
	tolua_constant(tolua_S, "MAIL", Enums::MAIL);
	tolua_constant(tolua_S, "PLATE", Enums::PLATE);
	tolua_constant(tolua_S, "POOR", Enums::POOR);
	tolua_constant(tolua_S, "NORMAL", Enums::NORMAL);
	tolua_constant(tolua_S, "ENHANCED", Enums::ENHANCED);
	tolua_constant(tolua_S, "RARE", Enums::RARE);
	tolua_constant(tolua_S, "LORE", Enums::LORE);
	tolua_constant(tolua_S, "TAKING_DAMAGE", Enums::TAKING_DAMAGE);
	tolua_constant(tolua_S, "EXECUTING_ACTION", Enums::EXECUTING_ACTION);
	tolua_constant(tolua_S, "BOTTOMLEFT", Enums::BOTTOMLEFT);
	tolua_constant(tolua_S, "BOTTOMCENTER", Enums::BOTTOMCENTER);
	tolua_constant(tolua_S, "LEFTCENTER", Enums::LEFTCENTER);
	tolua_constant(tolua_S, "CENTER", Enums::CENTER);
	tolua_endmodule(tolua_S);
	tolua_module(tolua_S, "EquipPosition", 0);
	tolua_beginmodule(tolua_S, "EquipPosition");
	tolua_constant(tolua_S, "MAIN_HAND", EquipPosition::MAIN_HAND);
	tolua_constant(tolua_S, "OFF_HAND", EquipPosition::OFF_HAND);
	tolua_constant(tolua_S, "CHEST", EquipPosition::CHEST);
	tolua_constant(tolua_S, "LEGS", EquipPosition::LEGS);
	tolua_constant(tolua_S, "BELT", EquipPosition::BELT);
	tolua_constant(tolua_S, "BOOTS", EquipPosition::BOOTS);
	tolua_constant(tolua_S, "RING", EquipPosition::RING);
	tolua_constant(tolua_S, "GLOVES", EquipPosition::GLOVES);
	tolua_constant(tolua_S, "HEAD", EquipPosition::HEAD);
	tolua_constant(tolua_S, "CLOAK", EquipPosition::CLOAK);
	tolua_constant(tolua_S, "AMULET", EquipPosition::AMULET);
	tolua_constant(tolua_S, "SHOULDER", EquipPosition::SHOULDER);
	tolua_constant(tolua_S, "NONE", EquipPosition::NONE);
	tolua_endmodule(tolua_S);
	tolua_module(tolua_S, "Globals", 0);
	tolua_beginmodule(tolua_S, "Globals");
	tolua_function(tolua_S, "useDisplaymode", tolua_Luainterface_Globals_useDisplaymode00);
	tolua_function(tolua_S, "isFullscreen", tolua_Luainterface_Globals_isFullscreen00);
	tolua_function(tolua_S, "setSoundVolume", tolua_Luainterface_Globals_setSoundVolume00);
	tolua_function(tolua_S, "setMusicVolume", tolua_Luainterface_Globals_setMusicVolume00);
	tolua_function(tolua_S, "setResolution", tolua_Luainterface_Globals_setResolution00);
	tolua_endmodule(tolua_S);
	tolua_module(tolua_S, "DawnInterface", 0);
	tolua_beginmodule(tolua_S, "DawnInterface");
	tolua_function(tolua_S, "loadimage", tolua_Luainterface_DawnInterface_loadimage00);
	tolua_function(tolua_S, "enterZone", tolua_Luainterface_DawnInterface_enterZone00);
	tolua_function(tolua_S, "setCurrentZone", tolua_Luainterface_DawnInterface_setCurrentZone00);
	tolua_function(tolua_S, "createNewMobType", tolua_Luainterface_DawnInterface_createNewMobType00);
	tolua_function(tolua_S, "addMobSpawnPoint", tolua_Luainterface_DawnInterface_addMobSpawnPoint00);
	tolua_function(tolua_S, "removeMobSpawnPoint", tolua_Luainterface_DawnInterface_removeMobSpawnPoint00);
	tolua_function(tolua_S, "addInteractionRegion", tolua_Luainterface_DawnInterface_addInteractionRegion00);
	tolua_function(tolua_S, "removeInteractionRegion", tolua_Luainterface_DawnInterface_removeInteractionRegion00);
	tolua_function(tolua_S, "addInteractionPoint", tolua_Luainterface_DawnInterface_addInteractionPoint00);
	tolua_function(tolua_S, "addCharacterInteractionPoint", tolua_Luainterface_DawnInterface_addCharacterInteractionPoint00);
	tolua_function(tolua_S, "removeInteractionPoint", tolua_Luainterface_DawnInterface_removeInteractionPoint00);
	tolua_function(tolua_S, "getItemReferenceRestore", tolua_Luainterface_DawnInterface_getItemReferenceRestore00);
	tolua_function(tolua_S, "getItemReferenceRestore", tolua_Luainterface_DawnInterface_getItemReferenceRestore01);
	tolua_function(tolua_S, "getItemReferenceRestore", tolua_Luainterface_DawnInterface_getItemReferenceRestore02);
	tolua_function(tolua_S, "getItemReferenceRestore", tolua_Luainterface_DawnInterface_getItemReferenceRestore03);
	tolua_function(tolua_S, "getItemReferenceRestore", tolua_Luainterface_DawnInterface_getItemReferenceRestore04);
	tolua_function(tolua_S, "getItemReferenceRestore", tolua_Luainterface_DawnInterface_getItemReferenceRestore05);
	tolua_function(tolua_S, "getItemReferenceRestore", tolua_Luainterface_DawnInterface_getItemReferenceRestore06);
	tolua_function(tolua_S, "storeGroundloot", tolua_Luainterface_DawnInterface_storeGroundloot00);
	tolua_function(tolua_S, "getSpellbookSaveText", tolua_Luainterface_DawnInterface_getSpellbookSaveText00);
	tolua_function(tolua_S, "getActionbarSaveText", tolua_Luainterface_DawnInterface_getActionbarSaveText00);
	tolua_function(tolua_S, "getReenterCurrentZoneText", tolua_Luainterface_DawnInterface_getReenterCurrentZoneText00);
	tolua_function(tolua_S, "restoreActionBar", tolua_Luainterface_DawnInterface_restoreActionBar00);
	tolua_function(tolua_S, "restoreGroundLootItem", tolua_Luainterface_DawnInterface_restoreGroundLootItem00);
	tolua_function(tolua_S, "restoreGroundGold", tolua_Luainterface_DawnInterface_restoreGroundGold00);
	tolua_function(tolua_S, "createTextWindow", tolua_Luainterface_DawnInterface_createTextWindow00);
	tolua_function(tolua_S, "createGeneralRayDamageSpell", tolua_Luainterface_DawnInterface_createGeneralRayDamageSpell00);
	tolua_function(tolua_S, "createGeneralAreaDamageSpell", tolua_Luainterface_DawnInterface_createGeneralAreaDamageSpell00);
	tolua_function(tolua_S, "createGeneralBoltDamageSpell", tolua_Luainterface_DawnInterface_createGeneralBoltDamageSpell00);
	tolua_function(tolua_S, "createGeneralHealingSpell", tolua_Luainterface_DawnInterface_createGeneralHealingSpell00);
	tolua_function(tolua_S, "createGeneralBuffSpell", tolua_Luainterface_DawnInterface_createGeneralBuffSpell00);
	tolua_function(tolua_S, "createMeleeDamageAction", tolua_Luainterface_DawnInterface_createMeleeDamageAction00);
	tolua_function(tolua_S, "createRangedDamageAction", tolua_Luainterface_DawnInterface_createRangedDamageAction00);
	tolua_function(tolua_S, "copySpell", tolua_Luainterface_DawnInterface_copySpell00);
	tolua_function(tolua_S, "copySpell", tolua_Luainterface_DawnInterface_copySpell01);
	tolua_function(tolua_S, "copySpell", tolua_Luainterface_DawnInterface_copySpell02);
	tolua_function(tolua_S, "copySpell", tolua_Luainterface_DawnInterface_copySpell03);
	tolua_function(tolua_S, "copySpell", tolua_Luainterface_DawnInterface_copySpell04);
	tolua_function(tolua_S, "copySpell", tolua_Luainterface_DawnInterface_copySpell05);
	tolua_function(tolua_S, "copySpell", tolua_Luainterface_DawnInterface_copySpell06);
	tolua_function(tolua_S, "inscribeSpellInPlayerSpellbook", tolua_Luainterface_DawnInterface_inscribeSpellInPlayerSpellbook00);
	tolua_function(tolua_S, "reloadSpellsFromPlayer", tolua_Luainterface_DawnInterface_reloadSpellsFromPlayer00);
	tolua_function(tolua_S, "getInventorySaveText", tolua_Luainterface_DawnInterface_getInventorySaveText00);
	tolua_function(tolua_S, "restoreItemInBackpack", tolua_Luainterface_DawnInterface_restoreItemInBackpack00);
	tolua_function(tolua_S, "restoreWieldItem", tolua_Luainterface_DawnInterface_restoreWieldItem00);
	tolua_function(tolua_S, "createNewItem", tolua_Luainterface_DawnInterface_createNewItem00);
	tolua_function(tolua_S, "getPlayer", tolua_Luainterface_DawnInterface_getPlayer00);
	tolua_function(tolua_S, "getCurrentZone", tolua_Luainterface_DawnInterface_getCurrentZone00);
	tolua_function(tolua_S, "giveItemToPlayer", tolua_Luainterface_DawnInterface_giveItemToPlayer00);
	tolua_function(tolua_S, "addShop", tolua_Luainterface_DawnInterface_addShop00);
	tolua_function(tolua_S, "addQuest", tolua_Luainterface_DawnInterface_addQuest00);
	tolua_function(tolua_S, "createEventHandler", tolua_Luainterface_DawnInterface_createEventHandler00);
	tolua_function(tolua_S, "isSavingAllowed", tolua_Luainterface_DawnInterface_isSavingAllowed00);
	tolua_function(tolua_S, "setSavingAllowed", tolua_Luainterface_DawnInterface_setSavingAllowed00);
	tolua_function(tolua_S, "restoreCharacterReference", tolua_Luainterface_DawnInterface_restoreCharacterReference00);
	tolua_function(tolua_S, "restoreInteractionPointReference", tolua_Luainterface_DawnInterface_restoreInteractionPointReference00);
	tolua_function(tolua_S, "restoreInteractionRegionReference", tolua_Luainterface_DawnInterface_restoreInteractionRegionReference00);
	tolua_function(tolua_S, "restoreEventHandlerReference", tolua_Luainterface_DawnInterface_restoreEventHandlerReference00);
	tolua_endmodule(tolua_S);
	tolua_module(tolua_S, "EditorInterface", 0);
	tolua_beginmodule(tolua_S, "EditorInterface");
	tolua_function(tolua_S, "getTileSet", tolua_Luainterface_EditorInterface_getTileSet00);
	tolua_function(tolua_S, "addGroundTile", tolua_Luainterface_EditorInterface_addGroundTile00);
	tolua_function(tolua_S, "addEnvironment", tolua_Luainterface_EditorInterface_addEnvironment00);
	tolua_function(tolua_S, "adjustLastRGBA", tolua_Luainterface_EditorInterface_adjustLastRGBA00);
	tolua_function(tolua_S, "adjustLastSize", tolua_Luainterface_EditorInterface_adjustLastSize00);
	tolua_function(tolua_S, "addCollisionRect", tolua_Luainterface_EditorInterface_addCollisionRect00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "AdjacencyEquivalenceClass", "AdjacencyEquivalenceClass", "", NULL);
	tolua_beginmodule(tolua_S, "AdjacencyEquivalenceClass");
	tolua_function(tolua_S, "addEquivalentTile", tolua_Luainterface_AdjacencyEquivalenceClass_addEquivalentTile00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "TileSet", "TileSet", "", NULL);
	tolua_beginmodule(tolua_S, "TileSet");
	tolua_function(tolua_S, "addTile", tolua_Luainterface_TileSet_addTile00);
	tolua_function(tolua_S, "addTileWithCollisionBox", tolua_Luainterface_TileSet_addTileWithCollisionBox00);
	tolua_function(tolua_S, "addAdjacency", tolua_Luainterface_TileSet_addAdjacency00);
	tolua_function(tolua_S, "createAdjacencyEquivalenceClass", tolua_Luainterface_TileSet_createAdjacencyEquivalenceClass00);
	tolua_function(tolua_S, "addEquivalenceAdjacency", tolua_Luainterface_TileSet_addEquivalenceAdjacency00);
	tolua_function(tolua_S, "printTileSet", tolua_Luainterface_TileSet_printTileSet00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "CharacterType", "CharacterType", "", NULL);
	tolua_beginmodule(tolua_S, "CharacterType");
	tolua_function(tolua_S, "addMoveTexture", tolua_Luainterface_CharacterType_addMoveTexture00);
	tolua_function(tolua_S, "calcNumMoveTexturesPerDirection", tolua_Luainterface_CharacterType_calcNumMoveTexturesPerDirection00);
	tolua_function(tolua_S, "baseOnType", tolua_Luainterface_CharacterType_baseOnType00);
	tolua_function(tolua_S, "inscribeSpellInSpellbook", tolua_Luainterface_CharacterType_inscribeSpellInSpellbook00);
	tolua_function(tolua_S, "addItemToLootTable", tolua_Luainterface_CharacterType_addItemToLootTable00);
	tolua_function(tolua_S, "setCoinDrop", tolua_Luainterface_CharacterType_setCoinDrop00);
	tolua_function(tolua_S, "setStrength", tolua_Luainterface_CharacterType_setStrength00);
	tolua_function(tolua_S, "setDexterity", tolua_Luainterface_CharacterType_setDexterity00);
	tolua_function(tolua_S, "setVitality", tolua_Luainterface_CharacterType_setVitality00);
	tolua_function(tolua_S, "setIntellect", tolua_Luainterface_CharacterType_setIntellect00);
	tolua_function(tolua_S, "setWisdom", tolua_Luainterface_CharacterType_setWisdom00);
	tolua_function(tolua_S, "setMaxHealth", tolua_Luainterface_CharacterType_setMaxHealth00);
	tolua_function(tolua_S, "setMaxMana", tolua_Luainterface_CharacterType_setMaxMana00);
	tolua_function(tolua_S, "setMaxFatigue", tolua_Luainterface_CharacterType_setMaxFatigue00);
	tolua_function(tolua_S, "setHealthRegen", tolua_Luainterface_CharacterType_setHealthRegen00);
	tolua_function(tolua_S, "setManaRegen", tolua_Luainterface_CharacterType_setManaRegen00);
	tolua_function(tolua_S, "setFatigueRegen", tolua_Luainterface_CharacterType_setFatigueRegen00);
	tolua_function(tolua_S, "setMinDamage", tolua_Luainterface_CharacterType_setMinDamage00);
	tolua_function(tolua_S, "setMaxDamage", tolua_Luainterface_CharacterType_setMaxDamage00);
	tolua_function(tolua_S, "setArmor", tolua_Luainterface_CharacterType_setArmor00);
	tolua_function(tolua_S, "setDamageModifierPoints", tolua_Luainterface_CharacterType_setDamageModifierPoints00);
	tolua_function(tolua_S, "setHitModifierPoints", tolua_Luainterface_CharacterType_setHitModifierPoints00);
	tolua_function(tolua_S, "setEvadeModifierPoints", tolua_Luainterface_CharacterType_setEvadeModifierPoints00);
	tolua_function(tolua_S, "setName", tolua_Luainterface_CharacterType_setName00);
	tolua_function(tolua_S, "setWanderRadius", tolua_Luainterface_CharacterType_setWanderRadius00);
	tolua_function(tolua_S, "setLevel", tolua_Luainterface_CharacterType_setLevel00);
	tolua_function(tolua_S, "setClass", tolua_Luainterface_CharacterType_setClass00);
	tolua_function(tolua_S, "setExperienceValue", tolua_Luainterface_CharacterType_setExperienceValue00);
	tolua_function(tolua_S, "modifyStrength", tolua_Luainterface_CharacterType_modifyStrength00);
	tolua_function(tolua_S, "modifyDexterity", tolua_Luainterface_CharacterType_modifyDexterity00);
	tolua_function(tolua_S, "modifyVitality", tolua_Luainterface_CharacterType_modifyVitality00);
	tolua_function(tolua_S, "modifyIntellect", tolua_Luainterface_CharacterType_modifyIntellect00);
	tolua_function(tolua_S, "modifyWisdom", tolua_Luainterface_CharacterType_modifyWisdom00);
	tolua_function(tolua_S, "modifyMaxHealth", tolua_Luainterface_CharacterType_modifyMaxHealth00);
	tolua_function(tolua_S, "modifyMaxFatigue", tolua_Luainterface_CharacterType_modifyMaxFatigue00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "Player", "Player", "Character", NULL);
	tolua_beginmodule(tolua_S, "Player");
	tolua_function(tolua_S, "Get", tolua_Luainterface_Player_Get00);
	tolua_function(tolua_S, "setBoundingBox", tolua_Luainterface_Player_setBoundingBox00);
	tolua_function(tolua_S, "setClass", tolua_Luainterface_Player_setClass00);
	tolua_function(tolua_S, "setName", tolua_Luainterface_Player_setName00);
	tolua_function(tolua_S, "setStrength", tolua_Luainterface_Player_setStrength00);
	tolua_function(tolua_S, "setDexterity", tolua_Luainterface_Player_setDexterity00);
	tolua_function(tolua_S, "setVitality", tolua_Luainterface_Player_setVitality00);
	tolua_function(tolua_S, "setIntellect", tolua_Luainterface_Player_setIntellect00);
	tolua_function(tolua_S, "setWisdom", tolua_Luainterface_Player_setWisdom00);
	tolua_function(tolua_S, "setMaxHealth", tolua_Luainterface_Player_setMaxHealth00);
	tolua_function(tolua_S, "setMaxMana", tolua_Luainterface_Player_setMaxMana00);
	tolua_function(tolua_S, "setMaxFatigue", tolua_Luainterface_Player_setMaxFatigue00);
	tolua_function(tolua_S, "setHealthRegen", tolua_Luainterface_Player_setHealthRegen00);
	tolua_function(tolua_S, "setManaRegen", tolua_Luainterface_Player_setManaRegen00);
	tolua_function(tolua_S, "setFatigueRegen", tolua_Luainterface_Player_setFatigueRegen00);
	tolua_function(tolua_S, "setMinDamage", tolua_Luainterface_Player_setMinDamage00);
	tolua_function(tolua_S, "setMaxDamage", tolua_Luainterface_Player_setMaxDamage00);
	tolua_function(tolua_S, "setArmor", tolua_Luainterface_Player_setArmor00);
	tolua_function(tolua_S, "setDamageModifierPoints", tolua_Luainterface_Player_setDamageModifierPoints00);
	tolua_function(tolua_S, "setHitModifierPoints", tolua_Luainterface_Player_setHitModifierPoints00);
	tolua_function(tolua_S, "setEvadeModifierPoints", tolua_Luainterface_Player_setEvadeModifierPoints00);
	tolua_function(tolua_S, "setParryModifierPoints", tolua_Luainterface_Player_setParryModifierPoints00);
	tolua_function(tolua_S, "setBlockModifierPoints", tolua_Luainterface_Player_setBlockModifierPoints00);
	tolua_function(tolua_S, "setMeleeCriticalModifierPoints", tolua_Luainterface_Player_setMeleeCriticalModifierPoints00);
	tolua_function(tolua_S, "setResistElementModifierPoints", tolua_Luainterface_Player_setResistElementModifierPoints00);
	tolua_function(tolua_S, "setResistAllModifierPoints", tolua_Luainterface_Player_setResistAllModifierPoints00);
	tolua_function(tolua_S, "setSpellEffectElementModifierPoints", tolua_Luainterface_Player_setSpellEffectElementModifierPoints00);
	tolua_function(tolua_S, "setSpellEffectAllModifierPoints", tolua_Luainterface_Player_setSpellEffectAllModifierPoints00);
	tolua_function(tolua_S, "setSpellCriticalModifierPoints", tolua_Luainterface_Player_setSpellCriticalModifierPoints00);
	tolua_function(tolua_S, "setLevel", tolua_Luainterface_Player_setLevel00);
	tolua_function(tolua_S, "setExperience", tolua_Luainterface_Player_setExperience00);
	tolua_function(tolua_S, "giveCoins", tolua_Luainterface_Player_giveCoins00);
	tolua_function(tolua_S, "setCoins", tolua_Luainterface_Player_setCoins00);
	tolua_function(tolua_S, "setPosition", tolua_Luainterface_Player_setPosition00);
	tolua_function(tolua_S, "setCharacterType", tolua_Luainterface_Player_setCharacterType00);
	tolua_function(tolua_S, "init", tolua_Luainterface_Player_init00);
	tolua_function(tolua_S, "getClass", tolua_Luainterface_Player_getClass00);
	tolua_function(tolua_S, "getSaveText", tolua_Luainterface_Player_getSaveText00);
	tolua_function(tolua_S, "setCurrentHealth", tolua_Luainterface_Player_setCurrentHealth00);
	tolua_function(tolua_S, "setCurrentMana", tolua_Luainterface_Player_setCurrentMana00);
	tolua_function(tolua_S, "setCurrentFatigue", tolua_Luainterface_Player_setCurrentFatigue00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "Npc", "Npc", "Character", NULL);
	tolua_beginmodule(tolua_S, "Npc");
	tolua_function(tolua_S, "setAttitude", tolua_Luainterface_Npc_setAttitude00);
	tolua_function(tolua_S, "addOnDieEventHandler", tolua_Luainterface_Npc_addOnDieEventHandler00);
	tolua_function(tolua_S, "removeOnDieEventHandler", tolua_Luainterface_Npc_removeOnDieEventHandler00);
	tolua_function(tolua_S, "setWanderRadius", tolua_Luainterface_Npc_setWanderRadius00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "InteractionPoint", "InteractionPoint", "", NULL);
	tolua_beginmodule(tolua_S, "InteractionPoint");
	tolua_function(tolua_S, "setPosition", tolua_Luainterface_InteractionPoint_setPosition00);
	tolua_function(tolua_S, "setInteractionType", tolua_Luainterface_InteractionPoint_setInteractionType00);
	tolua_function(tolua_S, "setBackgroundTexture", tolua_Luainterface_InteractionPoint_setBackgroundTexture00);
	tolua_function(tolua_S, "setInteractionCode", tolua_Luainterface_InteractionPoint_setInteractionCode00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "InteractionRegion", "InteractionRegion", "", NULL);
	tolua_beginmodule(tolua_S, "InteractionRegion");
	tolua_function(tolua_S, "setPosition", tolua_Luainterface_InteractionRegion_setPosition00);
	tolua_function(tolua_S, "setOnEnterText", tolua_Luainterface_InteractionRegion_setOnEnterText00);
	tolua_function(tolua_S, "setOnLeaveText", tolua_Luainterface_InteractionRegion_setOnLeaveText00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "TextureAtlasCreator", "TextureAtlasCreator", "", NULL);
	tolua_beginmodule(tolua_S, "TextureAtlasCreator");
	tolua_function(tolua_S, "Get", tolua_Luainterface_TextureAtlasCreator_Get00);
	tolua_function(tolua_S, "init", tolua_Luainterface_TextureAtlasCreator_init00);
	tolua_function(tolua_S, "addFrame", tolua_Luainterface_TextureAtlasCreator_addFrame00);
	tolua_function(tolua_S, "getAtlas", tolua_Luainterface_TextureAtlasCreator_getAtlas00);
	tolua_function(tolua_S, "resetLine", tolua_Luainterface_TextureAtlasCreator_resetLine00);
	tolua_function(tolua_S, "getName", tolua_Luainterface_TextureAtlasCreator_getName00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "TextureManager", "TextureManager", "", NULL);
	tolua_beginmodule(tolua_S, "TextureManager");
	tolua_function(tolua_S, "SetTextureAtlas", tolua_Luainterface_TextureManager_SetTextureAtlas00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "Zone", "Zone", "", NULL);
	tolua_beginmodule(tolua_S, "Zone");
	tolua_function(tolua_S, "loadZone", tolua_Luainterface_Zone_loadZone00);
	tolua_function(tolua_S, "setInit", tolua_Luainterface_Zone_setInit00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "SpellActionBase", "SpellActionBase", "", NULL);
	tolua_beginmodule(tolua_S, "SpellActionBase");
	tolua_function(tolua_S, "addAdditionalSpellOnTarget", tolua_Luainterface_SpellActionBase_addAdditionalSpellOnTarget00);
	tolua_function(tolua_S, "addAdditionalSpellOnCreator", tolua_Luainterface_SpellActionBase_addAdditionalSpellOnCreator00);
	tolua_function(tolua_S, "setRequiredClass", tolua_Luainterface_SpellActionBase_setRequiredClass00);
	tolua_function(tolua_S, "setRequiredLevel", tolua_Luainterface_SpellActionBase_setRequiredLevel00);
	tolua_function(tolua_S, "addRequiredWeapon", tolua_Luainterface_SpellActionBase_addRequiredWeapon00);
	tolua_function(tolua_S, "setSoundSpellCasting", tolua_Luainterface_SpellActionBase_setSoundSpellCasting00);
	tolua_function(tolua_S, "setSoundSpellStart", tolua_Luainterface_SpellActionBase_setSoundSpellStart00);
	tolua_function(tolua_S, "setSoundSpellHit", tolua_Luainterface_SpellActionBase_setSoundSpellHit00);
	tolua_function(tolua_S, "setRank", tolua_Luainterface_SpellActionBase_setRank00);
	tolua_function(tolua_S, "setInstant", tolua_Luainterface_SpellActionBase_setInstant00);
	tolua_function(tolua_S, "setCharacterState", tolua_Luainterface_SpellActionBase_setCharacterState00);
	tolua_function(tolua_S, "setSymbolTextureRect", tolua_Luainterface_SpellActionBase_setSymbolTextureRect00);
	tolua_function(tolua_S, "setNeedTarget", tolua_Luainterface_SpellActionBase_setNeedTarget00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "Action", "Action", "SpellActionBase", NULL);
	tolua_beginmodule(tolua_S, "Action");
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "Spell", "Spell", "SpellActionBase", NULL);
	tolua_beginmodule(tolua_S, "Spell");
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "ConfigurableSpell", "ConfigurableSpell", "Spell", NULL);
	tolua_beginmodule(tolua_S, "ConfigurableSpell");
	tolua_function(tolua_S, "setCastTime", tolua_Luainterface_ConfigurableSpell_setCastTime00);
	tolua_function(tolua_S, "setCooldown", tolua_Luainterface_ConfigurableSpell_setCooldown00);
	tolua_function(tolua_S, "setSpellCost", tolua_Luainterface_ConfigurableSpell_setSpellCost00);
	tolua_function(tolua_S, "setName", tolua_Luainterface_ConfigurableSpell_setName00);
	tolua_function(tolua_S, "setInfo", tolua_Luainterface_ConfigurableSpell_setInfo00);
	tolua_function(tolua_S, "setRange", tolua_Luainterface_ConfigurableSpell_setRange00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "ConfigurableAction", "ConfigurableAction", "Action", NULL);
	tolua_beginmodule(tolua_S, "ConfigurableAction");
	tolua_function(tolua_S, "setCastTime", tolua_Luainterface_ConfigurableAction_setCastTime00);
	tolua_function(tolua_S, "setCooldown", tolua_Luainterface_ConfigurableAction_setCooldown00);
	tolua_function(tolua_S, "setSpellCost", tolua_Luainterface_ConfigurableAction_setSpellCost00);
	tolua_function(tolua_S, "setName", tolua_Luainterface_ConfigurableAction_setName00);
	tolua_function(tolua_S, "setInfo", tolua_Luainterface_ConfigurableAction_setInfo00);
	tolua_function(tolua_S, "setRange", tolua_Luainterface_ConfigurableAction_setRange00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "GeneralDamageSpell", "GeneralDamageSpell", "ConfigurableSpell", NULL);
	tolua_beginmodule(tolua_S, "GeneralDamageSpell");
	tolua_function(tolua_S, "setDirectDamage", tolua_Luainterface_GeneralDamageSpell_setDirectDamage00);
	tolua_function(tolua_S, "setContinuousDamage", tolua_Luainterface_GeneralDamageSpell_setContinuousDamage00);
	tolua_function(tolua_S, "addAnimationFrame", tolua_Luainterface_GeneralDamageSpell_addAnimationFrame00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "GeneralRayDamageSpell", "GeneralRayDamageSpell", "GeneralDamageSpell", NULL);
	tolua_beginmodule(tolua_S, "GeneralRayDamageSpell");
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "GeneralAreaDamageSpell", "GeneralAreaDamageSpell", "GeneralDamageSpell", NULL);
	tolua_beginmodule(tolua_S, "GeneralAreaDamageSpell");
	tolua_function(tolua_S, "setRadius", tolua_Luainterface_GeneralAreaDamageSpell_setRadius00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "GeneralBoltDamageSpell", "GeneralBoltDamageSpell", "GeneralDamageSpell", NULL);
	tolua_beginmodule(tolua_S, "GeneralBoltDamageSpell");
	tolua_function(tolua_S, "setMoveSpeed", tolua_Luainterface_GeneralBoltDamageSpell_setMoveSpeed00);
	tolua_function(tolua_S, "setExpireTime", tolua_Luainterface_GeneralBoltDamageSpell_setExpireTime00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "GeneralHealingSpell", "GeneralHealingSpell", "ConfigurableSpell", NULL);
	tolua_beginmodule(tolua_S, "GeneralHealingSpell");
	tolua_function(tolua_S, "setEffectType", tolua_Luainterface_GeneralHealingSpell_setEffectType00);
	tolua_function(tolua_S, "setDirectHealing", tolua_Luainterface_GeneralHealingSpell_setDirectHealing00);
	tolua_function(tolua_S, "setContinuousHealing", tolua_Luainterface_GeneralHealingSpell_setContinuousHealing00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "GeneralBuffSpell", "GeneralBuffSpell", "ConfigurableSpell", NULL);
	tolua_beginmodule(tolua_S, "GeneralBuffSpell");
	tolua_function(tolua_S, "setEffectType", tolua_Luainterface_GeneralBuffSpell_setEffectType00);
	tolua_function(tolua_S, "setDuration", tolua_Luainterface_GeneralBuffSpell_setDuration00);
	tolua_function(tolua_S, "setStats", tolua_Luainterface_GeneralBuffSpell_setStats00);
	tolua_function(tolua_S, "setResistElementModifierPoints", tolua_Luainterface_GeneralBuffSpell_setResistElementModifierPoints00);
	tolua_function(tolua_S, "setSpellEffectElementModifierPoints", tolua_Luainterface_GeneralBuffSpell_setSpellEffectElementModifierPoints00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "MeleeDamageAction", "MeleeDamageAction", "ConfigurableAction", NULL);
	tolua_beginmodule(tolua_S, "MeleeDamageAction");
	tolua_function(tolua_S, "setDamageBonus", tolua_Luainterface_MeleeDamageAction_setDamageBonus00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "RangedDamageAction", "RangedDamageAction", "ConfigurableAction", NULL);
	tolua_beginmodule(tolua_S, "RangedDamageAction");
	tolua_function(tolua_S, "setDamageBonus", tolua_Luainterface_RangedDamageAction_setDamageBonus00);
	tolua_function(tolua_S, "setMoveSpeed", tolua_Luainterface_RangedDamageAction_setMoveSpeed00);
	tolua_function(tolua_S, "setExpireTime", tolua_Luainterface_RangedDamageAction_setExpireTime00);
	tolua_function(tolua_S, "addAnimationFrame", tolua_Luainterface_RangedDamageAction_addAnimationFrame00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "Item", "Item", "", NULL);
	tolua_beginmodule(tolua_S, "Item");
	tolua_function(tolua_S, "setDescription", tolua_Luainterface_Item_setDescription00);
	tolua_function(tolua_S, "setStats", tolua_Luainterface_Item_setStats00);
	tolua_function(tolua_S, "setResistElementModifierPoints", tolua_Luainterface_Item_setResistElementModifierPoints00);
	tolua_function(tolua_S, "setSpellEffectElementModifierPoints", tolua_Luainterface_Item_setSpellEffectElementModifierPoints00);
	tolua_function(tolua_S, "setMinDamage", tolua_Luainterface_Item_setMinDamage00);
	tolua_function(tolua_S, "setMaxDamage", tolua_Luainterface_Item_setMaxDamage00);
	tolua_function(tolua_S, "setRequiredLevel", tolua_Luainterface_Item_setRequiredLevel00);
	tolua_function(tolua_S, "setMaxStackSize", tolua_Luainterface_Item_setMaxStackSize00);
	tolua_function(tolua_S, "setValue", tolua_Luainterface_Item_setValue00);
	tolua_function(tolua_S, "setSpell", tolua_Luainterface_Item_setSpell00);
	tolua_function(tolua_S, "setItemTextureRect", tolua_Luainterface_Item_setItemTextureRect00);
	tolua_function(tolua_S, "addTriggerSpellOnSelf", tolua_Luainterface_Item_addTriggerSpellOnSelf00);
	tolua_function(tolua_S, "addTriggerSpellOnTarget", tolua_Luainterface_Item_addTriggerSpellOnTarget00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "Shop", "Shop", "", NULL);
	tolua_beginmodule(tolua_S, "Shop");
	tolua_function(tolua_S, "addItem", tolua_Luainterface_Shop_addItem00);
	tolua_function(tolua_S, "loadShopkeeperInventory", tolua_Luainterface_Shop_loadShopkeeperInventory00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "TextWindow", "TextWindow", "", NULL);
	tolua_beginmodule(tolua_S, "TextWindow");
	tolua_function(tolua_S, "setText", tolua_Luainterface_TextWindow_setText00);
	tolua_function(tolua_S, "setAutocloseTime", tolua_Luainterface_TextWindow_setAutocloseTime00);
	tolua_function(tolua_S, "setPosition", tolua_Luainterface_TextWindow_setPosition00);
	tolua_function(tolua_S, "center", tolua_Luainterface_TextWindow_center00);
	tolua_function(tolua_S, "setOnCloseText", tolua_Luainterface_TextWindow_setOnCloseText00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "Quest", "Quest", "", NULL);
	tolua_beginmodule(tolua_S, "Quest");
	tolua_function(tolua_S, "addRequiredItemForCompletion", tolua_Luainterface_Quest_addRequiredItemForCompletion00);
	tolua_function(tolua_S, "setDescription", tolua_Luainterface_Quest_setDescription00);
	tolua_function(tolua_S, "setExperienceReward", tolua_Luainterface_Quest_setExperienceReward00);
	tolua_function(tolua_S, "setCoinReward", tolua_Luainterface_Quest_setCoinReward00);
	tolua_function(tolua_S, "setItemReward", tolua_Luainterface_Quest_setItemReward00);
	tolua_function(tolua_S, "finishQuest", tolua_Luainterface_Quest_finishQuest00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "CallIndirection", "CallIndirection", "", NULL);
	tolua_beginmodule(tolua_S, "CallIndirection");
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "LuaCallIndirection", "LuaCallIndirection", "CallIndirection", NULL);
	tolua_beginmodule(tolua_S, "LuaCallIndirection");
	tolua_function(tolua_S, "setExecuteText", tolua_Luainterface_LuaCallIndirection_setExecuteText00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "ShopCanvas", "ShopCanvas", "", NULL);
	tolua_beginmodule(tolua_S, "ShopCanvas");
	tolua_function(tolua_S, "Get", tolua_Luainterface_ShopCanvas_Get00);
	tolua_function(tolua_S, "activate", tolua_Luainterface_ShopCanvas_activate00);
	tolua_endmodule(tolua_S);
	tolua_endmodule(tolua_S);
	return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
TOLUA_API int luaopen_Luainterface(lua_State* tolua_S) {
	return tolua_Luainterface_open(tolua_S);
};
#endif

