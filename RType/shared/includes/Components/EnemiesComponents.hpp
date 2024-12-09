#pragma once
#include <raylib.h>
#include <algorithm>

enum class BehaviorType {
    ChasePlayer,
    Patrol,
    ShootAtPlayer,
    FleeFromPlayer,
    RandomMovement,
    None
};

struct EnemyComponent {
    std::vector<BehaviorType> behaviors;

    EnemyComponent(std::initializer_list<BehaviorType> behaviorList = {})
        : behaviors(behaviorList) {}

    void addBehavior(BehaviorType behavior) {
        behaviors.push_back(behavior);
    }

    void removeBehavior(BehaviorType behavior) {
        behaviors.erase(std::remove(behaviors.begin(), behaviors.end(), behavior), behaviors.end());
    }
};

struct EnemyMovementComponent {
    float speed;
    Vector2 patrolStart;
    Vector2 patrolEnd;
    bool goingToEnd;
    float fleeDistance;
    float randomMovementInterval;
    float randomMovementTimer;
    Vector2 randomDirection = {0, 0};

    EnemyMovementComponent(float spd = 100.0f, Vector2 start = {0, 0}, Vector2 end = {0, 0}, float fleeDist = 100.0f, float interval = 1.0f)
        : speed(spd), patrolStart(start), patrolEnd(end), goingToEnd(true), fleeDistance(fleeDist), randomMovementInterval(interval), randomMovementTimer(interval) {}
};

struct EnemyShootComponent {
    float shootRange;
    float shootInterval;
    float shootCooldown;
    float bulletSpeed;

    EnemyShootComponent(float range = 100.0f, float interval = 1.0f, float speed = 200.0f)
        : shootRange(range), shootInterval(interval), shootCooldown(interval), bulletSpeed(speed) {}
};
