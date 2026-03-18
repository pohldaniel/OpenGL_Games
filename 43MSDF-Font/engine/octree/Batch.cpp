#include <iostream>
#include <algorithm>
#include "Batch.h"

inline bool CompareBatchKeys(const Batch& lhs, const Batch& rhs){
	return lhs.sortKey < rhs.sortKey;
}

void BatchQueue::clear() {
	m_batches.clear();
}

void BatchQueue::sort(BatchSortMode sortMode) {
	switch (sortMode){
		case SORT_STATE:
			std::sort(m_batches.begin(), m_batches.end(), CompareBatchKeys);
			break;
		default:
			break;
	}
}

bool BatchQueue::hasBatches() const { 
	return m_batches.size();
}

size_t BatchQueue::getSize() {
	return m_batches.size();
}