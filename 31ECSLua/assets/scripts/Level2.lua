Level = {
    ----------------------------------------------------
    -- Table to define the list of assets
    ----------------------------------------------------
    assets = {
        [0] =
        { type = "tileset",		id = "tilemap-texture",	file = "./assets/tilemaps/desert.png", begin_frame = 0, end_frame = 1199 },
        { type = "spritesheet", id = "tank-texture",	file = "./assets/images/tank-panther-spritesheet.png", begin_frame = 1200, end_frame = 1203 },
		{ type = "spritesheet", id = "su27-texture",	file = "./assets/images/su27-spritesheet.png", begin_frame = 1204, end_frame = 1205},
		{ type = "texture",		id = "carrier-texture",	file = "./assets/images/carrier.png", begin_frame = 1206, end_frame = 1207}
    },

    ----------------------------------------------------
    -- table to define the map config variables
    ----------------------------------------------------
    tilemap = {
        map_file = "./assets/tilemaps/desert.map",
        texture_asset_id = "tilemap-texture",
        num_rows = 30,
        num_cols = 40,
        tile_size = 32,
        scale = 2.0
    },
	
	 ----------------------------------------------------
    -- table to define entities and their components
    ----------------------------------------------------
    entities = {
        [0] =
		{
            -- Player
            tag = "player",
            components = {
                transform = {
                    position = { x = 750, y = 450 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = 0.0, y = 0.0 }
                },
                sprite = {
                    texture_asset_id = "tank-texture",
                    width = 32,
                    height = 32,
                    z_index = 6,
                    fixed = false,
                    src_rect_x = 0,
                    src_rect_y = 0
                },
                boxcollider = {
                    width = 32,
                    height = 25,
                    offset = { x = 0, y = 5 }
                },
                health = {
                    health_percentage = 100
                },
                projectile_emitter = {
                    projectile_velocity = { x = 200, y = 200 },
                    projectile_duration = 10, -- seconds
                    repeat_frequency = 0, -- seconds
                    hit_percentage_damage = 10,
                    friendly = true
                },
                keyboard_controller = {
                    up_velocity = { x = 0, y = -30 },
                    right_velocity = { x = 30, y = 0 },
                    down_velocity = { x = 0, y = 30 },
                    left_velocity = { x = -30, y = 0 }
                },
                camera_follow = {
                    follow = true
                }
            }
        },
		{
            -- Carrier
            components = {
                transform = {
                    position = { x = 270, y = 650 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "carrier-texture",
                    width = 59,
                    height = 191,
                    z_index = 1
                },
            }
        },
        {
            -- Carrier
            components = {
                transform = {
                    position = { x = 270, y = 950 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "carrier-texture",
                    width = 59,
                    height = 191,
                    z_index = 1
                },
            }
        },
	}

}

-- Define some useful global variables
map_width = Level.tilemap.num_cols * Level.tilemap.tile_size * Level.tilemap.scale
map_height = Level.tilemap.num_rows * Level.tilemap.tile_size * Level.tilemap.scale
