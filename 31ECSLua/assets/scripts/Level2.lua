Level = {
    ----------------------------------------------------
    -- Table to define the list of assets
    ----------------------------------------------------
    assets = {
        [0] =
        { type = "tileset",		id = "tilemap-texture",	file = "./assets/tilemaps/desert.png", begin_frame = 0, end_frame = 1199 },
        { type = "spritesheet", id = "tank-texture",	file = "./assets/images/tank-panther-spritesheet.png", begin_frame = 1200, end_frame = 1203 },
		{ type = "spritesheet", id = "su27-texture",	file = "./assets/images/su27-spritesheet.png", begin_frame = 1204, end_frame = 1205},
		{ type = "texture",		id = "carrier-texture",	file = "./assets/images/carrier.png", begin_frame = 1206, end_frame = 1206},
		{ type = "texture", 	id = "bullet-texture",	file = "./assets/images/bullet.png", begin_frame = 1207, end_frame = 1207},
		{ type = "spritesheet", id = "bomber-texture",	file = "./assets/images/bomber-spritesheet.png", begin_frame = 1208, end_frame = 1209},
		{ type = "texture",		id = "runway-texture",	file = "./assets/images/runway.png", begin_frame = 1210, end_frame = 1210},
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
                    up_velocity = { x = 0, y = 30 },
                    right_velocity = { x = 30, y = 0 },
                    down_velocity = { x = 0, y = -30 },
                    left_velocity = { x = -30, y = 0 }
                },
                camera_follow = {
                    follow = true
                }
            }
        },
		{
            -- SU-27 fighter jet
            group = "enemies",
            components = {
                transform = {
                    position = { x = 725, y = 1480 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 90.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = 20 , y = 0.0 }
                },
                sprite = {
                    texture_asset_id = "su27-texture",
                    width = 32,
                    height = 32,
                    z_index = 8
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 10 -- fps
                },
                boxcollider = {
                    width = 25,
                    height = 30,
                    offset = { x = 5, y = 0 }
                },
                health = {
                    health_percentage = 100
                }
            }
        },
		{
            -- SU-27 fighter jet
            group = "enemies",
            components = {
                transform = {
                    position = { x = 288, y = 810 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = -10.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = -5.5 , y = 35.0 }
                },
                sprite = {
                    texture_asset_id = "su27-texture",
                    width = 32,
                    height = 32,
                    z_index = 5
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 10 -- fps
                },
                boxcollider = {
                    width = 25,
                    height = 30,
                    offset = { x = 5, y = 0 }
                },
                health = {
                    health_percentage = 100
                }
            }
        },
        {
            -- SU-27 fighter jet
            group = "enemies",
            components = {
                transform = {
                    position = { x = 288, y = 1410 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = -10.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = -5.5 , y = 35.0 }
                },
                sprite = {
                    texture_asset_id = "su27-texture",
                    width = 32,
                    height = 32,
                    z_index = 5
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 10 -- fps
                },
                boxcollider = {
                    width = 25,
                    height = 30,
                    offset = { x = 5, y = 0 }
                },
                health = {
                    health_percentage = 100
                }
            }
        },
        {
            -- SU-27 fighter jet
            group = "enemies",
            components = {
                transform = {
                    position = { x = 1100, y = 1000 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = 0.0, y = 100.0 }
                },
                sprite = {
                    texture_asset_id = "su27-texture",
                    width = 32,
                    height = 32,
                    z_index = 5
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 10 -- fps
                },
                boxcollider = {
                    width = 32,
                    height = 32
                },
                health = {
                    health_percentage = 100
                },
                on_update_script = {
                    [0] =
                    function(entity, delta_time, ellapsed_time)
                        -- this function makes the fighter jet move up and down the map shooting projectiles
                        local current_position_x, current_position_y = get_position(entity)
                        local current_velocity_x, current_velocity_y = get_velocity(entity)
						
                         -- if it reaches the top or the bottom of the map
                        if current_position_y > 10 or current_position_y < 32 - map_height  then
                            set_velocity(entity, 0, current_velocity_y * -1); -- flip the entity y-velocity
                        else
                            set_velocity(entity, 0, current_velocity_y); -- do not flip y-velocity
                        end

                        -- set the transform rotation to match going up or down
                        if (current_velocity_y < 0) then
                            set_rotation(entity, 180) -- point up
                        else
                            set_rotation(entity, 0) -- point down
                        end
                    end
                }
            }
        },
		{
            -- Bomber plane
            group = "enemies",
            components = {
                transform = {
                    position = { x = 464, y = 520 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "bomber-texture",
                    width = 32,
                    height = 24,
                    z_index = 5
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 10 -- fps
                },
                boxcollider = {
                    width = 32,
                    height = 32,
                    offset = { x = 0, y = 0 }
                },
                health = {
                    health_percentage = 100
                }
            }
        },
		{
            -- Bomber airplane
            group = "enemies",
            components = {
                transform = {
                    position = { x = 317, y = 985 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                rigidbody = {
                    velocity = { x = 0.0, y = -50.0 }
                },
                sprite = {
                    texture_asset_id = "bomber-texture",
                    width = 32,
                    height = 24,
                    z_index = 8
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 10 -- fps
                },
                boxcollider = {
                    width = 32,
                    height = 24
                },
                health = {
                    health_percentage = 100
                },
                on_update_script = {
                    [0] =
                    function(entity, delta_time, ellapsed_time)
                        -- change the position of the the airplane to follow a circular path
                        local radius = 170
                        local distance_from_origin = 500

                        local angle = ellapsed_time * 0.0009

                        -- calculate the new x-y cartesian position using polar coordinates
                        local new_x = distance_from_origin + (math.cos(angle) * radius)
                        local new_y = distance_from_origin - (math.sin(angle) * radius)  
                        set_position(entity, new_x, new_y)

                        -- change the rotation of the sprite to match the circular motion
                        local angle_in_degrees = 180 + angle * 180 / math.pi
                        set_rotation(entity, angle_in_degrees)
                    end
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
		{
            -- Runway
            components = {
                transform = {
                    position = { x = 470, y = 385 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 0.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "runway-texture",
                    width = 21,
                    height = 191,
                    z_index = 1
                }
            }
        },
        {
            -- Runway
            components = {
                transform = {
                    position = { x = 800, y = 1400 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 90.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "runway-texture",
                    width = 21,
                    height = 191,
                    z_index = 1
                }
            }
        },
        {
            -- Runway
            components = {
                transform = {
                    position = { x = 800, y = 1500 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 90.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "runway-texture",
                    width = 21,
                    height = 191,
                    z_index = 1
                }
            }
        },
        {
            -- Runway
            components = {
                transform = {
                    position = { x = 800, y = 1600 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 90.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "runway-texture",
                    width = 21,
                    height = 191,
                    z_index = 1
                }
            }
        },
        {
            -- Runway
            components = {
                transform = {
                    position = { x = 1300, y = 1400 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 90.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "runway-texture",
                    width = 21,
                    height = 191,
                    z_index = 1
                }
            }
        },
        {
            -- Runway
            components = {
                transform = {
                    position = { x = 1300, y = 1500 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 90.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "runway-texture",
                    width = 21,
                    height = 191,
                    z_index = 1
                }
            }
        },
        {
            -- Runway
            components = {
                transform = {
                    position = { x = 1300, y = 1600 },
                    scale = { x = 1.0, y = 1.0 },
                    rotation = 90.0, -- degrees
                },
                sprite = {
                    texture_asset_id = "runway-texture",
                    width = 21,
                    height = 191,
                    z_index = 1
                }
            }
        }
	}

}

-- Define some useful global variables
map_width = Level.tilemap.num_cols * Level.tilemap.tile_size * Level.tilemap.scale
map_height = Level.tilemap.num_rows * Level.tilemap.tile_size * Level.tilemap.scale
