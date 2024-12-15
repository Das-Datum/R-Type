#pragma once

#include "ECS.hpp"
#include "Components/GameComponents.hpp"
#include "raylib.h"
#include "raymath.h"
#include <cmath>

class VelocitySystem : public System {
public:
    void update(float deltaTime) {
        for (auto const& entity : entities) {
            auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
            auto &velocity = gCoordinator.getComponent<VelocityComponent>(entity);

            float currentSpeed = Vector2Length(velocity.velocity);
            if (currentSpeed > velocity.maxSpeed) {
                velocity.velocity = Vector2Scale(
                    Vector2Normalize(velocity.velocity),
                    velocity.maxSpeed
                );
            }

            velocity.velocity.x -= velocity.velocity.x * velocity.deceleration * deltaTime;
            velocity.velocity.y -= velocity.velocity.y * velocity.deceleration * deltaTime;

            if (fabs(velocity.velocity.x) <= 0.05f) velocity.velocity.x = 0.0f;
            if (fabs(velocity.velocity.y) <= 0.05f) velocity.velocity.y = 0.0f;

            transform.position.x += velocity.velocity.x * deltaTime;
            transform.position.y += velocity.velocity.y * deltaTime * (16.0f / 9.0f);
        }
    }
};
