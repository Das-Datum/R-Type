#include "Server.hpp"

void initCoordinator() {
    gCoordinator.init();

    gCoordinator.registerComponent<NetworkComponent>();
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
    auto coreSystem = gCoordinator.registerSystem<CoreSystem>();
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
    signature.set(gCoordinator.getComponentTypeID<NetworkComponent>(), true);
    gCoordinator.setSystemSignature<ServerSystem>(signature);

    //? CoreSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<NetworkComponent>(), true);
    gCoordinator.setSystemSignature<CoreSystem>(signature);

    networkSystem->init("127.0.0.0", 5000);
    coreSystem->init(*networkSystem);
    auto& manager = ServerEntitiesManager::getInstance();
}
