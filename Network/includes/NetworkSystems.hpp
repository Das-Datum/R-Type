#pragma once
#include "ECS.hpp"
#include "NetworkComponent.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <functional>
#include <sstream>
#include <fstream>

#define BUFFER_SIZE 1024

extern Coordinator gCoordinator;


class NetworkSystem : public System {
    public:
        int sendTo(int socket, const std::string& message, std::string ip, int port);
};

/**
 * @class ServerSystem
 * @brief Manages the server system.
 */
class ServerSystem : public NetworkSystem {
public:
    /**
     * @brief Initialize the server system.
     * @param ip The server IP address.
     * @param port The server port.
     * @return void
     */
    void init(const std::string& ip, int port);

    /**
     * @brief Display an error message.
     * @param message The error message.
     * @return void
     */
    void getHelp();

    /**
     * @brief Display an error message.
     * @param message The error message.
     * @return void
     */
    void stop();


    /**
     * @brief Read file descriptors.
     * @return void
     */
    void readClients();

    void updateRead();

    /**
     * @brief Update the server system.
     * @return void
     */
    void update();

    /**
     * @brief Handle the command.
     * @return void
     */
    void handleCommand();
    /**
     * @brief Add a command.
     * @param command The command.
     * @param func The function to execute.
     * @param description The command description.
     * @return void
     */
    void addCommand(const std::string& command, const std::function<void()>& func, const std::string& description = "");

    /**
     * @brief Get the command option.
     * @return std::string
     */
    std::string getOption();

    /**
     * @brief Restart the server.
     * @return void
     */
    void restart();


    /**
     * @brief Display an error message.
     * @param message The error message.
     * @return void
     */
    void help();

    /**
     * @brief Send data to all players.
     * @return void
     */
    void sendDataAllPlayer();

    /**
     * @brief Send data to a player.
     * @param message The message to send.
     * @param playerID The player ID.
     * @return void
     */
    void sendDataToPlayer(const std::string& message, std::string ip, int port);

    /**
     * @brief Check if the server is running.
     * @return bool
     */
    bool isRunning() const { return _running; };

private:
    /**
     * @brief Display a prompt.
     * @return void
     */
    void prompt() {
        if (isRunning())
            std::cout << "> " << std::flush;
    }
    void info(const std::string& message) {
        std::cout << "\033[2K\r[INFO]: " << message << std::endl;
        prompt();
    }
    void error(const std::string& message) {
        std::cout << "\033[2K\r[ERROR]: " << message << std::endl;
        prompt();
    }

    void debug(const std::string& message) {
        std::cout << "\033[2K\r[DEBUG]: " << message << std::endl;
        prompt();
    }
    void warning(const std::string& message) {
        std::cout << "\033[2K\r[WARNING]: " << message << std::endl;
        prompt();
    }
    std::string checkNewClient(std::string msg);
    int getNewClientId(std::vector<int> client_ids);

    int _port;
    std::string _ip;
    int _server_fd;
    bool _running = false;
    std::string _lastCommand;
    bool _restart = false;
    std::vector<std::string> _commandOption;
    std::vector<std::string> _help;
    std::mutex _clientsMutex;
    std::thread _terminalThread;
    std::thread _receivedMsgThread;
    std::map<std::string, std::function<void()>> _commandMap = {
        {"stop", [this]() { stop(); }},
        {"restart", [this]() { restart(); }},
        {"send", [this]() { sendDataAllPlayer(); }},
        {"help", [this]() { help(); }},
    };
};


/**
 * @class ClientSystem
 * @brief Manages the client system.
 */
class ClientSystem : public NetworkSystem {
public:
    /**
     * @brief Initialize the client system.
     * @param ip The server IP address.
     * @param port The server port.
     * @return void
     */
    void init(const std::string& name, const std::string& ip, int port);

    /**
     * @brief Connect to the server.
     * @return bool
     */
    bool connectToServer();

    /**
     * @brief Disconnect from the server.
     * @return void
     */
    void disconnect();

    /**
     * @brief Restart the connection.
     * @return void
     */
    void restartConnection();

    /**
     * @brief Check if the client is connected to the server.
     * @return bool
     */
    bool isConnected() const { return _connected; }

    /**
     * @brief Send data to the server.
     * @param data The data to send.
     * @return bool
     */
    bool sendData(const std::string &data);

    /**
     * @brief Receive data from the server.
     * @return std::string
     */
    void receiveData();

    void update_read();

    /**
     * @brief Update the client system.
     * @return void
     */
    std::vector<std::string> update();

private:
    bool _connected = false;
    int _socket;
    int _port;
    std::string _name;
    std::string _ip;
    std::thread _receivedMsgThread;
    std::mutex _mutex;
    std::vector<std::string> _lastMessage;

    void info(const std::string& message) {
        std::cout << "[INFO]: " << message << std::endl;
    }
    void error(const std::string& message) {
        std::cout << "[ERROR]: " << message << std::endl;
    }
    void debug(const std::string& message) {
        std::cout << "[DEBUG]: " << message << std::endl;
    }
    void warning(const std::string& message) {
        std::cout << "[WARNING]: " << message << std::endl;
    }
};
