#include "../../../ECS/includes/ECS.hpp"
#include "../includes/Components.hpp"
#include "../includes/EntitiesManager.hpp"
#include "../includes/Systems/CollisionSystem.hpp"
#include "../../shared/includes/Systems/Game.hpp"
#include "../includes/Systems/ServerNetworkSystem.hpp"

Coordinator gCoordinator;

void all() {
    std::cout << "All command" << std::endl;
}

int main() {
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    const float SHIP_HEIGHT = WINDOW_HEIGHT * 0.05f;
    auto& entitiesManager = EntitiesManager::getInstance();
    entitiesManager.setWindowHeight(static_cast<float>(WINDOW_HEIGHT));

    gCoordinator.init();

    gCoordinator.registerComponent<PlayerNetworkComponents>();
    gCoordinator.registerComponent<ShipComponent>();
    gCoordinator.registerComponent<BulletComponent>();
    gCoordinator.registerComponent<EnemyComponent>();
    gCoordinator.registerComponent<TransformComponent>();

    //? Shared components
    gCoordinator.registerComponent<PhysicsComponent>();
    gCoordinator.registerComponent<CollisionComponent>();

    //* Systems
    auto networkSystem = gCoordinator.registerSystem<ServerNetworkSystem>();
    auto collisionSystem = gCoordinator.registerSystem<CollisionSystem>();
    auto physicsSystem = gCoordinator.registerSystem<PhysicsSystem>();

    Signature signature;

    //? CollisionSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<TransformComponent>(), true);
    signature.set(gCoordinator.getComponentTypeID<CollisionComponent>(), true);
    gCoordinator.setSystemSignature<CollisionSystem>(signature);

    //? PhysicsSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<TransformComponent>(), true);
    signature.set(gCoordinator.getComponentTypeID<BulletComponent>(), true);
    gCoordinator.setSystemSignature<PhysicsSystem>(signature);

    //? NetworkSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<PlayerNetworkComponents>(), true);
    gCoordinator.setSystemSignature<ServerNetworkSystem>(signature);

    networkSystem->init("127.0.0.0", 5000);


    //! MAIN LOOP
    while (networkSystem->isRunning()) {
        //? LOGIC

        //? NETWORK
        networkSystem->update();
    }
    return 0;
}
