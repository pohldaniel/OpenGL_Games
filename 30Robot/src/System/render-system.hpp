#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <array>
#include <vector>
#include <string>

#include "Isystem.h"
#include "Event/EventEmitter.h"
#include "components/transform.hpp"
#include "components/sprite.hpp"
#include "components/sprite-animation.hpp"
#include "Event/interactions/change-cursor.hpp"

/* TODO use framebuffer */

class RenderSystem : public ISystem {
public:
    RenderSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, glm::mat4& viewMat, glm::mat4& projMat);
	virtual ~RenderSystem();

    void update(float deltatime) override;

private:
	void renderSprite(std::uint32_t entity, cmpt::Sprite & sprite) const;
	void renderSpritesheet(std::uint32_t entity, cmpt::Sprite& sprite, cmpt::SpriteAnimation& animation) const;
	glm::mat4 getModelMatrix(unsigned int entityId) const;
	glm::mat4 getModelMatrix(cmpt::Transform& transform) const;
	glm::mat4 getViewMatrix() const;
	//void initCursors();
	//SDL_Cursor* createCustomCursor(std::string imagePath);

private:
	const glm::mat4& m_view;
	const glm::mat4& m_projection;
	//std::array<SDL_Cursor*, 7> m_cursors;
	//std::vector<unsigned char*> m_cursorImages;
	//std::vector<SDL_Surface*> m_cursorSurfaces;
};
