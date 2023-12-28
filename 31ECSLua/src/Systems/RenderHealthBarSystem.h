#ifndef RENDERHEALTHBARSYSTEM_H
#define RENDERHEALTHBARSYSTEM_H

#include <engine/Batchrenderer.h>
#include <engine/Fontrenderer.h>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/HealthComponent.h"

#include "TileSet.h"
#include "LevelLoader.h"
#include "Globals.h"

class RenderHealthBarSystem: public System {
    public:
        RenderHealthBarSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();
            RequireComponent<HealthComponent>();
        }

        void Update(const Rect& camera) {
            for (auto entity: GetSystemEntities()) {
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();
                const auto health = entity.GetComponent<HealthComponent>();

                // Bypass rendering if entities are outside the camera view
                bool isOutsideCameraView = (
                    transform.position.x + (transform.scale.x * sprite.textureRect.width) < camera.posX ||
                    transform.position.x > camera.posX + camera.width ||
                    transform.position.y + (transform.scale.y * sprite.textureRect.height) < camera.posY ||
                    transform.position.y > camera.posY + camera.height
                );
                
                if (isOutsideCameraView) {
                    //continue;
                }

                // Draw a the health bar with the correct color for the percentage
                Vector4f healthBarColor = {1.0f, 1.0f, 1.0f , 1.0f};

                if (health.healthPercentage >= 0 && health.healthPercentage < 40) {
                    // 0-40 = red
                    healthBarColor = {1.0f, 0.0f, 0.0f, 1.0f};
                }
                if (health.healthPercentage >= 40 && health.healthPercentage < 80) {
                    // 40-80 = yellow
                    healthBarColor = {1.0f, 1.0f, 0.0f, 1.0f};
                }
                if (health.healthPercentage >= 80 && health.healthPercentage <= 100) {
                    // 80-100 = green
                    healthBarColor = {0.0f, 1.0f, 0.0f, 1.0f};
                }

                // Position the health bar indicator in the top-right part of the entity sprite
				float healthBarWidth = 15.0f;
				float healthBarHeight = 3.0f;
                float healthBarPosX = (transform.position.x + (sprite.textureRect.width * transform.scale.x)) - camera.posX;
				float healthBarPosY = (transform.position.y) - camera.posY;
				const TextureRect& rect = TileSetManager::Get().getTileSet("desert").getTextureRects()[LevelLoader::SpriteMap.at("empty-texture").first];
				Batchrenderer::Get().addQuadAA(Vector4f(healthBarPosX, healthBarPosY, healthBarWidth * (health.healthPercentage / 100.0f), healthBarHeight),
					Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight),
					healthBarColor, rect.frame);

                std::string healthText = std::to_string(health.healthPercentage);
				Fontrenderer::Get().addText(Globals::fontManager.get("pico8_5"), healthBarPosX, healthBarPosY - 5.0f - static_cast<float>(Globals::fontManager.get("pico8_5").lineHeight), healthText, healthBarColor);
            }
        }
};

#endif
