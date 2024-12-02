#include "../../../ECS/includes/ECS.hpp"
#include "../includes/Components.hpp"
#include "../includes/TexturesManager.hpp"
#include "../includes/EntitiesManager.hpp"

#include "../includes/Systems/ClientRelatives.hpp"
#include "../includes/Systems/SpriteRelatives.hpp"
#include "../includes/Systems/Generics.hpp"
#include "../includes/Systems/CollisionSystem.hpp"
#include "../includes/Systems/BackgroundScrollSystem.hpp"

#include "../../shared/includes/Components/GameComponents.hpp"
#include "../../shared/includes/Systems/Game.hpp"

Coordinator gCoordinator;

int main() {
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    const float SHIP_HEIGHT = WINDOW_HEIGHT * 0.05f;
    auto& TexturesManager = TexturesManager::getInstance();
    auto& entitiesManager = EntitiesManager::getInstance();
    entitiesManager.setWindowHeight(static_cast<float>(WINDOW_HEIGHT));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RType");
    SetTargetFPS(60);

    gCoordinator.init();
    gCoordinator.registerComponent<TransformComponent>();
    gCoordinator.registerComponent<InputComponent>();
    gCoordinator.registerComponent<TimerComponent>();

    gCoordinator.registerComponent<SpriteComponent>();
    gCoordinator.registerComponent<SpriteAnimationComponent>();
    gCoordinator.registerComponent<SpriteFrameComponent>();

    gCoordinator.registerComponent<ShipComponent>();
    gCoordinator.registerComponent<BulletComponent>();
    gCoordinator.registerComponent<EnemyComponent>();
    gCoordinator.registerComponent<StaticComponent>();

    //? Shared components
    gCoordinator.registerComponent<PhysicsComponent>();
    gCoordinator.registerComponent<CollisionComponent>();
    gCoordinator.registerComponent<BackgroundScrollComponent>();

    //* Systems
    auto renderSystem = gCoordinator.registerSystem<RenderSystem>();
    auto inputSystem = gCoordinator.registerSystem<InputSystem>();
    auto physicsSystem = gCoordinator.registerSystem<PhysicsSystem>();
    auto timerSystem = gCoordinator.registerSystem<TimerSystem>();
    auto spriteFrameSystem = gCoordinator.registerSystem<SpriteFrameSystem>();
    auto collisionSystem = gCoordinator.registerSystem<CollisionSystem>();
    auto backgroundScrollSystem = gCoordinator.registerSystem<BackgroundScrollSystem>();
    Signature signature;

    //? RenderSystem
    signature.set(gCoordinator.getComponentTypeID<TransformComponent>(), true);
    gCoordinator.setSystemSignature<RenderSystem>(signature);

    //? InputSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<TransformComponent>(), true);
    signature.set(gCoordinator.getComponentTypeID<InputComponent>(), true);
    gCoordinator.setSystemSignature<InputSystem>(signature);

    //? PhysicsSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<TransformComponent>(), true);
    signature.set(gCoordinator.getComponentTypeID<BulletComponent>(), true);
    gCoordinator.setSystemSignature<PhysicsSystem>(signature);

    //? TimerSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<TimerComponent>(), true);
    gCoordinator.setSystemSignature<TimerSystem>(signature);

    //? SpriteFrameSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<SpriteFrameComponent>(), true);
    gCoordinator.setSystemSignature<SpriteFrameSystem>(signature);

    //? CollisionSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<TransformComponent>(), true);
    signature.set(gCoordinator.getComponentTypeID<CollisionComponent>(), true);
    gCoordinator.setSystemSignature<CollisionSystem>(signature);

    //? BackgroundScrollSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<BackgroundScrollComponent>(), true);
    gCoordinator.setSystemSignature<BackgroundScrollSystem>(signature);

    //? User 1 (main player)
    Vector2 shipPosition = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT * 0.8f};
    entitiesManager.createShip(shipPosition);

    //* Test entity
    Vector2 enemyPosition = {WINDOW_WIDTH * 0.9f, WINDOW_HEIGHT * 0.2f};
    entitiesManager.createEnemy(enemyPosition);
    // entitiesManager.removeEntity(enemy);

    //* Backgrounds
    entitiesManager.createScrollingBackground(
        "../assets/textures/backgrounds/bg-back.png",
        {0, 0},
        static_cast<float>(WINDOW_WIDTH),
        static_cast<float>(WINDOW_HEIGHT),
        25.0f,
        -1
    );

    entitiesManager.createScrollingBackground(
        "../assets/textures/backgrounds/bg-stars.png",
        {0, 0},
        static_cast<float>(WINDOW_WIDTH),
        static_cast<float>(WINDOW_HEIGHT),
        50.0f,
        -1
    );

    physicsSystem->setGameDimensions(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));

    //! MAIN LOOP
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        //? LOGIC
        inputSystem->update();
        spriteFrameSystem->update();
        timerSystem->update();
        collisionSystem->update();
        backgroundScrollSystem->update(deltaTime);
        physicsSystem->update(deltaTime);

        //! DESTROY
        gCoordinator.processEntityDestruction();

        //? RENDER
        BeginDrawing();
        ClearBackground(BLACK);
        renderSystem->update();
        EndDrawing();
    }

    TexturesManager.unloadAllTextures();
    CloseWindow();
    return 0;
}
