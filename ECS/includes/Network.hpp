#pragma once
#include "System.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <poll.h>
#include <vector>

class ServerSystem : public System {
public:
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
        info("Server is listening on port " + std::to_string(port));
    }
    void stop() {
        close(_server_fd);
        _running = false;
        info("Server stopped");
    };
    void restart() {
        stop();
        init(_ip, _port);
    };
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
    };
    void handleCommand() {
        if (_lastCommand == "")
            return prompt();
        if (_lastCommand == "stop")
            stop();
        else if (_lastCommand == "restart")
            restart();
        else
            info("Unknown command: " + _lastCommand);
        _lastCommand = "";
    };
    void update() {
        read_fds();
    };
    void sendDataAllPlayer(const std::string& message) {
        for (size_t i = 0; i < _fds.size(); ++i) {
            sendDataToPlayer(message, i);
        }
    };
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
    bool isRunning() const { return _running; };

private:
    struct sockaddr_in _address;
    int _port;
    std::string _ip;
    int _server_fd;
    std::vector<pollfd> _fds;
    bool _running = false;
    std::string _lastCommand;

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
    void prompt() {
        if (isRunning())
            std::cout << "> " << std::flush;
    }
};

class ClientSystem : public System {
public:
    void init(const std::string& ip, int port) {
        _ip = ip;
        _port = port;
        connectToServer();
    }
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
    };
    void disconnect() {
        if (isConnected()) {
            close(_socket);
            _connected = false;
            info("Disconnected from server");
        } else {
            warning("Not connected to server");
        }
    };
    void restartConnection() {
        disconnect();
        connectToServer();
    };
    bool isConnected() const { return _connected; };
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
    };
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
    };
    std::string update() {
        if (isConnected())
            return receiveData();
        connectToServer();
        return "";
    };

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