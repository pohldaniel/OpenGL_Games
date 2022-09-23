/*
** Lua binding: Luainterface
** Generated automatically by tolua++-1.0.92 on Fri Sep 23 01:58:28 2022.
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

/* function to register type */
static void tolua_reg_types(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "CCharacter");
	tolua_usertype(tolua_S, "TileSet");
	tolua_usertype(tolua_S, "AdjacencyEquivalenceClass");
	tolua_usertype(tolua_S, "TextureManager");
	tolua_usertype(tolua_S, "TextureAtlasCreator");
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
			CCharacter* tolua_ret = (CCharacter*)DawnInterface::createNewMobType(typeID);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "CCharacter");
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
		TileClassificationType::TileClassificationType tileType = ((TileClassificationType::TileClassificationType) (int)  tolua_tonumber(tolua_S, 1, 0));
		{
			TileSet* tolua_ret = (TileSet*)EditorInterface::getTileSet(tileType);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "TileSet");
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
		double red = ((double)tolua_tonumber(tolua_S, 1, 0));
		double green = ((double)tolua_tonumber(tolua_S, 2, 0));
		double blue = ((double)tolua_tonumber(tolua_S, 3, 0));
		double alpha = ((double)tolua_tonumber(tolua_S, 4, 0));
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
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TileSet* self = (TileSet*)tolua_tousertype(tolua_S, 1, 0);
		std::string filename = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
		TileClassificationType::TileClassificationType tileType = ((TileClassificationType::TileClassificationType) (int)  tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addTile'", NULL);
#endif
		{
			int tolua_ret = (int)self->addTile(filename, tileType);
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
		!tolua_isnumber(tolua_S, 7, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 8, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		TileSet* self = (TileSet*)tolua_tousertype(tolua_S, 1, 0);
		std::string filename = ((std::string)  tolua_tocppstring(tolua_S, 2, 0));
		TileClassificationType::TileClassificationType tileType = ((TileClassificationType::TileClassificationType) (int)  tolua_tonumber(tolua_S, 3, 0));
		int cbx = ((int)tolua_tonumber(tolua_S, 4, 0));
		int cby = ((int)tolua_tonumber(tolua_S, 5, 0));
		int cbw = ((int)tolua_tonumber(tolua_S, 6, 0));
		int cbh = ((int)tolua_tonumber(tolua_S, 7, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'addTileWithCollisionBox'", NULL);
#endif
		{
			int tolua_ret = (int)self->addTileWithCollisionBox(filename, tileType, cbx, cby, cbw, cbh);
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
		int tile1 = ((int)tolua_tonumber(tolua_S, 2, 0));
		AdjacencyType::AdjacencyType adjacencyType = ((AdjacencyType::AdjacencyType) (int)  tolua_tonumber(tolua_S, 3, 0));
		int tile2 = ((int)tolua_tonumber(tolua_S, 4, 0));
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
		AdjacencyType::AdjacencyType adjacencyType = ((AdjacencyType::AdjacencyType) (int)  tolua_tonumber(tolua_S, 3, 0));
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

/* method: setNumMoveTexturesPerDirection of class  CCharacter */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CCharacter_setNumMoveTexturesPerDirection00
static int tolua_Luainterface_CCharacter_setNumMoveTexturesPerDirection00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CCharacter", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 4, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CCharacter* self = (CCharacter*)tolua_tousertype(tolua_S, 1, 0);
		ActivityType::ActivityType activity = ((ActivityType::ActivityType) (int)  tolua_tonumber(tolua_S, 2, 0));
		int numTextures = ((int)tolua_tonumber(tolua_S, 3, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setNumMoveTexturesPerDirection'", NULL);
#endif
		{
			self->setNumMoveTexturesPerDirection(activity, numTextures);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setNumMoveTexturesPerDirection'.", &tolua_err);
				 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMoveTexture of class  CCharacter */
#ifndef TOLUA_DISABLE_tolua_Luainterface_CCharacter_setMoveTexture00
static int tolua_Luainterface_CCharacter_setMoveTexture00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "CCharacter", 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 3, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 4, 0, &tolua_err) ||
		!tolua_iscppstring(tolua_S, 5, 0, &tolua_err) ||
		!tolua_isnumber(tolua_S, 6, 1, &tolua_err) ||
		!tolua_isnumber(tolua_S, 7, 1, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 8, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		CCharacter* self = (CCharacter*)tolua_tousertype(tolua_S, 1, 0);
		ActivityType::ActivityType activity = ((ActivityType::ActivityType) (int)  tolua_tonumber(tolua_S, 2, 0));
		int direction = ((int)tolua_tonumber(tolua_S, 3, 0));
		int index = ((int)tolua_tonumber(tolua_S, 4, 0));
		std::string filename = ((std::string)  tolua_tocppstring(tolua_S, 5, 0));
		int textureOffsetX = ((int)tolua_tonumber(tolua_S, 6, 0));
		int textureOffsetY = ((int)tolua_tonumber(tolua_S, 7, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'setMoveTexture'", NULL);
#endif
		{
			self->setMoveTexture(activity, direction, index, filename, textureOffsetX, textureOffsetY);
		}
	}
	return 0;
#ifndef TOLUA_RELEASE
	tolua_lerror :
				 tolua_error(tolua_S, "#ferror in function 'setMoveTexture'.", &tolua_err);
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
	tolua_constant(tolua_S, "STOP", STOP);
	tolua_constant(tolua_S, "N", N);
	tolua_constant(tolua_S, "NE", NE);
	tolua_constant(tolua_S, "E", E);
	tolua_constant(tolua_S, "SE", SE);
	tolua_constant(tolua_S, "S", S);
	tolua_constant(tolua_S, "SW", SW);
	tolua_constant(tolua_S, "W", W);
	tolua_constant(tolua_S, "NW", NW);
	tolua_module(tolua_S, "ActivityType", 0);
	tolua_beginmodule(tolua_S, "ActivityType");
	tolua_constant(tolua_S, "Walking", ActivityType::Walking);
	tolua_constant(tolua_S, "Casting", ActivityType::Casting);
	tolua_constant(tolua_S, "Attacking", ActivityType::Attacking);
	tolua_constant(tolua_S, "Shooting", ActivityType::Shooting);
	tolua_constant(tolua_S, "Dying", ActivityType::Dying);
	tolua_constant(tolua_S, "Count", ActivityType::Count);
	tolua_endmodule(tolua_S);
	tolua_module(tolua_S, "DawnInterface", 0);
	tolua_beginmodule(tolua_S, "DawnInterface");
	tolua_function(tolua_S, "enterZone", tolua_Luainterface_DawnInterface_enterZone00);
	tolua_function(tolua_S, "setCurrentZone", tolua_Luainterface_DawnInterface_setCurrentZone00);
	tolua_function(tolua_S, "createNewMobType", tolua_Luainterface_DawnInterface_createNewMobType00);
	tolua_endmodule(tolua_S);
	tolua_module(tolua_S, "EditorInterface", 0);
	tolua_beginmodule(tolua_S, "EditorInterface");
	tolua_function(tolua_S, "getTileSet", tolua_Luainterface_EditorInterface_getTileSet00);
	tolua_function(tolua_S, "addGroundTile", tolua_Luainterface_EditorInterface_addGroundTile00);
	tolua_function(tolua_S, "addEnvironment", tolua_Luainterface_EditorInterface_addEnvironment00);
	tolua_function(tolua_S, "adjustLastRGBA", tolua_Luainterface_EditorInterface_adjustLastRGBA00);
	tolua_function(tolua_S, "addCollisionRect", tolua_Luainterface_EditorInterface_addCollisionRect00);
	tolua_endmodule(tolua_S);
	tolua_module(tolua_S, "TileClassificationType", 0);
	tolua_beginmodule(tolua_S, "TileClassificationType");
	tolua_constant(tolua_S, "FLOOR", TileClassificationType::FLOOR);
	tolua_constant(tolua_S, "ENVIRONMENT", TileClassificationType::ENVIRONMENT);
	tolua_constant(tolua_S, "SHADOW", TileClassificationType::SHADOW);
	tolua_endmodule(tolua_S);
	tolua_module(tolua_S, "AdjacencyType", 0);
	tolua_beginmodule(tolua_S, "AdjacencyType");
	tolua_constant(tolua_S, "RIGHT", AdjacencyType::RIGHT);
	tolua_constant(tolua_S, "LEFT", AdjacencyType::LEFT);
	tolua_constant(tolua_S, "TOP", AdjacencyType::TOP);
	tolua_constant(tolua_S, "BOTTOM", AdjacencyType::BOTTOM);
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
	tolua_cclass(tolua_S, "CCharacter", "CCharacter", "", NULL);
	tolua_beginmodule(tolua_S, "CCharacter");
	tolua_function(tolua_S, "setNumMoveTexturesPerDirection", tolua_Luainterface_CCharacter_setNumMoveTexturesPerDirection00);
	tolua_function(tolua_S, "setMoveTexture", tolua_Luainterface_CCharacter_setMoveTexture00);
	tolua_endmodule(tolua_S);
	tolua_cclass(tolua_S, "TextureAtlasCreator", "TextureAtlasCreator", "", NULL);
	tolua_beginmodule(tolua_S, "TextureAtlasCreator");
	tolua_function(tolua_S, "get", tolua_Luainterface_TextureAtlasCreator_get00);
	tolua_function(tolua_S, "init", tolua_Luainterface_TextureAtlasCreator_init00);
	tolua_function(tolua_S, "getAtlas", tolua_Luainterface_TextureAtlasCreator_getAtlas00);
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

