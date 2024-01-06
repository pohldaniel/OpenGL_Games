#include <engine/Batchrenderer.h>
#include <engine/Fontrenderer.h>

#include <Components/TransformComp.h>
#include <Components/Behaviour.h>
#include <Components/UIElement.h>
#include <Components/MeshComp.h>

#include "Scene.h"
#include "StringHelper.h"
#include "Application.h"
#include "Globals.h"
#include "TileSet.h"

Scene::Scene(): quit(false) {

	this->cam.cam3D = Camera();
	this->cam.cam3D.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	this->cam.cam3D.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 0.0f, 1.0f));
	//this->setCamera(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f), 90.0f);
}

Scene::~Scene() {
	for (auto& s : this->systems)
		delete s;
}

void Scene::lua_openscene(lua_State* L, Scene* scene) {
	lua_newtable(L);

	luaL_Reg methods[] = {
		{ "createSystem", lua_createSystem },
		{ "loadResource", lua_loadResource },
		{ "setScene", lua_setScene },
		{ "quit", lua_quit },
		{ "setCamera", lua_setCamera },
		{ "getCameraPos", lua_getCameraPos },
		{ "setCameraPos", lua_setCameraPos },
		{ "getCameraRot", lua_getCameraRot },
		{ "getEntityCount", lua_getEntityCount },
		{ "entityValid", lua_entityValid },
		{ "createEntity", lua_createEntity },
		{ "removeEntity", lua_removeEntity },
		{ "hasComponent", lua_hasComponent },
		{ "getComponent", lua_getComponent },
		{ "setComponent", lua_setComponent },
		{ "removeComponent", lua_removeComponent },
		{ NULL , NULL }
	};

	lua_pushlightuserdata(L, scene);
	luaL_setfuncs(L, methods, 1);
	lua_setglobal(L, "scene");

	lua_newtable(L);
	for (size_t i = 0; i < systemTypes.size(); i++){
		lua_pushnumber(L, (int)i);
		lua_setfield(L, -2, systemTypes[i].c_str());
	}
	lua_setglobal(L, "SystemType");

	lua_newtable(L);
	for (size_t i = 0; i < compTypes.size(); i++){
		lua_pushnumber(L, (int)i);
		lua_setfield(L, -2, compTypes[i].c_str());
	}
	lua_setglobal(L, "ComponentType");
}

void Scene::setScene(lua_State* L, std::string path)
{
	this->reg.clear();
	if (luaL_dofile(L, ("Scripts/Scenes/" + path).c_str()) != LUA_OK)
		LuaHelper::dumpError(L);
}

void Scene::render()
{
	/*BeginMode3D(this->cam.cam3D);

	auto view = this->reg.view<TransformComp, MeshComp>();
	view.each([&](const TransformComp& transform, const MeshComp& meshComp)
	{
		Model* model = this->resources.getModel(meshComp.modelName);
		if (model)
		{
			Matrix matTranslation = MatrixTranslate(transform.position.x, transform.position.y, transform.position.z);
			Matrix matRotation = MatrixRotateXYZ(Vector3Scale(transform.rotation, DEG2RAD));
			Matrix matScale = MatrixScale(transform.scale.x, transform.scale.y, transform.scale.z);
			Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
			DrawModelWiresEx(*model, transform.position, { 0.0f, 1.0f, 0.0f }, -transform.rotation.y, transform.scale, BLACK);

			for (int i = 0; i < model->meshCount; i++)
			{
				DrawMesh(model->meshes[i], model->materials[model->meshMaterial[i]], matTransform);
			}
		}
	});
	EndMode3D();*/


	Spritesheet::Bind(TileSetManager::Get().getTileSet("ui").getAtlas());
	auto buttonView = this->reg.view<UIElement>();
	buttonView.each([&](const UIElement& elem){
		if (elem.visibleBG) {
			const TextureRect& rect = TileSetManager::Get().getTileSet("ui").getTextureRects()[0];
			Batchrenderer::Get().addQuadAA(Vector4f(elem.position[0], Application::Height - (elem.position[1] + elem.dimensions[1]), elem.dimensions[0], elem.dimensions[1]),
				Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight),
				elem.colour / 255.0f,
				rect.frame);
		}

		std::string id = elem.fontSize == 80 ? "acme9_80" : "acme9_22";
		Fontrenderer::Get().addText(Globals::fontManager.get(id),
									elem.position[0] + (elem.dimensions[0] - Globals::fontManager.get(id).getWidth(elem.text)) * 0.5f,
									Application::Height - (elem.position[1] + (elem.dimensions[1] + Globals::fontManager.get(id).lineHeight) * 0.5f),
									elem.text, 
									Vector4f(1.0f, 1.0f, 1.0f,1.0f));
	});
	Batchrenderer::Get().drawBuffer();
	Spritesheet::Unbind();


	/*Fontrenderer::Get().bindTexture(Globals::fontManager.get("acme9_80"));
	auto buttonView = this->reg.view<UIElement>();
	buttonView.each([&](const UIElement& elem) {
		if (elem.visibleBG) {
			const TextureRect& rect = TileSetManager::Get().getTileSet("font").getTextureRects()[0];
			Batchrenderer::Get().addQuadAA(Vector4f(elem.position[0], Application::Height - (elem.position[1] + elem.dimensions[1]), elem.dimensions[0], elem.dimensions[1]),
				Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight),
				elem.colour, 
				rect.frame);
		}

		//std::string id = elem.fontSize == 80 ? "acme9_80" : "acme9_36";
		std::string id = "acme9_80";
		float scale = static_cast<float>(elem.fontSize) / 80.0f;

		Fontrenderer::Get().addText(Globals::fontManager.get(id),
			elem.position[0] + (elem.dimensions[0] - Globals::fontManager.get(id).getWidth(elem.text) * scale) * 0.5f,
			Application::Height - (elem.position[1] + (elem.dimensions[1] + Globals::fontManager.get(id).lineHeight * scale) * 0.5f),
			elem.text,
			Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
			scale);
	});
	Fontrenderer::Get().drawBuffer();
	Fontrenderer::Get().unbindTexture();*/
}

bool Scene::shouldQuit(){
	return this->quit;
}

void Scene::setCamera(const Vector3f& pos, const Vector3f& rotation, float fov){
	this->cam.cam3D.setPosition(pos);
	this->cam.rotation = rotation;
	this->cam.cam3D.setRotation(rotation[0], rotation[1], rotation[2]);
}

const Vector3f& Scene::getCameraPos() const{
	return this->cam.cam3D.getPosition();
}

void Scene::setCameraPos(const Vector3f& pos){
	this->setCamera(pos, this->cam.rotation, this->cam.cam3D.getFovXRad());
}

void Scene::setCameraRot(const Vector3f& rot){
	this->setCamera(this->cam.cam3D.getPosition(), rot, this->cam.cam3D.getFovXRad());
}

void Scene::updateSystems(float deltaTime){

	for (auto it = this->systems.begin(); it != this->systems.end();){
		if ((*it)->update(this->reg, deltaTime)){
			delete (*it);
			it = this->systems.erase(it);
		}else{
			it++;
		}
	}
}

int Scene::getEntityCount() const{
	return (int)this->reg.storage<entt::entity>()->in_use();
}

bool Scene::entityValid(int entity) const{
	return this->reg.valid((entt::entity)entity);
}

int Scene::createEntity(){
	int entity = (int)this->reg.create();
	this->setComponent<TransformComp>(entity);
	return entity;
}

bool Scene::removeEntity(int entity) {
	bool valid = this->entityValid(entity);
	if (valid)
		this->reg.destroy((entt::entity)entity);
	return valid;
}

int Scene::lua_createSystem(lua_State* L) {
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	int type = (int)lua_tointeger(L, 1);

	return 0;
}

int Scene::lua_loadResource(lua_State* L)
{
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	std::string path = lua_tostring(L, 1);
	std::string name = lua_tostring(L, 2);
	scene->resources.loadModel(path, name);

	return 0;
}

int Scene::lua_setScene(lua_State* L) {
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	std::string path = lua_tostring(L, 1);
	scene->setScene(L, path);

	return 0;
}

int Scene::lua_quit(lua_State* L) {
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	scene->quit = true;
	return 0;
}

int Scene::lua_setCamera(lua_State* L) {
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	Vector3f pos = lua_tovector(L, 1);
	Vector3f rot = lua_tovector(L, 2);
	float fov = lua_tonumber(L, 3);
	scene->setCamera(pos, rot, fov);

	return 0;
}

int Scene::lua_getCameraPos(lua_State* L) {
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushvector(L, scene->cam.cam3D.getPosition());
	return 1;
}

int Scene::lua_setCameraPos(lua_State* L){
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	Vector3f pos = lua_tovector(L, 1);
	scene->setCameraPos(pos);
	return 0;
}

int Scene::lua_getCameraRot(lua_State* L){
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushvector(L, scene->cam.rotation);
	return 1;
}

int Scene::lua_getEntityCount(lua_State* L)
{
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushnumber(L, scene->getEntityCount());
	return 1;
}

int Scene::lua_entityValid(lua_State* L){
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushboolean(L, scene->entityValid((int)lua_tointeger(L, 1)));
	return 1;
}

int Scene::lua_createEntity(lua_State* L)
{
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushnumber(L, scene->createEntity());
	return 1;
}

int Scene::lua_removeEntity(lua_State* L){
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushboolean(L, scene->removeEntity((int)lua_tointeger(L, 1)));
	return 1;
}

int Scene::lua_hasComponent(lua_State* L){
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	int entity = (int)lua_tointeger(L, 1);
	int type = (int)lua_tointeger(L, 2);
	bool hasComp = false;

	if (compTypes.at(type) == "Transform")
		hasComp = scene->hasComponents<TransformComp>(entity);
	else if (compTypes.at(type) == "Behaviour")
		hasComp = scene->hasComponents<Behaviour>(entity);
	else if (compTypes.at(type) == "UIElement")
		hasComp = scene->hasComponents<UIElement>(entity);

	lua_pushboolean(L, hasComp);
	return 1;
}

int Scene::lua_getComponent(lua_State* L){
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	int entity = (int)lua_tointeger(L, 1);
	int type = (int)lua_tointeger(L, 2);

	if (compTypes.at(type) == "Transform" && scene->hasComponents<TransformComp>(entity))
		lua_pushtransform(L, scene->getComponent<TransformComp>(entity));
	else if (compTypes.at(type) == "Behaviour" && scene->hasComponents<Behaviour>(entity))
		lua_rawgeti(L, LUA_REGISTRYINDEX, scene->getComponent<Behaviour>(entity).luaRef);
	else if (compTypes.at(type) == "UIElement" && scene->hasComponents<UIElement>(entity))
		lua_pushuielement(L, scene->getComponent<UIElement>(entity));
	else
		lua_pushnil(L);

	return 1;
}

int Scene::lua_setComponent(lua_State* L)
{
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	int entity = (int)lua_tointeger(L, 1);
	int type = (int)lua_tointeger(L, 2);

	if (compTypes.at(type) == "Transform")
		scene->setComponent<TransformComp>(entity, lua_totransform(L, 3));
	else if (compTypes.at(type) == "MeshComp")
		scene->setComponent<MeshComp>(entity, lua_tostring(L, 3));
	else if (compTypes.at(type) == "UIElement"){
		UIElement elem = lua_touielement(L, 3);
		if(elem.dimensions[0] == 0 && elem.dimensions[1] == 0)
			scene->setComponent<UIElement>(entity);
		else
			scene->setComponent<UIElement>(entity, lua_touielement(L, 3));
	}else if (compTypes.at(type) == "Behaviour"){
		if (scene->hasComponents<Behaviour>(entity))
			scene->removeComponent<Behaviour>(entity);

		std::string path = lua_tostring(L, 3);
		std::cout << "Path: " << path << std::endl;
		if (luaL_dofile(L, ("Scripts/Behaviour/" + path).c_str()) != LUA_OK)
			LuaHelper::dumpError(L);
		else
		{
			lua_pushvalue(L, -1);
			int luaRef = luaL_ref(L, LUA_REGISTRYINDEX);

			lua_pushinteger(L, entity);
			lua_setfield(L, -2, "ID");

			lua_pushstring(L, path.c_str());
			lua_setfield(L, -2, "path");

			lua_getfield(L, -1, "init");
			if (lua_type(L, -1) == LUA_TNIL)
			{
				lua_pop(L, 1);
				return 0;
			}

			lua_pushvalue(L, -2);
			if (lua_pcall(L, 1, 0, 0) != LUA_OK)
				LuaHelper::dumpError(L);
			else
			{
				scene->setComponent<Behaviour>(entity, path.c_str(), luaRef);
				return 1;
			}
		}
	}

	return 0;
}

int Scene::lua_removeComponent(lua_State* L)
{
	Scene* scene = (Scene*)lua_touserdata(L, lua_upvalueindex(1));
	int entity = (int)lua_tointeger(L, 1);
	int type = (int)lua_tointeger(L, 2);

	if (compTypes.at(type) == "Behaviour" && scene->hasComponents<Behaviour>(entity))
		scene->removeComponent<Behaviour>(entity);
	else if (compTypes.at(type) == "MeshComp" && scene->hasComponents<MeshComp>(entity))
		scene->removeComponent<MeshComp>(entity);
	else if (compTypes.at(type) == "UIElement" && scene->hasComponents<UIElement>(entity))
		scene->removeComponent<UIElement>(entity);

	return 0;
}
