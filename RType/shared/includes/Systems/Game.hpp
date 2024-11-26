#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../Components/GameComponents.hpp"

class PhysicsSystem : public System {
  public:
    void update(float frameTime = 0.0f) {
        for (auto const &entity : entities) {
            if (gCoordinator.hasComponent<PhysicsComponent>(entity)) {
                auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
                auto &mob = gCoordinator.getComponent<PhysicsComponent>(entity);

                transform.position.x += mob.velocity.x * frameTime;
                transform.position.y += mob.velocity.y * frameTime;

                if (transform.position.y < 0) {
                    gCoordinator.destroyEntity(entity);
                }
            }
        }
    }
};
