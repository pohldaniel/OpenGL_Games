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
	
	Camera& getCamera();
	const std::vector<Texture>& getTextures() const;
	const std::vector<Material>& getMaterials() const;
	const std::vector<AssimpModel*>& getMeshes() const;
	const std::vector<Entity*>& getEntitiesAfterClear() const;
	const std::vector<Entity*>& getEntities() const;
	const MeshSequence& getMeshSequence() const;
	Player* getPlayer() const;

	void parseCamera(rapidjson::GenericObject<false, rapidjson::Value> object, Camera& camera);
	void parseTextures(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Texture>& texures);
	void parseMaterials(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Material>& materials);
	void parseLights(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Light>& lights);
	void parseMeshes(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<AssimpModel*>& meshes);
	void parseMeshSequences(rapidjson::GenericArray<false, rapidjson::Value> array, MeshSequence& meshSequence);
	void parseNodes(rapidjson::GenericArray<false, rapidjson::Value> array, SceneNode*& root);

	Texture& addTexture(std::string path, std::vector<Texture>& texures);
	Material& addMaterial(const MaterialBuffer& materialBuffer, std::vector<Material>& materials);
	Light& addLight(const LightBuffer& lightBuffer, std::vector<Light>& lights);
	AssimpModel* addMesh(const rapidjson::GenericObject<false, rapidjson::Value> object, std::vector<AssimpModel*>& meshes);
	MeshSequence& addMeshSequence(const rapidjson::GenericObject<false, rapidjson::Value> object, MeshSequence& meshSequence);
	SceneNode* addNode(const rapidjson::GenericObject<false, rapidjson::Value> object, SceneNode*& root);
	Types resolveType(std::string type);

	Vector2f mapMinLimit, mapMaxLimit;
	Camera camera;
	std::vector<Texture> textures;
	std::vector<Material> materials;
	std::vector<AssimpModel*> meshes;
	MeshSequence meshSequence;
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

	static SceneManager& Get();

private:
	SceneManager();

	std::vector<Level> m_levels;
	std::unordered_map<std::string, Scene> m_scenes;

	int m_currentPosition;

	static SceneManager s_instance;
};