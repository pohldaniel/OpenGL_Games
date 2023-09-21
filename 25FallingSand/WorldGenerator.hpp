#pragma once

#include <vector>

class World;
class Populator;
class Chunk;

class WorldGenerator {

public:
    virtual void generateChunk(World* world, Chunk* ch) = 0;
    virtual std::vector<Populator*> getPopulators() = 0;
};

class MaterialTestGenerator : public WorldGenerator {

	void generateChunk(World* world, Chunk* ch) override;
	std::vector<Populator*> getPopulators() override;
};