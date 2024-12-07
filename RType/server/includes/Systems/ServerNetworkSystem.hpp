#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../Components.hpp"
#include "../EntitiesManager.hpp"
#include "../../../shared/includes/Components/GameComponents.hpp"

class ServerNetworkSystem : public ServerSystem {
  public:
    Entity createNewClient(sockaddr_in client_addr, std::string name, Vector2 pos) override {
        Entity player = EntitiesManager::getInstance().createPlayer(pos, client_addr, name);
        return player;
    }
};