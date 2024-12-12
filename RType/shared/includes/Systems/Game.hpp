#pragma once
#include "ECS.hpp"
#include "Components/GameComponents.hpp"

class PhysicsSystem : public System {
  public:
    void update(float frameTime = 0.0f) {
        for (auto const &entity : entities) {
            if (!gCoordinator.hasComponent<TransformComponent>(entity))
                continue;

            auto &transform = gCoordinator.getComponent<TransformComponent>(entity);

            if (gCoordinator.hasComponent<FixedVelocityComponent>(entity)) {
                auto &mob = gCoordinator.getComponent<FixedVelocityComponent>(entity);
                transform.position.x += mob.velocity.x * frameTime;
                transform.position.y += mob.velocity.y * frameTime;
            }

            if (gCoordinator.hasComponent<DestroyOutOfBoundsComponent>(entity)) {
                bool shouldDestroy = transform.position.x > 1.05f || transform.position.x < -0.05f ||
                                     transform.position.y > 1.05f || transform.position.y < -0.05f;
                if (shouldDestroy) {
                    gCoordinator.destroyEntity(entity);
                }
            }

            if (gCoordinator.hasComponent<BlockOutOfBoundsComponent>(entity)) {
                auto &collider = gCoordinator.getComponent<CollisionComponent>(entity);
                float normalizedWidth = transform.size.x;
                float normalizedHeight = transform.size.y;

                transform.position.x = std::clamp(transform.position.x, 0.0f, 1.0f - normalizedWidth);
                transform.position.y = std::clamp(transform.position.y, 0.0f, 1.0f - normalizedHeight);
            }
        }
    }

    void setViewport(float width, float height) {
        viewportWidth = width;
        viewportHeight = height;
    }

  private:
    const float baseWidth = 1920.0f;
    const float baseHeight = 1080.0f;
    float viewportWidth = 1920.0f;
    float viewportHeight = 1080.0f;
};

class SpawnSystem : public System {
    public:
        void update(float frameTime = 0.0f) {
            for (const auto& entity : entities) {
                if (!gCoordinator.hasComponent<SpawnComponent>(entity)) {
                    std::cout << "No SpawnComponent for entity " << entity << std::endl;
                    continue;
                }
                auto &spawn = gCoordinator.getComponent<SpawnComponent>(entity);
                if (spawn.time_left <= 0.0f) {
                    std::cout << "Removing SpawnComponent for entity " << entity << std::endl;
                    gCoordinator.removeComponent<SpawnComponent>(entity);
                    break;
                } else {
                    spawn.time_left -= frameTime;
                }
            }
        }
};
