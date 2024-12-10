
#include "../includes/NetworkSystems.hpp"
#include "../includes/ServerSystem.hpp"

#define BUFFER_SIZE 1024

extern Coordinator gCoordinator;

void ServerSystem::init(const std::string& ip, int port) {
    _ip = ip;
    _port = port;

    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            error("Failed to initialize Winsock.");
            exit(84);
        }
    #endif
    _server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_server_fd < 0) {
        error("socket creation failed");
        exit(84);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(_server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
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

void ServerSystem::getHelp() {
    _help.push_back("Available commands:");
    _help.push_back("send <message>                - Send a message to all client");
    _help.push_back("sendto <message> <clientId>   - Send a message to the clientId");
    _help.push_back("restart                       - Restart the server");
    _help.push_back("stop                          - Stop the server");
    _help.push_back("help                          - Display this help");
    _help.push_back("Added commands:");
}


void ServerSystem::stop() {
    if (!_restart && _terminal_thread.joinable()) {
        _terminal_thread.detach();
    }
    std::vector<Entity> entityList(entities.begin(), entities.end());
    for (size_t i = 0; i < entityList.size(); ++i) {
        auto& player = gCoordinator.getComponent<PlayerNetworkComponents>(entityList[i]);
        struct sockaddr_in client_addr;
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = player.port;
        if (inet_pton(AF_INET, player.ip.c_str(), &client_addr.sin_addr) <= 0) {
            error("Invalid client IP address");
            continue;
        }
        sendto(_server_fd, "STOP", 4, 64, (struct sockaddr*)&client_addr, sizeof(client_addr));
        gCoordinator.destroyEntity(entityList[i]);
    }
    #ifdef _WIN32
        closesocket(_server_fd);
        WSACleanup();
    #else
        close(_server_fd);
    #endif
    _running = false;
    info("Server stopped");
}

void ServerSystem::readClients() {
    char buffer[BUFFER_SIZE];
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    std::memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recvfrom(_server_fd, buffer, BUFFER_SIZE - 1, 64, (struct sockaddr*)&client_addr, &client_len);
    if (bytes_received < 0) {
        return;
    }

    std::string client_ip = inet_ntoa(client_addr.sin_addr);
    int client_port = ntohs(client_addr.sin_port);

    info("Received message from " + client_ip + ":" + std::to_string(client_port) + " - " + buffer);


    std::unique_lock<std::mutex> lock(_clients_mutex);
    bool client_exists = false;
    std::vector<Entity> entityList(entities.begin(), entities.end());
    for (size_t i = 0; i < entityList.size(); ++i) {
        auto& player = gCoordinator.getComponent<PlayerNetworkComponents>(entityList[i]);
        struct sockaddr_in client_addr;
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = player.port;
        if (inet_pton(AF_INET, player.ip.c_str(), &client_addr.sin_addr) <= 0) {
            error("Invalid client IP address");
            continue;
        }
        if (client_addr.sin_addr.s_addr == client_addr.sin_addr.s_addr && client_addr.sin_port == client_addr.sin_port) {
            client_exists = true;
            player.lastMessageReceived = buffer;
            break;
        }
    }
    if (!client_exists) {
        for (size_t i = 0; i < entityList.size(); ++i) {
            auto& player = gCoordinator.getComponent<PlayerNetworkComponents>(entityList[i]);
            sendto(_server_fd, "NEW", 3, 64, (struct sockaddr*)&client_addr, client_len);
        }
    }
}

void ServerSystem::update() {
    if (_restart) {
        init(_ip, _port);
    }
    readClients();
}

void ServerSystem::handleCommand() {
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

void ServerSystem::addCommand(const std::string& command, const std::function<void()>& func, const std::string& description) {
    _commandMap[command] = func;
    _help.push_back(command + " " + description);
}

std::string ServerSystem::getOption() {
    std::istringstream iss(_lastCommand);
    std::string token;
    _commandOption.clear();
    while (iss >> token) {
        _commandOption.push_back(token);
    }
    return _commandOption.empty() ? "" : _commandOption[0];
}

void ServerSystem::restart() {
    std::unique_lock<std::mutex> lock(_clients_mutex);
    stop();
    _running = true;
    _restart = true;
}

void ServerSystem::help() {
    for (const auto& command : _help) {
        std::cout << command << std::endl;
    }
    prompt();
}

void ServerSystem::sendDataAllPlayer() {
    std::unique_lock<std::mutex> lock(_clients_mutex);
    if (_commandOption.size() < 2) {
        error("Invalid command option");
        return;
    }
    std::vector<Entity> entityList(entities.begin(), entities.end());
    for (size_t i = 0; i < entityList.size(); ++i) {
        auto& player = gCoordinator.getComponent<PlayerNetworkComponents>(entityList[i]);
        struct sockaddr_in client_addr;
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = player.port;
        if (inet_pton(AF_INET, player.ip.c_str(), &client_addr.sin_addr) <= 0) {
            error("Invalid client IP address");
            continue;
        }
        sendDataToPlayer(_commandOption[1], player.ip, ntohs(player.port));
    }
}

void ServerSystem::sendToPlayer() {
    if (_commandOption.size() < 3) {
        error("Invalid command option");
        return;
    }
    warning("Not implemented");
    // sendDataToPlayer(_commandOption[2], std::stoi(_commandOption[1]));
}

void ServerSystem::sendDataToPlayer(const std::string& message, std::string ip, int port) {
    bool client_exists = true;
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = port;
    if (inet_pton(AF_INET, ip.c_str(), &client_addr.sin_addr) <= 0) {
        error("Invalid client IP address");
        return;
    }

    if (client_exists) {
        debug("Sent data to player " + std::to_string(ntohs(client_addr.sin_port)) + ": " + message);
        sendto(_server_fd, message.c_str(), message.size(), 64, (struct sockaddr*)&client_addr, sizeof(client_addr));
    } else {
        error("Client not found");
    }
};



void ClientSystem::init(const std::string& ip, int port) {
    _ip = ip;
    _port = port;
    connectToServer();
}

bool ClientSystem::connectToServer() {
    #ifdef _WIN32
        WSAData wsaData;
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

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);

    if (inet_pton(AF_INET, _ip.c_str(), &address.sin_addr) <= 0) {
        error("Invalid server address");
        return false;
    }
    if (sendto(_socket, "Start", 6, 0, (struct sockaddr*)&address, sizeof(address)) < 0) {
        // error("Connection failed");
        return false;
    }
    info("Connected to server at " + _ip + ":" + std::to_string(_port));
    _connected = true;
    return true;
}

void ClientSystem::disconnect() {
    if (isConnected()) {
        #ifdef _WIN32
            closesocket(_socket);
            WSACleanup();
        #else
            close(_socket);
        #endif
        _connected = false;
        info("Disconnected from server");
    } else {
        warning("Not connected to server");
    }
}

void ClientSystem::restartConnection() {
    disconnect();
    connectToServer();
}


bool ClientSystem::sendData(const std::string &data) {
    if (!isConnected()) {
        error("Not connected to server");
        info("Attempting to reconnect to server");
        if (!connectToServer()) {
            return false;
        }
        info("Reconnected to server");
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);
    if (inet_pton(AF_INET, _ip.c_str(), &address.sin_addr) <= 0) {
        error("Invalid server address");
        return false;
    }

    if (sendto(_socket, data.c_str(), data.size(), 64, (struct sockaddr*)&address, sizeof(address)) < 0) {
        error("Send failed");
        return false;
    }
    debug("Sent data to server: " + data);
    return true;
}

std::string ClientSystem::receiveData() {
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);
    if (inet_pton(AF_INET, _ip.c_str(), &address.sin_addr) <= 0) {
        error("Invalid server address");
        return "";
    }

    int bytes_received = recvfrom(_socket, buffer, BUFFER_SIZE - 1, 64, (struct sockaddr*)&address, (socklen_t*)sizeof(address));
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

std::string ClientSystem::update_read() {
    if (isConnected())
        return receiveData();
    connectToServer();
    return "";
}
