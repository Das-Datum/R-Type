#ifndef ENTITIES_MANAGER_HPP
#define ENTITIES_MANAGER_HPP

#include "../../../ECS/includes/ECS.hpp"
#include "../../../Network/includes/NetworkComponent.hpp"

#include "../../shared/includes/Components/GameComponents.hpp"

#include "Components.hpp"
#include "TexturesManager.hpp"

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

    Entity createServer(const std::string& ip, int port) {
        Entity server = gCoordinator.createEntity();

        gCoordinator.addComponent(server, NetworkComponent("Server", ip, port, 0));

        return server;
    }

    Entity createPlayer(Vector2 position, const std::string& name, const std::string& ip, int port, int id) {
        Entity ship = gCoordinator.createEntity();

        std::string shipTexturePath = "./assets/textures/ships/ship_1.png";
        auto& texturesManager = TexturesManager::getInstance();
        Texture2D shipTexture = texturesManager.loadTexture(shipTexturePath);

        float frameWidth = static_cast<float>(shipTexture.width) / 5;
        float frameAspectRatio = frameWidth / static_cast<float>(shipTexture.height);
        float ENTITY_HEIGHT = WINDOW_HEIGHT * 0.05f;
        float ENTITY_WIDTH = ENTITY_HEIGHT * frameAspectRatio;

        gCoordinator.addComponent(ship, TransformComponent(
            position,
            0.0f,
            Vector2{1.0f, 1.0f},
            Vector2{ENTITY_WIDTH, ENTITY_HEIGHT}
        ));

        Rectangle collider = {0, 0, ENTITY_WIDTH, ENTITY_HEIGHT};
        gCoordinator.addComponent(ship, ShipComponent());
        // gCoordinator.addComponent(ship, TimerComponent());
        gCoordinator.addComponent(ship, BlockOutOfBoundsComponent());
        gCoordinator.addComponent(ship, CollisionComponent(collider));
        gCoordinator.addComponent(ship, NetworkComponent(name, ip, port, id));

        return ship;
    }

    Entity createBullet(Vector2 position, Vector2 velocity) {
        Entity bullet = gCoordinator.createEntity();

        auto& texturesManager = TexturesManager::getInstance();
        std::string bulletTexturePath = "./assets/textures/bullets/bullet_1.png";
        Texture2D bulletTexture = texturesManager.loadTexture(bulletTexturePath);

        float frameWidth = static_cast<float>(bulletTexture.width) / 2;
        float frameAspectRatio = frameWidth / static_cast<float>(bulletTexture.height);
        float ENTITY_HEIGHT = WINDOW_HEIGHT * 0.025f;
        float ENTITY_WIDTH = ENTITY_HEIGHT * frameAspectRatio;

        gCoordinator.addComponent(bullet, TransformComponent(
            position,
            0.0f,
            Vector2{1.0f, 1.0f},
            Vector2{ENTITY_WIDTH, ENTITY_HEIGHT}
        ));

        Rectangle collider = {0, 0, ENTITY_WIDTH, ENTITY_HEIGHT};
        gCoordinator.addComponent(bullet, BulletComponent());
        gCoordinator.addComponent(bullet, FixedVelocityComponent{velocity});
        gCoordinator.addComponent(bullet, DestroyOutOfBoundsComponent());
        gCoordinator.addComponent(bullet, CollisionComponent(collider));

        return bullet;
    }

    Entity createEnemy(Vector2 position) {
        Entity enemy = gCoordinator.createEntity();

        auto& texturesManager = TexturesManager::getInstance();
        std::string enemyTexturePath = "./assets/textures/enemies/enemy_1.png";
        Texture2D enemyTexture = texturesManager.loadTexture(enemyTexturePath);

        float frameWidth = static_cast<float>(enemyTexture.width) / 12;
        float frameAspectRatio = frameWidth / static_cast<float>(enemyTexture.height);
        float ENTITY_HEIGHT = WINDOW_HEIGHT * 0.05f;
        float ENTITY_WIDTH = ENTITY_HEIGHT * frameAspectRatio;

        gCoordinator.addComponent(enemy, TransformComponent(
            position,
            0.0f,
            Vector2{1.0f, 1.0f},
            Vector2{ENTITY_WIDTH, ENTITY_HEIGHT}
        ));

        Rectangle collider = {0, 0, ENTITY_WIDTH, ENTITY_HEIGHT};
        gCoordinator.addComponent(enemy, CollisionComponent(collider));
        gCoordinator.addComponent(enemy, EnemyComponent({BehaviorType::RandomMovement, BehaviorType::ShootAtPlayer}));
        gCoordinator.addComponent(enemy, EnemyShootComponent(1000.0f, 1.0f, 200.0f));
        gCoordinator.addComponent(enemy, EnemyMovementComponent(50.0f, {position.x, position.y}, {position.x + 100.0f, position.y}, 100.0f, 1.0f));
        gCoordinator.addComponent(enemy, DestroyOutOfBoundsComponent());

        return enemy;
    }

    Entity createEnemyBullet(Vector2 position, Vector2 velocity) {
        Entity bullet = gCoordinator.createEntity();

        auto& texturesManager = TexturesManager::getInstance();
        std::string bulletTexturePath = "./assets/textures/bullets/bullet_1.png";
        Texture2D bulletTexture = texturesManager.loadTexture(bulletTexturePath);

        float frameWidth = static_cast<float>(bulletTexture.width) / 2;
        float frameAspectRatio = frameWidth / static_cast<float>(bulletTexture.height);
        float ENTITY_HEIGHT = WINDOW_HEIGHT * 0.025f;
        float ENTITY_WIDTH = ENTITY_HEIGHT * frameAspectRatio;

        // float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG;
        float rotation = 0.5;

        gCoordinator.addComponent(bullet, TransformComponent(
            position,
            rotation,
            Vector2{1.0f, 1.0f},
            Vector2{ENTITY_WIDTH, ENTITY_HEIGHT}
        ));

        Rectangle collider = {0, 0, ENTITY_WIDTH, ENTITY_HEIGHT};
        gCoordinator.addComponent(bullet, BulletComponent());
        gCoordinator.addComponent(bullet, FixedVelocityComponent{velocity});
        gCoordinator.addComponent(bullet, DestroyOutOfBoundsComponent());
        gCoordinator.addComponent(bullet, CollisionComponent(collider, rotation));

        return bullet;
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
