#pragma once
#include <unordered_map>
#include <string>

template<typename T>
class AssetManager {
public:

	void loadTexture(const std::string& name, const std::string& path, const bool flipVertical = true, const bool linear = false) { 
		m_assets[name].loadFromFile(path, flipVertical, linear); 
	}

	void loadTexture(const std::string& name, const std::string& path, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, unsigned int row, unsigned int column, unsigned int format = -1, const bool linear = false) {
		m_assets[name].loadFromFile(path, tileWidth, tileHeight, spacing, row, column, true, true, format, linear);
	}

	void loadTexture(const std::string& name, const std::string& path, unsigned int offsetX, unsigned int offsetY, unsigned int width, unsigned int height, unsigned int format = -1, const bool linear = false) {
		m_assets[name].loadFromFile(path, offsetX, offsetY, width, height, true, format, linear);
	}

	void loadCharacterSet(const std::string& name, const std::string& path, const float characterSize) {
		m_assets.insert(std::pair<std::string, T>(name, T()));
		m_assets[name].loadFromFile(path, characterSize);
	}

	void loadShader(const std::string& name, const std::string& vertex, const std::string& fragment) {
		//m_assets[name].loadFromFile(vertex, fragment);
		m_assetPointer[name] = new T(vertex, fragment);
	}

	void loadSpritesheet(const std::string& name, const std::string& path, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing, unsigned int yStart, unsigned int xStart, unsigned int xLength, unsigned int format = -1) {
		m_assetPointer[name] = new T(path, tileWidth, tileHeight, spacing, true, true, yStart, xStart, xLength, format);
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
			if (s.second) {
				delete s.second;
				s.second = NULL;
			}
		}

		/*for (const auto& s : m_assets) {
			s.second.~T();
		}*/			
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

	void loadCharacterSet(const std::string& name, const std::string& path, const float characterSize) {
		m_assets.insert(std::pair<std::string, T>(name, T()));
		m_assets[name].loadFromFile(path, characterSize);
	}

	void loadShader(const std::string& name, const std::string& vertex, const std::string& fragment) {
		m_assetPointer[name] = new T(vertex, fragment);
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
			if (s.second) {
				delete s.second;
				s.second = NULL;
			}
		}
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

