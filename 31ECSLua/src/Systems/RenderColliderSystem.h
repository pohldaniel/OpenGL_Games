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

        void Update(const Camera& camera) {
            for (auto entity: GetSystemEntities()) {
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto collider = entity.GetComponent<BoxColliderComponent>();

                // Bypass rendering if entities are outside the camera view
                /*bool isOutsideCameraView = (
                    transform.position.x + (transform.scale.x * collider.width) < camera.posX ||
                    transform.position.x > camera.posX + camera.width ||
                    transform.position.y + (transform.scale.y * collider.height) < camera.posY ||
                    transform.position.y > camera.posY + camera.height
                );

                if (isOutsideCameraView) {
                    //continue;
                }*/

				
                //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                //SDL_RenderDrawRect(renderer, &colliderRect);

				const TextureRect& rect = TileSetManager::Get().getTileSet("desert").getTextureRects()[LevelLoader::SpriteMap.at("empty-texture").first];

				Batchrenderer::Get().addQuadAA(Vector4f(transform.position.x + collider.offset.x, transform.position.y + collider.offset.y, collider.width * transform.scale.x, collider.height * transform.scale.y), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 0.0f, 0.0f, 1.0f), rect.frame);
            }
        }
};

#endif
