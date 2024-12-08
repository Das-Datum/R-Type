#pragma once
#include <float.h>
#include "../../../../ECS/includes/ECS.hpp"
#include "../Components.hpp"
#include "../../../shared/includes/Components/GameComponents.hpp"
#include "../../includes/EntitiesManager.hpp"
#include "raylib.h"
#include "raymath.h"

class EnemiesSystem : public System {
public:
    void update(float deltaTime) {
        for (auto const& entity : entities) {
            if (!gCoordinator.hasComponent<EnemyComponent>(entity))
                continue;

            auto &enemy = gCoordinator.getComponent<EnemyComponent>(entity);

            bool playerFound = false;
            Vector2 playerPos = {0, 0};
            float closestDistance = FLT_MAX;

            Vector2 enemyPos = {0,0};
            if (gCoordinator.hasComponent<TransformComponent>(entity)) {
                enemyPos = gCoordinator.getComponent<TransformComponent>(entity).position;
            }

            for (auto const& candidate : entities) {
                if (candidate == entity) continue;
                if (gCoordinator.hasComponent<ShipComponent>(candidate) &&
                    gCoordinator.hasComponent<TransformComponent>(candidate)) {

                    auto &playerTransform = gCoordinator.getComponent<TransformComponent>(candidate);
                    float dist = Vector2Distance(enemyPos, playerTransform.position);

                    if (dist < closestDistance) {
                        closestDistance = dist;
                        playerPos = playerTransform.position;
                        playerFound = true;
                    }
                }
            }

            for (auto behavior : enemy.behaviors) {
                switch (behavior) {
                    case BehaviorType::ChasePlayer:
                        if (playerFound) chasePlayer(entity, deltaTime, playerPos);
                        break;
                    case BehaviorType::Patrol:
                        patrol(entity, deltaTime);
                        break;
                    case BehaviorType::ShootAtPlayer:
                        if (playerFound) shootAtPlayer(entity, deltaTime, playerPos);
                        break;
                    case BehaviorType::FleeFromPlayer:
                        if (playerFound) fleeFromPlayer(entity, deltaTime, playerPos);
                        break;
                    case BehaviorType::RandomMovement:
                        randomMovement(entity, deltaTime);
                        break;
                    default:
                        break;
                }
            }
        }
    }

private:
    void chasePlayer(Entity entity, float deltaTime, const Vector2& playerPos) {
        if (!gCoordinator.hasComponent<TransformComponent>(entity) ||
            !gCoordinator.hasComponent<EnemyMovementComponent>(entity))
            return;

        auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
        auto &movement = gCoordinator.getComponent<EnemyMovementComponent>(entity);

        Vector2 direction = Vector2Subtract(playerPos, transform.position);
        float distance = Vector2Length(direction);
        if (distance > 1.0f) {
            direction = Vector2Normalize(direction);
            Vector2 velocity = Vector2Scale(direction, movement.speed * deltaTime);
            transform.position = Vector2Add(transform.position, velocity);
        }
    }

    void patrol(Entity entity, float deltaTime) {
        if (!gCoordinator.hasComponent<TransformComponent>(entity) ||
            !gCoordinator.hasComponent<EnemyMovementComponent>(entity))
            return;

        auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
        auto &movement = gCoordinator.getComponent<EnemyMovementComponent>(entity);

        Vector2 target = movement.goingToEnd ? movement.patrolEnd : movement.patrolStart;
        Vector2 direction = Vector2Subtract(target, transform.position);
        float distance = Vector2Length(direction);

        if (distance < 5.0f) {
            movement.goingToEnd = !movement.goingToEnd;
        } else {
            direction = Vector2Normalize(direction);
            Vector2 velocity = Vector2Scale(direction, movement.speed * deltaTime);
            transform.position = Vector2Add(transform.position, velocity);
        }
    }

    void shootAtPlayer(Entity entity, float deltaTime, const Vector2& playerPos) {
        if (!gCoordinator.hasComponent<TransformComponent>(entity) || !gCoordinator.hasComponent<EnemyShootComponent>(entity)) {
            return;
        }

        auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
        auto &shoot = gCoordinator.getComponent<EnemyShootComponent>(entity);
        float distance = Vector2Distance(transform.position, playerPos);

        if (distance <= shoot.shootRange) {
            shoot.shootCooldown -= deltaTime;
            if (shoot.shootCooldown <= 0.0f) {
                shoot.shootCooldown = shoot.shootInterval;
                auto &entitiesManager = EntitiesManager::getInstance();

                Vector2 direction = Vector2Normalize(Vector2Subtract(playerPos, transform.position));
                Vector2 bulletPos = transform.position;
                Vector2 normalizedVel = Vector2Normalize(Vector2Scale(direction, shoot.bulletSpeed));
                entitiesManager.createEnemyBullet(bulletPos, normalizedVel);
            }
        }
    }

    void fleeFromPlayer(Entity entity, float deltaTime, const Vector2& playerPos) {
        if (!gCoordinator.hasComponent<TransformComponent>(entity) ||
            !gCoordinator.hasComponent<EnemyMovementComponent>(entity))
            return;

        auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
        auto &movement = gCoordinator.getComponent<EnemyMovementComponent>(entity);

        Vector2 direction = Vector2Subtract(transform.position, playerPos);
        float distance = Vector2Length(direction);
        if (distance < movement.fleeDistance) {
            direction = Vector2Normalize(direction);
            Vector2 velocity = Vector2Scale(direction, movement.speed * deltaTime);
            transform.position = Vector2Add(transform.position, velocity);
        }
    }

    void randomMovement(Entity entity, float deltaTime) {
        if (!gCoordinator.hasComponent<TransformComponent>(entity) ||
            !gCoordinator.hasComponent<EnemyMovementComponent>(entity))
            return;

        auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
        auto &movement = gCoordinator.getComponent<EnemyMovementComponent>(entity);

        movement.randomMovementTimer -= deltaTime;
        if (movement.randomMovementTimer <= 0.0f) {
            float angle = GetRandomValue(0, 360) * (PI / 180.0f);
            movement.randomDirection = { cosf(angle), sinf(angle) };
            movement.randomMovementTimer = movement.randomMovementInterval;
        }

        movement.randomDirection.x = -1.0f;

        Vector2 velocity = Vector2Scale(movement.randomDirection, movement.speed * deltaTime);
        transform.position = Vector2Add(transform.position, velocity);

        if (transform.position.y < 0) {
            transform.position.y = 0;
            movement.randomDirection.y = fabs(movement.randomDirection.y);
        } else if (transform.position.y > 1080) {
            transform.position.y = 1080;
            movement.randomDirection.y = -fabs(movement.randomDirection.y);
        }
    }
};
