#pragma once
#include <string>

struct PlayerNetworkComponents {
    std::string name;
    std::string ip;
    int port;
    std::string lastMessageReceived;

    PlayerNetworkComponents(std::string n, std::string ip, int port)
        : name(n), ip(ip), port(port){}
};
