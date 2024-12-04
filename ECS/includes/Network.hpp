#pragma once
#include "System.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <functional>
#include <sstream>
#include <fstream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#define BUFFER_SIZE 1024

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

        #ifdef _WIN32
            if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != 0) {
                error("Failed to initialize Winsock.");
                exit(84);
            }
        #endif
        _server_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_server_fd < 0) {
            error("socket creation failed");
            exit(84);
        }

        std::memset(&_address, 0, sizeof(_address));
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons(port);

        if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
            error("bind failed");
            stop();
            exit(84);
        }

        _running = true;
        getHelp();
        info("Server is listening on port " + std::to_string(port));
        if (!_restart)
            _terminal_thread = std::thread(&ServerSystem::handleCommand, this);
        _restart = false;
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
        if (!_restart && _terminal_thread.joinable()) {
            _terminal_thread.detach();
        }
        for (const auto& addr : _clients_addr) {
            sendto(_server_fd, "Server stopped", 14, MSG_DONTWAIT, (struct sockaddr*)&addr, sizeof(addr));
        }
        _clients_addr.clear();
        #ifdef _WIN32
            closesocket(_server_fd);
            WSACleanup();
        #else
            close(_server_fd);
        #endif
        _running = false;
        info("Server stopped");
    }


    /**
     * @brief Read file descriptors.
     * @return void
     */
    void read_fds() {
        char buffer[BUFFER_SIZE];
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        std::memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recvfrom(_server_fd, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT, (struct sockaddr*)&client_addr, &client_len);
        if (bytes_received < 0) {
            return;
        }

        std::string client_ip = inet_ntoa(client_addr.sin_addr);
        int client_port = ntohs(client_addr.sin_port);

        info("Received message from " + client_ip + ":" + std::to_string(client_port) + " - " + buffer);


        std::unique_lock<std::mutex> lock(_clients_mutex);
        bool client_exists = false;
        for (const auto& addr : _clients_addr) {
            if (addr.sin_addr.s_addr == client_addr.sin_addr.s_addr && addr.sin_port == client_addr.sin_port) {
                client_exists = true;
                break;
            }
        }
        if (!client_exists) {
            _clients_addr.push_back(client_addr);
            for (const auto& addr : _clients_addr) {
                sendto(_server_fd, "New client connected", 20, MSG_DONTWAIT, (struct sockaddr*)&addr, client_len);
            }
        }
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
     * @brief Handle the command.
     * @return void
     */
    void handleCommand() {
        while (isRunning()) {
            std::string command;
            std::getline(std::cin, command);
            _lastCommand = command;
            if (_lastCommand == "")
                return prompt();
            auto it = _commandMap.find(getOption());
            if (it != _commandMap.end()) {
                it->second();
            } else {
                info("Unknown command: " + _lastCommand);
            }
        }
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
     * @brief Restart the server.
     * @return void
     */
    void restart() {
        std::unique_lock<std::mutex> lock(_clients_mutex);
        stop();
        _running = true;
        _restart = true;
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
     * @brief Send data to all players.
     * @return void
     */
    void sendDataAllPlayer() {
        std::unique_lock<std::mutex> lock(_clients_mutex);
        if (_commandOption.size() < 2) {
            error("Invalid command option");
            return;
        }
        for (const auto& addr : _clients_addr) {
            sendDataToPlayer(_commandOption[1], addr);
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
        warning("Not implemented");
        // sendDataToPlayer(_commandOption[2], std::stoi(_commandOption[1]));
    }

    /**
     * @brief Send data to a player.
     * @param message The message to send.
     * @param playerID The player ID.
     * @return void
     */
    void sendDataToPlayer(const std::string& message, sockaddr_in client_addr) {
        bool client_exists = false;
        for (const auto& addr : _clients_addr) {
            if (addr.sin_addr.s_addr == client_addr.sin_addr.s_addr && addr.sin_port == client_addr.sin_port) {
                client_exists = true;
                break;
            }
        }
        if (client_exists) {
            debug("Sent data to player " + std::to_string(ntohs(client_addr.sin_port)) + ": " + message);
            sendto(_server_fd, message.c_str(), message.size(), MSG_DONTWAIT, (struct sockaddr*)&client_addr, sizeof(client_addr));
        } else {
            error("Client not found");
        }
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
    bool _running = false;
    std::string _lastCommand;
    bool _restart = false;
    std::vector<std::string> _commandOption;
    std::vector<std::string> _help;
    std::mutex _clients_mutex;
    std::thread _terminal_thread;
    std::vector<sockaddr_in> _clients_addr;
    std::map<std::string, std::function<void()>> _commandMap = {
        {"stop", [this]() { stop(); }},
        {"restart", [this]() { restart(); }},
        {"send", [this]() { sendDataAllPlayer(); }},
        {"sendto", [this]() { sendToPlayer(); }},
        {"help", [this]() { help(); }},
    };
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
        #ifdef _WIN32
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cerr << "Failed to initialize Winsock." << std::endl;
                exit(EXIT_FAILURE);
            }
        #endif
        if (isConnected()) {
            warning("Already connected to server");
            return false;
        }
        _socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (_socket < 0) {
            error("Error creating socket");
            return false;
        }

        std::memset(&_address, 0, sizeof(_address));
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons(_port);

        if (inet_pton(AF_INET, _ip.c_str(), &_address.sin_addr) <= 0) {
            error("Invalid server address");
            return false;
        }
        if (sendto(_socket, "Start", 6, 0, (struct sockaddr*)&_address, sizeof(_address)) < 0) {
            error("Send failed");
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
        if (sendto(_socket, data.c_str(), data.size(), MSG_DONTWAIT, (struct sockaddr*)&_address, sizeof(_address)) < 0) {
            error("Send failed");
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
        char buffer[BUFFER_SIZE];
        std::memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recvfrom(_socket, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT, (struct sockaddr*)&_address, (socklen_t*)sizeof(_address));
        if (bytes_received < 0) {
            if (buffer[0] == '\0')
                return std::string(buffer);
            // return "";
        }
        if (bytes_received == 0) {
            warning("Server disconnected");
            disconnect();
            return "";
        }
        std::string message = std::string(buffer);
        debug("Received data from server: " + message);
        return message;
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
    sockaddr_in _address;
    int _port;
    std::string _ip;
    #ifdef _WIN32
        WSADATA wsaData;
    #endif

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
