#include "../../../ECS/includes/ECS.hpp"

Coordinator gCoordinator;

void all() {
    std::cout << "All command" << std::endl;
}

int main() {
    // gCoordinator.init();
    // auto networkSystem = gCoordinator.registerSystem<ServerSystem>();

    // //? NetworkSystem
    // networkSystem->init("127.0.0.0", 5000);
    // networkSystem->addCommand("all", all, "Print all command");


    // //! MAIN LOOP
    // while (networkSystem->isRunning()) {
    //     //? LOGIC

    //     //? NETWORK
    //     networkSystem->update();
    // }
    return 0;
}
