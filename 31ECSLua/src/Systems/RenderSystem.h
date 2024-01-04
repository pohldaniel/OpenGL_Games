#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <algorithm>
#include <engine/Batchrenderer.h>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"

#include "LevelLoader.h"
#include "ViewPort.h"

class RenderSystem: public System {
    public:
        RenderSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();
        }

        void Update() {
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

              
                if (!ViewPort::IsRectOnScreen(renderableEntity.transformComponent.position.x, renderableEntity.transformComponent.position.y, renderableEntity.spriteComponent.textureRect.width * renderableEntity.transformComponent.scale.x, renderableEntity.transformComponent.scale.y * renderableEntity.spriteComponent.textureRect.height) && !renderableEntity.spriteComponent.isFixed) {
                    continue;
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

				if (!entity.spriteComponent.isFixed) {
					Batchrenderer::Get().addRotatedQuadLH(Vector4f(transform.position.x, transform.position.y - srcRect.height * transform.scale.y, srcRect.width * transform.scale.x, srcRect.height * transform.scale.y),
						transform.rotation,
						srcRect.width * transform.scale.x * 0.5f,
						srcRect.height * transform.scale.y * 0.5f,
						Vector4f(srcRect.textureOffsetX, srcRect.textureOffsetY, srcRect.textureWidth, srcRect.textureHeight),
						Vector4f(1.0f, 1.0f, 1.0f, 1.0f), srcRect.frame);
				}else {
					ViewPort& viewPort = ViewPort::Get();
					Batchrenderer::Get().addRotatedQuadLH(Vector4f(viewPort.getPositionX() + transform.position.x, viewPort.getPositionY() + static_cast<float>(Application::Height) - ( transform.position.y + srcRect.height * transform.scale.y), srcRect.width * transform.scale.x, srcRect.height * transform.scale.y),
						transform.rotation,
						srcRect.width * transform.scale.x * 0.5f,
						srcRect.height * transform.scale.y * 0.5f,
						Vector4f(srcRect.textureOffsetX, srcRect.textureOffsetY, srcRect.textureWidth, srcRect.textureHeight),
						Vector4f(1.0f, 1.0f, 1.0f, 1.0f), srcRect.frame);
				}
            }

        }
};

#endif
