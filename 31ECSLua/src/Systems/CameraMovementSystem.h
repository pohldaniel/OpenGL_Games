#ifndef CAMERAMOVEMENTSYSTEM_H
#define CAMERAMOVEMENTSYSTEM_H

#include <engine/Rect.h>
#include "../ECS/ECS.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"

#include "Application.h"

class CameraMovementSystem: public System {
    public:
        CameraMovementSystem() {
            RequireComponent<CameraFollowComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update(Rect& camera) {
            for (auto entity: GetSystemEntities()) {
                auto transform = entity.GetComponent<TransformComponent>();

                if (transform.position.x + (camera.width / 2) < Application::MapWidth) {
                    camera.posX = transform.position.x - (Application::Width / 2);
                }

                if (transform.position.y + (camera.height / 2) < Application::MapHeight) {
                    camera.posY = transform.position.y - (Application::Height / 2);
                }

                // Keep camera rectangle view inside the screen limits
                camera.posX = camera.posX < 0 ? 0 : camera.posX;
                camera.posY = camera.posY < 0 ? 0 : camera.posY;
                camera.posX = camera.posX > camera.width ? camera.width : camera.posX;
                camera.posY = camera.posY > camera.height ? camera.height : camera.posY;
            }
        }
};

#endif
