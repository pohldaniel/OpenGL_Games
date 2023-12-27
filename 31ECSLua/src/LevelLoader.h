#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <ECS/ECS.h>
#include <sol/sol.hpp>
#include <memory>

struct Cell {
	TextureRect rect;
	float posX;
	float posY;
};

class LevelLoader {
   public:
	LevelLoader();
	~LevelLoader();
	void LoadLevel(sol::state& lua, const std::unique_ptr<Registry>& registry, int level);

	static unsigned int Atlas;
};

#endif
