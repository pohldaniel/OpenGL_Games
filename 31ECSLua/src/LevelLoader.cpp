#include <iostream>
#include <fstream>

#include <engine/Spritesheet.h>
#include <Components/TransformComponent.h>
#include <Components/RigidBodyComponent.h>
#include <Components/SpriteComponent.h>
#include <Components/AnimationComponent.h>
#include <Components/BoxColliderComponent.h>
#include <Components/KeyboardControlledComponent.h>
#include <Components/CameraFollowComponent.h>
#include <Components/ProjectileEmitterComponent.h>
#include <Components/HealthComponent.h>
#include <Components/TextLabelComponent.h>
#include <Components/ScriptComponent.h>

#include <string>
#include <sol/sol.hpp>

#include "LevelLoader.h"
#include "Application.h"
#include "Globals.h"
#include "TileSet.h"

unsigned int LevelLoader::Atlas = 0;

LevelLoader::LevelLoader() {
    //Logger::Log("LevelLoader constructor called!");    
	std::cout << "LevelLoader constructor called!" << std::endl;
}

LevelLoader::~LevelLoader() {
    //Logger::Log("LevelLoader destructor called!");  
	std::cout << "LevelLoader destructor called!" << std::endl;
}

void LevelLoader::LoadLevel(sol::state& lua, const std::unique_ptr<Registry>& registry, int levelNumber) {
	std::unordered_map<std::string, std::pair<int, int>> spriteMap;
	
	// This checks the syntax of our script, but it does not execute the script
    sol::load_result script = lua.load_file("./assets/scripts/Level" + std::to_string(levelNumber) + ".lua");
    if (!script.valid()) {
        sol::error err = script;
        std::string errorMessage = err.what();
        //Logger::Err("Error loading the lua script: " + errorMessage);
		std::cout << "Error loading the lua script: " << std::endl;
        return;
    }

    // Executes the script using the Sol state
    lua.script_file("./assets/scripts/Level" + std::to_string(levelNumber) + ".lua");

    // Read the big table for the current level
    sol::table level = lua["Level"];

    ////////////////////////////////////////////////////////////////////////////
    // Read the level assets
    ////////////////////////////////////////////////////////////////////////////
    sol::table assets = level["assets"];

	TextureAtlasCreator::Get().init(1024u, 1024u);
    int i = 0;
    while (true) {
        sol::optional<sol::table> hasAsset = assets[i];
        if (hasAsset == sol::nullopt) {
            break;
        }
        sol::table asset = assets[i];
        std::string assetType = asset["type"];
        std::string assetId = asset["id"];
		if (assetType == "tileset") {		
			TileSetManager::Get().getTileSet("desert").loadTileSetCpu("assets/tilemaps/desert.map", asset["file"], 40, 30, 32.0f);
			if (spriteMap.count(assetId) == 0) {
				spriteMap.insert({ assetId, {asset["begin_frame"], asset["end_frame"]} });
			}
		}else if (assetType == "texture") {
			TileSetManager::Get().getTileSet("desert").loadTileSetCpu2(asset["file"]);
			if (spriteMap.count(assetId) == 0) {
				spriteMap.insert({ assetId, {asset["begin_frame"], asset["end_frame"]} });
			}
		}else if (assetType == "spritesheet") {
			TileSetManager::Get().getTileSet("desert").loadTileSetCpu(asset["file"], 32.0f);
			if (spriteMap.count(assetId) == 0) {
				spriteMap.insert({ assetId, {asset["begin_frame"], asset["end_frame"]} });
			}
		}/*else (assetType == "font") {
            //assetStore->AddFont(assetId, asset["file"], asset["font_size"]);
            //Logger::Log("A new font asset was added to the asset store, id: " + assetId);
        }*/
        i++;
    }

	TileSetManager::Get().getTileSet("desert").loadTileSetGpu();
	Atlas = TileSetManager::Get().getTileSet("desert").getAtlas();
	Spritesheet::Safe("tmp", Atlas);

    ////////////////////////////////////////////////////////////////////////////
    // Read the level tilemap information
    ////////////////////////////////////////////////////////////////////////////
    sol::table map = level["tilemap"];
    std::string mapFilePath = map["map_file"];
    std::string mapTextureAssetId = map["texture_asset_id"];
    int mapNumRows = map["num_rows"];
    int mapNumCols = map["num_cols"];
    float tileSize = map["tile_size"];
    float mapScale = map["scale"];

    for (int y = 0; y < mapNumRows; y++) {
		for (int x = 0; x < mapNumCols; x++) {			
			Entity tile = registry->CreateEntity();
			tile.AddComponent<TransformComponent>(glm::vec2(x * (mapScale * tileSize), Application::Height - y * (mapScale * tileSize)), glm::vec2(mapScale, mapScale), 0.0);

			const TextureRect& rect = TileSetManager::Get().getTileSet("desert").getTextureRects()[y * mapNumCols + x];
			tile.AddComponent<SpriteComponent>(mapTextureAssetId, rect, 0, false);
			//tile.GetComponent<SpriteComponent>().init(std::vector<TextureRect>(TileSetManager::Get().getTileSet("desert").getTextureRects().begin() + y * mapNumCols + x, TileSetManager::Get().getTileSet("desert").getTextureRects().begin() + y * mapNumCols + x + 1));
        }
    }

	Application::MapWidth = mapNumCols * tileSize * mapScale;
	Application::MapHeight = mapNumRows * tileSize * mapScale;

    ////////////////////////////////////////////////////////////////////////////
    // Read the level entities and their components
    ////////////////////////////////////////////////////////////////////////////
    sol::table entities = level["entities"];
    i = 0;
    while (true) {
        sol::optional<sol::table> hasEntity = entities[i];
        if (hasEntity == sol::nullopt) {
            break;
        }
        sol::table entity = entities[i];

        Entity newEntity = registry->CreateEntity();

        // Tag
        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt) {
            newEntity.Tag(entity["tag"]);
        }

        // Group
        sol::optional<std::string> group = entity["group"];
        if (group != sol::nullopt) {
            newEntity.Group(entity["group"]);
        }

        // Components
        sol::optional<sol::table> hasComponents = entity["components"];
        if (hasComponents != sol::nullopt) {
            // Transform
            sol::optional<sol::table> transform = entity["components"]["transform"];
            if (transform != sol::nullopt) {
				float posY = entity["components"]["transform"]["position"]["y"];
                newEntity.AddComponent<TransformComponent>(
                    glm::vec2(
                        entity["components"]["transform"]["position"]["x"],
						static_cast<float>(Application::Height) - posY
						//entity["components"]["transform"]["position"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["transform"]["scale"]["x"].get_or(1.0),
                        entity["components"]["transform"]["scale"]["y"].get_or(1.0)
                    ),
                    entity["components"]["transform"]["rotation"].get_or(0.0)
                );
            }

            // RigidBody
            sol::optional<sol::table> rigidbody = entity["components"]["rigidbody"];
            if (rigidbody != sol::nullopt) {
                newEntity.AddComponent<RigidBodyComponent>(
                    glm::vec2(
                        entity["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
                        entity["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)
                    )
                );
            }

            // Sprite
            sol::optional<sol::table> sprite = entity["components"]["sprite"];
            if (sprite != sol::nullopt) {

				std::string assetId = entity["components"]["sprite"]["texture_asset_id"];
				int beginFrame = spriteMap.at(assetId).first;
				int endFrame = spriteMap.at(assetId).second;
				const TextureRect& rect = TileSetManager::Get().getTileSet("desert").getTextureRects()[beginFrame];
                newEntity.AddComponent<SpriteComponent>(
					assetId,
					rect,
                    entity["components"]["sprite"]["z_index"].get_or(1),
                    entity["components"]["sprite"]["fixed"].get_or(false)
                );

				if(beginFrame != endFrame)
					newEntity.GetComponent<SpriteComponent>().init(std::vector<TextureRect>(TileSetManager::Get().getTileSet("desert").getTextureRects().begin() + beginFrame, TileSetManager::Get().getTileSet("desert").getTextureRects().begin() + endFrame + 1));
            }

            // Animation
            sol::optional<sol::table> animation = entity["components"]["animation"];
            if (animation != sol::nullopt) {
                newEntity.AddComponent<AnimationComponent>(
                    entity["components"]["animation"]["num_frames"].get_or(1),
                    entity["components"]["animation"]["speed_rate"].get_or(1)
                );
            }

            // BoxCollider
            sol::optional<sol::table> collider = entity["components"]["boxcollider"];
            if (collider != sol::nullopt) {
                newEntity.AddComponent<BoxColliderComponent>(
                    entity["components"]["boxcollider"]["width"],
                    entity["components"]["boxcollider"]["height"],
                    glm::vec2(
                        entity["components"]["boxcollider"]["offset"]["x"].get_or(0),
                        entity["components"]["boxcollider"]["offset"]["y"].get_or(0)
                    )
                );
            }
            
            // Health
            sol::optional<sol::table> health = entity["components"]["health"];
            if (health != sol::nullopt) {
                newEntity.AddComponent<HealthComponent>(
                    static_cast<int>(entity["components"]["health"]["health_percentage"].get_or(100))
                );
            }
            
            // ProjectileEmitter
            sol::optional<sol::table> projectileEmitter = entity["components"]["projectile_emitter"];
            if (projectileEmitter != sol::nullopt) {
                newEntity.AddComponent<ProjectileEmitterComponent>(
                    glm::vec2(
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["x"],
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["y"]
                    ),
                    static_cast<int>(entity["components"]["projectile_emitter"]["repeat_frequency"].get_or(1)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["projectile_duration"].get_or(10)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["hit_percentage_damage"].get_or(10)),
                    entity["components"]["projectile_emitter"]["friendly"].get_or(false)
                );
            }

            // CameraFollow
            sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
            if (cameraFollow != sol::nullopt) {
                newEntity.AddComponent<CameraFollowComponent>();
            }

            // KeyboardControlled
            sol::optional<sol::table> keyboardControlled = entity["components"]["keyboard_controller"];
            if (keyboardControlled != sol::nullopt) {
                newEntity.AddComponent<KeyboardControlledComponent>(
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["up_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["up_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["right_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["right_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["down_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["down_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["left_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["left_velocity"]["y"]
                    )
                );
            }

            // Script
            sol::optional<sol::table> script = entity["components"]["on_update_script"];
            if (script != sol::nullopt) {
                sol::function func = entity["components"]["on_update_script"][0];
                newEntity.AddComponent<ScriptComponent>(func);
            }
        }
        i++;
    }
}
