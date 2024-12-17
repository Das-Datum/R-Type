#include "Systems/ServerManageNetworkSystem.hpp"
#include "StageLoader.hpp"

void ServerManageNetworkSystem::shoot(Entity player) {
    auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
    sendAllPlayer(playerNetwork.id, "SHT" + std::to_string(playerNetwork.id) + std::to_string(_x) + "," + std::to_string(_y) + ";");

    auto &entitiesManager = ServerEntitiesManager::getInstance();

    Vector2 bulletPosition = {_x, _y};
    entitiesManager.createBullet(bulletPosition, {0.9f, 0.0f});
}

void ServerManageNetworkSystem::beam(Entity player) {
    auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
    sendAllPlayer(playerNetwork.id, "DEM" + std::to_string(playerNetwork.id) + std::to_string(_x) + "," + std::to_string(_y) + ";");

    auto &entitiesManager = ServerEntitiesManager::getInstance();

    Vector2 bulletPosition = {_x, _y};
    entitiesManager.createBullet(bulletPosition, {0.4f, 0.0f});
}

void ServerManageNetworkSystem::up(Entity player) {
    auto &velocity = gCoordinator.getComponent<VelocityComponent>(player);
    velocity.velocity.y -= 1.0f * velocity.acceleration;
}

void ServerManageNetworkSystem::down(Entity player) {
    auto &velocity = gCoordinator.getComponent<VelocityComponent>(player);
    velocity.velocity.y += 1.0f * velocity.acceleration;
}

void ServerManageNetworkSystem::right(Entity player) {
    auto &velocity = gCoordinator.getComponent<VelocityComponent>(player);
    velocity.velocity.x += 0.7f * velocity.acceleration;
}

void ServerManageNetworkSystem::left(Entity player) {
    auto &velocity = gCoordinator.getComponent<VelocityComponent>(player);
    velocity.velocity.x -= 0.7f * velocity.acceleration;
}

void ServerManageNetworkSystem::disconnectClient(Entity entity) {
    auto &player = gCoordinator.getComponent<NetworkComponent>(entity);
    sendAllPlayer(player.id, "DEL" + std::to_string(player.id));
    info("Client disconnected: " + player.id);
    gCoordinator.destroyEntity(entity);
}

void ServerManageNetworkSystem::startGame(Entity player) {
    auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
    std::string nowTimestamp = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    info("Game started by the player " + playerNetwork.id);

    auto &stageLoader = StageLoader::getInstance();
    try {
        stageLoader.loadConfig("stages/stage1.json");
        stageLoader.genWaves();
        stageLoader.genMobsEntities(ServerEntitiesManager::getInstance());
    } catch (const std::exception &e) {
        error(e.what());
    }

    sendAllPlayer(0, "STA" + std::to_string(playerNetwork.id) + nowTimestamp);
    sendAllPlayer(0, "LOD0stages/stage1.json");

    _gameStarted = true;
    info("Game started");
}

void ServerManageNetworkSystem::pauseGame() {
    info("Game paused");
    _gamePaused = true;

    sendAllPlayer(0, "PAU");
}

void ServerManageNetworkSystem::resumeGame() {
    info("Game resumed");
    _gamePaused = false;

    sendAllPlayer(0, "RES");
}

void ServerManageNetworkSystem::update() {
    // info("update");
    // std::unique_lock<std::mutex> lock(clientsMutex);
    for (auto const &entity : entities) {
        auto &player = gCoordinator.getComponent<NetworkComponent>(entity);
        int playerSize = player.lastMessagesReceived.size();
        if (playerSize != 0)
            debug("Size " + std::to_string(playerSize));
        while (!player.lastMessagesReceived.empty()) {
            std::string command = getCommand(player.lastMessagesReceived.front());
            if (_protocolMap.find(command) != _protocolMap.end()) {
                _protocolMap[command](entity);
            }
            player.lastMessagesReceived.erase(player.lastMessagesReceived.begin());
        }
    }
}

std::string ServerManageNetworkSystem::getCommand(std::string command) {
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

int ServerManageNetworkSystem::getPos(std::string text) {
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

void ServerManageNetworkSystem::createNewClient(std::string name, int id, std::string ip, int port) {
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

    //! if the game is already started, send STA and LOD
    if (_gameStarted) {
        std::string nowTimestamp = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        sendDataToPlayer("STA" + std::to_string(id) + nowTimestamp, ip, port);
        sendDataToPlayer("LOD0stages/stage1.json", ip, port);
    }
}

void ServerManageNetworkSystem::sendAllPlayersPosition() {
    for (auto const &entity : entities) {
        auto &player = gCoordinator.getComponent<NetworkComponent>(entity);
        auto &pos = gCoordinator.getComponent<TransformComponent>(entity);
        sendAllPlayer(0, "POS" + std::to_string(player.id) + std::to_string(pos.position.x) + std::string(",") + std::to_string(pos.position.y) + ";");
    }
}
