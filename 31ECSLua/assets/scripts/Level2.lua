Level = {
    ----------------------------------------------------
    -- Table to define the list of assets
    ----------------------------------------------------
    assets = {
        [0] =
        { type = "texture", id = "tilemap-texture",             file = "./assets/tilemaps/desert.png" }
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

}

-- Define some useful global variables
map_width = Level.tilemap.num_cols * Level.tilemap.tile_size * Level.tilemap.scale
map_height = Level.tilemap.num_rows * Level.tilemap.tile_size * Level.tilemap.scale
