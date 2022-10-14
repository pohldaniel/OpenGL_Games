/*
** Lua binding: Luainterface
** Generated automatically by tolua++-1.0.92 on Fri Oct 14 23:26:51 2022.
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

/* function to register type */
static void tolua_reg_types(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "TextureManager");
	tolua_usertype(tolua_S, "TileSet");
	tolua_usertype(tolua_S, "InteractionPoint");
	tolua_usertype(tolua_S, "Npc");
	tolua_usertype(tolua_S, "TextureAtlasCreator");
	tolua_usertype(tolua_S, "Character");
	tolua_usertype(tolua_S, "InteractionRegion");
	tolua_usertype(tolua_S, "AdjacencyEquivalenceClass");
	tolua_usertype(tolua_S, "TextWindow");
	tolua_usertype(tolua_S, "CharacterType");
	tolua_usertype(tolua_S, "Player");
}

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
			DawnInterface::addMobSpawnPoint(mobID, name, x_pos, y_pos, respawn_rate, do_respawn, attitude);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'addMobSpawnPoint'.", &tolua_err);
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
		!tolua_isusertype(tolua_S, 1, "Character", 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 2, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		Character* character = ((Character*)tolua_tousertype(tolua_S, 1, 0));
		{
			std::string tolua_ret = (std::string)  DawnInterface::getItemReferenceRestore(character);
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
		!tolua_isboolean(tolua_S, 8, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 9, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 10, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 11, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 12, 1, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 13, &tolua_err)
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
		bool reload = ((bool)tolua_toboolean(tolua_S, 8, false));
		int paddingLeft = ((int)tolua_tonumber(tolua_S, 9, 0));
		int paddingRight = ((int)tolua_tonumber(tolua_S, 10, 0));
		int paddingTop = ((int)tolua_tonumber(tolua_S, 11, 0));
		int paddingBottom = ((int)tolua_tonumber(tolua_S, 12, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addMoveTexture'", NULL);
#endif
		{
			self->addMoveTexture(activity, direction, index, filename, maxWidth, maxHeight, reload, paddingLeft, paddingRight, paddingTop, paddingBottom);
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

/* method: init of class  Player */
#ifndef TOLUA_DISABLE_tolua_Luainterface_Player_init00
static int tolua_Luainterface_Player_init00(lua_State* tolua_S)
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
		int x = ((int)tolua_tonumber(tolua_S, 2, 0));
		int y = ((int)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'init'", NULL);
#endif
		{
			self->init(x, y);
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
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		InteractionPoint* self = (InteractionPoint*)tolua_tousertype(tolua_S, 1, 0);
		std::string texturename = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setBackgroundTexture'", NULL);
#endif
		{
			self->setBackgroundTexture(texturename);
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

/* method: get of class  TextureAtlasCreator */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextureAtlasCreator_get00
static int tolua_Luainterface_TextureAtlasCreator_get00(lua_State* tolua_S)
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
			TextureAtlasCreator& tolua_ret = (TextureAtlasCreator&)TextureAtlasCreator::get();
			tolua_pushusertype(tolua_S, (void*)&tolua_ret, "TextureAtlasCreator");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'get'.", &tolua_err);
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
		!tolua_isnumber(tolua_S, 2, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 1, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TextureAtlasCreator* self = (TextureAtlasCreator*)tolua_tousertype(tolua_S, 1, 0);
		unsigned int width = ((unsigned int)tolua_tonumber(tolua_S, 2, 1024u));
		unsigned int height = ((unsigned int)tolua_tonumber(tolua_S, 3, 1024u));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'init'", NULL);
#endif
		{
			self->init(width, height);
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

/* method: safeAtlas of class  TextureAtlasCreator */
#ifndef TOLUA_DISABLE_tolua_Luainterface_TextureAtlasCreator_safeAtlas00
static int tolua_Luainterface_TextureAtlasCreator_safeAtlas00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "TextureAtlasCreator", 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TextureAtlasCreator* self = (TextureAtlasCreator*)tolua_tousertype(tolua_S, 1, 0);
		std::string name = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'safeAtlas'", NULL);
#endif
		{
			self->safeAtlas(name);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'safeAtlas'.", &tolua_err);
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
	tolua_endmodule(tolua_S);
	tolua_module(tolua_S, "DawnInterface", 0);
	tolua_beginmodule(tolua_S, "DawnInterface");
	tolua_function(tolua_S, "enterZone", tolua_Luainterface_DawnInterface_enterZone00);
	tolua_function(tolua_S, "setCurrentZone", tolua_Luainterface_DawnInterface_setCurrentZone00);
	tolua_function(tolua_S, "createNewMobType", tolua_Luainterface_DawnInterface_createNewMobType00);
	tolua_function(tolua_S, "addMobSpawnPoint", tolua_Luainterface_DawnInterface_addMobSpawnPoint00);
	tolua_function(tolua_S, "addInteractionRegion", tolua_Luainterface_DawnInterface_addInteractionRegion00);
	tolua_function(tolua_S, "removeInteractionRegion", tolua_Luainterface_DawnInterface_removeInteractionRegion00);
	tolua_function(tolua_S, "addInteractionPoint", tolua_Luainterface_DawnInterface_addInteractionPoint00);
	tolua_function(tolua_S, "addCharacterInteractionPoint", tolua_Luainterface_DawnInterface_addCharacterInteractionPoint00);
	tolua_function(tolua_S, "removeInteractionPoint", tolua_Luainterface_DawnInterface_removeInteractionPoint00);
	tolua_function(tolua_S, "getItemReferenceRestore", tolua_Luainterface_DawnInterface_getItemReferenceRestore00);
	tolua_function(tolua_S, "createTextWindow", tolua_Luainterface_DawnInterface_createTextWindow00);
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
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "Player", "Player", "Character", NULL);
	tolua_beginmodule(tolua_S, "Player");
	tolua_function(tolua_S, "Get", tolua_Luainterface_Player_Get00);
	tolua_function(tolua_S, "setBoundingBox", tolua_Luainterface_Player_setBoundingBox00);
	tolua_function(tolua_S, "setMaxHealth", tolua_Luainterface_Player_setMaxHealth00);
	tolua_function(tolua_S, "setMaxMana", tolua_Luainterface_Player_setMaxMana00);
	tolua_function(tolua_S, "setMaxFatigue", tolua_Luainterface_Player_setMaxFatigue00);
	tolua_function(tolua_S, "setStrength", tolua_Luainterface_Player_setStrength00);
	tolua_function(tolua_S, "setVitality", tolua_Luainterface_Player_setVitality00);
	tolua_function(tolua_S, "setDexterity", tolua_Luainterface_Player_setDexterity00);
	tolua_function(tolua_S, "setWisdom", tolua_Luainterface_Player_setWisdom00);
	tolua_function(tolua_S, "setIntellect", tolua_Luainterface_Player_setIntellect00);
	tolua_function(tolua_S, "setHealthRegen", tolua_Luainterface_Player_setHealthRegen00);
	tolua_function(tolua_S, "setManaRegen", tolua_Luainterface_Player_setManaRegen00);
	tolua_function(tolua_S, "setFatigueRegen", tolua_Luainterface_Player_setFatigueRegen00);
	tolua_function(tolua_S, "giveCoins", tolua_Luainterface_Player_giveCoins00);
	tolua_function(tolua_S, "init", tolua_Luainterface_Player_init00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "Npc", "Npc", "Character", NULL);
	tolua_beginmodule(tolua_S, "Npc");
	tolua_function(tolua_S, "setAttitude", tolua_Luainterface_Npc_setAttitude00);
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
	tolua_function(tolua_S, "get", tolua_Luainterface_TextureAtlasCreator_get00);
	tolua_function(tolua_S, "init", tolua_Luainterface_TextureAtlasCreator_init00);
	tolua_function(tolua_S, "addFrame", tolua_Luainterface_TextureAtlasCreator_addFrame00);
	tolua_function(tolua_S, "getAtlas", tolua_Luainterface_TextureAtlasCreator_getAtlas00);
	tolua_function(tolua_S, "safeAtlas", tolua_Luainterface_TextureAtlasCreator_safeAtlas00);
	tolua_function(tolua_S, "resetLine", tolua_Luainterface_TextureAtlasCreator_resetLine00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "TextureManager", "TextureManager", "", NULL);
	tolua_beginmodule(tolua_S, "TextureManager");
	tolua_function(tolua_S, "SetTextureAtlas", tolua_Luainterface_TextureManager_SetTextureAtlas00);
	tolua_endmodule(tolua_S);
	tolua_endmodule(tolua_S);
	return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
TOLUA_API int luaopen_Luainterface(lua_State* tolua_S) {
	return tolua_Luainterface_open(tolua_S);
};
#endif

