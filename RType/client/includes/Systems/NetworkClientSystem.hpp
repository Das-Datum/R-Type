#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../../../../Network/includes/NetworkComponent.hpp"
#include "../../../../Network/includes/NetworkSystems.hpp"
#include "../Components.hpp"
#include "../../shared/includes/Components/GameComponents.hpp"
#include "../EntitiesManager.hpp"
#include <map>
#include <functional>

class NetworkClientSystem : public System {
    public:

        void newPlayer(std::string msg) {
            auto &entitiesManager = EntitiesManager::getInstance();
            entitiesManager.createShip({0, 0}, _id, _options, 0);
        }

        void shoot(Entity player) {
            auto &transform = gCoordinator.getComponent<TransformComponent>(player);
            auto &entitiesManager = EntitiesManager::getInstance();

            Vector2 bulletPosition = {transform.position.x + transform.size.x / 2,
                transform.position.y};
                entitiesManager.createBullet(bulletPosition, {400.0f, 0.0f});
        }

        void beam(Entity player) {
            auto &transform = gCoordinator.getComponent<TransformComponent>(player);
            auto &entitiesManager = EntitiesManager::getInstance();

            Vector2 bulletPosition = {transform.position.x + transform.size.x / 2,
                transform.position.y};
                entitiesManager.createBullet(bulletPosition, {400.0f, 0.0f});
        }

        void up(Entity player) {
            float speed = 400.0f * GetFrameTime();
            auto &playerNetwork = gCoordinator.getComponent<TransformComponent>(player);
            playerNetwork.position.y -= speed;
        }

        void down(Entity player) {
            float speed = 400.0f * GetFrameTime();
            auto &playerNetwork = gCoordinator.getComponent<TransformComponent>(player);
            playerNetwork.position.y += speed;
        }

        void right(Entity player) {
            float speed = 400.0f * GetFrameTime();
            auto &playerNetwork = gCoordinator.getComponent<TransformComponent>(player);
            playerNetwork.position.x += speed;
        }

        void left(Entity player) {
            float speed = 400.0f * GetFrameTime();
            auto &playerNetwork = gCoordinator.getComponent<TransformComponent>(player);
            playerNetwork.position.x -= speed;
        }

        void update(std::vector<std::string> messages) {
            for (auto const &msg : messages) {
                std::string command = getCommand(msg);
                if (_commandMap.find(command) != _commandMap.end()) {
                    _commandMap[command](getEntityById(_id));
                }
            }
        }

        Entity getEntityById(int id) {
            for (auto const &entity : entities) {
                auto &player = gCoordinator.getComponent<NetworkComponent>(entity);
                if (player.id == id)
                    return entity;
            }
            return 0;
        }

        std::string getCommand(std::string command) {
            std::cout << command << std::endl;
            std::cout << command.size() << std::endl;
            if (command.size() < 3)
                return "";
            if (command.size() > 3)
                _id = std::stoi(command.substr(3));
            _options = command.substr(3);
            return command.substr(0, 3);
        }
    private:
        std::map<std::string, std::function<void(Entity)>> _commandMap = {
            {"NEW", [this](Entity entity) { newPlayer(_options); }},
            {"SHT", [this](Entity entity) { shoot(entity); }},
            {"DEM", [this](Entity entity) { beam(entity); }},
            {"MUP", [this](Entity entity) { up(entity); }},
            {"MDW", [this](Entity entity) { down(entity); }},
            {"MRT", [this](Entity entity) { right(entity); }},
            {"MLF", [this](Entity entity) { left(entity); }}

        };
        int _id;
        std::string _options;
};

class PlayerNetworkSystem : public System {
    public:
        void init(ClientSystem &clientSystem) {
            _clientSystem = &clientSystem;
        }

        void update() {
            for (auto const &entity : entities) {
                auto &player = gCoordinator.getComponent<PlayerNetworkComponent>(entity);
                while (!player.lastMessagesReceived.empty()) {
                    _clientSystem->sendData(player.lastMessagesReceived.front());
                    player.lastMessagesReceived.erase(player.lastMessagesReceived.begin());
                }
            }
        }

    private:
        ClientSystem *_clientSystem;
};
