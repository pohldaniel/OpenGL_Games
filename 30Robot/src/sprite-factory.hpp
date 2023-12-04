#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "graphics/shaderRo.hpp"
#include "graphics/index-buffer.hpp"
#include "components/sprite.hpp"

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
	ShaderRo& getShader(ShaderType shaderType);
		//Default
	ShaderRo m_shaderTex;
	ShaderRo m_shaderTexArray;
		//Custom
	ShaderRo m_towerExplosionShader;
	ShaderRo m_enemyExplosionShader;
};