#pragma once

#include <map>
#include <string>

#include "Shader2.h"

class ShaderManager {

private:

	static ShaderManager* _shaderManager;
	std::map<std::string, Shader2*> _shaders;

	ShaderManager();

public:

	static ShaderManager* Instance();

	Shader2* LoadFromFile(std::string name, std::string vertexFile, std::string fragmentFile);
	Shader2* LoadFromMemory(std::string name, const std::string& vertexShader, const std::string& fragmentShader);

	Shader2* Get(std::string name);

	void Remove(std::string name);
	void Remove(Shader2* shader);
	void RemoveAll();
};
	
