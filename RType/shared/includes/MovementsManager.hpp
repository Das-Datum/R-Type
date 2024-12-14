#pragma once

#include "ECS.hpp"
#include "Components/GameComponents.hpp"
#include "raylib.h"
#include "raymath.h"
#include <cmath>

class MovementsManager {
public:
    static MovementsManager& getInstance() {
        static MovementsManager instance;
        return instance;
    }

    Vector2 calculateMovement(Entity entity, Vector2 direction, float deltaTime) {
        auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
        auto &velocity = gCoordinator.getComponent<VelocityComponent>(entity);

        const float acceleration = 6.0f;
        const float deceleration = 4.0f;

        if (Vector2Length(direction) > 0) {
            direction = Vector2Normalize(direction);
        }

        if (Vector2Length(direction) > 0) {
            velocity.velocity.x = lerp(velocity.velocity.x, direction.x * velocity.maxSpeed, acceleration * deltaTime);
            velocity.velocity.y = lerp(velocity.velocity.y, direction.y * velocity.maxSpeed, acceleration * deltaTime);
        } else {
            velocity.velocity.x = lerp(velocity.velocity.x, 0.0f, deceleration * deltaTime);
            velocity.velocity.y = lerp(velocity.velocity.y, 0.0f, deceleration * deltaTime);
        }

        transform.position.x += (velocity.velocity.x * deltaTime);
        transform.position.y += (velocity.velocity.y * deltaTime) * (16.0 / 9.0);

        return {velocity.velocity.x * deltaTime, velocity.velocity.y * deltaTime};
    }

private:
    MovementsManager() {}

    float lerp(float start, float end, float amount) {
        return start + amount * (end - start);
    }
};
