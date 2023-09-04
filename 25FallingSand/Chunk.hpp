#pragma once



#ifndef INC_Tiles
#include "Tiles.hpp"
#endif // !INC_Tiles

#ifndef INC_MaterialInstance
#include "MaterialInstance.hpp"
#endif // !INC_MaterialInstance

#include <iostream>
#include <fstream>

#define INC_Chunk

#define CHUNK_W 128
#define CHUNK_H 128

#ifndef INC_Biome
#include "Biome.hpp"
#endif // !INC_Biome

#include "RigidBody.hpp"

typedef struct {
	uint16_t index;
	uint32_t color;
    int32_t temperature;
} MaterialInstanceData;

class Chunk {
    std::string fname;
public:
    int x;
    int y;
    bool hasMeta = false;
    // in order for a chunk to execute phase generationPhase+1, all surrounding chunks must be at least generationPhase
    int8_t generationPhase = 0;
    bool pleaseDelete = false;

    Chunk(int x, int y, char* worldName);
    Chunk() : Chunk(0, 0, (char*)"chunks") {};
    ~Chunk();

    void loadMeta();

    //static MaterialInstanceData* readBuf;
    void read();
    void write(MaterialInstance* tiles, MaterialInstance* layer2, uint32_t* background);
    bool hasFile();

    bool hasTileCache = false;
    MaterialInstance* tiles = nullptr;
    MaterialInstance* layer2 = nullptr;
	uint32_t* background = nullptr;
    Biome** biomes = nullptr;

    std::vector<b2PolygonShape> polys = {};
    RigidBody* rb = nullptr;
};
