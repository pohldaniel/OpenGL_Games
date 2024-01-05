#ifndef RENDERCOLLIDERSYSTEM_H
#define RENDERCOLLIDERSYSTEM_H

#include <engine/Batchrenderer.h>
#include <engine/Rect.h>
#include <engine/Camera.h>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"

#include "TileSet.h"

class RenderColliderSystem: public System {
    public:
        RenderColliderSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update() {
            for (auto entity: GetSystemEntities()) {
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto collider = entity.GetComponent<BoxColliderComponent>();
           
				if (!ViewPort::IsRectOnScreen(transform.position.x + collider.offset.x, transform.position.y + collider.offset.y, collider.width * transform.scale.x, collider.height * transform.scale.y)) {
					continue;
				}

				const TextureRect& rect = TileSetManager::Get().getTileSet("desert").getTextureRects()[LevelLoader::SpriteMap.at("empty-texture").first];

				Batchrenderer::Get().addQuadAA(Vector4f(transform.position.x + collider.offset.x, transform.position.y + collider.offset.y, collider.width * transform.scale.x, collider.height * transform.scale.y), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 0.0f, 0.0f, 1.0f), rect.frame);
            }
        }
};

#endif
