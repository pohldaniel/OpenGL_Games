#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "Globals.h"

class AnimationSystem: public System {
    public:
        AnimationSystem() {
            RequireComponent<SpriteComponent>();
            RequireComponent<AnimationComponent>();
        }

        void Update() {
            for (auto entity: GetSystemEntities()) {
                auto& animation = entity.GetComponent<AnimationComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();
				
                animation.currentFrame = ((Globals::clock.getElapsedTimeMilli() - animation.startTime) * animation.frameSpeedRate / 1000) % animation.numFrames;
				sprite.textureRect= sprite.animationRects[animation.currentFrame];
            }
        }
};

#endif
