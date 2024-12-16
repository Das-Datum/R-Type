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
    auto &entitiesManager = EntitiesManager::getInstance();

    Vector2 bulletPosition = {_x, _y};
    entitiesManager.createBullet(bulletPosition, {0.5f, 0.0f});
}

void ClientManageNetworkSystem::beam(Entity player) {
    auto &transform = gCoordinator.getComponent<TransformComponent>(player);
    auto &entitiesManager = EntitiesManager::getInstance();

    Vector2 bulletPosition = {_x, _y};
    entitiesManager.createBullet(bulletPosition, {0.9f, 0.0f});
}

void ClientManageNetworkSystem::up(Entity player) {
    auto &velocity = gCoordinator.getComponent<VelocityComponent>(player);
    velocity.velocity.y -= 1.0f * velocity.acceleration;
}

void ClientManageNetworkSystem::down(Entity player) {
    auto &velocity = gCoordinator.getComponent<VelocityComponent>(player);
    velocity.velocity.y += 1.0f * velocity.acceleration;
}

void ClientManageNetworkSystem::right(Entity player) {
    auto &velocity = gCoordinator.getComponent<VelocityComponent>(player);
    velocity.velocity.x += 0.7f * velocity.acceleration;
}

void ClientManageNetworkSystem::left(Entity player) {
    auto &velocity = gCoordinator.getComponent<VelocityComponent>(player);
    velocity.velocity.x -= 0.7f * velocity.acceleration;
}

void ClientManageNetworkSystem::disconnectPlayer(Entity entity) {
    gCoordinator.destroyEntity(entity);
}

void ClientManageNetworkSystem::loadStage(Entity entity) {
    std::cout << "Loading stage remotly!" << std::endl;
    auto &stageLoader = StageLoader::getInstance();
    try {
        stageLoader.loadConfig(_options);
        stageLoader.genWaves();
        stageLoader.genMobsEntities(EntitiesManager::getInstance());
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void ClientManageNetworkSystem::startGame(Entity entity) {
    (void)entity;
    std::cout << "Game started remotly!" << std::endl;
    auto &menuManager = MenuManager::getInstance();
    menuManager.closeCurrentPage();
}

void ClientManageNetworkSystem::pauseGame() {
    std::cout << "Game paused remotly!" << std::endl;
    auto &menuManager = MenuManager::getInstance();
    menuManager.setActivePage("PauseMenu", GetScreenWidth(), GetScreenHeight());
}

void ClientManageNetworkSystem::resumeGame() {
    std::cout << "Game resumed remotly!" << std::endl;
    auto &menuManager = MenuManager::getInstance();
    menuManager.closeCurrentPage();
}

void ClientManageNetworkSystem::setPos(Entity entity) {
    if (gCoordinator.hasComponent<NetworkPositionComponent>(entity)) {
        auto &networkPos = gCoordinator.getComponent<NetworkPositionComponent>(entity);
        auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
        if (gCoordinator.hasComponent<InputComponent>(entity)) {
            networkPos.lastPosition = transform.position;
        } else {
            networkPos.lastPosition = networkPos.targetPosition;
        }
        networkPos.targetPosition = {_x, _y};
        networkPos.lerpFactor = 0.0f;
    }
}

void ClientManageNetworkSystem::update() {
    std::vector<std::string> messages = getLastMessages();
    for (const auto &msg : messages) {
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
    // std::cout << command << std::endl;
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
    if (pos == 0)
        _options = command.substr(pos + 4);
    else
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

void ClientManageNetworkSystem::syncEnemy() {
    try {
        int id = std::stoi(_options);
        auto &entitiesManager = EntitiesManager::getInstance();
        Entity entity = gCoordinator.getSystem<EnemiesSystem>()->getEnemyByUniqueId(id);

        auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
        transform.position = {_x, _y};
        std::cout << "EnemyID: " << id << " Syncing at " << _x << ", " << _y << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
