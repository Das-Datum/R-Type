#pragma once

#include "ECS.hpp"
#include "NetworkComponent.hpp"
#include "NetworkSystems.hpp"
#include "Components/GameComponents.hpp"
#include "ServerEntitiesManager.hpp"
#include <map>
#include <functional>

/**
 * @class ServerManageNetworkSystem
 * @brief Manage the network for the server
 */

class ServerManageNetworkSystem : public ServerNetworkSystem {
    public:

        /**
         * @brief Handle the shooting action for the specified player entity.
         *
         * @param player The player entity that is shooting.
         */
        void shoot(Entity player);

        /**
         * @brief Handle the beam action for the specified player entity.
         *
         * @param player The player entity that is using the beam.
         */
        void beam(Entity player);

        /**
         * @brief Handle the upward movement action for the specified player entity.
         *
         * @param player The player entity that is moving up.
         */
        void up(Entity player);

        /**
         * @brief Handle the downward movement action for the specified player entity.
         *
         * @param player The player entity that is moving down.
         */
        void down(Entity player);

        /**
         * @brief Handle the rightward movement action for the specified player entity.
         *
         * @param player The player entity that is moving right.
         */
        void right(Entity player);

        /**
         * @brief Handle the leftward movement action for the specified player entity.
         *
         * @param player The player entity that is moving left.
         */
        void left(Entity player);

        /**
         * @brief Start the game with the specified player entity.
         *
         * @param player The player entity that is starting the game.
         */
        void startGame(Entity player);

        /**
         * @brief Update the network system with the elapsed time.
         *
         */
        void update();

        /**
         * @brief Get the command string based on the input command.
         *
         * @param command The input command string.
         * @return The corresponding command string.
         */
        std::string getCommand(std::string command);

        /**
         * @brief Create a new client with the specified details.
         *
         * @param name The name of the new client.
         * @param id The id of the new client.
         * @param ip The IP address of the new client.
         * @param port The port number of the new client.
         */
        void createNewClient(std::string name, int id, std::string ip, int port) override;

        /**
         * @brief Disconnect the specified client entity.
         *
         * @param entity The entity of the client to disconnect.
         */
        void disconnectClient(Entity entity);

        /**
         * @brief Send the position of all players to all clients.
         */
        void sendAllPlayersPosition();
    private:
        std::map<std::string, std::function<void(Entity)>> _protocolMap = {
            {"SHT", [this](Entity entity) { shoot(entity); }},
            {"MUP", [this](Entity entity) { up(entity); }},
            {"MDW", [this](Entity entity) { down(entity); }},
            {"MRT", [this](Entity entity) { right(entity); }},
            {"MLF", [this](Entity entity) { left(entity); }},
            {"DEM", [this](Entity entity) { beam(entity); }},
            {"STA", [this](Entity entity) { startGame(entity); }},
            {"QIT", [this](Entity entity) { disconnectClient(entity); }},

        };
        ServerNetworkSystem *_ServerNetworkSystem;
        std::string _options;
        double _elapsed_time;

        bool _gameStarted = false;
};
