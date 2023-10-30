#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GraphicsEnums.h"
#include "AnimMat4.h"

class ShaderManager;

class Shader2 {
protected:

	std::string _name;

	std::string _vertexShaderFile;
	std::string _fragmentShaderFile;

	VertexType _vertexType;

	Shader2();

	void SetName(std::string name);

public:

	virtual ~Shader2();

	//
	std::string GetName();

	//use this current shader
	virtual void Bind() = 0;

	//load shader from file or from text
	virtual bool LoadFromFile(std::string vertexFile, std::string fragmentFile, VertexType vertexType) = 0;
	virtual bool LoadFromMemory(std::string vertexShader, std::string fragmentShader, VertexType vertexType) = 0;

	//set uniforms values
	virtual void SetUniform(ShaderType shaderType, std::string name, int val) = 0;
	virtual void SetUniform(ShaderType shaderType, std::string name, float val) = 0;
	virtual void SetUniform(ShaderType shaderType, std::string name, glm::vec3 & val) = 0;
	virtual void SetUniform(ShaderType shaderType, std::string name, glm::vec4 & val) = 0;
	virtual void SetUniform(ShaderType shaderType, std::string name, glm::mat3 & val) = 0;
	virtual void SetUniform(ShaderType shaderType, std::string name, glm::mat4 & val) = 0;

	virtual void SetUniform(ShaderType shaderType, std::string name, AnimMat4* inputArray, unsigned int arrayLength) = 0;
	virtual void Set(ShaderType shaderType, std::string name, std::vector<AnimMat4>& value) = 0;

	friend class ShaderManager;
};