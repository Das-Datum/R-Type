
#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../Components.hpp"
#include "../../../shared/includes/Components/GameComponents.hpp"

class CollisionSystem : public System {
  public:
    void update() {
        std::vector<Entity> entityList(entities.begin(), entities.end());

        for (size_t i = 0; i < entityList.size(); ++i) {
            for (size_t j = i + 1; j < entityList.size(); ++j) {
                Entity entityA = entityList[i];
                Entity entityB = entityList[j];

                auto& transformA = gCoordinator.getComponent<TransformComponent>(entityA);
                auto& colliderA = gCoordinator.getComponent<CollisionComponent>(entityA);

                auto& transformB = gCoordinator.getComponent<TransformComponent>(entityB);
                auto& colliderB = gCoordinator.getComponent<CollisionComponent>(entityB);

                Rectangle rectA = {
                    transformA.position.x,
                    transformA.position.y,
                    colliderA.collider.width,
                    colliderA.collider.height
                };

                Rectangle rectB = {
                    transformB.position.x,
                    transformB.position.y,
                    colliderB.collider.width,
                    colliderB.collider.height
                };

                if (CheckCollisionRecs(rectA, rectB)) {
                    printf("Collision detected between entities %d and %d\n", entityA, entityB);
                }
            }
        }
    }
};
