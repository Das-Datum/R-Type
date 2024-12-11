#include "../includes/client.hpp"

Coordinator gCoordinator;

int main() {
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    const float SHIP_HEIGHT = WINDOW_HEIGHT * 0.05f;
    auto& TexturesManager = TexturesManager::getInstance();
    auto& entitiesManager = EntitiesManager::getInstance();
    entitiesManager.setWindowHeight(static_cast<float>(WINDOW_HEIGHT));

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RType");
    SetTargetFPS(60);

    gCoordinator.init();
    gCoordinator.registerComponent<InputComponent>();
    gCoordinator.registerComponent<TimerComponent>();

    gCoordinator.registerComponent<SpriteComponent>();
    gCoordinator.registerComponent<SpriteAnimationComponent>();
    gCoordinator.registerComponent<SpriteFrameComponent>();

    gCoordinator.registerComponent<StaticComponent>();

    gCoordinator.registerComponent<EnemyComponent>();
    gCoordinator.registerComponent<EnemyMovementComponent>();
    gCoordinator.registerComponent<EnemyShootComponent>();

    gCoordinator.registerComponent<SpawnComponent>();
    gCoordinator.registerComponent<EnemyHealthComponent>();

    //? Shared components
    gCoordinator.registerComponent<TransformComponent>();
    gCoordinator.registerComponent<ShipComponent>();
    gCoordinator.registerComponent<BulletComponent>();
    gCoordinator.registerComponent<FixedVelocityComponent>();
    gCoordinator.registerComponent<DestroyOutOfBoundsComponent>();
    gCoordinator.registerComponent<BlockOutOfBoundsComponent>();
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

    auto enemiesSystem = gCoordinator.registerSystem<EnemiesSystem>();
    auto clientNetworkSystem = gCoordinator.registerSystem<ClientSystem>();

    Signature signature;

    //? NetworkSystem
    clientNetworkSystem->init("127.0.0.0", 5000);

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
    Vector2 enemyPosition = {0.9f, 0.5f};
    Enemy enemyInfos = Enemy(enemyPosition.x, enemyPosition.y, 0.0, EnemyType("ship", Vector2{145.0, 29.0}, 5, 100, true, true));
    entitiesManager.createEnemy(enemyInfos);
    // entitiesManager.removeEntity(enemy);

    //* Backgrounds
    entitiesManager.createScrollingBackground(
        "./assets/textures/backgrounds/bg-back.png",
        {0, 0},
        static_cast<float>(WINDOW_WIDTH),
        static_cast<float>(WINDOW_HEIGHT),
        25.0f,
        -1
    );

    entitiesManager.createScrollingBackground(
        "./assets/textures/backgrounds/bg-stars.png",
        {0, 0},
        static_cast<float>(WINDOW_WIDTH),
        static_cast<float>(WINDOW_HEIGHT),
        50.0f,
        -1
    );

    //! MAIN LOOP
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        float screenWidth = static_cast<float>(GetScreenWidth());
        float screenHeight = static_cast<float>(GetScreenHeight());

        float aspectRatio = 16.0f / 9.0f;
        float viewportWidth = screenWidth;
        float viewportHeight = viewportWidth / aspectRatio;

        if (viewportHeight > screenHeight) {
            viewportHeight = screenHeight;
            viewportWidth = viewportHeight * aspectRatio;
        }

        float viewportX = (screenWidth - viewportWidth) * 0.5f;
        float viewportY = (screenHeight - viewportHeight) * 0.5f;

        physicsSystem->setViewport(viewportWidth, viewportHeight);
        renderSystem->setViewport(viewportX, viewportY, viewportWidth, viewportHeight);

        //? LOGIC
        inputSystem->update();
        spriteFrameSystem->update();
        timerSystem->update();
        collisionSystem->update();
        backgroundScrollSystem->update(deltaTime);
        physicsSystem->update(deltaTime);

        enemiesSystem->update(deltaTime);

        //! DESTROY
        gCoordinator.processEntityDestruction();

        //? NETWORK
        std::string mes = clientNetworkSystem->update_read();
        if (mes != "") {
            std::cout << mes << std::endl;
        }

        //? RENDER
        BeginDrawing();
        ClearBackground(BLACK);

        renderSystem->update();

        EndDrawing();
    }
    // networkSystem->disconnect();
    TexturesManager.unloadAllTextures();
    CloseWindow();
    return 0;
}
