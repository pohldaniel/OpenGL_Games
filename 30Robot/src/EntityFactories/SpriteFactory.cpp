#include "SpriteFactory.h"

#include "graphics/vertex-array.hpp"
#include "Globals.h"

SpriteFactory::SpriteFactory()
:	m_shaderTex("res/shaders/texture/texture.vert", "res/shaders/texture/texture.frag"),
	m_shaderTexArray("res/shaders/texture/texture.vert", "res/shaders/texture/texture-array.frag"),
	m_towerExplosionShader("res/shaders/texture/texture.vert", "res/shaders/texture/tower-explosion.frag"),
	m_enemyExplosionShader("res/shaders/texture/texture.vert", "res/shaders/texture/enemy-explosion.frag")
{
   
}

cmpt::Sprite SpriteFactory::createSingle(const std::string& textureFilepath, glm::vec2 displaySize, Shape* shape, float scaleX, float scaleY) {
	TextureCache::Get().addTexture(textureFilepath);
    return cmpt::Sprite(TextureCache::Get().getTexture(textureFilepath).getTexture(), GL_TEXTURE_2D, &m_shaderTex, shape, scaleX, scaleY, 0.0f, 0.0f);
}

cmpt::Sprite SpriteFactory::createAtlas(const std::string& textureFilepath, glm::vec2 displaySize, glm::vec2 tileSize, Shape* shape, float scaleX, float scaleY) {
	return createAtlas(textureFilepath, displaySize, tileSize, ShaderType::BASIC_ATLAS, shape, scaleX, scaleY);
}

cmpt::Sprite SpriteFactory::createAtlas(const std::string& textureFilepath, glm::vec2 displaySize, glm::vec2 tileSize, ShaderType shaderType, Shape* shape, float scaleX, float scaleY) {
	Shader& shader = getShader(shaderType);
	SpritesheetCache::Get().addSpritesheet(textureFilepath, (unsigned short)tileSize.x, (unsigned short)tileSize.y);
    return cmpt::Sprite(SpritesheetCache::Get().getSpritesheet(textureFilepath).getAtlas(), GL_TEXTURE_2D_ARRAY, &shader, shape, scaleX, scaleY, 0.0f, 0.0f);
}

Shader& SpriteFactory::getShader(ShaderType shaderType) {
	switch (shaderType) {		
		case ShaderType::BASIC_ATLAS :
			return m_shaderTexArray;
			break;
		case ShaderType::TOWER_EXPLOSION :
			return m_towerExplosionShader;
			break;
		case ShaderType::ENEMY_EXPLOSION:
			return m_enemyExplosionShader;
			break;
		default:case ShaderType::BASIC_SINGLE:
			return m_shaderTex;
			break;
	}
}

TextureCache TextureCache::s_instance;

const Texture& TextureCache::getTexture(std::string name) const {
	return m_textures.at(name);
}

void TextureCache::addTexture(std::string name) {
	m_textures[name].loadFromFile(name, true, GL_RGBA8, GL_RGBA);
}

bool TextureCache::containsTexture(std::string name) {
	return m_textures.count(name) == 1;
}

TextureCache& TextureCache::Get() {
	return s_instance;
}

SpritesheetCache SpritesheetCache::s_instance;

const Spritesheet& SpritesheetCache::getSpritesheet(std::string name) const {
	return m_spritesheets.at(name);
}

void SpritesheetCache::addSpritesheet(std::string name, unsigned short tileWidth, unsigned short tileHeight) {
	m_spritesheets[name].loadFromFile(name, tileWidth, tileHeight, 0u, false, true);
}

bool SpritesheetCache::containsSpritesheet(std::string name) {
	return m_spritesheets.count(name) == 1;
}

SpritesheetCache& SpritesheetCache::Get() {
	return s_instance;
}