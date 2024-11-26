#include "../../../ECS/includes/ECS.hpp"
#include "../includes/Components.hpp"
#include "../includes/TexturesManager.hpp"

#include "../includes/Systems/ClientRelatives.hpp"
#include "../includes/Systems/SpriteRelatives.hpp"
#include "../includes/Systems/Generics.hpp"

#include "../../shared/includes/Components/GameComponents.hpp"
#include "../../shared/includes/Systems/Game.hpp"

Coordinator gCoordinator;

int main() {
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    const float SHIP_HEIGHT = WINDOW_HEIGHT * 0.05f;
    auto& TexturesManager = TexturesManager::getInstance();

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RType");
    SetTargetFPS(60);

    gCoordinator.init();
    gCoordinator.registerComponent<TransformComponent>();
    gCoordinator.registerComponent<ShipComponent>();
    gCoordinator.registerComponent<BulletComponent>();
    gCoordinator.registerComponent<InputComponent>();
    gCoordinator.registerComponent<TimerComponent>();
    gCoordinator.registerComponent<SpriteComponent>();
    gCoordinator.registerComponent<SpriteAnimationComponent>();
    gCoordinator.registerComponent<SpriteFrameComponent>();

    //? Shared components
    gCoordinator.registerComponent<PhysicsComponent>();

    //* Systems
    auto renderSystem = gCoordinator.registerSystem<RenderSystem>();
    auto inputSystem = gCoordinator.registerSystem<InputSystem>();
    auto physicsSystem = gCoordinator.registerSystem<PhysicsSystem>();
    auto timerSystem = gCoordinator.registerSystem<TimerSystem>();
    auto spriteFrameSystem = gCoordinator.registerSystem<SpriteFrameSystem>();
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

    //? entity
    Entity ship = gCoordinator.createEntity();
    gCoordinator.addComponent(ship, ShipComponent());
    gCoordinator.addComponent(ship, InputComponent());
    gCoordinator.addComponent(ship, TimerComponent());

    std::string shipTexturePath = "../assets/textures/ships/ship_1.png";
    Texture2D shipTexture = TexturesManager.loadTexture(shipTexturePath);
    Rectangle initialFrame = TexturesManager.getFrame(shipTexturePath, 2, 5);

    gCoordinator.addComponent(ship, SpriteComponent(
        shipTexture,
        initialFrame
    ));

    float frameWidth = (float)(shipTexture.width / 5);
    float frameAspectRatio = frameWidth / shipTexture.height;
    float SHIP_WIDTH = SHIP_HEIGHT * frameAspectRatio;

    gCoordinator.addComponent(ship, TransformComponent(
        Vector2{WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT * 0.8f},
        0.0f,
        Vector2{1.0f, 1.0f},
        Vector2{SHIP_WIDTH, SHIP_HEIGHT}
    ));
    gCoordinator.addComponent(ship, SpriteFrameComponent(1, 5));

    //! MAIN LOOP
    while (!WindowShouldClose()) {
        //? LOGIC
        inputSystem->update();
        spriteFrameSystem->update();
        timerSystem->update();
        physicsSystem->update(GetFrameTime());

        //? RENDER
        BeginDrawing();
        ClearBackground(BLACK);
        renderSystem->update();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
