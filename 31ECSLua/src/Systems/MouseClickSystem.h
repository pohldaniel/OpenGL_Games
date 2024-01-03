#pragma once

#include <ECS/ECS.h>
#include <EventBus/EventBus.h>
#include <Events/MouseClickedEvent.h>

#include <Components/MouseClickedComponent.h>
#include <Components/BoxColliderComponent.h>

class MouseClickSystem : public System {
public:
	MouseClickSystem() {
		RequireComponent<MouseClickedComponent>();
		RequireComponent<BoxColliderComponent>();
		RequireComponent<TransformComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<MouseClickedEvent>(this, &MouseClickSystem::OnMouseClicked);
	}

	void OnMouseClicked(MouseClickedEvent& event) {
		for (auto entity : GetSystemEntities()) {
		
			const TransformComponent& transform = entity.GetComponent<TransformComponent>();
			const BoxColliderComponent& collider = entity.GetComponent<BoxColliderComponent>();
			auto& mouseClick = entity.GetComponent<MouseClickedComponent>();

			if (transform.position.x + collider.offset.x < event.x && event.x < transform.position.x + collider.width * transform.scale.x && transform.position.y + collider.offset.y < event.y && event.y < transform.position.y + collider.offset.y + collider.height * transform.scale.y) {
				mouseClick.clicked = true;
			}else {
				mouseClick.clicked = false;
			}
		}
	}

	void Update() {

	}
};