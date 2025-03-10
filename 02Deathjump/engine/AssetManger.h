#pragma once
#include <unordered_map>
#include <string>

template<typename T>
class AssetManager {
public:

	void loadTexture(const std::string& name, const std::string& path, const bool flipVertical = true, const bool linear = false) { 
		m_assets[name].loadFromFile(path, flipVertical, linear); 
	}

	void loadCharacterSet(const std::string& name, const std::string& path, const float characterSize) {
		m_assets.insert(std::pair<std::string, T>(name, T()));
		m_assets[name].loadFromFile(path, characterSize);
	}

	void loadShader(const std::string& name, const std::string& vertex, const std::string& fragment) {
		//m_assets[name].loadFromFile(vertex, fragment);
		m_assetPointer[name] = new T(vertex, fragment);
	}

	void loadSpritesheet(const std::string& name, const std::string& path, unsigned short tileWidth, unsigned short tileHeight, unsigned int yStart, unsigned int xLength, unsigned int format = -1) {
		m_assetPointer[name] = new T(path, tileWidth, tileHeight, true, true, yStart, xLength, format);
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

	void loadCharacterSet(const std::string& name, const std::string& path, const float characterSize) {
		m_assets.insert(std::pair<std::string, T>(name, T()));
		m_assets[name].loadFromFile(path, characterSize);
	}

	void loadShader(const std::string& name, const std::string& vertex, const std::string& fragment) {
		m_assetPointer[name] = new T(vertex, fragment);
	}

	void loadSpritesheet(const std::string& name, const std::string& path, unsigned short tileWidth, unsigned short tileHeight, unsigned int yStart, unsigned int xLength, unsigned int format = -1) {
		m_assetPointer[name] = new T(path, tileWidth, tileHeight, true, true, yStart, xLength, format);
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

