#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <engine/Vector.h>
#include <engine/Rect.h>

class TileSet;
class eAnimationController;

struct Prefab {
	Prefab(const TileSet& tileSet);

	void setAnimationController(const eAnimationController* animationController);
	void setBounds(Vector4f& bounds);
	void setOffset(Vector2f& offset);
	void setBoundingBox(const Rect& boundingBox);

	const TileSet& tileSet;
	std::shared_ptr<eAnimationController> animationController;
	Vector4f bounds;
	Vector2f offset;
	Rect boundingBox;
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