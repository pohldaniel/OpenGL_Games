#ifndef RENDERCOLLIDERSYSTEM_H
#define RENDERCOLLIDERSYSTEM_H

#include <engine/Batchrenderer.h>
#include <engine/Rect.h>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"

class RenderColliderSystem: public System {
    public:
        RenderColliderSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        }

        void Update(const Rect& camera) {
            for (auto entity: GetSystemEntities()) {
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto collider = entity.GetComponent<BoxColliderComponent>();

                // Bypass rendering if entities are outside the camera view
                bool isOutsideCameraView = (
                    transform.position.x + (transform.scale.x * collider.width) < camera.posX ||
                    transform.position.x > camera.posX + camera.width ||
                    transform.position.y + (transform.scale.y * collider.height) < camera.posY ||
                    transform.position.y > camera.posY + camera.height
                );

                if (isOutsideCameraView) {
                    continue;
                }

				Rect colliderRect = {
                    static_cast<int>(transform.position.x + collider.offset.x - camera.posX),
                    static_cast<int>(transform.position.y + collider.offset.y - camera.posY),
                    static_cast<int>(collider.width * transform.scale.x),
                    static_cast<int>(collider.height * transform.scale.y)
                };
                //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                //SDL_RenderDrawRect(renderer, &colliderRect);
            }
        }
};

#endif
