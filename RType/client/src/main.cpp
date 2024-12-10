#include "Menus.hpp"
#include "client.hpp"
#include "raylib.h"
#include "rlgl.h"
#include "raylib.h"
#include "raymath.h"

Coordinator gCoordinator;

enum ColorBlindMode {
    NORMAL = 0,
    PROTANOPIA,
    DEUTERANOPIA,
    TRITANOPIA,
    ACHROMATOPSIA
};

void initMenus(std::shared_ptr<MenuManager> manager, std::shared_ptr<Settings> settings, int windowWidth, int windowHeight) {
    std::cout << "Initializing menus\n";
    settings->Load();

    auto style = std::make_shared<UIStyle>();
    style->font = GetFontDefault();
    style->fontSize = settings->getFontSize();
    style->baseColor = LIGHTGRAY;
    style->hoverColor = GRAY;
    style->textColor = WHITE;

    auto pageMain = std::make_shared<MenuPage>("MainMenu", style);

    pageMain->AddWidget(std::make_shared<LabelWidget>(
        "R-TYPE",
        2, 2,
        style,
        3.0f
    ));

    pageMain->AddWidget(std::make_shared<ButtonWidget>(
        "Play Game",
        5, 2,
        []() {
            std::cout << "Starting game...\n";
        },
        style));

    pageMain->AddWidget(std::make_shared<ButtonWidget>(
        "Settings",
        6, 2,
        [manager, windowWidth, windowHeight]() {
            manager->SetActivePage("Settings", windowWidth, windowHeight);
        },
        style));

    pageMain->AddWidget(std::make_shared<ButtonWidget>(
        "Exit",
        7, 2,
        []() {
            auto &TexturesManager = TexturesManager::getInstance();
            TexturesManager.unloadAllTextures();
            CloseWindow();
            exit(0);
        },
        style));

    auto pageSettings = std::make_shared<MenuPage>("Settings", style);

    //? SLIDER
    pageSettings->AddWidget(std::make_shared<SliderWidget>(
        "Master Volume",
        2, 1,
        0.0f, 1.0f,
        [&settings](float value) { settings->setMasterVolume(value); },
        style,
        [&settings]() -> float { return settings->getMasterVolume(); }));

    pageSettings->AddWidget(std::make_shared<SliderWidget>(
        "Music Volume",
        3, 1,
        0.0f, 1.0f,
        [&settings](float value) { settings->setMusicVolume(value); },
        style,
        [&settings]() -> float { return settings->getMusicVolume(); }));

    pageSettings->AddWidget(std::make_shared<SliderWidget>(
        "SFX Volume",
        4, 1,
        0.0f, 1.0f,
        [&settings](float value) { settings->setSfxVolume(value); },
        style,
        [&settings]() -> float { return settings->getSfxVolume(); }));

    pageSettings->AddWidget(std::make_shared<SliderWidget>(
        "Font Size",
        5, 1,
        10.0f, 40.0f,
        [settings, style](float value) {
            settings->setFontSize(value);
            style->fontSize = value;
        },
        style,
        [settings]() -> float { return settings->getFontSize(); }
    ));

    std::vector<std::pair<std::string, std::pair<std::function<int()>, std::function<void(int)>>>> keyBindings = {
        {"Shoot", {
            [&settings]() { return settings->getShootKey(); },
            [&settings](int key) { settings->setShootKey(key); }
        }},
        {"Move Up", {
            [&settings]() { return settings->getMoveUpKey(); },
            [&settings](int key) { settings->setMoveUpKey(key); }
        }},
        {"Move Down", {
            [&settings]() { return settings->getMoveDownKey(); },
            [&settings](int key) { settings->setMoveDownKey(key); }
        }},
        {"Move Left", {
            [&settings]() { return settings->getMoveLeftKey(); },
            [&settings](int key) { settings->setMoveLeftKey(key); }
        }},
        {"Move Right", {
            [&settings]() { return settings->getMoveRightKey(); },
            [&settings](int key) { settings->setMoveRightKey(key); }
        }}
    };

    for (size_t i = 0; i < keyBindings.size(); ++i) {
        pageSettings->AddWidget(std::make_shared<KeyBindingWidget>(
            keyBindings[i].first,
            i + 2, 3,
            keyBindings[i].second.first,
            keyBindings[i].second.second,
            style));
    }

    pageSettings->AddWidget(std::make_shared<ButtonWidget>(
        "Apply & Back",
        8, 2,
        [manager, settings, windowWidth, windowHeight]() {
            settings->Save();
            manager->SetActivePage("MainMenu", windowWidth, windowHeight);
        },
        style));

    manager->AddPage(pageMain);
    manager->AddPage(pageSettings);
    manager->SetActivePage("MainMenu", windowWidth, windowHeight);
}

int main() {
    std::cout << "START\n";
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RType");
    SetTargetFPS(60);

    std::shared_ptr<MenuManager> menuManager = std::make_shared<MenuManager>();
    std::shared_ptr<Settings> settings = std::make_shared<Settings>();

    initMenus(menuManager, settings, WINDOW_WIDTH, WINDOW_HEIGHT);

    const float SHIP_HEIGHT = WINDOW_HEIGHT * 0.05f;
    auto &TexturesManager = TexturesManager::getInstance();
    auto &entitiesManager = EntitiesManager::getInstance();
    entitiesManager.setWindowHeight(static_cast<float>(WINDOW_HEIGHT));

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
    clientNetworkSystem->init("127.0.0.1", 5000);

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
    entitiesManager.createEnemy(enemyPosition);
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

    bool showMenu = true;

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

        if (showMenu) {
            menuManager->HandleEvent();
            menuManager->Update(deltaTime);
            backgroundScrollSystem->update(deltaTime);

            BeginDrawing();
            ClearBackground(BLACK);

            renderSystem->update();
            menuManager->Draw();

            EndDrawing();
            continue;
        }

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
