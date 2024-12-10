#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../../../../Network/includes/NetworkComponents.hpp"
#include "../Components.hpp"
#include "../../shared/includes/Components/GameComponents.hpp"
#include "../EntitiesManager.hpp"

class CoreSystem : public System {
public:
    void updateServer(Entity& server) {
        auto& network = gCoordinator.getComponent<NetworkComponents>(server);

        for (auto const &lastMsg : network.lastMessagesReceived) {
            if (lastMsg == "NEW") {
                auto& entitiesManager = EntitiesManager::getInstance();
                entitiesManager.createPlayer({100, 100}, "Player", network.ip, network.port, network.id);
            } else
            if (lastMsg == "STOP") {
                gCoordinator.destroyEntity(server);
                return;
            }
        }
    }

    void update(float deltaTime) {
        for (auto const &entity : entities) {
            auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
            auto &fixedVelocity = gCoordinator.getComponent<FixedVelocityComponent>(entity);

            transform.position.x += fixedVelocity.velocity.x * deltaTime;
            transform.position.y += fixedVelocity.velocity.y * deltaTime;
        }
    }
};