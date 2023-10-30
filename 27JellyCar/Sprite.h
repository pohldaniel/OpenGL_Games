#pragma once

#include "RenderManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Shader2.h"
#include "VertexTypes.h"

#include <glm/glm.hpp>

class Sprite
{
private:

	static int _objectCount;

	//vertex array
	VertexArrayObject* _arrayObject;

	//shader
	Shader2* _shader;

	//texture
	Texture2* _texture;

	std::string _name;

	float _angle;
	glm::vec2 _position;
	glm::vec2 _size;
	glm::vec2 _scale;
	glm::vec4 _color;
	glm::vec2 _location;

private:

	void CreateVertexObject(bool customUV);
	void UpdateSize();

public:

	static int GetObjectCount() { return _objectCount; }

public:

	Sprite(std::string name, std::string imageName, std::string vertexShader, std::string fragmentShader);
	Sprite(std::string name, std::string imageName, Shader2* shader);
	Sprite(std::string name, Texture2* texture, Shader2* shader);
	Sprite(std::string name, Texture2* texture, Shader2* shader, glm::vec2 location, glm::vec2 size);

	~Sprite();

	void SetShader(Shader2* shader);
	void SetTexture(Texture2* texture);

	Shader2* GetSHader();
	Texture2* GetTexture();

	void SetAngle(float angle);
	void SetPosition(glm::vec2 position);
	void SetSize(glm::vec2 size);
	void SetScale(glm::vec2 scale);
	void SetSolor(glm::vec4 color);

	void Draw(glm::mat4 &projection);
};