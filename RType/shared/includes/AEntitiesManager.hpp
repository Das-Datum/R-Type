#pragma once

#include "Components/GameComponents.hpp"
#include "ECS.hpp"

extern Coordinator gCoordinator;

struct EnemyType {
    std::string name;
    std::size_t maxHealth;
    bool isShooting;
    bool isDestructible;
    Vector2 size;
    int nbFrame;

    EnemyType(const std::string& enemyName, Vector2 assetSize, int nbAssetFrame, const std::size_t& enemyHP = 100, bool isEnemyShooting = true, bool isEnemyDestructible = true)
    : name(enemyName), size(assetSize), nbFrame(nbAssetFrame), maxHealth(enemyHP), isShooting(isEnemyShooting), isDestructible(isEnemyDestructible) {}
};

struct Enemy {
    float x;
    float y;
    float spawnTime;
    EnemyType type;

    Enemy(float xPos, float yPos, float enemySpawnTime, EnemyType enemyType)
    : x(xPos), y(yPos), spawnTime(enemySpawnTime), type(enemyType) {}
};

/**
 * @class AEntitiesManager
 * @brief Abstract base class for EntitiesManager for server and client
 */
class AEntitiesManager {
    public:
        virtual ~AEntitiesManager() = default;

        /**
         * @brief Create a Enemy object
         *
         * @return Entity
         */
        virtual Entity createEnemy(Enemy) = 0;

        /**
         * @brief Remove an entity from the manager
         *
         * @param entity
         * @return void
         */
        void removeEntity(Entity entity) {
            gCoordinator.destroyEntity(entity);
        }
};