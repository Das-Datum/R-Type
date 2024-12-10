#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../../../../Network/includes/NetworkComponent.hpp"
#include "../../../../Network/includes/NetworkSystems.hpp"
#include "../Components.hpp"
#include "../../shared/includes/Components/GameComponents.hpp"
#include "../EntitiesManager.hpp"
#include <map>
#include <functional>

class CoreSystem : public System {
public:
    void init(ServerSystem &serverSystem) {
        _serverSystem = &serverSystem;
    }

    void sendAllPlayer(int id, const std::string &message) {
        for (auto const &entity : entities) {
            auto &player = gCoordinator.getComponent<NetworkComponent>(entity);
            if (player.id == id)
                continue;
            _serverSystem->sendDataToPlayer(message, player.ip, player.port);
        }
    }

    void shoot(Entity player) {
        auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
        sendAllPlayer(playerNetwork.id, "SHT" + std::to_string(playerNetwork.id));
    }

    void beam(Entity player) {
        auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
        sendAllPlayer(playerNetwork.id, "DEM" + std::to_string(playerNetwork.id));
    }

    void up(Entity player) {
        auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
        sendAllPlayer(playerNetwork.id, "MUP" + std::to_string(playerNetwork.id));
    }

    void down(Entity player) {
        auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
        sendAllPlayer(playerNetwork.id, "MDW" + std::to_string(playerNetwork.id));
    }

    void right(Entity player) {
        auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
        sendAllPlayer(playerNetwork.id, "MRT" + std::to_string(playerNetwork.id));
    }

    void left(Entity player) {
        auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
        sendAllPlayer(playerNetwork.id, "MLF" + std::to_string(playerNetwork.id));
    }

    void update() {
        for (auto const &entity : entities) {
            auto &player = gCoordinator.getComponent<NetworkComponent>(entity);
            while (!player.lastMessagesReceived.empty()) {
                std::string command = getCommand(player.lastMessagesReceived.front());
                if (_commandMap.find(command) != _commandMap.end()) {
                    _commandMap[command](entity);
                }
                player.lastMessagesReceived.erase(player.lastMessagesReceived.begin());
            }
        }
    }

    std::string getCommand(std::string command) {
        _options = command.substr(3);
        return command.substr(0, 3);
    }
private:
    std::map<std::string, std::function<void(Entity)>> _commandMap = {
        {"SHT", [this](Entity entity) { shoot(entity); }},
        {"MUP", [this](Entity entity) { up(entity); }},
        {"MDW", [this](Entity entity) { down(entity); }},
        {"MRT", [this](Entity entity) { right(entity); }},
        {"MLF", [this](Entity entity) { left(entity); }}

    };
    ServerSystem *_serverSystem;
    std::string _options;
};