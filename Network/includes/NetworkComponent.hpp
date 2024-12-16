#pragma once
#include <string>
#include <vector>

struct NetworkComponent {
    std::string name;
    std::string ip;
    int port;
    std::vector<std::string> lastMessagesReceived;
    int id;

    NetworkComponent(std::string n, std::string ip, int port, int id) : name(n), ip(ip), port(port), id(id) {}
};

struct NetworkInstructionsComponent {
    std::string name;
    int id;
    std::vector<std::string> instructionsBuffer;

    NetworkInstructionsComponent(std::string n, int id) : name(n), id(id) {}
};