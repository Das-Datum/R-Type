#pragma once

#include <queue>
#include <array>
#include <bitset>
#include <stdexcept>
#include "Types.hpp"

/**
 * @class EntityManager
 * @brief Manages the creation and destruction of entities.
 */
class EntityManager
{
    public:
        EntityManager()
        {
            for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
            {
                availableEntities.push(entity);
            }
        }

        /**
         * @brief Creates a new entity.
         * @return The new entity ID.
         */
        Entity createEntity()
        {
            if (livingEntityCount >= MAX_ENTITIES)
            {
                throw std::runtime_error("Too many entities in existence.");
            }
            Entity id = availableEntities.front();
            availableEntities.pop();
            ++livingEntityCount;
            return id;
        }

        /**
         * @brief Destroys an entity.
         * @param entity The entity to destroy.
         */
        void destroyEntity(Entity entity)
        {
            signatures[entity].reset();
            availableEntities.push(entity);
            --livingEntityCount;
        }

        /**
         * @brief Sets the signature of an entity.
         * @param entity The entity.
         * @param signature The signature to set.
         */
        void setSignature(Entity entity, Signature signature)
        {
            signatures[entity] = signature;
        }

        /**
         * @brief Gets the signature of an entity.
         * @param entity The entity.
         * @return The signature of the entity.
         */
        Signature getSignature(Entity entity)
        {
            return signatures[entity];
        }

    private:
        std::queue<Entity> availableEntities{};
        std::array<Signature, MAX_ENTITIES> signatures{};
        std::uint32_t livingEntityCount{};
};
