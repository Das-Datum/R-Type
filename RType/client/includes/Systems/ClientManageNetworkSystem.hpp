#pragma once
#include "ECS.hpp"
#include "NetworkComponent.hpp"
#include "NetworkSystems.hpp"
#include "Components.hpp"
#include "Components/GameComponents.hpp"
#include "ClientEntitiesManager.hpp"
#include <map>
#include <functional>

extern Coordinator gCoordinator;

class ClientManageNetworkSystem : public ClientNetworkSystem {
    public:

        /**
         * @brief Create a new player
         * @param msg The message received
         * @return void
         */
        void newPlayer(std::string msg);

        /**
         * @brief Create a player ship
         * @param player The player entity
         * @return void
         */
        void createPlayerShip(Entity entity);

        /**
         * @brief Shoot
         * @param player The player entity
         * @return void
         */
        void shoot(Entity player);

        /**
         * @brief Beam
         * @param player The player entity
         * @return void
         */
        void beam(Entity player);

        /**
         * @brief Move up
         * @param player The player entity
         * @return void
         */
        void up(Entity player);

        /**
         * @brief Move down
         * @param player The player entity
         * @return void
         */
        void down(Entity player);

        /**
         * @brief Move right
         * @param player The player entity
         * @return void
         */
        void right(Entity player);

        /**
         * @brief Move left
         * @param player The player entity
         * @return void
         */
        void left(Entity player);

        /**
         * @brief Disconnect a player
         * @param entity The player entity
         * @return void
         */
        void disconnectPlayer(Entity entity);

        /**
         * @brief Update the system
         * @return void
         */
        void update();

        /**
         * @brief Get the entity by id
         * @param id The entity id
         * @return Entity
         */
        Entity getEntityById(int id);

        /**
         * @brief Get the last messages
         * @return std::vector<std::string>
         */
        std::string getCommand(std::string command);

        int getPos(std::string text);
    private:
        std::map<std::string, std::function<void(Entity)>> _protocolMap = {
            {"NEW", [this](Entity entity) { newPlayer(_options); }},
            {"SHT", [this](Entity entity) { shoot(entity); }},
            {"DEM", [this](Entity entity) { beam(entity); }},
            {"MUP", [this](Entity entity) { up(entity); }},
            {"MDW", [this](Entity entity) { down(entity); }},
            {"MRT", [this](Entity entity) { right(entity); }},
            {"MLF", [this](Entity entity) { left(entity); }},
            {"BGN", [this](Entity entity) { createPlayerShip(entity); }},
            {"DEL", [this](Entity entity) { disconnectPlayer(entity); }},

        };
        int _id;
        std::string _options;
        float _x;
        float _y;
};

class NetworkInstructionsSystem : public System {
    public:

        /**
         * @brief Update the system
         * @return void
         */
        void update() {
            for (auto const &entity : entities) {
                auto &player = gCoordinator.getComponent<NetworkInstructionsComponent>(entity);
                while (!player.instructionsBuffer.empty()) {
                    gCoordinator.getSystem<ClientManageNetworkSystem>()->sendData(player.instructionsBuffer.front());
                    player.instructionsBuffer.erase(player.instructionsBuffer.begin());
                }
            }
        }

    private:
};
