#pragma once

#include <string>
#include <unordered_map>
#include <rapidjson/document.h>

#include <engine/Vector.h>
#include <engine/Camera.h>
#include <engine/Texture.h>
#include <engine/Material.h>
#include <engine/AssimpModel.h>

#include <Scene/SceneNode.h>
#include <Scene/SceneNodeLC.h>
#include <Entities/Light.h>


#include "MeshSequence.h"

class Player;
class Entity;

enum Types {
	ENTITY,
	PLAYER,
	LIGHT,
	ANT,
	INVALID
};

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
	void loadSceneGpu();

	Camera& getCamera();
	const std::vector<Texture>& getTextures() const;
	const std::vector<AssimpModel*>& getMeshes() const;
	const std::vector<Entity*>& getEntitiesAfterClear() const;
	std::vector<Entity*>& getEntities();
	std::vector <MeshSequence>& getMeshSequences();
	Player* getPlayer() const;
	void unloadScene();

	void parseCamera(rapidjson::GenericObject<false, rapidjson::Value> object, Camera& camera);
	void parseTextures(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Texture>& textures);
	void parseMaterials(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Material>& materials);
	void parseLights(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Light*>& lights);
	void parseMeshes(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<AssimpModel*>& meshes);
	void parseMeshSequences(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<MeshSequence>& meshSequences);
	void parseNodes(rapidjson::GenericArray<false, rapidjson::Value> array, SceneNode*& root);

	Texture& addTexture(std::string path, std::vector<Texture>& textures);
	Material& addMaterial(const MaterialBuffer& materialBuffer, std::vector<Material>& materials);
	MeshSequence& addMeshSequence(const rapidjson::GenericObject<false, rapidjson::Value> object, std::vector<MeshSequence>& meshSequences);

	Light* addLight(const LightBuffer& lightBuffer, std::vector<Light*>& lights);
	AssimpModel* addMesh(const rapidjson::GenericObject<false, rapidjson::Value> object, std::vector<AssimpModel*>& meshes);
	SceneNode* addNode(const rapidjson::GenericObject<false, rapidjson::Value> object, SceneNode*& root);
	Types resolveType(std::string type);

	Vector2f mapMinLimit, mapMaxLimit;
	Camera camera;
	std::vector<Texture> textures;
	std::vector <MeshSequence> meshSequences;
	std::vector<AssimpModel*> meshes;	

	SceneNode* root;
	Player* player;
	std::vector<Entity*> entitiesAfterClear;
	std::vector<Entity*> entities;
};

class SceneManager {

public:

	Scene& getScene(std::string name);
	bool containsScene(std::string name);
	void loadSettings(std::string path);
	const std::string& getCurrentSceneFile() const;
	const std::vector<Level>& getLevels() const;
	const std::vector<std::string> getThumbs();

	int m_currentPosition;
	bool m_loadSettings;
	static SceneManager& Get();
	
private:

	SceneManager();

	std::vector<Level> m_levels;
	std::unordered_map<std::string, Scene> m_scenes;

	static SceneManager s_instance;
};