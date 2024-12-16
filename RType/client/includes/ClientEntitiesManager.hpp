#pragma once

#include "NetworkComponent.hpp"
#include "AEntitiesManager.hpp"
#include "Components.hpp"
#include "TexturesManager.hpp"

#include <cmath>

class EntitiesManager : public AEntitiesManager {
public:
    static EntitiesManager &getInstance() {
        static EntitiesManager instance;
        return instance;
    }

    Entity createShip(Vector2 position, int id, const std::string &name, bool playableEntity = true) {
        Vector2 normalizedPos = position;

        Entity ship = gCoordinator.createEntity();

        std::string shipTexturePath = "./assets/textures/ships/ship_" + std::to_string(id) + ".png";
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
        if (playableEntity) {
            gCoordinator.addComponent(ship, InputComponent());
            gCoordinator.addComponent(ship, NetworkInstructionsComponent(name, id));
            gCoordinator.addComponent(ship, SpriteFrameComponent(1, 5));
        } else {
            gCoordinator.addComponent(ship, NetworkComponent{name, "", 0, id});
            gCoordinator.addComponent(ship, NetworkPositionComponent(position));
        }
        gCoordinator.addComponent(ship, TimerComponent());
        gCoordinator.addComponent(ship, BlockOutOfBoundsComponent());
        gCoordinator.addComponent(ship, CollisionComponent(collider));
        gCoordinator.addComponent(ship, VelocityComponent(Vector2{0.0f, 0.0f}));

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
        gCoordinator.addComponent(bullet, CollisionComponent(collider, 0.0f));
        gCoordinator.addComponent(bullet, SpriteFrameComponent(1, 2));

        return bullet;
    }

    Entity createEnemy(Enemy enemyInfos) override {
        Entity enemy = gCoordinator.createEntity();
        Vector2 position = Vector2{enemyInfos.x, enemyInfos.y};

        auto &texturesManager = TexturesManager::getInstance();
        std::string enemyTexturePath = "./assets/textures/enemies/" + enemyInfos.type.name + ".png";
        Texture2D enemyTexture = texturesManager.loadTexture(enemyTexturePath);
        Rectangle initialFrame = texturesManager.getFrame(enemyTexturePath, 0, enemyInfos.type.nbFrame);

        float normalizedHeight = 0.05f;
        float frameAspectRatio = static_cast<float>(enemyTexture.width / enemyInfos.type.nbFrame) / enemyTexture.height;
        float normalizedWidth = normalizedHeight * frameAspectRatio;

        gCoordinator.addComponent(enemy, TransformComponent(position, 0.0f, Vector2{1.0f, 1.0f}, Vector2{normalizedWidth, normalizedHeight}));

        Rectangle collider = {0, 0, normalizedWidth, normalizedHeight};
        gCoordinator.addComponent(enemy, SpriteComponent(enemyTexture, initialFrame, 1));
        gCoordinator.addComponent(enemy, CollisionComponent(collider));
        gCoordinator.addComponent(enemy, SpriteAnimationComponent(enemyInfos.type.nbFrame, 0.15f));
        gCoordinator.addComponent(enemy, DestroyOutOfBoundsComponent());

        std::vector<BehaviorType> behaviors;
        gCoordinator.addComponent(enemy, SpawnComponent(enemyInfos.spawnTime));

        if (enemyInfos.type.isShooting) {
            gCoordinator.addComponent(enemy, EnemyShootComponent(1000.0f, 1.0f, 100.0f));
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

        gCoordinator.addComponent(enemy, FixedVelocityComponent{Vector2{-0.1f, 0.0f}});
        gCoordinator.addComponent(enemy, EnemyComponent((behaviors.size() > 0 ? behaviors : std::vector<BehaviorType>{BehaviorType::None}), enemyInfos.type.name));

        return enemy;
    }

    Entity createEnemyBullet(Vector2 position, Vector2 velocity) {
        Vector2 normalizedPos = position;
        Vector2 normalizedVelocity = velocity;
        Entity bullet = gCoordinator.createEntity();

        auto &texturesManager = TexturesManager::getInstance();
        std::string bulletTexturePath = "./assets/textures/enemies/projectile.png";
        Texture2D bulletTexture = texturesManager.loadTexture(bulletTexturePath);
        Rectangle initialFrame = texturesManager.getFrame(bulletTexturePath, 0, 1);

        float normalizedHeight = 0.02f;
        float frameAspectRatio = (bulletTexture.width / 2.0f) / bulletTexture.height;
        float normalizedWidth = normalizedHeight * frameAspectRatio;
        float rotation = atan2f(velocity.y, velocity.x) * RAD2DEG;

        Rectangle collider = {
            -normalizedWidth * 0.5f,
            -normalizedHeight * 0.5f,
            normalizedWidth,
            normalizedHeight};

        gCoordinator.addComponent(bullet, TransformComponent(normalizedPos, rotation, Vector2{1.0f, 1.0f}, Vector2{normalizedWidth, normalizedHeight}));
        gCoordinator.addComponent(bullet, SpriteComponent(bulletTexture, initialFrame, 2));
        gCoordinator.addComponent(bullet, EnemyBulletComponent());
        gCoordinator.addComponent(bullet, FixedVelocityComponent{normalizedVelocity});
        gCoordinator.addComponent(bullet, DestroyOutOfBoundsComponent());
        gCoordinator.addComponent(bullet, CollisionComponent(collider, rotation));
        // gCoordinator.addComponent(bullet, SpriteFrameComponent(1, 2));

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
};
