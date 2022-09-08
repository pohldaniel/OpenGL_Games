#pragma once

#include <string>

#include "engine/Spritesheet.h"

#include "Tileset.h"
#include "CTexture.h"

struct sTileMap {
	int x_pos, y_pos;
	Tile *tile;
	sTileMap(int _x, int _y, Tile *_tile) {
		x_pos = _x;
		y_pos = _y;
		tile = _tile;
	};

	bool operator<(const sTileMap& tile1) const;
};


struct sEnvironmentMap {
	int x_pos, y_pos, z_pos;
	Tile *tile;
	float transparency, red, green, blue, x_scale, y_scale;
	sEnvironmentMap(int _x, int _y, Tile *_tile, float _tp, float _red, float _green, float _blue, float _x_scale, float _y_scale, int _z_pos) {
		x_pos = _x;
		y_pos = _y;
		tile = _tile;
		transparency = _tp;
		red = _red;
		green = _green;
		blue = _blue;
		x_scale = _x_scale;
		y_scale = _y_scale;
		z_pos = _z_pos;
	};

	bool operator<(const sEnvironmentMap& environmentObject) const { // instead of using a predicate in our sort call.
		if (z_pos == environmentObject.z_pos)
		{
			if (y_pos == environmentObject.y_pos) {
				return x_pos > environmentObject.x_pos;
			}
			else {
				return y_pos > environmentObject.y_pos;
			}
		}
		else {
			return z_pos < environmentObject.z_pos;
		}
	};

};

struct sCollisionMap {
	int x, y, h, w;
	sCollisionMap(int _x, int _y, int _h, int _w) {
		x = _x;
		y = _y;
		h = _h;
		w = _w;
	};
};

class Zone{

public:
	Zone();
	~Zone();

	void drawZone();
	void drawTiles();
	void drawEnvironment();
	void loadZone(std::string file);
	bool zoneDataLoaded() const;
	void addEnvironment(int x_pos, int y_pos, Tile *tile, bool centeredOnPos);

	std::vector<sEnvironmentMap> EnvironmentMap;
	std::vector<sCollisionMap> CollisionMap;
	std::vector<sTileMap> TileMap;

	unsigned int m_textureAtlas;

private:

	std::string m_zoneName;
	bool m_mapLoaded = true;
	
	
};