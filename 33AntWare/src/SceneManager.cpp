#include <iostream>
#include <fstream>
#include <functional>

#include <rapidjson/istreamwrapper.h>
#include <Entities/Entity.h>
#include <Entities/Player.h>
#include <Entities/Ant.h>

#include "SceneManager.h"
#include "Application.h"

#ifdef GetObject
#undef GetObject
#endif

Scene::Scene() {
}

void Scene::parseCamera(rapidjson::GenericObject<false, rapidjson::Value> object, Camera& camera){
	camera.perspective(object["fov"].GetFloat(), static_cast<float>(Application::Width) / static_cast<float>(Application::Height), object["near"].GetFloat(), object["far"].GetFloat());
	//camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	camera.orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f);

	rapidjson::GenericObject<false, rapidjson::Value> lookAt = object["lookAt"].GetObject();

	rapidjson::Value::Array positon = lookAt["position"].GetArray();
	rapidjson::Value::Array target = lookAt["target"].GetArray();
	rapidjson::Value::Array up = lookAt["up"].GetArray();

	camera.lookAt({ positon[0].GetFloat(),  positon[1].GetFloat(), positon[2].GetFloat() }, 
                  { target[0].GetFloat(),  target[1].GetFloat(), target[2].GetFloat() },
                  { up[0].GetFloat(),  up[1].GetFloat(), up[2].GetFloat() });
}

Texture& Scene::addTexture(std::string path, std::vector<Texture>& texures) {
	textures.resize(textures.size() + 1);
	Texture& texture = textures.back();
	texture.loadFromFile(path, true);
	texture.setFilter(GL_LINEAR);
	texture.setWrapMode(GL_REPEAT);
	return texture;
}

void Scene::parseTextures(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Texture>& texures) {
	for (rapidjson::Value::ConstValueIterator texture = array.Begin(); texture != array.End(); ++texture) {
		addTexture(texture->GetString(), texures);
	}

	std::for_each(textures.begin(), textures.end(), std::mem_fn(&Texture::markForDelete));
}

Material& Scene::addMaterial(const MaterialBuffer& materialBuffer, std::vector<Material>& materials) {
	materials.resize(materials.size() + 1);
	Material& material = materials.back();

	material.buffer.ambient[0] = materialBuffer.ambient[0];
	material.buffer.ambient[1] = materialBuffer.ambient[1];
	material.buffer.ambient[2] = materialBuffer.ambient[2];
	material.buffer.ambient[3] = materialBuffer.ambient[3];

	material.buffer.diffuse[0] = materialBuffer.diffuse[0];
	material.buffer.diffuse[1] = materialBuffer.diffuse[1];
	material.buffer.diffuse[2] = materialBuffer.diffuse[2];
	material.buffer.diffuse[3] = materialBuffer.diffuse[3];

	material.buffer.specular[0] = materialBuffer.specular[0];
	material.buffer.specular[1] = materialBuffer.specular[1];
	material.buffer.specular[2] = materialBuffer.specular[2];
	material.buffer.specular[3] = materialBuffer.specular[3];

	material.buffer.shininess = materialBuffer.shininess;
	material.buffer.alpha = materialBuffer.alpha;
	return material;
}

void Scene::parseMaterials(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Material>& materials) {
	
	for (rapidjson::Value::ConstValueIterator material = array.Begin(); material != array.End(); ++material) {
		const rapidjson::Value& mat = *material;
		rapidjson::GenericArray<true, rapidjson::Value>  ambient = mat["ambient"].GetArray();
		rapidjson::GenericArray<true, rapidjson::Value>  diffuse = mat["diffuse"].GetArray();
		rapidjson::GenericArray<true, rapidjson::Value>  specular = mat["specular"].GetArray();

		addMaterial({ {ambient[0].GetFloat(), ambient[1].GetFloat(), ambient[2].GetFloat(), ambient[3].GetFloat()},
					  {diffuse[0].GetFloat(), diffuse[1].GetFloat(), diffuse[2].GetFloat(), diffuse[3].GetFloat()},
					  {specular[0].GetFloat(), specular[1].GetFloat(), specular[2].GetFloat(), specular[3].GetFloat()}, 
			          mat["shininess"].GetFloat() 
			       }, materials);

	}
}

Light& Scene::addLight(const LightBuffer& lightBuffer, std::vector<Light>& lights) {
	lights.resize(lights.size() + 1);
	Light& light = lights.back();

	int index = lights.size() - 1;
	light.m_index = index;

	Light::Buffer[index].ambient[0] = lightBuffer.ambient[0];
	Light::Buffer[index].ambient[1] = lightBuffer.ambient[1];
	Light::Buffer[index].ambient[2] = lightBuffer.ambient[2];
	Light::Buffer[index].ambient[3] = lightBuffer.ambient[3];

	Light::Buffer[index].diffuse[0] = lightBuffer.diffuse[0];
	Light::Buffer[index].diffuse[1] = lightBuffer.diffuse[1];
	Light::Buffer[index].diffuse[2] = lightBuffer.diffuse[2];
	Light::Buffer[index].diffuse[3] = lightBuffer.diffuse[3];

	Light::Buffer[index].specular[0] = lightBuffer.specular[0];
	Light::Buffer[index].specular[1] = lightBuffer.specular[1];
	Light::Buffer[index].specular[2] = lightBuffer.specular[2];
	Light::Buffer[index].specular[3] = lightBuffer.specular[3];

	Light::Buffer[index].position[0] = lightBuffer.position[0];
	Light::Buffer[index].position[1] = lightBuffer.position[1];
	Light::Buffer[index].position[2] = lightBuffer.position[2];

	Light::Buffer[index].direction[0] = lightBuffer.direction[0];
	Light::Buffer[index].direction[1] = lightBuffer.direction[1];
	Light::Buffer[index].direction[2] = lightBuffer.direction[2];

	Light::Buffer[index].angle = lightBuffer.angle;
	Light::Buffer[index].type = lightBuffer.type;
	Light::Buffer[index].enabled = lightBuffer.enabled;

	if (Light::Buffer[index].type == SPOT_LIGHT)
		light.m_isStatic = false;

	light.setPosition({ lightBuffer.position[0], lightBuffer.position[1], lightBuffer.position[2] });
	light.setDirection({ lightBuffer.direction[0], lightBuffer.direction[1], lightBuffer.direction[2] });

	return light;
}

void Scene::parseLights(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Light>& lights) {
	
	for (rapidjson::Value::ConstValueIterator light = array.Begin(); light != array.End(); ++light) {
		const rapidjson::Value& _light = *light;
		rapidjson::GenericArray<true, rapidjson::Value>  ambient = _light["ambient"].GetArray();
		rapidjson::GenericArray<true, rapidjson::Value>  diffuse = _light["diffuse"].GetArray();
		rapidjson::GenericArray<true, rapidjson::Value>  specular = _light["specular"].GetArray();
		rapidjson::GenericArray<true, rapidjson::Value>  position = _light["position"].GetArray();
		rapidjson::GenericArray<true, rapidjson::Value>  direction = _light["direction"].GetArray();
		int  type = _light["type"].GetInt();
		bool enabled = _light["enabled"].GetBool();

		addLight({ {ambient[0].GetFloat(), ambient[1].GetFloat(), ambient[2].GetFloat(), ambient[3].GetFloat()},
				   {diffuse[0].GetFloat(), diffuse[1].GetFloat(), diffuse[2].GetFloat(), diffuse[3].GetFloat()},
                   {specular[0].GetFloat(), specular[1].GetFloat(), specular[2].GetFloat(), specular[3].GetFloat()},
				   {position[0].GetFloat(), position[1].GetFloat(), position[2].GetFloat()}, _light["angle"].GetFloat(),
			       {direction[0].GetFloat(), direction[1].GetFloat(), direction[2].GetFloat()},
                   static_cast<LightType>(type),
				   enabled, 
			       false, 0, 0.0f, 0.0f, 0.0f },
			     lights);

	}
}

AssimpModel* Scene::addMesh(rapidjson::GenericObject<false, rapidjson::Value> object, std::vector<AssimpModel*>& meshes) {
	meshes.push_back(new AssimpModel());
	AssimpModel* mesh = meshes.back();

	mesh->loadModel(object["path"].GetString(), false, false, false);
	if (object.HasMember("materialIndex")){
		mesh->getMeshes()[0]->setMaterialIndex(object["materialIndex"].GetInt());
	}

	if (object.HasMember("textureIndex")) {
		mesh->getMeshes()[0]->setTextureIndex(object["textureIndex"].GetInt());
	}

	return mesh;
}

void Scene::parseMeshes(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<AssimpModel*>& meshes) {
	for (rapidjson::Value::ValueIterator mesh = array.Begin(); mesh != array.End(); ++mesh) {
		addMesh(mesh->GetObject(), meshes);
	}

}

MeshSequence& Scene::addMeshSequence(const rapidjson::GenericObject<false, rapidjson::Value> object, std::vector<MeshSequence>& meshSequences) {
	meshSequences.resize(meshSequences.size() + 1);
	MeshSequence& meshSequence = meshSequences.back();
	
	meshSequence.loadSequence(object["path"].GetString());
	
	

	if (object.HasMember("additionalMeshesFromFile")) {
		rapidjson::GenericArray<false, rapidjson::Value> array = object["additionalMeshesFromFile"].GetArray();
		for (rapidjson::Value::ValueIterator path = array.Begin(); path != array.End(); ++path) {
			//std::cout << "Paht: " << (*path).GetString() << std::endl;
			meshSequence.addMeshFromFile((*path).GetString());
		}
	}

	if (object.HasMember("additionalMeshes")) {
		rapidjson::GenericArray<false, rapidjson::Value> array = object["additionalMeshes"].GetArray();
		for (rapidjson::Value::ValueIterator mesh = array.Begin(); mesh != array.End(); ++mesh) {
			meshSequence.addMesh(meshes[(*mesh).GetInt()]->getMeshes()[0]->getVertexBuffer(), meshes[(*mesh).GetInt()]->getMeshes()[0]->getIndexBuffer());
		}
	}

	//ant
	const BoundingBox& box = meshSequence.getLocalBoundingBox(41);
	box.inset(Vector3f(-(2.0f * box.min[0] + 2.5f), 0.6f, 1.3f), Vector3f(2.0f * box.max[0] - 2.5f, 0.1f, 0.5f));

	//player
	meshSequence.getLocalBoundingBox(46).maximize(0.2f);

	meshSequence.loadSequenceGpu();
	return meshSequence;
}

void Scene::parseMeshSequences(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<MeshSequence>& meshSequences) {
	for (rapidjson::Value::ValueIterator mesh = array.Begin(); mesh != array.End(); ++mesh) {
		addMeshSequence(mesh->GetObject(), meshSequences);
	}
	std::for_each(textures.begin(), textures.end(), std::mem_fn(&Texture::markForDelete));
}

Types Scene::resolveType(std::string input) {
	if (input == "player") return PLAYER;
	if (input == "entity") return ENTITY;
	if (input == "light") return LIGHT;
	if (input == "ant") return ANT;
	return INVALID;
}

SceneNode* Scene::addNode(const rapidjson::GenericObject<false, rapidjson::Value> object, SceneNode*& root) {
	std::string type = object["type"].GetString();

	int reference;
	if (object.HasMember("mesh")) {
		reference = object["mesh"].GetInt();
	}else if (object.HasMember("light")) {
		reference = object["light"].GetInt();
	}else if (object.HasMember("sequence")) {
		reference = object["sequence"].GetInt();
	}

	SceneNode* child = nullptr;
	switch (resolveType(type)){
		case PLAYER: {
			player = new Player(camera, meshSequences[reference], object["meshIndex"].GetInt(), Vector2f(-51.5f, -51.5f), Vector2f(51.5f, 51.5f));
			player->setPosition(object["position"].GetArray()[0].GetFloat(), object["position"].GetArray()[1].GetFloat(), object["position"].GetArray()[2].GetFloat());
			if (object.HasMember("materialIndex")) {
				player->setMaterialIndex(object["materialIndex"].GetInt());
			}

			if (object.HasMember("textureIndex")) {
				player->setTextureIndex(object["textureIndex"].GetInt());
			}
			
			root->addChild(player);
			child = player;
		}break;
		case ENTITY: {

			if (strcmp(object["tag"].GetString(), "muzzle") == 0 ||
				strcmp(object["tag"].GetString(), "gun") == 0 ||
				strcmp(object["tag"].GetString(), "hands") == 0 ||
				strcmp(object["tag"].GetString(), "gloves") == 0) {
				entitiesAfterClear.push_back(new Entity(meshSequences[reference], object["meshIndex"].GetInt()));
				if (object.HasMember("materialIndex")) {
					entitiesAfterClear.back()->setMaterialIndex(object["materialIndex"].GetInt());
				}

				if (object.HasMember("textureIndex")) {
					entitiesAfterClear.back()->setTextureIndex(object["textureIndex"].GetInt());
				}

		

				child = dynamic_cast<SceneNode*>(root->addChild(entitiesAfterClear.back()));
			}else {
				entities.push_back(new Entity(meshSequences[reference], object["meshIndex"].GetInt()));
				if (object.HasMember("materialIndex")) {
					entities.back()->setMaterialIndex(object["materialIndex"].GetInt());
				}

				if (object.HasMember("textureIndex")) {
					entities.back()->setTextureIndex(object["textureIndex"].GetInt());
				}

				child = dynamic_cast<SceneNode*>(root->addChild(entities.back()));
			}

		}break;
		case LIGHT: {
			child = dynamic_cast<SceneNode*>(root->addChild(&Light::GetLights()[reference]));
			child->setIsFixed(true);
			
		}break; 
		case ANT: {
			entities.push_back(new Ant(meshSequences[reference], object["meshIndex"].GetInt(), player));
			if (object.HasMember("materialIndex")) {
				entities.back()->setMaterialIndex(object["materialIndex"].GetInt());
			}

			if (object.HasMember("textureIndex")) {
				entities.back()->setTextureIndex(object["textureIndex"].GetInt());
			}

			entities.back()->setPosition(object["position"].GetArray()[0].GetFloat(), object["position"].GetArray()[1].GetFloat(), object["position"].GetArray()[2].GetFloat());
			entities.back()->setOrientation(object["rotation"].GetArray()[0].GetFloat(), object["rotation"].GetArray()[1].GetFloat(), object["rotation"].GetArray()[2].GetFloat());
			entities.back()->setScale(object["scale"].GetArray()[0].GetFloat(), object["scale"].GetArray()[1].GetFloat(), object["scale"].GetArray()[2].GetFloat());
			entities.back()->setRigidbody(Rigidbody());
			child = dynamic_cast<SceneNode*>(root->addChild(entities.back()));		
		}break;
		default: {
			child = new SceneNode();			
		}
	}

	if (object.HasMember("nodes")) {
		parseNodes(object["nodes"].GetArray(), child);
	}

	return child;
}

void Scene::parseNodes(rapidjson::GenericArray<false, rapidjson::Value> array, SceneNode*& root) {
	for (rapidjson::Value::ValueIterator node = array.Begin(); node != array.End(); ++node) {
		if (!node->HasMember("guard")) {
			addNode(node->GetObject(), root);
		}
	}
}

void Scene::loadScene(std::string path) {
	std::ifstream file(path, std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << path << std::endl;
	}

	rapidjson::IStreamWrapper streamWrapper(file);
	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);

	rapidjson::Value::Array mapMinLimitData = doc["mapMinLimit"].GetArray();
	rapidjson::Value::Array mapMaxLimitData = doc["mapMaxLimit"].GetArray();
	mapMinLimit = { mapMinLimitData[0].GetFloat(), mapMinLimitData[1].GetFloat() };
	mapMaxLimit = { mapMaxLimitData[0].GetFloat(), mapMaxLimitData[1].GetFloat() };

	parseCamera(doc["camera"].GetObject(), camera);
	parseTextures(doc["textures"].GetArray(), textures);
	parseMaterials(doc["materials"].GetArray(), materials);
	parseLights(doc["lights"].GetArray(), Light::GetLights());
	parseMeshes(doc["meshes"].GetArray(), meshes);
	parseMeshSequences(doc["meshSequences"].GetArray(), meshSequences);
	
	root = new SceneNode();
	parseNodes(doc["nodes"].GetArray(), root);
}

Camera& Scene::getCamera(){
	return camera;
}

const std::vector<Texture>& Scene::getTextures() const {
	return textures;
}

const std::vector<Material>& Scene::getMaterials() const {
	return materials;
}

const std::vector<AssimpModel*>& Scene::getMeshes() const {
	return meshes;
}

std::vector<MeshSequence>& Scene::getMeshSequences(){
	return meshSequences;
}

Player* Scene::getPlayer() const {
	return player;
}

const std::vector<Entity*>& Scene::getEntitiesAfterClear() const {
	return entitiesAfterClear;
}

const std::vector<Entity*>& Scene::getEntities() const {
	return entities;
}

///////////////////////ScenetManager//////////////////////////
SceneManager SceneManager::s_instance;

SceneManager::SceneManager() : m_currentPosition(0){

}

void SceneManager::loadSettings(std::string path)  {
	std::ifstream file(path, std::ios::in);
	if (!file.is_open()){
		std::cerr << "Could not open file: " << path << std::endl;
	}

	rapidjson::IStreamWrapper streamWrapper(file);
	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);
	for (rapidjson::Value::ConstValueIterator levels = doc["levels"].Begin(); levels != doc["levels"].End(); ++levels) {
		m_levels.push_back({ levels->GetObject()["file"].GetString(), levels->GetObject()["thumb"].GetString(), levels->GetObject()["music"].GetString() });		
	}
	file.close();
}

const std::string& SceneManager::getCurrentSceneFile() const {
	return m_levels[m_currentPosition].sceneFile;
}

const std::vector<Level>& SceneManager::getLevels() const {
	return m_levels;
}

const std::vector<std::string> SceneManager::getThumbs() {
	std::vector<std::string> sceneFiles;
	std::transform(m_levels.begin(), m_levels.end(), std::back_inserter(sceneFiles), [](const Level& level)-> std::string { return level.thumb; });
	return sceneFiles;
}

Scene& SceneManager::getScene(std::string name) {
	return m_scenes[name];
}

bool SceneManager::containsScene(std::string name) {
	return m_scenes.count(name) == 1;
}

SceneManager& SceneManager::Get() {
	return s_instance;
}
