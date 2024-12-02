#include "../../../ECS/includes/ECS.hpp"
#include "../includes/Components.hpp"
#include "../includes/Systems.hpp"

Coordinator gCoordinator;

int main() {
    InitWindow(800, 600, "RType");
    SetTargetFPS(60);

    gCoordinator.init();
    gCoordinator.registerComponent<TransformComponent>();
    gCoordinator.registerComponent<ShipComponent>();
    gCoordinator.registerComponent<BulletComponent>();
    gCoordinator.registerComponent<InputComponent>();

    //* Systems
    auto renderSystem = gCoordinator.registerSystem<RenderSystem>();
    auto inputSystem = gCoordinator.registerSystem<InputSystem>();
    auto physicsSystem = gCoordinator.registerSystem<PhysicsSystem>();
    auto networkSystem = gCoordinator.registerSystem<ClientSystem>();
    Signature signature;

    //? NetworkSystem
    networkSystem->init("127.0.0.0", 5000);

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

    //? entity
    Entity ship = gCoordinator.createEntity();
    gCoordinator.addComponent(ship, TransformComponent({400, 500}));
    gCoordinator.addComponent(ship, ShipComponent());
    gCoordinator.addComponent(ship, InputComponent());

    //! MAIN LOOP
    while (!WindowShouldClose()) {
        //? LOGIC
        inputSystem->update();
        physicsSystem->update();

        //? NETWORK
        std::string response = networkSystem->update();
        if (response != "") {
            networkSystem->sendData("Ok");
        }

        //? RENDER
        BeginDrawing();
        ClearBackground(BLACK);
        renderSystem->update();
        EndDrawing();
    }
    networkSystem->disconnect();
    CloseWindow();
    return 0;
}
