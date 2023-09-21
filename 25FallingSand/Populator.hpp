#pragma once

#include <vector>
#include "PlacedStructure.hpp"

class World;
class MaterialInstance;
class Chunk;

class Populator {

public:

    virtual int getPhase() = 0;
    virtual std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk** area, bool* dirty, int tx, int ty, int tw, int th, Chunk* ch, World* world) = 0;
};

class TestPhase1Populator : public Populator {

public:

	int getPhase();
	std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world);
};

class TestPhase2Populator : public Populator {

public:

	int getPhase();
	std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world);
};

class TestPhase3Populator : public Populator {

public:

	int getPhase();
	std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world);
};

class TestPhase4Populator : public Populator {

public:

	int getPhase();
	std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world);
};


class TestPhase5Populator : public Populator {

public:

	int getPhase();
	std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world);
};

class TestPhase6Populator : public Populator {

public:

	int getPhase();
	std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world);
};

class TestPhase0Populator : public Populator {

public:
	int getPhase();
	std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world);
};

class CavePopulator : public Populator {

public:

	int getPhase();
	std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk** area, bool* dirty, int tx, int ty, int tw, int th, Chunk* ch, World* world);
};

class CobblePopulator : public Populator {

public:

	int getPhase();
	std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk** area, bool* dirty, int tx, int ty, int tw, int th, Chunk* ch, World* world);
};

class OrePopulator : public Populator {

public:

	int getPhase();
	std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk** area, bool* dirty, int tx, int ty, int tw, int th, Chunk* ch, World* world);
};

class TreePopulator : public Populator {

public:

	int getPhase();
	std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk** area, bool* dirty, int tx, int ty, int tw, int th, Chunk* ch, World* world);
};
