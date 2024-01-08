#pragma once

#include <engine/MeshObject/Shape.h>
#include <engine/ObjModel.h>
#include <Systems/System.h>
#include <Components/TransformComp.h>
#include <Components/MeshComp.h>
#include <Components/Behaviour.h>
#include <Components/UIElement.h>

#include "LuaHelper.h"
#include "Resources.h"

class CollisionSystem : public System{

public:

	CollisionSystem(lua_State* L, Resources* resource) : L(L), resource(resource){}

	bool update(entt::registry& reg, float deltaTime) final{

		auto view = reg.view<Behaviour, MeshComp, TransformComp>();
		view.each([&](Behaviour& script1, MeshComp& mesh1, TransformComp& transform1){

			auto view2 = reg.view<Behaviour, MeshComp, TransformComp>();
			view2.each([&](Behaviour& script2, MeshComp& mesh2, TransformComp& transform2){

				/*std::cout << "Name: " << mesh1.modelName << "  " << mesh2.modelName << std::endl;



				ObjModel* model1 = resource->getModel(mesh1.modelName);
				ObjModel* model2 = resource->getModel(mesh2.modelName);
				if (!model1 || !model2)
					return;*/

				BoundingBox& box1 = strcmp(mesh1.modelName, "Bullet") == 0 ? resource->getShape(mesh1.modelName)->getAABB() : resource->getModel(mesh1.modelName)->getAABB();
				//box1.position += transform1.position;
				
				BoundingBox& box2 = strcmp(mesh2.modelName, "Bullet") == 0 ? resource->getShape(mesh2.modelName)->getAABB() : resource->getModel(mesh2.modelName)->getAABB();
				//box1.position += transform2.position;
				
				if (&script1 == &script2 || !CheckCollisionBoxes(box1, transform1.position, box2, transform2.position)) {
					return;
				}

				lua_rawgeti(L, LUA_REGISTRYINDEX, script1.luaRef);
				if (luaL_dofile(L, ("Scripts/Behaviour/" + std::string(script1.path)).c_str()) != LUA_OK)
					LuaHelper::dumpError(L);
				else{
					lua_getfield(L, -1, "collision"); // Get new collision function
					lua_setfield(L, -3, "collision"); // Set instance collision function to the new one
					lua_pop(L, 1);
				}

				lua_getfield(L, -1, "collision");
				if (lua_type(L, -1) == LUA_TNIL){
					lua_pop(L, 1);
				}else{
					lua_pushvalue(L, -2);
					lua_rawgeti(L, LUA_REGISTRYINDEX, script2.luaRef);

					if (lua_pcall(L, 2, 0, 0) != LUA_OK)
						LuaHelper::dumpError(L);
					else
						lua_pop(L, 1);
				}

				lua_rawgeti(L, LUA_REGISTRYINDEX, script2.luaRef);
				if (luaL_dofile(L, ("Scripts/Behaviour/" + std::string(script2.path)).c_str()) != LUA_OK)
					LuaHelper::dumpError(L);
				else{
					lua_getfield(L, -1, "collision"); // Get new collision function
					lua_setfield(L, -3, "collision"); // Set instance collision function to the new one
					lua_pop(L, 1);
				}

				lua_getfield(L, -1, "collision");
				if (lua_type(L, -1) == LUA_TNIL){
					lua_pop(L, 1);
				}else{
					lua_pushvalue(L, -2);
					lua_rawgeti(L, LUA_REGISTRYINDEX, script1.luaRef);

					if (lua_pcall(L, 2, 0, 0) != LUA_OK)
						LuaHelper::dumpError(L);
					else
						lua_pop(L, 1);
				}
			});
		});

		return false;
	}

	bool CheckCollisionBoxes(const BoundingBox& box1, const Vector3f pos1, const BoundingBox& box2, const Vector3f pos2){
		/*bool collision = true;

		if ((box1.max.x >= box2.min.x) && (box1.min.x <= box2.max.x)){
			if ((box1.max.y < box2.min.y) || (box1.min.y > box2.max.y)) collision = false;
			if ((box1.max.z < box2.min.z) || (box1.min.z > box2.max.z)) collision = false;
		}
		else collision = false;

		return collision;*/

		return (
			pos1[0] + box1.position[0]				  <= pos2[0] + box2.position[0] + box2.size[0] &&
			pos1[0] + box1.position[0] + box1.size[0] >= pos2[0] + box2.position[0] &&
			pos1[1] + box1.position[1]				  <= pos2[1] + box2.position[1] + box2.size[1] &&
			pos1[1] + box1.position[1] + box1.size[1] >= pos2[1] + box2.position[1] &&
			pos1[2] + box1.position[2]				  <= pos2[2] + box2.position[2] + box2.size[2] &&
			pos1[2] + box1.position[2] + box1.size[2] >= pos2[2] + box2.position[2]
			);
	}

	private:
		lua_State* L;
		Resources* resource;
};