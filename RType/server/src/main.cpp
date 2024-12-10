#include "../includes/server.hpp"
#include "../includes/StageLoader.hpp"

Coordinator gCoordinator;


int main() {
    std::srand(42);

    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    const float SHIP_HEIGHT = WINDOW_HEIGHT * 0.05f;

    gCoordinator.init();

    // gCoordinator.registerComponent<PlayerNetworkComponents>();
    gCoordinator.registerComponent<ShipComponent>();
    gCoordinator.registerComponent<BulletComponent>();
    gCoordinator.registerComponent<TransformComponent>();
    gCoordinator.registerComponent<SpawnComponent>();
    gCoordinator.registerComponent<DestroyOutOfBoundsComponent>();
    gCoordinator.registerComponent<EnemyComponent>();
    gCoordinator.registerComponent<EnemyHealthComponent>();
    gCoordinator.registerComponent<EnemyShootComponent>();
    gCoordinator.registerComponent<EnemyMovementComponent>();
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
    signature.set(gCoordinator.getComponentTypeID<PlayerNetworkComponents>(), true);
    gCoordinator.setSystemSignature<ServerSystem>(signature);

    networkSystem->init("127.0.0.0", 5000);

    auto& manager = EntitiesManager::getInstance();

    try {
        StageLoader loader("./RType/shared/stages/stage1.json");
        loader.loadConfig();
        loader.genWaves();
        loader.genMobsEntities(manager);
    } catch(std::exception& e) {
        std::cerr << "Error while loading the stage: " << e.what() << std::endl;
    }

     //! MAIN LOOP
     while (networkSystem->isRunning()) {
         //? LOGIC

         //? NETWORK
         networkSystem->update();
    }
    return 0;
}


