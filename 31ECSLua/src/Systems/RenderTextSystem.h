#ifndef RENDERTEXTSYSTEM_H
#define RENDERTEXTSYSTEM_H

#include <engine/Rect.h>
#include <engine/Fontrenderer.h>

#include <ECS/ECS.h>
#include <Components/TextLabelComponent.h>
#include <Components/MouseClickedComponent.h>
#include <Components/BoxColliderComponent.h>

#include "Application.h"
#include "ViewPort.h"

class RenderTextSystem: public System {
    public:
        RenderTextSystem() {
            RequireComponent<TextLabelComponent>();
			RequireComponent<MouseClickedComponent>();
			RequireComponent<BoxColliderComponent>();
        }

        void Update() {
            for (auto entity: GetSystemEntities()) {
                const auto textlabel = entity.GetComponent<TextLabelComponent>();
				const auto mouseClick = entity.GetComponent<MouseClickedComponent>();

				if (mouseClick.clicked) {
					ViewPort& viewPort = ViewPort::Get();
					Fontrenderer::Get().addText(Globals::fontManager.get("pico8_10"), viewPort.getPositionX() + static_cast<float>(Application::Width) - Globals::fontManager.get("pico8_10").getWidth(textlabel.text) - 20.0f, viewPort.getPositionY() + static_cast<float>(Application::Height) - Globals::fontManager.get("pico8_10").lineHeight - 20.0f, textlabel.text, textlabel.color);
				}
            }
        }
};

#endif
