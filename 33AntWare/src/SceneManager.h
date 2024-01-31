#pragma once

#include <string>
#include <unordered_map>
#include <rapidjson/document.h>
#include <engine/Vector.h>
#include <engine/Camera.h>
#include <engine/Texture.h>
#include <engine/Material.h>
#include <engine/AssimpModel.h>

#include "ObjSequence.h"

struct Level {
	std::string sceneFile;
	std::string thumb;
	std::string music;
};

class Scene {

	friend class SceneManager;

public:

	Scene();
	void loadScene(std::string path);
	
	const Camera& getCamera() const;
	const std::vector<Texture>& getTextures() const;
	const std::vector<Material>& getMaterials() const;
	const std::vector<AssimpModel*>& getMeshes() const;
	const ObjSequence& getObjSequence() const;
	
	void parseCamera(rapidjson::GenericObject<false, rapidjson::Value> object, Camera& camera);
	void parseTextures(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Texture>& texures);
	void parseMaterials(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Material>& materials);
	void parseMeshes(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<AssimpModel*>& meshes);
	void parseObjSequences(rapidjson::GenericArray<false, rapidjson::Value> array, ObjSequence& objSequence);

	Texture& addTexture(std::string path, std::vector<Texture>& texures);
	Material& addMaterial(const MaterialBuffer& material, std::vector<Material>& materials);
	AssimpModel* addMesh(const rapidjson::GenericObject<true, rapidjson::Value> object, std::vector<AssimpModel*>& meshes);
	ObjSequence& addObjSequence(const rapidjson::GenericObject<true, rapidjson::Value> object, ObjSequence& objSequence);

	Vector2f mapMinLimit, mapMaxLimit;
	Camera camera;
	std::vector<Texture> textures;
	std::vector<Material> materials;
	std::vector<AssimpModel*> meshes;
	ObjSequence objSequence;
};

class SceneManager {

public:

	Scene& getScene(std::string name);
	bool containsScene(std::string name);
	void loadSettings(std::string path);
	const std::string& getCurrentSceneFile() const;

	static SceneManager& Get();

private:
	SceneManager();

	std::vector<Level> m_levels;
	std::unordered_map<std::string, Scene> m_scenes;

	int m_currentPosition;

	static SceneManager s_instance;
};