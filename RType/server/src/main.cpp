#include "server.hpp"
#define TPS 30.0

using Clock = std::chrono::high_resolution_clock;

Coordinator gCoordinator;

void game_tick(double elapsedTimeSeconds, int tick) {
    //! VELOCITY, PHYSICS, COLLISION
    gCoordinator.getSystem<VelocitySystem>()->update(elapsedTimeSeconds);
    gCoordinator.getSystem<PhysicsSystem>()->update(elapsedTimeSeconds);
    gCoordinator.getSystem<CollisionSystem>()->update([](Entity entityA, Entity entityB) {
        // if (gCoordinator.hasComponent<ShipComponent>(entityA) && gCoordinator.hasComponent<EnemyComponent>(entityB)) {
        //     std::cout << "Ship collided with enemy\n";
        // }

        if (gCoordinator.hasComponent<EnemyComponent>(entityA) && gCoordinator.hasComponent<BulletComponent>(entityB)) {
            gCoordinator.destroyEntity(entityA);
        }
    });

    //! SPAWN
    gCoordinator.getSystem<SpawnSystem>()->update(elapsedTimeSeconds);

    //? Update position of all players
    gCoordinator.getSystem<ServerManageNetworkSystem>()->sendAllPlayersPosition();

    //! DESTROY
    gCoordinator.processEntityDestruction();
}

int main() {
    std::srand(42);

    auto &manager = ServerEntitiesManager::getInstance();
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    const float SHIP_HEIGHT = WINDOW_HEIGHT * 0.05f;
    const double interval = 1.0 / TPS;
    auto last_tick_time = Clock::now();
    int tickCount = 1;

    initCoordinator();

    //? MAIN LOOP
    while (gCoordinator.getSystem<ServerManageNetworkSystem>()->isRunning()) {
        auto current_time = Clock::now();
        std::chrono::duration<double> elapsed_time = current_time - last_tick_time;
        // std::cout << "Elapsed time: " << elapsed_time.count() << std::endl;
        gCoordinator.getSystem<ServerManageNetworkSystem>()->update();
        if (elapsed_time.count() >= interval) {
            last_tick_time = current_time;
            try {
                tickCount++;
                if (tickCount >= static_cast<int>(TPS))
                    tickCount -= static_cast<int>(TPS);
                game_tick(elapsed_time.count(), tickCount);
            } catch(std::exception& e) {
                std::cerr << "Error while executing tick: " << e.what() << std::endl;
            }
        }
    }

    return 0;
}
