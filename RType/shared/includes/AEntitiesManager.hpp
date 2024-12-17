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
    float xSpeed;
    float ySpeed;

    EnemyType(const std::string& enemyName, Vector2 assetSize, int nbAssetFrame, const std::size_t& enemyHP = 100, bool isEnemyShooting = true, bool isEnemyDestructible = true, float enemyXSpeed = 50.0f, float enemyYSpeed = 0.0f)
    : name(enemyName), size(assetSize), nbFrame(nbAssetFrame), maxHealth(enemyHP), isShooting(isEnemyShooting), isDestructible(isEnemyDestructible), xSpeed(enemyXSpeed), ySpeed(enemyYSpeed) {}
};

struct Enemy {
    float x;
    float y;
    float spawnTime;
    EnemyType type;
    bool shootAtPlayer = false;
    bool hasSpecificMove = false;
    BehaviorType specificMove;

    int uniqueId;

    Enemy(float xPos, float yPos, float enemySpawnTime, EnemyType enemyType, bool isEnemyShootAtPlayer = false, bool hasEnemySpecificMove = false, BehaviorType enemySpecificMove = BehaviorType::None)
    : x(xPos), y(yPos), spawnTime(enemySpawnTime), type(enemyType), shootAtPlayer(isEnemyShootAtPlayer), hasSpecificMove(hasEnemySpecificMove), specificMove(enemySpecificMove) {}

    void setUniqueId(int id) {
        uniqueId = id;
    }
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

        /**
         * @brief Create a enemy bullet object
         *
         * @param position
         * @param velocity
         * @return Entity
         */
        virtual Entity createEnemyBullet(Vector2 position, Vector2 velocity) = 0;

        /**
         * @brief Create a bullet object
         *
         * @param position
         * @param velocity
         * @return Entity
         */
        virtual Entity createBullet(Vector2 position, Vector2 velocity) = 0;

        int getTotalScore() const { return _totalScore; }
        void addScore(int score) { _totalScore += score; }

    private:
        int _totalScore = 0;
};
