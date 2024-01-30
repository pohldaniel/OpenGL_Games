#pragma once

#include <string>
#include <unordered_map>
#include <rapidjson/document.h>
#include <engine/Vector.h>
#include <engine/Camera.h>
#include <engine/Texture.h>
#include <engine/Material.h>

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
	
	void parseCamera(rapidjson::GenericObject<false, rapidjson::Value> object, Camera& camera);
	void parseTextures(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Texture>& texures);
	void parseMaterials(rapidjson::GenericArray<false, rapidjson::Value> array, std::vector<Material>& materials);

	Texture& addTexture(std::string path);
	Material& addMaterial(const MaterialBuffer& material);

	Vector2f mapMinLimit, mapMaxLimit;
	Camera camera;
	std::vector<Texture> textures;
	std::vector<Material> materials;
};

class SceneManager {

public:

	Scene& getScene(std::string name);
	bool containsScene(std::string name);
	void loadSettings(std::string path);
	const std::string& getCurrentSceneFile() const;
	const Scene& getCurrentSceneInfo() const;

	static SceneManager& Get();

private:
	SceneManager();

	std::vector<Level> m_levels;
	std::unordered_map<std::string, Scene> m_scenes;

	int m_currentPosition;

	static SceneManager s_instance;
};