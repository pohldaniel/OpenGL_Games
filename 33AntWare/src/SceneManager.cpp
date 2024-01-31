#include <iostream>
#include <fstream>
#include <functional>

#include <rapidjson/istreamwrapper.h>

#include "SceneManager.h"
#include "Application.h"

#ifdef GetObject
#undef GetObject
#endif

Scene::Scene() {
}

void Scene::parseCamera(rapidjson::GenericObject<false, rapidjson::Value> object, Camera& camera){
	camera.perspective(object["fov"].GetFloat(), static_cast<float>(Application::Width) / static_cast<float>(Application::Height), object["near"].GetFloat(), object["far"].GetFloat());
	
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
	return texture;
}

void Scene::parseTextures(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Texture>& texures) {
	for (rapidjson::Value::ConstValueIterator texture = array.Begin(); texture != array.End(); ++texture) {
		addTexture(texture->GetString(), texures);
	}

	std::for_each(textures.begin(), textures.end(), std::mem_fn(&Texture::markForDelete));
}

Material& Scene::addMaterial(const MaterialBuffer& _material, std::vector<Material>& materials) {
	materials.resize(materials.size() + 1);
	Material& material = materials.back();

	material.buffer.ambient[0] = _material.ambient[0];
	material.buffer.ambient[1] = _material.ambient[1];
	material.buffer.ambient[2] = _material.ambient[2];
	material.buffer.ambient[3] = _material.ambient[3];

	material.buffer.diffuse[0] = _material.diffuse[0];
	material.buffer.diffuse[1] = _material.diffuse[1];
	material.buffer.diffuse[2] = _material.diffuse[2];
	material.buffer.diffuse[3] = _material.diffuse[3];

	material.buffer.specular[0] = _material.specular[0];
	material.buffer.specular[1] = _material.specular[1];
	material.buffer.specular[2] = _material.specular[2];
	material.buffer.specular[3] = _material.specular[3];

	material.buffer.shininess = _material.shininess;
	material.buffer.alpha = _material.alpha;
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

AssimpModel* Scene::addMesh(rapidjson::GenericObject<true, rapidjson::Value> object, std::vector<AssimpModel*>& meshes) {
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
	for (rapidjson::Value::ConstValueIterator mesh = array.Begin(); mesh != array.End(); ++mesh) {
		addMesh(mesh->GetObject(), meshes);
	}

}

ObjSequence& Scene::addObjSequence(const rapidjson::GenericObject<true, rapidjson::Value> object, ObjSequence& objSequence) {
	objSequence.loadSequence(object["path"].GetString());
	if (object.HasMember("materialIndex")) {
		objSequence.setMaterialIndex(object["materialIndex"].GetInt());
	}

	if (object.HasMember("textureIndex")) {
		objSequence.setTextureIndex(object["textureIndex"].GetInt());
	}

	if (object.HasMember("additionalMeshes")) {
		rapidjson::GenericArray<true, rapidjson::Value> array = object["additionalMeshes"].GetArray();
		for (rapidjson::Value::ConstValueIterator mesh = array.Begin(); mesh != array.End(); ++mesh) {
			objSequence.addMesh(meshes[(*mesh).GetInt()]->getMeshes()[0]->getVertexBuffer(), meshes[(*mesh).GetInt()]->getMeshes()[0]->getIndexBuffer());
		}
	}

	objSequence.loadSequenceGpu();
	objSequence.markForDelete();
	return objSequence;
}

void Scene::parseObjSequences(rapidjson::GenericArray<false, rapidjson::Value> array, ObjSequence& objSequence) {
	for (rapidjson::Value::ConstValueIterator mesh = array.Begin(); mesh != array.End(); ++mesh) {
		addObjSequence(mesh->GetObject(), objSequence);
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
	parseMeshes(doc["meshes"].GetArray(), meshes);
	parseObjSequences(doc["objSequences"].GetArray(), objSequence);
}

const Camera& Scene::getCamera() const {
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

const ObjSequence& Scene::getObjSequence() const {
	return objSequence;
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
		//std::cout << m_levels.back().sceneFile << "  " << m_levels.back().thumb << "  " << m_levels.back().music << std::endl;
	}
	file.close();
}

const std::string& SceneManager::getCurrentSceneFile() const {
	return m_levels[m_currentPosition].sceneFile;
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
