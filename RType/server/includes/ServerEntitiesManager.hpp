#pragma once

#include "AEntitiesManager.hpp"

class ServerEntitiesManager : public AEntitiesManager {
public:
    static ServerEntitiesManager& getInstance() {
        static ServerEntitiesManager instance;
        return instance;
    }

    Entity createEnemy(Enemy enemyInfos) override {
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

    Entity createShip(Vector2 normalizedPos, int id, const std::string &name, const std::string &ip, int port) {

        Entity ship = gCoordinator.createEntity();

        float frameWidth = static_cast<float>(1315) / 5;
        float frameAspectRatio = frameWidth / static_cast<float>(116);
        float normalizedHeight = 0.05f;
        float normalizedWidth = normalizedHeight * frameAspectRatio;

        gCoordinator.addComponent(ship, TransformComponent(normalizedPos, 0.0f, Vector2{1.0f, 1.0f}, Vector2{normalizedWidth, normalizedHeight}));

        Rectangle collider = {0, 0, normalizedWidth, normalizedHeight};
        gCoordinator.addComponent(ship, ShipComponent());
 
        gCoordinator.addComponent(ship, NetworkComponent{name, ip, port, id});
        gCoordinator.addComponent(ship, BlockOutOfBoundsComponent());
        gCoordinator.addComponent(ship, CollisionComponent(collider));
        return ship;
    }

private:
    ServerEntitiesManager() = default;
    ~ServerEntitiesManager() = default;

    float WINDOW_HEIGHT = 600.0f;
};
