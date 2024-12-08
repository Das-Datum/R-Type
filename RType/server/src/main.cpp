#include "../includes/server.hpp"

Coordinator gCoordinator;


int main() {
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    const float SHIP_HEIGHT = WINDOW_HEIGHT * 0.05f;

    gCoordinator.init();

    // gCoordinator.registerComponent<NetworkComponents>();
    gCoordinator.registerComponent<ShipComponent>();
    gCoordinator.registerComponent<BulletComponent>();
    gCoordinator.registerComponent<EnemyComponent>();
    gCoordinator.registerComponent<TransformComponent>();

    //? Shared components
    gCoordinator.registerComponent<CollisionComponent>();

    //* Systems
    auto networkSystem = gCoordinator.registerSystem<ServerSystem>();
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
    signature.set(gCoordinator.getComponentTypeID<NetworkComponents>(), true);
    gCoordinator.setSystemSignature<ServerSystem>(signature);

    networkSystem->init("127.0.0.0", 5000);



     //! MAIN LOOP
     while (networkSystem->isRunning()) {
         //? LOGIC

         //? NETWORK
         networkSystem->update();
    }
    return 0;
}


