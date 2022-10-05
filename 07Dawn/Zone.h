#pragma once

#include <string>

#include "engine/Spritesheet.h"

#include "Luainterface.h"

#include "TilesetManager.h"
#include "TextureManager.h"
#include "Npc.h"
#include "InteractionRegion.h"
#include "InteractionPoint.h"

struct TileMap {
	int x_pos, y_pos;
	Tile tile;
	TileMap(int _x, int _y, Tile _tile) {
		x_pos = _x;
		y_pos = _y;
		tile = _tile;
	};

	bool operator<(const TileMap& tile1) const;
};


struct EnvironmentMap {
	int x_pos, y_pos, z_pos;
	Tile tile;
	float transparency, red, green, blue, width, height;
	EnvironmentMap(int _x, int _y, Tile _tile, float _tp, float _red, float _green, float _blue, float _width, float _height, int _z_pos) {
		x_pos = _x;
		y_pos = _y;
		tile = _tile;
		transparency = _tp;
		red = _red;
		green = _green;
		blue = _blue;
		width = _width;
		height = _height;
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

	void update(float deltaTime);

	void loadZone(std::string file);
	bool zoneDataLoaded() const;
	void addEnvironment(int x_pos, int y_pos, Tile tile, bool centeredOnPos);
	void replaceEnvironment(int x_pos, int y_pos, Tile tile, bool centeredOnPos, int replaceId);
	std::string getZoneName() const;

	int deleteEnvironment(int x, int y);
	int locateEnvironment(int x, int y);
	int locateShadow(int x, int y);
	int locateNPC(int x, int y);
	int locateCollisionbox(int x, int y);
	void addCollisionbox(int x_pos, int y_pos);
	int deleteCollisionbox(int x, int y);
	void addShadow(int x_pos, int y_pos, Tile tile);
	int deleteShadow(int x, int y);
	void replaceTile(int iId, Tile tile_);
	void deleteTile(int iId);
	int locateTile(int x, int y);
	std::string getName() const;
	void addNPC(Npc* npcToAdd);
	int deleteNPC(int x, int y);
	void removeNPC(Npc *npcToDelete);
	void cleanupNPCList();
	std::vector<Npc*> getNPCs();
	void addInteractionRegion(InteractionRegion *interactionRegionToAdd);
	std::vector<InteractionRegion*> getInteractionRegions();

	std::vector<InteractionPoint*> getInteractionPoints();
	bool findInteractionPointForCharacter(Character *character) const;
	void addInteractionPoint(InteractionPoint *interactionPointToAdd);

	void findCharacter(Character *character, bool &found, size_t &foundPos) const;

	std::vector<TileMap> tileMap;
	std::vector<EnvironmentMap> environmentMap;
	std::vector<EnvironmentMap> shadowMap;
	std::vector<CollisionRect> collisionMap;
	std::vector <Npc*> npcs;
	std::vector<InteractionRegion*> interactionRegions;
	std::vector<InteractionPoint*> interactionPoints;
private:

	std::string m_name;
	bool m_mapLoaded = true;
	
	const unsigned int* m_textureAtlas;
};