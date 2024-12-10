#pragma once
#include <string>
#include <vector>

struct NetworkComponents {
    std::string name;
    std::string ip;
    int port;
    std::vector<std::string> lastMessagesReceived;
    int id;

    NetworkComponents(std::string n, std::string ip, int port, int id) : name(n), ip(ip), port(port), id(id) {}
};

