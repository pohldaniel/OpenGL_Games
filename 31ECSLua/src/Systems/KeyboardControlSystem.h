#ifndef KEYBOARDCONTROLSYSTEM_H
#define KEYBOARDCONTROLSYSTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"

class KeyboardControlSystem: public System {
    public:
        KeyboardControlSystem() {
            RequireComponent<KeyboardControlledComponent>();
            RequireComponent<SpriteComponent>();
            RequireComponent<RigidBodyComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
        }

        void OnKeyPressed(KeyPressedEvent& event) {
            for (auto entity: GetSystemEntities()) {
				auto& keyboardcontrol = entity.GetComponent<KeyboardControlledComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();
                auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

                switch (event.symbol) {
				case Keyboard::KEY_UP:
						keyboardcontrol.direction = Direction::NORTH;
                        rigidbody.velocity = keyboardcontrol.upVelocity;					
                        sprite.textureRect = sprite.animationRects[0];
                        break;
                    case Keyboard::KEY_RIGHT:
						keyboardcontrol.direction = Direction::EAST;
                        rigidbody.velocity = keyboardcontrol.rightVelocity;						
						sprite.textureRect = sprite.animationRects[1];
                        break;
                    case Keyboard::KEY_DOWN:
						keyboardcontrol.direction = Direction::SOUTH;
                        rigidbody.velocity = keyboardcontrol.downVelocity;
						sprite.textureRect = sprite.animationRects[2];
                        break;
                    case Keyboard::KEY_LEFT:
						keyboardcontrol.direction = Direction::WEST;
                        rigidbody.velocity = keyboardcontrol.leftVelocity;
						sprite.textureRect = sprite.animationRects[3];
                        break;
					case Keyboard::KEY_LCTRL:
						rigidbody.velocity.x = 0.0f;
						rigidbody.velocity.y = 0.0f;
						break;					
                }
            }
        }

        void Update() {
                
        }
};

#endif
