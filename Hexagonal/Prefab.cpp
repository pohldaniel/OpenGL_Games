#include "Prefab.h"
#include "TileSet.h"
#include "AnimationController.h"
#include <iostream>

Prefab::Prefab(const TileSet& _tileSet) : tileSet(_tileSet) {

}

void Prefab::setAnimationController(const eAnimationController* _animationController) {
	animationController = std::make_unique<eAnimationController>(*_animationController);
}

void Prefab::setBounds(Vector4f& _bounds) {
	bounds = _bounds;
}

void Prefab::setOffset(Vector2f& _offset) {
	offset = _offset;
}

void Prefab::setBoundingBox(const Rect& _boundingBox) {
	boundingBox = _boundingBox;
}

////////////////////////////////////////////////////////////////////////////////
PrefabManager PrefabManager::s_instance;

PrefabManager& PrefabManager::Get() {
	return s_instance;
}

void PrefabManager::addPrefab(std::string name, const TileSet& tileSet) {
	m_prefabs.insert(std::pair<std::string, Prefab>(name, Prefab(tileSet)));
	//m_prefabs[name] = Prefab(tileSet);
}

Prefab& PrefabManager::getPrefab(std::string name){
	return m_prefabs.at(name);
}

std::unordered_map<std::string, Prefab>& PrefabManager::getPrefabs() {
	return m_prefabs;
}

bool PrefabManager::contains(std::string name) {
	return m_prefabs.count(name) == 1;
}