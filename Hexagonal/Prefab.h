#pragma once

#include <string>
#include <memory>
#include <unordered_map>

class TileSet;
class eAnimationController;

struct Prefab {
	Prefab(const TileSet& tileSet);

	void setAnimationController(const eAnimationController* animationController);

	const TileSet& tileSet;
	std::unique_ptr<eAnimationController> animationController;
};


class PrefabManager {

public:
	
	bool contains(std::string name);
	void addPrefab(std::string name, const TileSet& tileSet);

	Prefab& getPrefab(std::string name);
	std::unordered_map<std::string, Prefab>& getPrefabs();

	static PrefabManager& Get();

private:
	PrefabManager() = default;
	std::unordered_map<std::string, Prefab> m_prefabs;
	static PrefabManager s_instance;
};