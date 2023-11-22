#pragma once

class IPathMap{

public:
	virtual ~IPathMap();

	virtual bool IsCellWalkable(unsigned int r, unsigned int c) const = 0;
};

inline IPathMap::~IPathMap() { }