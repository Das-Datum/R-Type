
#include "../includes/NetworkSystems.hpp"
#include "../includes/ServerSystem.hpp"

#define BUFFER_SIZE 1024

extern Coordinator gCoordinator;

int NetworkSystem::sendTo(int socket, const std::string& message, std::string ip, int port) {
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &client_addr.sin_addr) <= 0) {
        std::cerr << "Invalid client IP address" << std::endl;
        return -1;
    }
    return sendto(socket, message.c_str(), message.size(), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
}

struct sockaddr_in get_sockaddr_in(const char* ip, int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    return addr;
}

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
        _terminalThread = std::thread(&ServerSystem::handleCommand, this);
        _receivedMsgThread = std::thread(&ServerSystem::updateRead, this);
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
    if (!_restart && _terminalThread.joinable()) {
        _terminalThread.detach();
    }
    if (!_restart && _receivedMsgThread.joinable()) {
        _receivedMsgThread.detach();
    }
    std::vector<Entity> entityList(entities.begin(), entities.end());
    for (size_t i = 0; i < entityList.size(); ++i) {
        auto& player = gCoordinator.getComponent<NetworkComponent>(entityList[i]);
        sendTo(_server_fd, "QIT", player.ip, player.port);
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
    sockaddr_in new_client_addr;
    socklen_t client_len = sizeof(new_client_addr);
    std::memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recvfrom(_server_fd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&new_client_addr, &client_len);
    if (bytes_received < 0) {
        perror("Receive failed");
        prompt();
        return;
    }

    std::string client_ip = inet_ntoa(new_client_addr.sin_addr);
    int client_port = ntohs(new_client_addr.sin_port);

    info("Received message from " + client_ip + ":" + std::to_string(client_port) + " - " + buffer);


    std::unique_lock<std::mutex> lock(_clientsMutex);
    bool client_exists = false;
    std::vector<Entity> entityList(entities.begin(), entities.end());
    std::vector<int> client_ids;
    for (size_t i = 0; i < entityList.size(); ++i) {
        auto& player = gCoordinator.getComponent<NetworkComponent>(entityList[i]);
        client_ids.push_back(player.id);
        if (client_ip == player.ip && client_port == player.port) {
            client_exists = true;
            player.lastMessagesReceived.push_back(std::string(buffer));
            break;
        }
    }
    info("Client exists: " + std::to_string(client_exists));
    info("Client IDs: " + std::to_string(client_ids.size()));
    int new_client_id = getNewClientId(client_ids);
    info("New client ID: " + std::to_string(new_client_id));
    if (new_client_id > 4) {
        sendTo(_server_fd, "ERRServer is full", client_ip, client_port);
        error("No available client ID");
        return;
    }
    if (!client_exists) {
        info(std::string(buffer));
        std::string new_client_name = checkNewClient(std::string(buffer));
        info("New client name: " + new_client_name);
        if (new_client_name == "") {
            error("Invalid client name");
            sendTo(_server_fd, "ERRInvalid client name", client_ip, client_port);
            return;
        }
        for (size_t i = 0; i < entityList.size(); ++i) {
            auto& player = gCoordinator.getComponent<NetworkComponent>(entityList[i]);
            debug("Sent new client info to " + player.id);
            if (sendTo(_server_fd, std::string("NEW") + std::to_string(new_client_id) + new_client_name, player.ip, player.port) <= 0) {
                perror("Connection failed");
                return;
            }
        }
        Entity new_client = gCoordinator.createEntity();
        gCoordinator.addComponent(new_client, NetworkComponent(new_client_name, client_ip, client_port, new_client_id));
        info("New client added: " + new_client_name);
        if (sendTo(_server_fd, "BGN" + std::to_string(new_client_id), client_ip, client_port) <= 0) {
            error("Connection failed");
            return;
        }
    }
}

std::string ServerSystem::checkNewClient(std::string msg) {
    if (msg.rfind("NEW", 0) == 0) {
        return msg.substr(4);
    }
    return "";
}

int ServerSystem::getNewClientId(std::vector<int> client_ids) {
    int id = 1;
    for (size_t i = 0; i < client_ids.size(); ++i)
        if (client_ids[i] == id)
            id++;
        else
            break;
    return id;
}

void ServerSystem::updateRead() {
    while (isRunning())
        readClients();
}

void ServerSystem::update() {
    if (_restart) {
        init(_ip, _port);
    }
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
    std::unique_lock<std::mutex> lock(_clientsMutex);
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
    std::unique_lock<std::mutex> lock(_clientsMutex);
    if (_commandOption.size() < 2) {
        error("Invalid command option");
        return;
    }
    std::vector<Entity> entityList(entities.begin(), entities.end());
    for (size_t i = 0; i < entityList.size(); ++i) {
        auto& player = gCoordinator.getComponent<NetworkComponent>(entityList[i]);
        sendDataToPlayer(_commandOption[1], player.ip, player.port);
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
    if (sendTo(_server_fd, message, ip, port) < 0) {
        error("Send failed");
    }
    info("Sent data to " + ip + ":" + std::to_string(port) + " - " + message);
};



void ClientSystem::init(const std::string& name, const std::string& ip, int port) {
    _ip = ip;
    _port = port;
    _name = name;
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

    if (sendTo(_socket, "NEW0" + _name, _ip, _port) < 0) {
        // error("Connection failed");
        return false;
    }
    info("Connected to server at " + _ip + ":" + std::to_string(_port));
    _connected = true;
    _receivedMsgThread = std::thread(&ClientSystem::update_read, this);
    return true;
}

void ClientSystem::disconnect() {
    if (isConnected()) {
        info("Disconnected from server 1");
        if (_receivedMsgThread.joinable()) {
            _receivedMsgThread.detach();
        }
        info("Disconnected from server 2");
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
    if (sendTo(_socket, data, _ip, _port) < 0) {
        error("Send failed");
        return false;
    }
    debug("Sent data to server: " + data);
    return true;
}

void ClientSystem::receiveData() {
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(_port);
    if (inet_pton(AF_INET, _ip.c_str(), &address.sin_addr) <= 0) {
        perror("Invalid server address");
        return;
    }
    socklen_t addr_len = sizeof(address);
    int bytes_received = recvfrom(_socket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&address, &addr_len);
    if (bytes_received < 0) {
        return;
    }
    if (bytes_received == 0) {
        warning("Server disconnected");
        disconnect();
        return;
    }
    std::lock_guard<std::mutex> lock(_mutex);
    _lastMessage.push_back(std::string(buffer));
    debug("Received data from server: " + std::string(buffer));
    return;
}

void ClientSystem::update_read() {
    while (isConnected())
        receiveData();
}

std::vector<std::string> ClientSystem::update() {
    if (isConnected()) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_lastMessage.empty())
            return {};
        std::vector<std::string> messages(_lastMessage.begin(), _lastMessage.end());
        _lastMessage.clear();
        return messages;
    }
    connectToServer();
    return {};
}
