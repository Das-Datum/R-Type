#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../Components.hpp"
#include "../EntitiesManager.hpp"
#include "../../../shared/includes/Components/GameComponents.hpp"

class ClientNetworkSystem : public ClientSystem {
  public:
    Entity createNewPlayer(int id, std::string name, Vector2 pos) {
        Entity player = EntitiesManager::getInstance().createPlayer(pos);
        return player;
    }

    void update() {
        std::string message = update_read();
        if (message == "")
            return;
        if (message == "NEW")
            createNewPlayer(0, "Player", Vector2{0, 0});
    }
};
