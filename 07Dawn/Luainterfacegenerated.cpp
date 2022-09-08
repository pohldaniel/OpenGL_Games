/*
** Lua binding: Luainterface
** Generated automatically by tolua++-1.0.92 on Mon Sep  5 16:32:05 2022.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_Luainterface_open(lua_State* tolua_S);

#include "luainterface.h"

/* function to register type */
static void tolua_reg_types(lua_State* tolua_S)
{
	tolua_usertype(tolua_S, "AdjacencyEquivalenceClass");
	tolua_usertype(tolua_S, "TileSet");
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

/* function: EditorInterface::getTileSet */
#ifndef TOLUA_DISABLE_tolua_Luainterface_EditorInterface_getTileSet00
static int tolua_Luainterface_EditorInterface_getTileSet00(lua_State* tolua_S)
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
			TileSet* tolua_ret = (TileSet*)EditorInterface::getTileSet();
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

/* Open function */
TOLUA_API int tolua_Luainterface_open(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	tolua_reg_types(tolua_S);
	tolua_module(tolua_S, NULL, 0);
	tolua_beginmodule(tolua_S, NULL);
	tolua_module(tolua_S, "DawnInterface", 0);
	tolua_beginmodule(tolua_S, "DawnInterface");
	tolua_function(tolua_S, "enterZone", tolua_Luainterface_DawnInterface_enterZone00);
	tolua_function(tolua_S, "setCurrentZone", tolua_Luainterface_DawnInterface_setCurrentZone00);
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
	tolua_module(tolua_S, "EditorInterface", 0);
	tolua_beginmodule(tolua_S, "EditorInterface");
	tolua_function(tolua_S, "getTileSet", tolua_Luainterface_EditorInterface_getTileSet00);
	tolua_function(tolua_S, "addGroundTile", tolua_Luainterface_EditorInterface_addGroundTile00);
	tolua_function(tolua_S, "addEnvironment", tolua_Luainterface_EditorInterface_addEnvironment00);
	tolua_function(tolua_S, "adjustLastRGBA", tolua_Luainterface_EditorInterface_adjustLastRGBA00);
	tolua_function(tolua_S, "addCollisionRect", tolua_Luainterface_EditorInterface_addCollisionRect00);
	tolua_endmodule(tolua_S);
	tolua_endmodule(tolua_S);
	return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
TOLUA_API int luaopen_Luainterface(lua_State* tolua_S) {
	return tolua_Luainterface_open(tolua_S);
};
#endif

