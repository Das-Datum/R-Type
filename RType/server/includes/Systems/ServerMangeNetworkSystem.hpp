#pragma once

#include "ECS.hpp"
#include "NetworkComponent.hpp"
#include "NetworkSystems.hpp"
#include "Components/GameComponents.hpp"
#include "ServerEntitiesManager.hpp"
#include <map>
#include <functional>

using Milliseconds = std::chrono::duration<double, std::milli>;

class ServerMangeNetworkSystem : public ServerNetworkSystem {
public:

    void sendAllPlayer(int id, const std::string &message) {
        for (auto const &entity : entities) {
            auto &player = gCoordinator.getComponent<NetworkComponent>(entity);
            if (player.id == id)
                continue;
            sendDataToPlayer(message, player.ip, player.port);
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
        float speed = (1000.0f / 1920.0f) * _elapsed_time.count();
        auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
        auto &pos = gCoordinator.getComponent<TransformComponent>(player);
        pos.position.y -= speed * (16.0 / 9.0);
        sendAllPlayer(playerNetwork.id, "MUP" + std::to_string(playerNetwork.id));
    }

    void down(Entity player) {
        float speed = (1000.0f / 1920.0f) * _elapsed_time.count();
        auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
        auto &pos = gCoordinator.getComponent<TransformComponent>(player);
        pos.position.y += speed * (16.0 / 9.0);
        sendAllPlayer(playerNetwork.id, "MDW" + std::to_string(playerNetwork.id));
    }

    void right(Entity player) {
        float speed = (1000.0f / 1920.0f) * _elapsed_time.count();
        auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
        auto &pos = gCoordinator.getComponent<TransformComponent>(player);
        pos.position.x += speed;
        sendAllPlayer(playerNetwork.id, "MRT" + std::to_string(playerNetwork.id));
    }

    void left(Entity player) {
        float speed = (1000.0f / 1920.0f) * _elapsed_time.count();
        auto &playerNetwork = gCoordinator.getComponent<NetworkComponent>(player);
        auto &pos = gCoordinator.getComponent<TransformComponent>(player);
        sendAllPlayer(playerNetwork.id, "MLF" + std::to_string(playerNetwork.id));
    }

    void update(Milliseconds elapsed_time) {
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

    std::string getCommand(std::string command) {
        if (command.size() < 3)
            return "";
        if (command.size() == 3)
            return command;
        _options = command.substr(3);
        return command.substr(0, 3);
    }

    void createNewClient(std::string name, int id, std::string ip, int port) override {
        if (name == "") {
            error("Invalid client name");
            sendTo(getServerFd(), "ERRInvalid client name", ip, port);
            return;
        }
        sendAllPlayer(id, "NEW" + std::to_string(id) + "0,0;" + name);
        std::vector<Entity> entityList(entities.begin(), entities.end());
        auto &entitiesManager = ServerEntitiesManager::getInstance();
        entitiesManager.createShip({0.20f, 0.2f * id}, id, name, ip, port);
        sendDataToPlayer("BGN" + std::to_string(id) + std::string("0,0;"), ip, port);
        for (size_t i = 0; i < entityList.size(); ++i) {
            auto& player = gCoordinator.getComponent<NetworkComponent>(entityList[i]);
            auto& pos = gCoordinator.getComponent<TransformComponent>(entityList[i]);
            sendDataToPlayer("NEW" + std::to_string(player.id) + std::to_string(pos.position.x) + std::string(",") + std::to_string(pos.position.x) + ";" + player.name, ip, port);
        }
    }

private:
    std::map<std::string, std::function<void(Entity)>> _protocolMap = {
        {"SHT", [this](Entity entity) { shoot(entity); }},
        {"MUP", [this](Entity entity) { up(entity); }},
        {"MDW", [this](Entity entity) { down(entity); }},
        {"MRT", [this](Entity entity) { right(entity); }},
        {"MLF", [this](Entity entity) { left(entity); }}

    };
    ServerNetworkSystem *_ServerNetworkSystem;
    std::string _options;
    Milliseconds _elapsed_time;
};
