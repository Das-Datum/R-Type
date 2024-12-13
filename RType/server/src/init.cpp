#include "server.hpp"

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
    gCoordinator.registerComponent<BlockOutOfBoundsComponent>();
    //? Shared components
    gCoordinator.registerComponent<CollisionComponent>();
    gCoordinator.registerComponent<FixedVelocityComponent>();

    //* Systems
    auto serverManageNetworkSystem = gCoordinator.registerSystem<ServerManageNetworkSystem>();
    auto collisionSystem = gCoordinator.registerSystem<CollisionSystem>();
    auto physicsSystem = gCoordinator.registerSystem<PhysicsSystem>();
    auto spawnSystem = gCoordinator.registerSystem<SpawnSystem>();

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
    gCoordinator.setSystemSignature<ServerNetworkSystem>(signature);

    //? ServerManageNetworkSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<NetworkComponent>(), true);
    gCoordinator.setSystemSignature<ServerManageNetworkSystem>(signature);

    //? SpawnSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<SpawnComponent>(), true);
    gCoordinator.setSystemSignature<SpawnSystem>(signature);

    serverManageNetworkSystem->init("127.0.0.0", 5000);
    auto& manager = ServerEntitiesManager::getInstance();
}
