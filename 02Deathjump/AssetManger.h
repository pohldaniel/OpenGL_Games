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
		m_assets[name].loadFromFile(path, characterSize);
	}

	void loadShader(const std::string& name, const std::string& vertex, const std::string& fragment) {
		//m_assets[name].loadFromFile(vertex, fragment);
		m_assetPointer[name] = new T(vertex, fragment);
	}

	T& get(const std::string& name) { 
		return m_assets[name]; 
	}

	T* getAssetPointer(const std::string& name) {
		return m_assetPointer[name];
	}

	void clear() {
		for (auto& s : m_assetPointer)
			if (s.second) {
				delete s.second;
				s.second = NULL;
			}

		for (const auto& s : m_assets) {
			s.second.~T();
		}			
	}

private:
	std::unordered_map<std::string, T> m_assets;
	std::unordered_map<std::string, T*> m_assetPointer;
};

template<typename T>
class AssetManagerStatic {
public:
	//AssetManagerStatic(const AssetManagerStatic&) = delete;

	static AssetManagerStatic& get() {
		return s_instance;
	}

	void loadTexture(const std::string& name, const std::string& path, const bool flipVertical = true, const bool linear = false) {
		m_assets[name].loadFromFile(path, flipVertical, linear);
	}
	
	void loadShader(const std::string& name, const std::string& vertex, const std::string& fragment) {
		m_assetPointer[name] = new T(vertex, fragment);;
	}

	T& get(const std::string& name) {
		return m_assets[name];
	}

	T* AssetManagerStatic::getAssetPointer(const std::string& name) {
		return m_assetPointer[name];
	}

	void clear() {
		for (auto& s : m_assetPointer)
			if (s.second) {
				delete s.second;
				s.second = NULL;
			}

		for (const auto& s : m_assets) {
			s.second.~T();
		}
	}
	
	
	AssetManagerStatic() {}
	~AssetManagerStatic() {}
	
private:
	std::unordered_map<std::string, T> m_assets;
	std::unordered_map<std::string, T*> m_assetPointer;

	static AssetManagerStatic s_instance;
};

template<typename T> AssetManagerStatic<T> AssetManagerStatic<T>::s_instance = AssetManagerStatic<T>();

