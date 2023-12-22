#include "SpriteFactory.h"

#include "graphics/vertex-array.hpp"
#include "Globals.h"

SpriteFactory::SpriteFactory()
:	m_shaderTex("res/shaders/texture/texture.vert", "res/shaders/texture/texture.frag"),
	m_shaderTexArray("res/shaders/texture/texture.vert", "res/shaders/texture/texture-array.frag"),
	m_towerExplosionShader("res/shaders/texture/texture.vert", "res/shaders/texture/tower-explosion.frag"),
	m_enemyExplosionShader("res/shaders/texture/texture.vert", "res/shaders/texture/enemy-explosion.frag")
{
    /* Index buffer */
	unsigned int indices[] = {
		0, 1, 2, // First triangle
		3, 1, 2  // Second triangle
	};
	unsigned int arraySize = sizeof(indices) / sizeof(unsigned int);
    m_ib.init(indices, arraySize);
    m_ib.unbind();
}

cmpt::Sprite SpriteFactory::createSingle(const std::string& textureFilepath, glm::vec2 displaySize, Shape* shape, float scaleX, float scaleY) {
	displaySize /= 2;
	/* Vertex buffer */
	float positions[] = {
        // Pos                           // UV
		-displaySize.x,  displaySize.y,  0.0f, 1.0f, // 0 topleft
		 displaySize.x,  displaySize.y,  1.0f, 1.0f, // 1 topright
		-displaySize.x, -displaySize.y,  0.0f, 0.0f, // 2 bottomleft
		 displaySize.x, -displaySize.y,  1.0f, 0.0f  // 3 bottomright
	};
    unsigned int arraySize = sizeof(positions) / sizeof(float);
    VertexBuffer vb(positions, arraySize * sizeof(float), GL_STATIC_DRAW);
	VertexBufferLayout vbLayout;
	vbLayout.push<float>(2); // Pos (x, y)
    vbLayout.push<float>(2); // Texture coordinates

    /* Vertex array */
    VertexArray va;
	va.addBuffer(vb, vbLayout);

    /* Uniforms */
    m_shaderTex.use();
    m_shaderTex.loadMatrix("u_mvp", (const float*)glm::value_ptr(glm::mat4(1.0f)));
	//Texture texture(textureFilepath, true, GL_RGBA8, GL_RGBA);
	TextureCache::Get().addTexture(textureFilepath);

    //texture.bind();
	m_shaderTex.loadInt("u_texture", 0);
    
    /* Cleanup */
    m_shaderTex.unuse();
    va.unbind();
    vb.unbind();
	//texture.unbind();

    /* Send copy of object */
    cmpt::Sprite mySprite(scaleX, scaleY, shape, TextureCache::Get().getTexture(textureFilepath).getTexture(), va.getID(), GL_TEXTURE_2D, &m_shaderTex, &m_ib);
    return mySprite;
}

cmpt::Sprite SpriteFactory::createAtlas(const std::string& textureFilepath, glm::vec2 displaySize, glm::vec2 tileSize, Shape* shape, float scaleX, float scaleY) {
	return createAtlas(textureFilepath, displaySize, tileSize, ShaderType::BASIC_ATLAS, shape, scaleX, scaleY);
}

cmpt::Sprite SpriteFactory::createAtlas(const std::string& textureFilepath, glm::vec2 displaySize, glm::vec2 tileSize, ShaderType shaderType, Shape* shape, float scaleX, float scaleY) {
	Shader& shader = getShader(shaderType);
	displaySize /= 2;
    float positions[] = {
        // Pos                           // Inverted UV to start at topleft
		-displaySize.x,  displaySize.y,  0.0f, 1.0f, // 0 topleft
		 displaySize.x,  displaySize.y,  1.0f, 1.0f, // 1 topright
		-displaySize.x, -displaySize.y,  0.0f, 0.0f, // 2 bottomleft
		 displaySize.x, -displaySize.y,  1.0f, 0.0f  // 3 bottomright
	};
    unsigned int arraySize = sizeof(positions) / sizeof(float);
    VertexBuffer vb(positions, arraySize * sizeof(float), GL_STATIC_DRAW);
	VertexBufferLayout vbLayout;
	vbLayout.push<float>(2); // Pos (x, y)
    vbLayout.push<float>(2); // Texture coordinates

    /* Vertex array */
    VertexArray va;
	va.addBuffer(vb, vbLayout);
	
    /* Uniforms */
	shader.use();
	shader.loadMatrix("u_mvp", (const float*)glm::value_ptr(glm::mat4(1.0f)));
	SpritesheetCache::Get().addSpritesheet(textureFilepath, (unsigned short)tileSize.x, (unsigned short)tileSize.y);

	shader.loadInt("u_texture", 0);
	shader.loadInt("u_activeTile", 0);
    
    /* Cleanup */
	shader.unuse();
    va.unbind();
    vb.unbind();

    
    /* Send copy of object */
    cmpt::Sprite mySprite(scaleX, scaleY, shape, SpritesheetCache::Get().getSpritesheet(textureFilepath).getAtlas(), va.getID(), GL_TEXTURE_2D_ARRAY, &shader, &m_ib);
    return mySprite;
}


// TODO ajouter fonctions "createAtlasInstanced" + "createSingleInstanced" pour des sprites partagés entre plusieurs entités (fonctions opengl spéciales)

Shader& SpriteFactory::getShader(ShaderType shaderType) {
	switch (shaderType) {
	case ShaderType::BASIC_SINGLE :
		return m_shaderTex;
		break;
	case ShaderType::BASIC_ATLAS :
		return m_shaderTexArray;
		break;
	case ShaderType::TOWER_EXPLOSION :
		return m_towerExplosionShader;
		break;
	case ShaderType::ENEMY_EXPLOSION:
		return m_enemyExplosionShader;
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