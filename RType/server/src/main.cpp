#include "server.hpp"
#define TPS 60.0

using Clock = std::chrono::high_resolution_clock;

Coordinator gCoordinator;

void game_tick(double elapsedTimeSeconds) {
    gCoordinator.getSystem<ServerManageNetworkSystem>()->update(elapsedTimeSeconds);

    //! DESTROY
    gCoordinator.processEntityDestruction();
    gCoordinator.getSystem<PhysicsSystem>()->update(elapsedTimeSeconds);
    gCoordinator.getSystem<CollisionSystem>()->update();

    //! SPAWN
    gCoordinator.getSystem<SpawnSystem>()->update(elapsedTimeSeconds);
}

int main() {
    std::srand(42);

    auto &manager = ServerEntitiesManager::getInstance();
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    const float SHIP_HEIGHT = WINDOW_HEIGHT * 0.05f;
    const double interval = 1.0 / TPS;
    auto last_tick_time = Clock::now();

    initCoordinator();

    // try {
    //     const std::string path = "stages/stage1.json";
    //     StageLoader loader = StageLoader::getInstance();
    //     loader.loadConfig(path);
    //     loader.genWaves();
    //     loader.genMobsEntities(manager);
    // } catch(std::exception& e) {
    //     std::cerr << "Error while loading the stage: " << e.what() << std::endl;
    // }

    //? MAIN LOOP
    while (gCoordinator.getSystem<ServerManageNetworkSystem>()->isRunning()) {
        auto current_time = Clock::now();
        std::chrono::duration<double> elapsed_time = current_time - last_tick_time;

        if (elapsed_time.count() >= interval) {
            last_tick_time = current_time;
            try {
                game_tick(elapsed_time.count());
            } catch(std::exception& e) {
                std::cerr << "Error while executing tick: " << e.what() << std::endl;
            }
        }
    }

    return 0;
}
