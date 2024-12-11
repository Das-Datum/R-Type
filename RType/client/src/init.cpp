#include "client.hpp"

void initCoordinator() {
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

    gCoordinator.registerComponent<NetworkComponent>();
    gCoordinator.registerComponent<NetworkInstructionsComponent>();

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

    auto clientManageNetworkSystem = gCoordinator.registerSystem<ClientManageNetworkSystem>();
    auto networkInstructionsSystem = gCoordinator.registerSystem<NetworkInstructionsSystem>();


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

    //? NetworkClientNetworkSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<NetworkComponent>(), true);
    gCoordinator.setSystemSignature<ClientManageNetworkSystem>(signature);

    //? NetworkInstructionsSystem
    signature.reset();
    signature.set(gCoordinator.getComponentTypeID<NetworkInstructionsComponent>(), true);
    gCoordinator.setSystemSignature<NetworkInstructionsSystem>(signature);


}
