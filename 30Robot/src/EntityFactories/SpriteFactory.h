#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <engine/Shader.h>
#include <engine/Texture.h>
#include <engine/Spritesheet.h>
#include <Components/Components.h>

#include "graphics/index-buffer.hpp"


class SpritesheetCache {

public:

	const Spritesheet& getSpritesheet(std::string name) const;
	void addSpritesheet(std::string name, unsigned short tileWidth, unsigned short tileHeight);
	static SpritesheetCache& Get();

	bool containsSpritesheet(std::string name);

private:
	SpritesheetCache() = default;

	std::unordered_map<std::string, Spritesheet> m_spritesheets;
	static SpritesheetCache s_instance;
};

class TextureCache {

public:

	const Texture& getTexture(std::string name) const;
	void addTexture(std::string name);
	static TextureCache& Get();

	bool containsTexture(std::string name);

private:
	TextureCache() = default;

	std::unordered_map<std::string, Texture> m_textures;
	static TextureCache s_instance;
};

enum class ShaderType {
	BASIC_SINGLE,
	BASIC_ATLAS,
	TOWER_EXPLOSION,
	ENEMY_EXPLOSION
};

class SpriteFactory {
public:
    SpriteFactory();

    cmpt::Sprite createSingle(const std::string& textureFilepath, glm::vec2 displaySize);
    cmpt::Sprite createAtlas(const std::string& textureFilepath, glm::vec2 displaySize, glm::vec2 tileSize, ShaderType shaderType);
	cmpt::Sprite createAtlas(const std::string& textureFilepath, glm::vec2 displaySize, glm::vec2 tileSize);
    // TODO batch rendering with glTextureView ? https://learnopengl.com/Advanced-OpenGL/Instancing for all since they share vertex data

private:
	// TODO make shader static because shared among each instance
	// -> Init problem because glad is not up at compile time to create the shader

	IndexBuffer m_ib;    // All sprites shares the same index buffer
	//Shaders
	Shader& getShader(ShaderType shaderType);
		//Default
	Shader m_shaderTex;
	Shader m_shaderTexArray;
		//Custom
	Shader m_towerExplosionShader;
	Shader m_enemyExplosionShader;
};