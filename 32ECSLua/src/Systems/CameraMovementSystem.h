#ifndef CAMERAMOVEMENTSYSTEM_H
#define CAMERAMOVEMENTSYSTEM_H

#include <engine/Camera.h>
#include "../ECS/ECS.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"

#include "Application.h"
#include "ViewPort.h"

class CameraMovementSystem: public System {
    public:
        CameraMovementSystem() {
            RequireComponent<CameraFollowComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update() {
            for (auto entity: GetSystemEntities()) {
				auto transform = entity.GetComponent<TransformComponent>();
				
				float posX = transform.position.x - (Application::Width / 2);
				float posY = transform.position.y - (Application::Height / 2);
				
                // Keep camera rectangle view inside the screen limits
				posX = posX < 0.0f ? 0.0f : posX;
				posY = posY > 0.0f ? 0.0f : posY;			
				posX = posX > Application::MapWidth - Application::Width ? Application::MapWidth - Application::Width : posX;
				posY = posY < -(Application::MapHeight - Application::Height) ? -(Application::MapHeight - Application::Height) : posY;

				ViewPort::Get().setPosition(posX, posY, 0.0f);
            }
        }
};

#endif
