#pragma once

class MaterialInstance;

class ChunkReadyToMerge {
public:
    int cx;
    int cy;
    MaterialInstance* tiles;
    ChunkReadyToMerge(int cx, int cy, MaterialInstance* tiles) {
        this->cx = cx;
        this->cy = cy;
        this->tiles = tiles;
    }
    ChunkReadyToMerge() : ChunkReadyToMerge(0, 0, nullptr) {};
};
