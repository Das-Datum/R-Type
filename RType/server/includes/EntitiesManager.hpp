#ifndef ENTITIES_MANAGER_HPP
#define ENTITIES_MANAGER_HPP

#include "../../../ECS/includes/ECS.hpp"
#include "Components.hpp"
#include "../../shared/includes/Components/GameComponents.hpp"

extern Coordinator gCoordinator;

class EntitiesManager {
public:
    static EntitiesManager& getInstance() {
        static EntitiesManager instance;
        return instance;
    }

    void setWindowHeight(float windowHeight) {
        WINDOW_HEIGHT = windowHeight;
    }

    Entity createPlayer(Vector2 position, sockaddr_in client_addr, std::string name) {
        Entity player = gCoordinator.createEntity();
        gCoordinator.addComponent(player, ShipComponent());
        gCoordinator.addComponent(player, PlayerNetworkComponents(name, client_addr));

        float ENTITY_HEIGHT = WINDOW_HEIGHT * 0.05f;
        float ENTITY_WIDTH = ENTITY_HEIGHT * 2; // Demander à Mathis


        gCoordinator.addComponent(player, TransformComponent(
            position,
            0.0f,
            Vector2{1.0f, 1.0f},
            Vector2{ENTITY_WIDTH, ENTITY_HEIGHT}
        ));

        Rectangle collider = {0, 0, ENTITY_WIDTH, ENTITY_HEIGHT};
        gCoordinator.addComponent(player, CollisionComponent(collider));

        return player;
    }

    Entity createBullet(Vector2 position, Vector2 velocity) {
        Entity bullet = gCoordinator.createEntity();
        gCoordinator.addComponent(bullet, BulletComponent());
        gCoordinator.addComponent(bullet, PhysicsComponent{velocity});

        float ENTITY_HEIGHT = WINDOW_HEIGHT * 0.025f;
        float ENTITY_WIDTH = ENTITY_HEIGHT * 2; // Demander à Mathis

        gCoordinator.addComponent(bullet, TransformComponent(
            position,
            0.0f,
            Vector2{1.0f, 1.0f},
            Vector2{ENTITY_WIDTH, ENTITY_HEIGHT}
        ));

        Rectangle collider = {0, 0, ENTITY_WIDTH, ENTITY_HEIGHT};
        gCoordinator.addComponent(bullet, CollisionComponent(collider));

        return bullet;
    }

    Entity createEnemy(Vector2 position) {
        Entity enemy = gCoordinator.createEntity();
        gCoordinator.addComponent(enemy, EnemyComponent());

        float ENTITY_HEIGHT = WINDOW_HEIGHT * 0.05f;
        float ENTITY_WIDTH = ENTITY_HEIGHT * 2; // Demander à Mathis

        gCoordinator.addComponent(enemy, TransformComponent(
            position,
            0.0f,
            Vector2{1.0f, 1.0f},
            Vector2{ENTITY_WIDTH, ENTITY_HEIGHT}
        ));

        Rectangle collider = {0, 0, ENTITY_WIDTH, ENTITY_HEIGHT};
        gCoordinator.addComponent(enemy, CollisionComponent(collider));

        return enemy;
    }

    Entity createStaticEntity(const std::string& texturePath, Vector2 position, float width, float height, int zIndex = 0) {
        Entity entity = gCoordinator.createEntity();

        float ENTITY_WIDTH = width;
        float ENTITY_HEIGHT = height;


        gCoordinator.addComponent(entity, TransformComponent(
            position,
            0.0f,
            Vector2{1.0f, 1.0f},
            Vector2{ENTITY_WIDTH, ENTITY_HEIGHT}
        ));

        return entity;
    }

    void removeEntity(Entity entity) {
        gCoordinator.destroyEntity(entity);
    }

private:
    EntitiesManager() = default;
    ~EntitiesManager() = default;

    float WINDOW_HEIGHT = 600.0f;
};

#endif // ENTITIES_MANAGER_HPP
