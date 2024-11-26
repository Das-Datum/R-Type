#include "../../../ECS/includes/ECS.hpp"

Coordinator gCoordinator;

int main() {
    gCoordinator.init();
    auto networkSystem = gCoordinator.registerSystem<ServerSystem>();

    //? NetworkSystem
    networkSystem->init("127.0.0.0", 5000);


    //! MAIN LOOP
    while (networkSystem->isRunning()) {
        //? LOGIC

        //? NETWORK
        networkSystem->update();
    }
    return 0;
}
