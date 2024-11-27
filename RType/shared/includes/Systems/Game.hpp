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

                if (transform.position.x > gameWidth || transform.position.x < 0 || transform.position.y > gameHeight || transform.position.y < 0) {
                    gCoordinator.destroyEntity(entity);
                }
            }
        }
    }

    void setGameDimensions(float width = 1280, float height = 720) {
        gameWidth = width;
        gameHeight = height;
    }

    private:
        float gameWidth = 1280;
        float gameHeight = 720;
};
