#include <string>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <netinet/in.h>
#endif

struct PlayerNetworkComponents {
    std::string name;
    struct sockaddr_in address;
    std::string lastMessageReceived;

    PlayerNetworkComponents(std::string n, struct sockaddr_in addr)
        : name(n), address(addr) {}
};
