#pragma once

#include <set>
#include "Types.hpp"

/**
 * @class System
 * @brief Base class for all systems.
 */
class System
{
    public:
        std::set<Entity> entities;
};
