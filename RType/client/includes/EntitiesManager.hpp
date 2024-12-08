#ifndef ENTITIES_MANAGER_HPP
#define ENTITIES_MANAGER_HPP

#include "../../../ECS/includes/ECS.hpp"
#include "../../shared/includes/Components/GameComponents.hpp"
#include "Components.hpp"
#include "TexturesManager.hpp"

extern Coordinator gCoordinator;

class EntitiesManager {
  public:
    static EntitiesManager &getInstance() {
        static EntitiesManager instance;
        return instance;
    }

    void setWindowHeight(float windowHeight) {
        WINDOW_HEIGHT = windowHeight;
    }

    Entity createShip(Vector2 position) {
        Vector2 normalizedPos = {
            position.x / baseWidth,
            position.y / baseHeight};

        Entity ship = gCoordinator.createEntity();

        std::string shipTexturePath = "./assets/textures/ships/ship_1.png";
        auto &texturesManager = TexturesManager::getInstance();
        Texture2D shipTexture = texturesManager.loadTexture(shipTexturePath);
        Rectangle initialFrame = texturesManager.getFrame(shipTexturePath, 2, 5);

        float frameWidth = static_cast<float>(shipTexture.width) / 5;
        float frameAspectRatio = frameWidth / static_cast<float>(shipTexture.height);
        float normalizedHeight = 0.05f;
        float normalizedWidth = normalizedHeight * frameAspectRatio;

        gCoordinator.addComponent(ship, SpriteComponent(shipTexture, initialFrame, 1));
        gCoordinator.addComponent(ship, TransformComponent(normalizedPos, 0.0f, Vector2{1.0f, 1.0f}, Vector2{normalizedWidth, normalizedHeight}));

        Rectangle collider = {0, 0, normalizedWidth, normalizedHeight};
        gCoordinator.addComponent(ship, ShipComponent());
        gCoordinator.addComponent(ship, InputComponent());
        gCoordinator.addComponent(ship, TimerComponent());
        gCoordinator.addComponent(ship, BlockOutOfBoundsComponent());
        gCoordinator.addComponent(ship, CollisionComponent(collider));
        gCoordinator.addComponent(ship, SpriteFrameComponent(1, 5));

        return ship;
    }

    Entity createBullet(Vector2 position, Vector2 velocity) {
        Vector2 normalizedPos = position;
        Vector2 normalizedVelocity = velocity;

        Entity bullet = gCoordinator.createEntity();

        auto &texturesManager = TexturesManager::getInstance();
        std::string bulletTexturePath = "./assets/textures/bullets/bullet_1.png";
        Texture2D bulletTexture = texturesManager.loadTexture(bulletTexturePath);
        Rectangle initialFrame = texturesManager.getFrame(bulletTexturePath, 0, 2);

        float normalizedHeight = 0.02f;
        float frameAspectRatio = (float)initialFrame.width / initialFrame.height;
        float normalizedWidth = normalizedHeight * frameAspectRatio;

        Rectangle collider = {
            -normalizedWidth * 0.5f,
            -normalizedHeight * 0.5f,
            normalizedWidth,
            normalizedHeight};

        gCoordinator.addComponent(bullet, TransformComponent(normalizedPos, 0.0f, Vector2{1.0f, 1.0f}, Vector2{normalizedWidth, normalizedHeight}));

        gCoordinator.addComponent(bullet, SpriteComponent(bulletTexture, initialFrame, 2));
        gCoordinator.addComponent(bullet, BulletComponent());
        gCoordinator.addComponent(bullet, FixedVelocityComponent{normalizedVelocity});
        gCoordinator.addComponent(bullet, DestroyOutOfBoundsComponent());
        gCoordinator.addComponent(bullet, CollisionComponent(collider));
        gCoordinator.addComponent(bullet, SpriteFrameComponent(1, 2));

        return bullet;
    }

    Entity createEnemy(Vector2 position) {
        Vector2 normalizedPos = position;
        Entity enemy = gCoordinator.createEntity();

        auto &texturesManager = TexturesManager::getInstance();
        std::string enemyTexturePath = "./assets/textures/enemies/enemy_1.png";
        Texture2D enemyTexture = texturesManager.loadTexture(enemyTexturePath);
        Rectangle initialFrame = texturesManager.getFrame(enemyTexturePath, 0, 12);

        float normalizedHeight = 0.05f;
        float frameAspectRatio = static_cast<float>(enemyTexture.width / 12) / enemyTexture.height;
        float normalizedWidth = normalizedHeight * frameAspectRatio;

        gCoordinator.addComponent(enemy, TransformComponent(normalizedPos, 0.0f, Vector2{1.0f, 1.0f}, Vector2{normalizedWidth, normalizedHeight}));

        Rectangle collider = {0, 0, normalizedWidth, normalizedHeight};
        gCoordinator.addComponent(enemy, SpriteComponent(enemyTexture, initialFrame, 1));
        gCoordinator.addComponent(enemy, CollisionComponent(collider));
        gCoordinator.addComponent(enemy, SpriteAnimationComponent(12, 0.15f));
        gCoordinator.addComponent(enemy, EnemyComponent({BehaviorType::RandomMovement, BehaviorType::ShootAtPlayer}));
        gCoordinator.addComponent(enemy, EnemyShootComponent(1000.0f, 1.0f, 200.0f));
        gCoordinator.addComponent(enemy, EnemyMovementComponent(0.1f, normalizedPos, Vector2{normalizedPos.x + 0.2f, normalizedPos.y}, 0.1f, 1.0f));
        gCoordinator.addComponent(enemy, DestroyOutOfBoundsComponent());

        return enemy;
    }

    Entity createEnemyBullet(Vector2 position, Vector2 velocity) {
        Vector2 normalizedPos = position;
        Vector2 normalizedVelocity = velocity;
        Entity bullet = gCoordinator.createEntity();

        auto &texturesManager = TexturesManager::getInstance();
        std::string bulletTexturePath = "./assets/textures/bullets/bullet_1.png";
        Texture2D bulletTexture = texturesManager.loadTexture(bulletTexturePath);
        Rectangle initialFrame = texturesManager.getFrame(bulletTexturePath, 0, 2);

        float normalizedHeight = 0.02f;
        float frameAspectRatio = static_cast<float>(bulletTexture.width / 2) / bulletTexture.height;
        float normalizedWidth = normalizedHeight * frameAspectRatio;
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG;

        Rectangle collider = {
            -normalizedWidth * 0.5f,
            -normalizedHeight * 0.5f,
            normalizedWidth,
            normalizedHeight};

        gCoordinator.addComponent(bullet, TransformComponent(normalizedPos, rotation, Vector2{1.0f, 1.0f}, Vector2{normalizedWidth, normalizedHeight}));

        gCoordinator.addComponent(bullet, SpriteComponent(bulletTexture, initialFrame, 2));
        gCoordinator.addComponent(bullet, BulletComponent());
        gCoordinator.addComponent(bullet, FixedVelocityComponent{normalizedVelocity});
        gCoordinator.addComponent(bullet, DestroyOutOfBoundsComponent());
        gCoordinator.addComponent(bullet, CollisionComponent(collider, rotation));
        gCoordinator.addComponent(bullet, SpriteFrameComponent(1, 2));

        return bullet;
    }

    Entity createStaticEntity(const std::string &texturePath, Vector2 position, float width, float height, int zIndex = 0) {
        Vector2 normalizedPos = position;
        Entity entity = gCoordinator.createEntity();
        gCoordinator.addComponent(entity, StaticComponent());

        auto &texturesManager = TexturesManager::getInstance();
        Texture2D texture = texturesManager.loadTexture(texturePath);
        Rectangle sourceRect = {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)};
        gCoordinator.addComponent(entity, SpriteComponent(texture, sourceRect, zIndex));
        gCoordinator.addComponent(entity, TransformComponent(normalizedPos, 0.0f, Vector2{1.0f, 1.0f}, Vector2{width, height}));

        return entity;
    }

    Entity createScrollingBackground(const std::string &texturePath, Vector2 position, float width, float height, float scrollSpeed, int zIndex = -1) {
        Vector2 normalizedPos = {
            position.x / 1920.0f,
            position.y / 1080.0f};

        Entity entity = gCoordinator.createEntity();
        gCoordinator.addComponent(entity, StaticComponent());

        auto &texturesManager = TexturesManager::getInstance();
        Texture2D texture = texturesManager.loadTexture(texturePath);
        Rectangle sourceRect = {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)};

        gCoordinator.addComponent(entity, BackgroundScrollComponent(scrollSpeed, static_cast<float>(texture.width), width));

        float ENTITY_WIDTH = width;
        float ENTITY_HEIGHT = height;

        gCoordinator.addComponent(entity, SpriteComponent(texture, sourceRect, zIndex));
        gCoordinator.addComponent(entity, TransformComponent(normalizedPos, 0.0f, Vector2{1.0f, 1.0f}, Vector2{ENTITY_WIDTH, ENTITY_HEIGHT}));

        return entity;
    }

    void removeEntity(Entity entity) {
        gCoordinator.destroyEntity(entity);
    }

  private:
    EntitiesManager() = default;
    ~EntitiesManager() = default;

    const float baseWidth = 1920.0f;
    const float baseHeight = 1080.0f;
    float WINDOW_HEIGHT = 600.0f;
};

#endif // ENTITIES_MANAGER_HPP
