#include "client.hpp"
#include "Systems/InterpolationSystem.hpp"

Coordinator gCoordinator;

int main() {
    std::cout << "START\n";

    std::srand(42);
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RType");
    SetTargetFPS(60);

    auto &menuManager = MenuManager::getInstance();

    initMenus(WINDOW_WIDTH, WINDOW_HEIGHT);

    const float SHIP_HEIGHT = WINDOW_HEIGHT * 0.05f;
    auto &texturesManager = TexturesManager::getInstance();
    auto &entitiesManager = EntitiesManager::getInstance();
    auto &shadersManager = ShadersManager::getInstance();
    auto &settings = Settings::getInstance();

    initCoordinator();

    //? User 1 (main player)
    // Vector2 shipPosition = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT * 0.8f};
    // entitiesManager.createShip({0, 0}, 1, "Player");

    //* Test entity
    // Vector2 enemyPosition = {0.9f, 0.5f};
    // Enemy enemyInfos = Enemy(enemyPosition.x, enemyPosition.y, 0.0, EnemyType("ship", Vector2{145.0, 29.0}, 5, 100, true, true));
    // entitiesManager.createEnemy(enemyInfos);
    // entitiesManager.removeEntity(enemy);

    //* Backgrounds
    entitiesManager.createScrollingBackground(
        "./assets/textures/backgrounds/bg-back.png",
        {0, 0},
        static_cast<float>(WINDOW_WIDTH),
        static_cast<float>(WINDOW_HEIGHT),
        25.0f,
        -1);

    entitiesManager.createScrollingBackground(
        "./assets/textures/backgrounds/bg-stars.png",
        {0, 0},
        static_cast<float>(WINDOW_WIDTH),
        static_cast<float>(WINDOW_HEIGHT),
        50.0f,
        -1);
    //! END OF TEMPORARY CODE

    //! MAIN LOOP
    while (!(WindowShouldClose() && !IsKeyPressed(KEY_ESCAPE))) {
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

        if (IsKeyPressed(KEY_ESCAPE) && !menuManager.isPageActive()) {
            menuManager.setActivePage("PauseMenu", WINDOW_WIDTH, WINDOW_HEIGHT);
        }

        gCoordinator.getSystem<RenderSystem>()->setViewport(viewportX, viewportY, viewportWidth, viewportHeight);

        //? NETWORK
        gCoordinator.getSystem<ClientManageNetworkSystem>()->update();
        gCoordinator.getSystem<NetworkInstructionsSystem>()->update();

        if (menuManager.isPageActive()) {
            menuManager.handleEvent();
            menuManager.update(deltaTime);
            gCoordinator.getSystem<BackgroundScrollSystem>()->update(deltaTime);

            BeginDrawing();
            ClearBackground(BLACK);

            Shader shader = shadersManager.getShaderForMode(settings.getColorBlindMode());

            if (settings.getColorBlindMode() != NORMAL) {
                BeginShaderMode(shader);
            }

            gCoordinator.getSystem<RenderSystem>()->update();
            menuManager.draw();

            if (settings.getColorBlindMode() != NORMAL) {
                EndShaderMode();
            }

            EndDrawing();
            continue;
        }

        //? VelocitySystem
        gCoordinator.getSystem<VelocitySystem>()->update(deltaTime);

        //? LOGIC
        gCoordinator.getSystem<InputSystem>()->update();
        gCoordinator.getSystem<SpriteFrameSystem>()->update();
        gCoordinator.getSystem<TimerSystem>()->update();
        gCoordinator.getSystem<CollisionSystem>()->update([](Entity entityA, Entity entityB) {
            // if (gCoordinator.hasComponent<ShipComponent>(entityA) && gCoordinator.hasComponent<EnemyComponent>(entityB)) {
            //     std::cout << "Ship collided with enemy\n";
            // }

            if (gCoordinator.hasComponent<EnemyComponent>(entityA) && gCoordinator.hasComponent<BulletComponent>(entityB)) {
                gCoordinator.destroyEntity(entityA);
            }
        });

        gCoordinator.getSystem<BackgroundScrollSystem>()->update(deltaTime);
        gCoordinator.getSystem<PhysicsSystem>()->update(deltaTime);
        gCoordinator.getSystem<EnemiesSystem>()->update(deltaTime);
        gCoordinator.getSystem<InterpolationSystem>()->update(deltaTime);

        //! DESTROY
        gCoordinator.processEntityDestruction();

        //? RENDER
        BeginDrawing();
        ClearBackground(BLACK);
        Shader shader = shadersManager.getShaderForMode(settings.getColorBlindMode());
        if (settings.getColorBlindMode() != NORMAL) {
            BeginShaderMode(shader);
        }

        gCoordinator.getSystem<RenderSystem>()->update();

        if (IsKeyDown(KEY_G))
            gCoordinator.getSystem<CollisionSystem>()->drawGrid(viewportX, viewportY, viewportWidth, viewportHeight);

        if (settings.getColorBlindMode() != NORMAL) {
            EndShaderMode();
        }
        EndDrawing();

        //? SPAWN
        gCoordinator.getSystem<SpawnSystem>()->update(deltaTime);
    }

    gCoordinator.getSystem<ClientManageNetworkSystem>()->disconnect();
    texturesManager.unloadAllTextures();
    shadersManager.unloadAllShaders();
    CloseWindow();
    return 0;
}
