#include "Systems/ServerMangeNetworkSystem.hpp"

void ServerMangeNetworkSystem::shoot(Entity player) {
    auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
    sendAllPlayer(playerNetwork.id, "SHT" + std::to_string(playerNetwork.id));
}

void ServerMangeNetworkSystem::beam(Entity player) {
    auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
    sendAllPlayer(playerNetwork.id, "DEM" + std::to_string(playerNetwork.id));
}

void ServerMangeNetworkSystem::up(Entity player) {
    float speed = (1000.0f / 1920.0f) * _elapsed_time.count();
    auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
    auto &pos = gCoordinator.getComponent<TransformComponent>(player);
    pos.position.y -= speed * (16.0 / 9.0);
    sendAllPlayer(playerNetwork.id, "MUP" + std::to_string(playerNetwork.id));
}

void ServerMangeNetworkSystem::down(Entity player) {
    float speed = (1000.0f / 1920.0f) * _elapsed_time.count();
    auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
    auto &pos = gCoordinator.getComponent<TransformComponent>(player);
    pos.position.y += speed * (16.0 / 9.0);
    sendAllPlayer(playerNetwork.id, "MDW" + std::to_string(playerNetwork.id));
}

void ServerMangeNetworkSystem::right(Entity player) {
    float speed = (1000.0f / 1920.0f) * _elapsed_time.count();
    auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
    auto &pos = gCoordinator.getComponent<TransformComponent>(player);
    pos.position.x += speed;
    sendAllPlayer(playerNetwork.id, "MRT" + std::to_string(playerNetwork.id));
}

void ServerMangeNetworkSystem::left(Entity player) {
    float speed = (1000.0f / 1920.0f) * _elapsed_time.count();
    auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
    auto &pos = gCoordinator.getComponent<TransformComponent>(player);
    sendAllPlayer(playerNetwork.id, "MLF" + std::to_string(playerNetwork.id));
}

void ServerMangeNetworkSystem::disconnectClient(Entity entity) {
    auto &player = gCoordinator.getComponent<NetworkComponent>(entity);
    sendAllPlayer(player.id, "DEL" + std::to_string(player.id));
    info("Client disconnected: " + player.id);
    gCoordinator.destroyEntity(entity);
}

void ServerMangeNetworkSystem::update(Milliseconds elapsed_time) {
    _elapsed_time = elapsed_time;
    for (auto const &entity : entities) {
        auto &player = gCoordinator.getComponent<NetworkComponent>(entity);
        while (!player.lastMessagesReceived.empty()) {
            std::string command = getCommand(player.lastMessagesReceived.front());
            if (_protocolMap.find(command) != _protocolMap.end()) {
                _protocolMap[command](entity);
            }
            player.lastMessagesReceived.erase(player.lastMessagesReceived.begin());
        }
    }
}

std::string ServerMangeNetworkSystem::getCommand(std::string command) {
    if (command.size() < 3)
        return "";
    if (command.size() == 3)
        return command;
    _options = command.substr(3);
    return command.substr(0, 3);
}

void ServerMangeNetworkSystem::createNewClient(std::string name, int id, std::string ip, int port) {
    if (name == "") {
        error("Invalid client name");
        sendTo(getServerFd(), "ERRInvalid client name", ip, port);
        return;
    }
    float x = 0.20;
    float y = 0.2 * id;
    std::string stringX = std::to_string(x).substr(0, 3);
    std::string stringY = std::to_string(y).substr(0, 3);
    sendAllPlayer(id, "NEW" + std::to_string(id) + stringX + "," + stringY + ";" + name);
    std::vector<Entity> entityList(entities.begin(), entities.end());
    auto &entitiesManager = ServerEntitiesManager::getInstance();
    entitiesManager.createShip({x, y}, id, name, ip, port);
    sendDataToPlayer("BGN" + std::to_string(id) + stringX + "," + stringY + ";", ip, port);
    for (size_t i = 0; i < entityList.size(); ++i) {
        auto& player = gCoordinator.getComponent<NetworkComponent>(entityList[i]);
        auto& pos = gCoordinator.getComponent<TransformComponent>(entityList[i]);
        sendDataToPlayer("NEW" + std::to_string(player.id) + std::to_string(pos.position.x) + std::string(",") + std::to_string(pos.position.y) + ";" + player.name, ip, port);
    }
}