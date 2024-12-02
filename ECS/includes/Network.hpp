#pragma once
#include "System.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <poll.h>
#include <vector>
#include <map>
#include <functional>
#include <sstream>
#include <fstream>

/**
 * @class ServerSystem
 * @brief Manages the server system.
 */
class ServerSystem : public System {
public:
    /**
     * @brief Initialize the server system.
     * @param ip The server IP address.
     * @param port The server port.
     * @return void
     */
    void init(const std::string& ip, int port) {
        _ip = ip;
        _port = port;
        _server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (_server_fd == 0) {
            error("socket failed");
            exit(84);
        }

        int opt = 1;

        if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            error("setsockopt");
            exit(84);
        }

        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons(port);

        if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
            error("bind failed");
            exit(84);
        }
        if (listen(_server_fd, 3) < 0) {
            error("listen");
            exit(84);
        }

        _fds = {{_server_fd, POLLIN, 0}, {STDIN_FILENO, POLLIN, 0}};
        _running = true;
        _restart = false;
        getHelp();
        info("Server is listening on port " + std::to_string(port));
    }

    /**
     * @brief Display an error message.
     * @param message The error message.
     * @return void
     */
    void getHelp() {
        _help.push_back("Available commands:");
        _help.push_back("send <message>                - Send a message to all client");
        _help.push_back("sendto <message> <clientId>   - Send a message to the clientId");
        _help.push_back("restart                       - Restart the server");
        _help.push_back("stop                          - Stop the server");
        _help.push_back("help                          - Display this help");
        _help.push_back("Added commands:");
    }

    /**
     * @brief Display an error message.
     * @param message The error message.
     * @return void
     */
    void stop() {
        close(_server_fd);
        _running = false;
        info("Server stopped");
    }

    /**
     * @brief Restart the server.
     * @return void
     */
    void restart() {
        stop();
        _running = true;
        _restart = true;
    }

    /**
     * @brief Read file descriptors.
     * @return void
     */
    void read_fds() {
        poll(_fds.data(), _fds.size(), 0);
        for (size_t i = 0; i < _fds.size(); ++i) {
            if (_fds[i].revents & POLLIN) {
                if (_fds[i].fd == _server_fd) {
                    int client_fd = accept(_server_fd, nullptr, nullptr);
                    _fds.push_back({client_fd, POLLIN, 0});
                    info("From: " + std::to_string(_fds.size() - 1) + " New connection");
                } else if (_fds[i].fd == STDIN_FILENO) {
                    char buffer[1024];
                    int n = read(STDIN_FILENO, buffer, sizeof(buffer));
                    if (n <= 0) {
                        error("Failed to read from stdin");
                    } else {
                        _lastCommand = std::string(buffer, n - 1);
                        handleCommand();
                    }
                } else {
                    char buffer[1024];
                    int n = read(_fds[i].fd, buffer, sizeof(buffer));
                    if (n <= 0) {
                        close(_fds[i].fd);
                        info("From: " + std::to_string(i) + " Connection closed");
                        _fds.erase(_fds.begin() + i);
                        --i;
                    } else {
                        debug("From: " + std::to_string(i) + " Received: " + std::string(buffer, n));
                    }
                }
            }
        }
    }

    /**
     * @brief Get the command option.
     * @return std::string
     */
    std::string getOption() {
        std::istringstream iss(_lastCommand);
        std::string token;
        _commandOption.clear();
        while (iss >> token) {
            _commandOption.push_back(token);
        }
        return _commandOption.empty() ? "" : _commandOption[0];
    }

    /**
     * @brief Handle the command.
     * @return void
     */
    void handleCommand() {
        if (_lastCommand == "")
            return prompt();
        auto it = _commandMap.find(getOption());
        if (it != _commandMap.end()) {
            it->second();
        } else {
            info("Unknown command: " + _lastCommand);
        }
    }

    /**
     * @brief Display an error message.
     * @param message The error message.
     * @return void
     */
    void help() {
        for (const auto& command : _help) {
            std::cout << command << std::endl;
        }
        prompt();
    }

    /**
     * @brief Add a command.
     * @param command The command.
     * @param func The function to execute.
     * @param description The command description.
     * @return void
     */
    void addCommand(const std::string& command, const std::function<void()>& func, const std::string& description = "") {
        _commandMap[command] = func;
        _help.push_back(command + " " + description);
    }

    /**
     * @brief Update the server system.
     * @return void
     */
    void update() {
        if (_restart) {
            init(_ip, _port);
        }
        read_fds();
    }

    /**
     * @brief Send data to all players.
     * @return void
     */
    void sendDataAllPlayer() {
        if (_commandOption.size() < 2) {
            error("Invalid command option");
            return;
        }
        std::string message = _commandOption[1];
        for (size_t i = 2; i < _fds.size(); ++i) {
            sendDataToPlayer(message, i);
        }
    }

    /**
     * @brief Send data to a player.
     * @return void
     */
    void sendToPlayer() {
        if (_commandOption.size() < 3) {
            error("Invalid command option");
            return;
        }
        sendDataToPlayer(_commandOption[2], std::stoi(_commandOption[1]));
    }

    /**
     * @brief Send data to a player.
     * @param message The message to send.
     * @param playerID The player ID.
     * @return void
     */
    void sendDataToPlayer(const std::string& message, int playerID) {
        if (playerID < 0 || playerID >= _fds.size()) {
            error("Invalid player ID");
            return;
        }
        if (send(_fds[playerID].fd, message.c_str(), message.size(), 0) < 0) {
            error("Failed to send data to player " + std::to_string(playerID));
        }
        debug("Sent data to player " + std::to_string(playerID) + ": " + message);
    };

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

    struct sockaddr_in _address;
    int _port;
    std::string _ip;
    int _server_fd;
    std::vector<pollfd> _fds;
    bool _running = false;
    std::string _lastCommand;
    bool _restart = false;
    std::map<std::string, std::function<void()>> _commandMap = {
        {"stop", [this]() { stop(); }},
        {"restart", [this]() { restart(); }},
        {"send", [this]() { sendDataAllPlayer(); }},
        {"sendto", [this]() { sendToPlayer(); }},
        {"help", [this]() { help(); }},
    };
    std::vector<std::string> _commandOption;
    std::vector<std::string> _help;
};


/**
 * @class ClientSystem
 * @brief Manages the client system.
 */
class ClientSystem : public System {
public:
    /**
     * @brief Initialize the client system.
     * @param ip The server IP address.
     * @param port The server port.
     * @return void
     */
    void init(const std::string& ip, int port) {
        _ip = ip;
        _port = port;
        connectToServer();
    }

    /**
     * @brief Connect to the server.
     * @return bool
     */
    bool connectToServer() {
        if (isConnected()) {
            warning("Already connected to server");
            return false;
        }
        _socket = socket(AF_INET, SOCK_STREAM, 0);
        if (_socket < 0) {
            error("Error creating socket");
            return false;
        }

        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons(_port);

        if (inet_pton(AF_INET, _ip.c_str(), &_address.sin_addr) <= 0) {
            error("Invalid server address");
            return false;
        }

        if (connect(_socket, (struct sockaddr*)&_address, sizeof(_address)) < 0) {
            // error("Failed to connect to server");
            return false;
        }
        info("Connected to server at " + _ip + ":" + std::to_string(_port));
        _connected = true;
        return true;
    }

    /**
     * @brief Disconnect from the server.
     * @return void
     */
    void disconnect() {
        if (isConnected()) {
            close(_socket);
            _connected = false;
            info("Disconnected from server");
        } else {
            warning("Not connected to server");
        }
    }

    /**
     * @brief Restart the connection.
     * @return void
     */
    void restartConnection() {
        disconnect();
        connectToServer();
    }

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
    bool sendData(const std::string &data) {
        if (!isConnected()) {
            error("Not connected to server");
            info("Attempting to reconnect to server");
            if (!connectToServer()) {
                return false;
            }
            info("Reconnected to server");
        }
        if (send(_socket, data.c_str(), data.size(), 0) < 0) {
            error("Failed to send data to server");
            return false;
        }
        debug("Sent data to server: " + data);
        return true;
    }

    /**
     * @brief Receive data from the server.
     * @return std::string
     */
    std::string receiveData() {
        if (!isConnected()) {
            error("Not connected to server");
            return "";
        }

        struct pollfd _fds;
        _fds.fd = _socket;
        _fds.events = POLLIN;

        int pollResult = poll(&_fds, 1, 0);
        if (pollResult == -1) {
            error("Poll error");
            return "";
        } else if (pollResult == 0) {
            return "";
        }

        if (_fds.revents & POLLIN) {
            char buffer[1024] = {0};
            int valread = read(_socket, buffer, 1024);
            if (valread < 0) {
                error("Failed to read data from server");
                return "";
            }
            if (valread == 0) {
                error("Server disconnected");
                disconnect();
                return "";
            }
            std::string data(buffer, valread);
            debug("Received data from server: " + data);
            return data;
        }
        return "";
    }

    /**
     * @brief Update the client system.
     * @return std::string
     */
    std::string update() {
        if (isConnected())
            return receiveData();
        connectToServer();
        return "";
    }

private:
    bool _connected = false;
    int _socket;
    struct sockaddr_in _address;
    int _port;
    std::string _ip;

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