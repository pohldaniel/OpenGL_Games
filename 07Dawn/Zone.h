#pragma once

#include <string>

#include "engine/Spritesheet.h"

#include "Luainterface.h"

#include "TilesetManager.h"
#include "TextureManager.h"
#include "Npc.h"

struct TileMap {
	int x_pos, y_pos;
	Tile *tile;
	TileMap(int _x, int _y, Tile *_tile) {
		x_pos = _x;
		y_pos = _y;
		tile = _tile;
	};

	bool operator<(const TileMap& tile1) const;
};


struct EnvironmentMap {
	int x_pos, y_pos, z_pos;
	Tile *tile;
	float transparency, red, green, blue, x_scale, y_scale;
	EnvironmentMap(int _x, int _y, Tile *_tile, float _tp, float _red, float _green, float _blue, float _x_scale, float _y_scale, int _z_pos) {
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

	bool operator<(const EnvironmentMap& environmentObject) const { // instead of using a predicate in our sort call.
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



class Zone{
	friend class Editor;

public:
	Zone();
	~Zone();

	void drawZoneBatched();
	void drawTilesBatched();
	void drawEnvironmentBatched();
	void drawShadowsBatched();
	void drawNpcsBatched();

	void drawZoneInstanced();
	void drawTilesInstanced();
	void drawEnvironmentInstanced();

	void update();

	void loadZone(std::string file);
	bool zoneDataLoaded() const;
	void addEnvironment(int x_pos, int y_pos, Tile *tile, bool centeredOnPos);
	int deleteEnvironment(int x, int y);
	void addShadow(int x_pos, int y_pos, Tile *tile);
	int deleteShadow(int x, int y);
	void changeTile(int iId, Tile *tile_);
	void deleteTile(int iId);
	int locateTile(int x, int y);
	std::string getName() const;
	void addNPC(Npc* npcToAdd);
	std::vector<Npc*> getNPCs();


	std::vector<TileMap> tileMap;
	std::vector<EnvironmentMap> environmentMap;
	std::vector<EnvironmentMap> shadowMap;
	std::vector<CollisionRect> collisionMap;
	std::vector <Npc*> npcs;
private:

	std::string m_name;
	bool m_mapLoaded = true;
	
	const unsigned int* m_textureAtlas;
};