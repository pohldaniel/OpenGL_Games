#pragma once

#include <vector>

class OctreeNode;
enum BatchSortMode{
	SORT_STATE = 0,
};

struct Batch{

	union{
		int sortKey;
	};
	OctreeNode* octreeNode;
};


struct BatchQueue{
	void clear();
	void sort(BatchSortMode sortMode);
	bool hasBatches() const;
	size_t getSize();


	std::vector<Batch> m_batches;
};
