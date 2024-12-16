
#pragma once
#include "ECS.hpp"
#include "Components/GameComponents.hpp"

class InterpolationSystem : public System {
  public:
    void update(float deltaTime) {
        for (auto const &entity : entities) {
            auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
            auto &networkPos = gCoordinator.getComponent<NetworkPositionComponent>(entity);

            networkPos.lerpFactor += deltaTime * interpolationSpeed;
            if (networkPos.lerpFactor > 1.0f)
                networkPos.lerpFactor = 1.0f;

            transform.position = Vector2Lerp(networkPos.lastPosition,
                                             networkPos.targetPosition,
                                             networkPos.lerpFactor);
        }
    }

  private:
    const float interpolationSpeed = 3.0f;
};