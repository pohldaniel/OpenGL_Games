#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <ECS/ECS.h>
#include <sol/sol.hpp>
#include <memory>

class LevelLoader {
    public:
        LevelLoader();
        ~LevelLoader();
        void LoadLevel(sol::state& lua, const std::unique_ptr<Registry>& registry, int level);
};

#endif
