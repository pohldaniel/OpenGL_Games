#include <iostream>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Components/Components.h>
#include <Event/EventListener.h>
#include <Services/IHelper.h>
#include <Services/Locator.h>

#include "RenderSystem.h"
#include "Tags.h"
#include "Maths.h"
#include "Constants.h"
#include "Application.h"
#include "Globals.h"

RenderSystem::RenderSystem(entt::DefaultRegistry& registry, EventEmitter& emitter, glm::mat4& viewMat, glm::mat4& projMat) : ISystem(registry, emitter), m_view(viewMat), m_projection(projMat), m_currentCursor(CursorType::ARROW){
	
	m_emitter.on<evnt::EntityDamaged>([this](const evnt::EntityDamaged & event, EventEmitter & emitter) {
		if (m_registry.valid(event.entity)) {
			//Shake
			if (m_registry.has<cmpt::Shake>(event.entity)) {
				IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
				float agl = randomService.random(imaths::TAU);
				m_registry.get<cmpt::Shake>(event.entity).offset = 0.3f*glm::vec2(cos(agl), sin(agl));
			}
		}
	});

	initCursors();

	m_emitter.on<evnt::ChangeCursor>([this](const evnt::ChangeCursor & event, EventEmitter & emitter) {
		if (m_currentCursor != event.cursor) {
			Application::SetCursorIcon(m_cursors.at(event.cursor));
			m_currentCursor = event.cursor;
		}
	});
}

void RenderSystem::initCursors() {
	m_cursors.at(CursorType::ARROW) = LoadCursor(nullptr, IDC_ARROW);
	m_cursors.at(CursorType::NO) = LoadCursor(nullptr, IDC_NO);
	m_cursors.at(CursorType::ROTATION) = LoadCursorFromFileA("res/cursors/rotate.cur");
	m_cursors.at(CursorType::ACTIVATE) = LoadCursorFromFileA("res/cursors/on.cur");
	m_cursors.at(CursorType::DESACTIVATE) = LoadCursorFromFileA("res/cursors/off.cur");
}

RenderSystem::~RenderSystem() {
	
}

void RenderSystem::renderSpritesheet(std::uint32_t entity, cmpt::Sprite& sprite, cmpt::SpriteAnimation& animation) const {	
	
	if (sprite.shape) {
	
		IHelper& helper = entt::ServiceLocator<IHelper>::ref();

		sprite.shader->use();

		glm::mat4 mvp = m_projection * this->getViewMatrix() * this->getModelMatrix(entity, 0.0f, 0.0f, sprite.scaleX, sprite.scaleY);
		sprite.shader->loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
		sprite.shader->loadInt("u_activeTile", animation.activeTile);
		if (m_registry.valid(entity)) {
			sprite.shader->loadVector("tintColour", (const float*)glm::value_ptr(helper.getColour(entity)));
			sprite.shader->loadFloat("u_alpha", helper.getAlpha(entity));
			if (m_registry.has<cmpt::AnimationPixelsVanish>(entity) || (m_registry.has<cmpt::AttachedTo>(entity) && m_registry.has<cmpt::AnimationPixelsVanish>(m_registry.get<cmpt::AttachedTo>(entity).mainEntity))) {
				cmpt::Animated animated = cmpt::Animated(0);
				bool bForward;
				if (m_registry.has<cmpt::AnimationPixelsVanish>(entity)) {
					animated = m_registry.get<cmpt::Animated>(entity);
					bForward = m_registry.get<cmpt::AnimationPixelsVanish>(entity).bForward;
				}
				else {
					animated = m_registry.get<cmpt::Animated>(m_registry.get<cmpt::AttachedTo>(entity).mainEntity);
					bForward = m_registry.get<cmpt::AnimationPixelsVanish>(m_registry.get<cmpt::AttachedTo>(entity).mainEntity).bForward;
				}
				if (bForward) {
					sprite.shader->loadFloat("probaDisappear", 1 - animated.age / animated.duration);
				}
				else {
					sprite.shader->loadFloat("probaDisappear", animated.age / animated.duration);
				}
			}
			else {
				sprite.shader->loadFloat("probaDisappear", 0);
			}
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(sprite.target, sprite.textureID);

		sprite.shape->drawRaw();

		glBindTexture(sprite.target, 0);
		sprite.shader->unuse();
	}
}

void RenderSystem::renderSprite(std::uint32_t entity, cmpt::Sprite & sprite) const {
	if (sprite.shape) {
		IHelper& helper = entt::ServiceLocator<IHelper>::ref();

		sprite.shader->use();
		glm::mat4 mvp = m_projection * this->getViewMatrix() * this->getModelMatrix(entity, 0.0f, 0.0f, sprite.scaleX, sprite.scaleY);
		sprite.shader->loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
		if (m_registry.valid(entity)) {
			sprite.shader->loadVector("tintColour", (const float*)glm::value_ptr(helper.getColour(entity)));
			sprite.shader->loadFloat("u_alpha", helper.getAlpha(entity));
			if (m_registry.has<cmpt::AnimationPixelsVanish>(entity) || (m_registry.has<cmpt::AttachedTo>(entity) && m_registry.has<cmpt::AnimationPixelsVanish>(m_registry.get<cmpt::AttachedTo>(entity).mainEntity))) {
				cmpt::Animated animated = cmpt::Animated(0);
				bool bForward;
				if (m_registry.has<cmpt::AnimationPixelsVanish>(entity)) {
					animated = m_registry.get<cmpt::Animated>(entity);
					bForward = m_registry.get<cmpt::AnimationPixelsVanish>(entity).bForward;
				}
				else {
					animated = m_registry.get<cmpt::Animated>(m_registry.get<cmpt::AttachedTo>(entity).mainEntity);
					bForward = m_registry.get<cmpt::AnimationPixelsVanish>(m_registry.get<cmpt::AttachedTo>(entity).mainEntity).bForward;
				}
				if (bForward) {
					sprite.shader->loadFloat("probaDisappear", 1 - animated.age / animated.duration);
				}
				else {
					sprite.shader->loadFloat("probaDisappear", animated.age / animated.duration);
				}
			}
			else {
				sprite.shader->loadFloat("probaDisappear", 0);
			}
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(sprite.target, sprite.textureID);

		sprite.shape->drawRaw();

		glBindTexture(sprite.target, 0);
		sprite.shader->unuse();
	}
}

void RenderSystem::update(float deltatime) {
	/* 
        TODO find a way to use only a few glDraw by sharing buffer or using vertex array. Each draw call should draw all sprites of a particular type. For uniforms, transfer them to vertex attributes
        https://community.khronos.org/t/best-practices-to-render-multiple-2d-sprite-with-vbo/74096
    */

	IHelper& helper = entt::ServiceLocator<IHelper>::ref();
	helper.updateScreenShake(deltatime);

    m_registry.view<cmpt::Transform, cmpt::Primitive>().each([this](auto entity, cmpt::Transform& transform, cmpt::Primitive& primitive) {

		primitive.shader->use();
		glm::mat4 mvp = this->m_projection * this->getViewMatrix() * this->getModelMatrix(entity, primitive.pivotX, primitive.pivotY, primitive.scaleX, primitive.scaleY);
		primitive.shader->loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
		primitive.shader->loadVector("u_color", primitive.color.r, primitive.color.g, primitive.color.b, primitive.color.a);
		if (m_registry.valid(entity) && m_registry.has<cmpt::TintColour>(entity)) {
			cmpt::TintColour& tint = m_registry.get<cmpt::TintColour>(entity);
			primitive.shader->loadVector("tintColour", tint.col.r, tint.col.g, tint.col.b, tint.col.a);
			if (tint.bOneTimeOnly) {
				m_registry.remove<cmpt::TintColour>(entity);
			}
		}else {
			primitive.shader->loadVector("tintColour", 0.0f, 0.0f, 0.0f, 0.0f);
		}
		primitive.shape->drawRaw();
		primitive.shader->unuse();

    });

	
	//Render tiles
	m_registry.view<renderTag::Single, cmpt::Sprite, renderOrderTag::o_Tile>().each([this](auto entity, auto, cmpt::Sprite & sprite, auto) {
		this->renderSprite(entity, sprite);
	});
	m_registry.view<renderTag::Atlas, cmpt::Sprite, cmpt::SpriteAnimation, renderOrderTag::o_Tile>().each([this](auto entity, auto, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation, auto) {
		this->renderSpritesheet(entity, sprite, animation);
	});
	//Highlight tile under mouse while an entity is being placed
	if (m_emitter.entityBeingPlaced) {
		IHelper& helper = entt::ServiceLocator<IHelper>::ref();
		IDebugDraw& debugDraw = entt::ServiceLocator<IDebugDraw>::ref();
		std::uint32_t tile = helper.getTileFromProjCoord(m_emitter.mousePos.x, m_emitter.mousePos.y);
		if (m_registry.valid(tile)) {
			if (m_registry.has<tileTag::Constructible>(tile)) {
				debugDraw.setColor(glm::vec4(0, 255, 0, 0.3));
			}
			else {
				debugDraw.setColor(glm::vec4(255, 0, 0, 0.3));
			}
			glm::vec2 tilePos = m_registry.get<cmpt::Transform>(tile).position;
			debugDraw.rect(tilePos.x - TILE_SIZE / 2, tilePos.y - TILE_SIZE / 2, tilePos.x + TILE_SIZE / 2, tilePos.y + TILE_SIZE / 2, Z_INDEX_TOWER);
		}
	}
	//Render enemies
	m_registry.view<renderTag::Single, cmpt::Sprite, renderOrderTag::o_Enemy>().each([this](auto entity, auto, cmpt::Sprite & sprite, auto) {
		this->renderSprite(entity, sprite);
	});
	m_registry.view<renderTag::Atlas, cmpt::Sprite, cmpt::SpriteAnimation, renderOrderTag::o_Enemy>().each([this](auto entity, auto, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation, auto) {
		this->renderSpritesheet(entity, sprite, animation);
	});
	m_registry.view<renderTag::Single, cmpt::Sprite, renderOrderTag::o_Enemy2>().each([this](auto entity, auto, cmpt::Sprite & sprite, auto) {
		this->renderSprite(entity, sprite);
	});
	m_registry.view<renderTag::Atlas, cmpt::Sprite, cmpt::SpriteAnimation, renderOrderTag::o_Enemy2>().each([this](auto entity, auto, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation, auto) {
		this->renderSpritesheet(entity, sprite, animation);
	});
	//Render buildings
	m_registry.view<renderTag::Single, cmpt::Sprite, renderOrderTag::o_Building>().each([this](auto entity, auto, cmpt::Sprite & sprite, auto) {
		this->renderSprite(entity, sprite);
	});
	m_registry.view<renderTag::Atlas, cmpt::Sprite, cmpt::SpriteAnimation, renderOrderTag::o_Building>().each([this](auto entity, auto, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation, auto) {
		this->renderSpritesheet(entity, sprite, animation);
	});
	//Render projectiles
	m_registry.view<renderTag::Single, cmpt::Sprite, renderOrderTag::o_Projectile>().each([this](auto entity, auto, cmpt::Sprite & sprite, auto) {
		this->renderSprite(entity, sprite);
	});
	m_registry.view<renderTag::Atlas, cmpt::Sprite, cmpt::SpriteAnimation, renderOrderTag::o_Projectile>().each([this](auto entity, auto, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation, auto) {
		this->renderSpritesheet(entity, sprite, animation);
	});
	//Render VFX
	m_registry.view<renderTag::Single, cmpt::Sprite, renderOrderTag::o_VFX>().each([this](auto entity, auto, cmpt::Sprite & sprite, auto) {
		this->renderSprite(entity, sprite);
	});
	m_registry.view<renderTag::Atlas, cmpt::Sprite, cmpt::SpriteAnimation, renderOrderTag::o_VFX>().each([this](auto entity, auto, cmpt::Sprite & sprite, cmpt::SpriteAnimation & animation, auto) {
		this->renderSpritesheet(entity, sprite, animation);
	});

	//Healthbars
	m_registry.view<cmpt::Transform, cmpt::Health, cmpt::HealthBar>().each([this](auto entity, cmpt::Transform & transform, cmpt::Health & health, cmpt::HealthBar & healthbar) {
		if (health.current != health.max && !m_registry.has<cmpt::Animated>(entity)) {
			IHelper& helper = entt::ServiceLocator<IHelper>::ref();
			// Background
			{

				healthbar.background.shader->use();
				// Update pos
				cmpt::Transform healthTransform(helper.getPosition(entity), Z_INDEX_HUD - 1);
				healthTransform.position += healthbar.relativePos;

				// Updates
				glm::mat4 mvp = this->m_projection * this->getViewMatrix() * this->getModelMatrix(healthTransform, healthbar.background.pivotX, healthbar.background.pivotY, healthbar.background.scaleX, healthbar.background.scaleY);
				healthbar.background.shader->loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
				healthbar.background.shader->loadVector("u_color", healthbar.background.color.r, healthbar.background.color.g, healthbar.background.color.b, healthbar.background.color.a);
					
				healthbar.background.shape->drawRaw();
				healthbar.background.shader->unuse();

			}

			// Foreground
			{
				healthbar.background.shader->use();
				// Update pos
				cmpt::Transform healthTransform(helper.getPosition(entity), Z_INDEX_HUD);
				healthTransform.position += healthbar.relativePos;
				float scale = imaths::rangeMapping(health.current, 0, health.max, 0, 1);
				healthTransform.scale = glm::vec2(scale, 1.0f);

				// Updates
				glm::mat4 mvp = this->m_projection * this->getViewMatrix() * this->getModelMatrix(healthTransform, healthbar.background.pivotX, healthbar.background.pivotY, healthbar.background.scaleX, healthbar.background.scaleY);
				healthbar.bar.shader->loadMatrix("u_mvp", (const float*)glm::value_ptr(mvp));
				if (scale > 0.4f) {
						healthbar.bar.shader->loadVector("u_color", healthbar.bar.color.r, healthbar.bar.color.g, healthbar.bar.color.b, healthbar.bar.color.a);
				}else {
						healthbar.bar.shader->loadVector("u_color", 1.0f, 0.0f, 0.0f, healthbar.bar.color.a);
				}

				healthbar.background.shape->drawRaw();
				healthbar.background.shader->unuse();

			}
		}
	});

	//Growing kamikaze explosion
	m_registry.view<cmpt::GrowingCircle, cmpt::Age, cmpt::Transform, renderOrderTag::o_VFX>().each([this](auto entity, cmpt::GrowingCircle& growingCircle, cmpt::Age& age, cmpt::Transform& transform, auto) {
		IDebugDraw& debugDraw = entt::ServiceLocator<IDebugDraw>::ref();
		float r = growingCircle.growthSpeed * age.age;
		debugDraw.circleExplosion(transform.position.x, transform.position.y, r, growingCircle.maxRadius);
	});
}

glm::mat4 RenderSystem::getModelMatrix(unsigned int entityId, float pivotX, float pivotY, float scaleX, float scaleY) const {
	IHelper& helper = entt::ServiceLocator<IHelper>::ref();
	glm::mat4 model(1.0f);
	cmpt::Transform& transform = m_registry.get<cmpt::Transform>(entityId);
	model = glm::translate(model, glm::vec3(helper.getPosition(entityId), transform.zIndex));
	if (!m_registry.has<entityTag::Tower>(entityId) && !m_registry.has<entityTag::Mirror>(entityId)) {
		model = glm::rotate(model, transform.rotation, glm::vec3(0, 0, 1));
	}
	model = glm::scale(model, glm::vec3(helper.getScale(entityId), 0.0f));
	model = glm::scale(model, glm::vec3(scaleX, scaleY, 1.0f));
	model = glm::translate(model, glm::vec3(pivotX, pivotY, 1.0f));
	return model;
}

glm::mat4 RenderSystem::getModelMatrix(cmpt::Transform& transform, float pivotX, float pivotY, float scaleX, float scaleY) const {
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(transform.position, transform.zIndex));
	model = glm::rotate(model, transform.rotation, glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(transform.scale, 0.0f));
	model = glm::scale(model, glm::vec3(scaleX, scaleY, 1.0f));
	model = glm::translate(model, glm::vec3(pivotX, pivotY, 1.0f));
	return model;
}

glm::mat4 RenderSystem::getViewMatrix() const {
	IHelper& helper = entt::ServiceLocator<IHelper>::ref();
	glm::mat4 view(1.0f);
	view *= m_view;
	view = glm::translate(view, glm::vec3(helper.getScreenShake(),0));
	return view;
}