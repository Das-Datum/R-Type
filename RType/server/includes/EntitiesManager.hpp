#pragma once

#include "../../shared/includes/Components/GameComponents.hpp"
#include "../../../ECS/includes/ECS.hpp"

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

class EntitiesManager {
public:
    static EntitiesManager& getInstance() {
        static EntitiesManager instance;
        return instance;
    }

    Entity createEnemy(Enemy enemyInfos) {
        Entity enemy = gCoordinator.createEntity();
        Vector2 position = Vector2{enemyInfos.x, enemyInfos.y};

        float frameWidth = static_cast<float>(enemyInfos.type.size.x) / 12;
        float frameAspectRatio = frameWidth / static_cast<float>(enemyInfos.type.size.y);
        float ENTITY_HEIGHT = WINDOW_HEIGHT * 0.05f;
        float ENTITY_WIDTH = ENTITY_HEIGHT * frameAspectRatio;

        gCoordinator.addComponent(enemy, TransformComponent(
            position,
            0.0f,
            Vector2{1.0f, 1.0f},
            Vector2{ENTITY_WIDTH, ENTITY_HEIGHT}
        ));

        Rectangle collider = {0, 0, ENTITY_WIDTH, ENTITY_HEIGHT};
        std::vector<BehaviorType> behaviors;
        gCoordinator.addComponent(enemy, CollisionComponent(collider));
        gCoordinator.addComponent(enemy, SpawnComponent(enemyInfos.spawnTime));
        gCoordinator.addComponent(enemy, DestroyOutOfBoundsComponent());

        if (enemyInfos.type.isShooting) {
            gCoordinator.addComponent(enemy, EnemyShootComponent(1000.0f, 1.0f, 200.0f));
            int choice = rand() % 2;
            if (choice == 1)
                behaviors.push_back(BehaviorType::ShootAtPlayer);
        }
        int hasParticularMove = rand() % 2;
        if (hasParticularMove == 1) {
            gCoordinator.addComponent(enemy, EnemyMovementComponent(50.0f, {position.x, position.y}, {position.x + 100.0f, position.y}, 100.0f, 1.0f));
            int choice = rand() % 4;
            switch (choice)
            {
            case 0:
                behaviors.push_back(BehaviorType::ChasePlayer);
                break;
            case 1:
                behaviors.push_back(BehaviorType::FleeFromPlayer);
                break;
            case 2:
                behaviors.push_back(BehaviorType::Patrol);
                break;
            default:
                behaviors.push_back(BehaviorType::RandomMovement);
                break;
            }
        }
        if (enemyInfos.type.isDestructible)
            gCoordinator.addComponent(enemy, EnemyHealthComponent(enemyInfos.type.maxHealth));

        gCoordinator.addComponent(enemy, EnemyComponent((behaviors.size() > 0 ? behaviors : std::vector<BehaviorType>{BehaviorType::None}), enemyInfos.type.name));

        return enemy;
    }

    void removeEntity(Entity entity) {
        gCoordinator.destroyEntity(entity);
    }

private:
    EntitiesManager() = default;
    ~EntitiesManager() = default;

    float WINDOW_HEIGHT = 600.0f;
};
