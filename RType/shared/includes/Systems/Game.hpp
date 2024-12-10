#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../Components/GameComponents.hpp"

class PhysicsSystem : public System {
  public:
    void update(float frameTime = 0.0f) {
        for (auto const &entity : entities) {
            if (!gCoordinator.hasComponent<TransformComponent>(entity))
                continue;

            if (gCoordinator.hasComponent<FixedVelocityComponent>(entity)) {
                auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
                auto &mob = gCoordinator.getComponent<FixedVelocityComponent>(entity);

                transform.position.x += mob.velocity.x * frameTime;
                transform.position.y += mob.velocity.y * frameTime;
            }

            if (gCoordinator.hasComponent<DestroyOutOfBoundsComponent>(entity)) {
                auto &transform = gCoordinator.getComponent<TransformComponent>(entity);

                if (transform.position.x > gameWidth || transform.position.x < 0 || transform.position.y > gameHeight || transform.position.y < 0) {
                    gCoordinator.destroyEntity(entity);
                }
            }

            if (gCoordinator.hasComponent<BlockOutOfBoundsComponent>(entity)) {
                auto &transform = gCoordinator.getComponent<TransformComponent>(entity);

                if (transform.position.x > gameWidth || transform.position.x < 0 || transform.position.y > gameHeight || transform.position.y < 0) {
                    transform.position.x = std::clamp(transform.position.x, 0.0f, gameWidth);
                    transform.position.y = std::clamp(transform.position.y, 0.0f, gameHeight);
                }
            }
        }
    }

    void setGameDimensions(float width = 1920, float height = 1080) {
        gameWidth = width;
        gameHeight = height;
    }

    private:
        float gameWidth = 1920;
        float gameHeight = 1080;
};

class SpawnSystem : public System {
    public:
        void update(float frameTime = 0.0f) {
            for (const auto& entity : entities) {
                auto &spawn = gCoordinator.getComponent<SpawnComponent>(entity);

                spawn.time_left -= frameTime;
            }
        }
};
