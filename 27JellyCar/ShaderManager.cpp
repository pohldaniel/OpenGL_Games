#include "ShaderManager.h"
#include "FileSystem/FileManager.h"
#include "RenderManager.h"

ShaderManager* ShaderManager::_shaderManager = 0;

ShaderManager::ShaderManager() {

}

ShaderManager* ShaderManager::Instance(){
	if (_shaderManager == 0)
	{
		_shaderManager = new ShaderManager();
	}

	return _shaderManager;
}

Shader2* ShaderManager::LoadFromFile(std::string name, std::string vertexFile, std::string fragmentFile, VertexType vertexType) {

	if (_shaders.find(name) == _shaders.end()){
		//Utils::Logger::Instance()->Log("ShaderManager::LoadFromFile: %s \n", name.c_str());

		//here change name based on platform
		vertexFile = vertexFile + RenderManager::Instance()->GetVertexShaderName();
		fragmentFile = fragmentFile + RenderManager::Instance()->GetFragmnetShaderName();

		if (!FileManager::Instance()->FileExists(vertexFile))
		{
			//Utils::Logger::Instance()->Log("ShaderManager::LoadFromFile::Can't load file: %s \n", vertexFile.c_str());
			return nullptr;
		}

		if (!FileManager::Instance()->FileExists(fragmentFile))
		{
			//Utils::Logger::Instance()->Log("ShaderManager::LoadFromFile::Can't load file: %s \n", fragmentFile.c_str());
			return nullptr;
		}


		Shader2* shader = RenderManager::Instance()->CreateShader();

		//Utils::Logger::Instance()->Log("ShaderManager::LoadFromFile: shader created\n");
		//Utils::Logger::Instance()->Log("ShaderManager::LoadFromFile: loading %s and %s\n", vertexFile.c_str(), fragmentFile.c_str());

		//set name
		shader->SetName(name);

		//load shader
		shader->LoadFromFile(vertexFile, fragmentFile, vertexType);

		//add to collection
		_shaders.insert(std::pair<std::string, Shader2*>(name, shader));

		return shader;
	}

	return _shaders[name];
}

Shader2* ShaderManager::LoadFromMemory(std::string name, const std::string& vertexShader, const std::string& fragmentShader, VertexType vertexType)
{
	if (_shaders.find(name) == _shaders.end())
	{
		//Utils::Logger::Instance()->Log("ShaderManager::LoadFromMemory: %s \n", name.c_str());

		if (vertexShader.empty())
		{
			//Utils::Logger::Instance()->Log("ShaderManager::LoadFromMemory::Vertex shader is empty \n");
			return nullptr;
		}

		if (fragmentShader.empty())
		{
			//Utils::Logger::Instance()->Log("ShaderManager::LoadFromMemory::Fragment shader is empty \n");
			return nullptr;
		}

		Shader2* shader = RenderManager::Instance()->CreateShader();

		//set name
		shader->SetName(name);

		//load shader
		shader->LoadFromMemory(vertexShader, fragmentShader, vertexType);

		//add to collection
		_shaders.insert(std::pair<std::string, Shader2*>(name, shader));

		return shader;
	}

	return _shaders[name];
}

Shader2* ShaderManager::Get(std::string name)
{
	if (_shaders.find(name) == _shaders.end())
	{
		return 0;
	}

	return _shaders[name];
}

void ShaderManager::Remove(std::string name)
{
	std::map<std::string, Shader2*>::iterator it = _shaders.find(name);

	if (it == _shaders.end())
	{
		return;
	}

	//Utils::Logger::Instance()->Log("ShaderManager::Remove: %s \n", name.c_str());

	delete _shaders[name];
	_shaders.erase(it);
}

void ShaderManager::Remove(Shader2* shader)
{
	Remove(shader->GetName());
}

void ShaderManager::RemoveAll()
{
	std::map<std::string, Shader2*>::iterator iter;

	for (iter = _shaders.begin(); iter != _shaders.end(); ++iter)
	{
		//Utils::Logger::Instance()->Log("Delete shader: %s \n", iter->first.c_str());
		delete iter->second;
	}

	_shaders.clear();
}