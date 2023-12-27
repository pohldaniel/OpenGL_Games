#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <algorithm>
#include <engine/Batchrenderer.h>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "LevelLoader.h"

class RenderSystem: public System {
    public:
        RenderSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();
        }

        void Update(Rect& camera) {
            // Create a vector with both Sprite and Transform component of all entities
            struct RenderableEntity {
                TransformComponent transformComponent;
                SpriteComponent spriteComponent;
            };
            std::vector<RenderableEntity> renderableEntities; 
            for (auto entity: GetSystemEntities()) {
                RenderableEntity renderableEntity;
                renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
                renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();

                // Check if the entity sprite is outside the camera view
                bool isOutsideCameraView = (
                    renderableEntity.transformComponent.position.x + (renderableEntity.transformComponent.scale.x * renderableEntity.spriteComponent.textureRect.width) < camera.posX ||
                    renderableEntity.transformComponent.position.x > camera.posX + camera.width ||
                    renderableEntity.transformComponent.position.y + (renderableEntity.transformComponent.scale.y * renderableEntity.spriteComponent.textureRect.height) < camera.posY ||
                    renderableEntity.transformComponent.position.y > camera.posY + camera.height
                );

                // Cull sprites that are outside the camera viww (and are not fixed)
                if (isOutsideCameraView && !renderableEntity.spriteComponent.isFixed) {
                    //continue;
                }

                renderableEntities.emplace_back(renderableEntity);
            }

            // Sort the vector by the z-index value
            std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity& a, const RenderableEntity& b) {
                return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
            });

            // Loop all entities that the system is interested in
            for (auto entity: renderableEntities) {
				
                const auto transform = entity.transformComponent;
                // Set the source rectangle of our original sprite texture
				const TextureRect& srcRect = entity.spriteComponent.textureRect;
				Batchrenderer::Get().addQuadAA(Vector4f(transform.position.x, transform.position.y, srcRect.width * transform.scale.x, srcRect.height * transform.scale.y), Vector4f(srcRect.textureOffsetX, srcRect.textureOffsetY, srcRect.textureWidth, srcRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), srcRect.frame);

            }

        }
};

#endif
