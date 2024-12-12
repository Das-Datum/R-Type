#include "client.hpp"

void ClientManageNetworkSystem::newPlayer(std::string msg) {
    auto &entitiesManager = EntitiesManager::getInstance();
    entitiesManager.createShip({_x, _y}, _id, _options, false);
}

void ClientManageNetworkSystem::createPlayerShip(Entity entity) {
    (void)entity;
    auto &entitiesManager = EntitiesManager::getInstance();
    entitiesManager.createShip({_x, _y}, _id, "Player");
}

void ClientManageNetworkSystem::shoot(Entity player) {
    auto &transform = gCoordinator.getComponent<TransformComponent>(player);
    auto &entitiesManager = EntitiesManager::getInstance();

    Vector2 bulletPosition = {transform.position.x + transform.size.x / 2,
        transform.position.y};
        entitiesManager.createBullet(bulletPosition, {0.5f, 0.0f});
}

void ClientManageNetworkSystem::beam(Entity player) {
    auto &transform = gCoordinator.getComponent<TransformComponent>(player);
    auto &entitiesManager = EntitiesManager::getInstance();

    Vector2 bulletPosition = {transform.position.x + transform.size.x / 2,
        transform.position.y};
        entitiesManager.createBullet(bulletPosition, {0.9f, 0.0f});
}

void ClientManageNetworkSystem::up(Entity player) {
    float speed = (1000.0f / 1920.0f) * GetFrameTime();
    auto &playerNetwork = gCoordinator.getComponent<TransformComponent>(player);
    playerNetwork.position.y -= speed * (16.0 / 9.0);
}

void ClientManageNetworkSystem::down(Entity player) {
    float speed = (1000.0f / 1920.0f) * GetFrameTime();
    auto &playerNetwork = gCoordinator.getComponent<TransformComponent>(player);
    playerNetwork.position.y += speed * (16.0 / 9.0);
}

void ClientManageNetworkSystem::right(Entity player) {
    float speed = (1000.0f / 1920.0f) * GetFrameTime();
    auto &playerNetwork = gCoordinator.getComponent<TransformComponent>(player);
    playerNetwork.position.x += speed;
}

void ClientManageNetworkSystem::left(Entity player) {
    float speed = (1000.0f / 1920.0f) * GetFrameTime();
    auto &playerNetwork = gCoordinator.getComponent<TransformComponent>(player);
    playerNetwork.position.x -= speed;
}

void ClientManageNetworkSystem::disconnectPlayer(Entity entity) {
    gCoordinator.destroyEntity(entity);
}

void ClientManageNetworkSystem::update() {
    std::vector<std::string> messages = getLastMessages();
    for (auto const &msg : messages) {
        std::string command = getCommand(msg);
        if (_protocolMap.find(command) != _protocolMap.end()) {
            _protocolMap[command](getEntityById(_id));
        }
    }
}

Entity ClientManageNetworkSystem::getEntityById(int id) {
    for (auto const &entity : entities) {
        auto &player = gCoordinator.getComponent<NetworkComponent>(entity);
        if (player.id == id)
            return entity;
    }
    return 0;
}

std::string ClientManageNetworkSystem::getCommand(std::string command) {
    std::cout << command << std::endl;
    if (command.size() < 3)
        return "";
    if (command.size() == 3)
        return command;
    _id = std::stoi(command.substr(3, 1));
    if (command.size() == 4)
        return command.substr(0, 3);
    int pos = getPos(command.substr(4));
    if (command.size() == pos + 5)
        return command.substr(0, 3);
    _options = command.substr(pos + 5);
    return command.substr(0, 3);
}

int ClientManageNetworkSystem::getPos(std::string text) {
    size_t commaPos = text.find(',');
    size_t commaPosEnd = text.find(';');
    if (commaPos == std::string::npos || commaPosEnd == std::string::npos)
        return 0;
    try {
        _x = std::stof(text.substr(0, commaPos));
        _y = std::stof(text.substr(commaPos + 1, commaPosEnd));
        return commaPosEnd;
    } catch (const std::exception &e) {
        _x = 0;
        _y = 0;
        return 0;
    }
}