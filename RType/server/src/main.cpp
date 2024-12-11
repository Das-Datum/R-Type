#include "server.hpp"
#define TPS 30.0

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Milliseconds = std::chrono::duration<double, std::milli>;

Coordinator gCoordinator;

void game_tick() {
    gCoordinator.getSystem<ServerSystem>()->update();
    gCoordinator.getSystem<CoreSystem>()->update();
}

int main() {
    std::srand(42);

    auto &manager = ServerEntitiesManager::getInstance();
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    const float SHIP_HEIGHT = WINDOW_HEIGHT * 0.05f;
    double intervalle = 1000.0 / TPS;
    TimePoint last_tick_time = Clock::now();

    initCoordinator();

    try {
        const std::string path = "../shared/stages/stage1.json";
        StageLoader loader(path);
        loader.loadConfig();
        loader.genWaves();
        loader.genMobsEntities(manager);
    } catch(std::exception& e) {
        std::cerr << "Error while loading the stage: " << e.what() << std::endl;
    }

    //? MAIN LOOP
    while (gCoordinator.getSystem<ServerSystem>()->isRunning()) {
        TimePoint current_time = Clock::now();
        Milliseconds elapsed_time = current_time - last_tick_time;

        if (elapsed_time.count() >= intervalle) {
            last_tick_time += std::chrono::milliseconds(static_cast<int>(intervalle));
            try {
                game_tick();
            } catch(std::exception& e) {
                std::cerr << "Error while executing tick: " << e.what() << std::endl;
            }
        }
    }

    return 0;
}


