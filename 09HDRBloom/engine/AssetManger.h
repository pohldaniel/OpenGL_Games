#pragma once
#include <unordered_map>
#include <string>
#include <iostream>

template<typename T>
class AssetManager {
public:

	void loadTexture(const std::string& name, const std::string& path, const bool flipVertical = true, unsigned int internalFormat = 0, unsigned int format = 0) {
		m_assets[name].loadFromFile(path, flipVertical, internalFormat, format);
	}

	void loadTexture(const std::string& name, const std::string& path, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, unsigned int posY, unsigned int posX, const bool flipVertical = true, unsigned int format = 0) {
		m_assets[name].loadFromFile(path, tileWidth, tileHeight, spacing, posY, posX, flipVertical, format);
	}

	void loadTexture(const std::string& name, const std::string& path, unsigned int offsetX, unsigned int offsetY, unsigned int width, unsigned int height, const bool flipVertical = true, unsigned int format = 0) {
		m_assets[name].loadFromFile(path, offsetX, offsetY, width, height, flipVertical, format);
	}

	void createNullTexture(const std::string& name, unsigned int width = 2, unsigned int height = 2, unsigned int color = 255) {
		m_assets[name].createNullTexture(width, height, color);
	}

	void createPerlinTexture(const std::string& name, unsigned int width = 2, unsigned int height = 2, unsigned int seed = 0) {
		m_assets[name].createPerlinTexture(width, height, seed);
	}

	void createEmptyTexture(const std::string& name, unsigned int width, unsigned int height, unsigned int internalFormat = 0, unsigned int format = 0, unsigned int type = 0) {
		m_assets[name].createEmptyTexture(width, height, internalFormat, format, type);
	}

	void loadCharacterSet(const std::string& name, const std::string& path, const float characterSize) {
		m_assets.insert(std::pair<std::string, T>(name, T()));
		m_assets[name].loadFromFile(path, characterSize);
	}

	void loadShader(const std::string& name, const std::string& vertex, const std::string& fragment) {
		//m_assets[name].loadFromFile(vertex, fragment);
		m_assetPointer[name] = new T(vertex.c_str(), fragment.c_str(), true);
	}

	void loadShader(const std::string& name, const std::string& vertex, const std::string& fragment, const std::string& geometry) {
		m_assetPointer[name] = new T(vertex.c_str(), fragment.c_str(), geometry.c_str(), true);
	}

	void loadShader(const std::string& name, const std::string& compute) {
		m_assetPointer[name] = new T(compute.c_str(), true);
	}

	void loadShaderFromString(const std::string& name, const std::string& vertex, const std::string& fragment) {
		m_assetPointer[name] = new T(vertex.c_str(), fragment.c_str(), false);
	}

	void loadShaderFromString(const std::string& name, const std::string& vertex, const std::string& fragment, const std::string& geometry) {
		m_assetPointer[name] = new T(vertex.c_str(), fragment.c_str(), geometry.c_str(), false);
	}

	void loadShaderFromString(const std::string& name, const std::string& compute) {
		m_assetPointer[name] = new T(compute.c_str(), false);
	}

	void loadSpritesheet(const std::string& name, const std::string& path, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, unsigned int yStart, unsigned int xStart, unsigned int xLength, unsigned int format = -1) {
		m_assetPointer[name] = new T(path, tileWidth, tileHeight, spacing, true, true, yStart, xStart, xLength, format);
	}

	void createSpritesheet(const std::string& name, unsigned int texture, unsigned int width, unsigned int height, unsigned int format = -1) {
		m_assetPointer[name] = new T();
		m_assetPointer[name]->createSpritesheet(texture, width, height, format);
	}

	void createNullSpritesheet(const std::string& name, unsigned int width = 2, unsigned int height = 2, unsigned short layer = 4) {
		m_assetPointer[name] = new T();
		m_assetPointer[name]->createNullSpritesheet(width, height, layer);
	}

	void loadCubeMap(const std::string& name, std::string* textureFiles, const bool flipVertical = true, unsigned int format = -1) {
		m_assets[name].loadFromFile(textureFiles, flipVertical, format);
	}

	void createNullCubemap(const std::string& name, unsigned int width = 2, unsigned int height = 2, unsigned int color = 255) {
		m_assets[name].createNullCubemap(width, height, color);
	}

	void loadSoundEffect(const std::string& name, const std::string& path) {
		m_assets[name].loadFromFile(path);
	}

	void loadMusic(const std::string& name, const std::string& path) {
		m_assets.insert(std::pair<std::string, T>(name, T()));
		m_assets[name].loadFromFile(path);
	}

	void loadAnimation(const std::string& name, const std::string& path, std::string sourceName, std::string destName, bool looping = false, unsigned int animationOffset = 0u, unsigned int animationCuttOff = 0u, float timeScale = 1.0f) {
		m_assetPointer[name] = new T();
		m_assetPointer[name]->loadAnimation(path, sourceName, destName, looping, animationOffset, animationCuttOff, timeScale);
	}

	void loadAnimationFbx(const std::string& name, const std::string& path, std::string sourceName, std::string destName, bool looping = false, unsigned int animationOffset = 0u, unsigned int animationCuttOff = 0u, unsigned int timeShift = 0u) {
		m_assetPointer[name] = new T();
		m_assetPointer[name]->loadAnimationFbx(path, sourceName, destName, looping, animationOffset, animationCuttOff, timeShift);
	}

	T& get(const std::string& name) {
		return m_assets[name];
	}

	T* getAssetPointer(const std::string& name) {
		return m_assetPointer[name];
	}

	void clear() {
		for (auto& s : m_assetPointer) {
			delete s.second;
			s.second = nullptr;
		}
		m_assetPointer.clear();
		m_assets.clear();
	}

	bool checkAsset(const std::string& name) {
		return (m_assetPointer.count(name) != 0) || (m_assets.count(name) != 0);
	}

private:
	std::unordered_map<std::string, T> m_assets;
	std::unordered_map<std::string, T*> m_assetPointer;
};

template<typename T>
class AssetManagerStatic {
public:

	static AssetManagerStatic& get() {
		if (!s_instance.init) {
			s_instance = AssetManagerStatic();
		}
		return s_instance;
	}

	void loadTexture(const std::string& name, const std::string& path, const bool flipVertical = true, const bool linear = false) {
		m_assets[name].loadFromFile(path, flipVertical, linear);
	}

	void loadTexture(const std::string& name, const std::string& path, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, unsigned int row, unsigned int column, unsigned int format = -1, const bool linear = false) {
		m_assets[name].loadFromFile(path, tileWidth, tileHeight, spacing, row, column, true, true, format, linear);
	}

	void loadTexture(const std::string& name, const std::string& path, unsigned int offsetX, unsigned int offsetY, unsigned int width, unsigned int height, unsigned int format = -1, const bool linear = false) {
		m_assets[name].loadFromFile(path, offsetX, offsetY, width, height, true, format, linear);
	}

	void createNullTexture(const std::string& name, unsigned int width = 2, unsigned int height = 2) {
		m_assets[name].createNullTexture(width, height);
	}

	void createPerlinTexture(const std::string& name, unsigned int width = 2, unsigned int height = 2, unsigned int seed = 0) {
		m_assets[name].createPerlinTexture(width, height, seed);
	}

	void loadCharacterSet(const std::string& name, const std::string& path, const float characterSize) {
		m_assets.insert(std::pair<std::string, T>(name, T()));
		m_assets[name].loadFromFile(path, characterSize);
	}

	void loadShader(const std::string& name, const std::string& vertex, const std::string& fragment) {
		m_assetPointer[name] = new T(vertex, fragment);
	}

	void loadShader(const std::string& name, const std::string& vertex, const std::string& fragment, const std::string& geometry) {
		m_assetPointer[name] = new T(vertex, fragment, geometry);
	}

	void loadSpritesheet(const std::string& name, const std::string& path, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, unsigned int yStart, unsigned int xLength, unsigned int format = -1) {
		m_assetPointer[name] = new T(path, tileWidth, tileHeight, spacing, true, true, yStart, xLength, format);
	}

	void loadSoundEffect(const std::string& name, const std::string& path) {
		m_assets[name].loadFromFile(path);
	}

	void loadMusic(const std::string& name, const std::string& path) {
		m_assets.insert(std::pair<std::string, T>(name, T()));
		m_assets[name].loadFromFile(path);
	}

	T& get(const std::string& name) {
		return m_assets[name];
	}

	T* AssetManagerStatic::getAssetPointer(const std::string& name) {
		return m_assetPointer[name];
	}

	void clear() {
		for (auto& s : m_assetPointer) {
			delete s.second;
			s.second = nullptr;
		}
		m_assetPointer.clear();
		m_assets.clear();
	}

	~AssetManagerStatic() {}
private:
	AssetManagerStatic() {
		init = true;
	}

	std::unordered_map<std::string, T> m_assets;
	std::unordered_map<std::string, T*> m_assetPointer;
	bool init = false;
	static AssetManagerStatic s_instance;
};

template<typename T> AssetManagerStatic<T> AssetManagerStatic<T>::s_instance;

