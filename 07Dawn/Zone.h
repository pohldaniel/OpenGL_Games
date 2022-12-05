#pragma once

#include <string>

#include "Luainterface.h"
#include "TilesetManager.h"
#include "TextureManager.h"
#include "Npc.h"
#include "InteractionRegion.h"
#include "InteractionPoint.h"
#include "Groundloot.h"

struct TileMap {
	int x_pos, y_pos;
	Tile tile;

	bool operator<(const TileMap& tile1) const {
		return tile.tileId < tile1.tile.tileId;
	};
};

struct EnvironmentMap {
	int x_pos, y_pos, z_pos;
	Tile tile;
	float transparency, red, green, blue, width, height;
	
	bool operator<(const EnvironmentMap& environmentObject) const { // instead of using a predicate in our sort call.
		if (z_pos == environmentObject.z_pos){
			if (y_pos == environmentObject.y_pos) {
				return x_pos > environmentObject.x_pos;
			}else {
				return y_pos > environmentObject.y_pos;
			}
		}else {
			return z_pos < environmentObject.z_pos;
		}
	};

};

class CMagic;
class CallIndirection;

class Zone{
	friend class Editor;
	friend class ZoneManager;
	

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
	void processInput(int mouseX, int mouseY, int characterXpos, int characterYpos);

	void loadZone();
	bool zoneDataLoaded() const;
	void addEnvironment(int x_pos, int y_pos, Tile tile, bool centeredOnPos);
	void replaceEnvironment(int x_pos, int y_pos, Tile& tile, bool centeredOnPos, int replaceId);
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


	bool findInteractionPointForCharacter(Character *character) const;
	void findCharacter(Character *character, bool &found, size_t &foundPos) const;

	std::vector<TileMap>& getTileMap();
	std::vector<EnvironmentMap>& getEnvironmentMap();
	std::vector<EnvironmentMap>& getShadowMap();
	std::vector<CollisionRect>& getCollisionMap();
	std::vector<InteractionRegion*>& getInteractionRegions();
	std::vector<InteractionPoint*>& getInteractionPoints();


	std::vector<CMagic*> MagicMap;
	void addActiveAoESpell(SpellActionBase *spell);
	std::vector<std::pair<SpellActionBase*, uint32_t> > getActiveAoESpells();
	void cleanupActiveAoESpells();
	void clearActiveAoESpells();
	void removeActiveAoESpell(SpellActionBase* activeSpell);
	GroundLoot* getGroundLoot();
	void addEventHandler(CallIndirection *newEventHandler);

	int groundOffset = 0;
	int environmentOffset = 0;
	int shadowOffset = 0;

	void updateInteractionRegion();
	void addInteractionRegion(InteractionRegion* interactionRegion);	
	void addInteractionPoint(InteractionPoint* interactionPoint);
	void addCharacterInteractionPoint(CharacterInteractionPoint *characterInteractionPoint);

private:

	std::vector< std::pair<SpellActionBase*, uint32_t> > activeAoESpells;
	std::vector<TileMap> m_tileMap;
	std::vector<EnvironmentMap> m_environmentMap;
	std::vector<EnvironmentMap> m_shadowMap;
	std::vector<CollisionRect> m_collisionMap;
	std::vector <Npc*> m_npcs;
	std::vector<InteractionRegion*> m_interactionRegions;
	std::vector<InteractionPoint*> m_interactionPoints;
	std::vector<CallIndirection*> eventHandlers;
	std::string m_file;
	bool m_mapLoaded = false;
	unsigned int m_textureAtlas;
	GroundLoot groundLoot;

};

class ZoneManager {

public:

	Zone& getZone(std::string zoneName);
	static ZoneManager& Get();

	void setCurrentZone(Zone* zone);
	Zone* getCurrentZone();
	std::unordered_map<std::string, Zone> getAllZones();

private:
	ZoneManager() = default;
	std::unordered_map<std::string, Zone> m_zones;	
	Zone* m_currentZone;
	static ZoneManager s_instance;
};
