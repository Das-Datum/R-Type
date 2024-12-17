
#pragma once
#include "ECS.hpp"
#include "Components/GameComponents.hpp"

class InterpolationSystem : public System {
  public:
    void update(float deltaTime) {
        for (auto const &entity : entities) {
            auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
            auto &networkPos = gCoordinator.getComponent<NetworkPositionComponent>(entity);

            if (gCoordinator.hasComponent<InputComponent>(entity)) {
              if (gCoordinator.hasComponent<VelocityComponent>(entity)) {
                auto &velocity = gCoordinator.getComponent<VelocityComponent>(entity);

                if (abs(velocity.velocity.x) > 0.0f || abs(velocity.velocity.y) > 0.0f) {
                  networkPos.lastPosition = transform.position;
                  networkPos.targetPosition = transform.position;
                  networkPos.lerpFactor = 0.0f;
                  continue;
                }
              }
            }

            networkPos.lerpFactor += deltaTime * interpolationSpeed;
            if (networkPos.lerpFactor > 1.0f)
                networkPos.lerpFactor = 1.0f;

            transform.position = Vector2Lerp(networkPos.lastPosition,
                                             networkPos.targetPosition,
                                             networkPos.lerpFactor);
        }
    }

  private:
    const float interpolationSpeed = 2.0f;
};
