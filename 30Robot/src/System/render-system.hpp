#pragma once

#include <Windows.h>

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <array>
#include <vector>
#include <string>

#include <Event/EventEmitter.h>
#include <Event/EventListener.h>
#include <Components/Components.h>

#include "Isystem.h"

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
	void initCursors();
	//SDL_Cursor* createCustomCursor(std::string imagePath);

private:
	const glm::mat4& m_view;
	const glm::mat4& m_projection;
	std::array<HCURSOR, 5> m_cursors;
	//std::vector<unsigned char*> m_cursorImages;
	//std::vector<SDL_Surface*> m_cursorSurfaces;

	CursorType m_currentCursor;
};
