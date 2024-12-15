#pragma once
#include <raylib.h>
#include "EnemiesComponents.hpp"

struct TransformComponent {
    Vector2 position;
    float rotation;
    Vector2 scale;
    Vector2 size;

    TransformComponent(Vector2 pos = {0, 0}, float rot = 0.0f, Vector2 scl = {1, 1}, Vector2 sz = {1, 1})
        : position(pos), rotation(rot), scale(scl), size(sz) {}
};

struct VelocityComponent {
    Vector2 velocity;
    float maxSpeed;
    float acceleration;
    float deceleration;

    VelocityComponent(Vector2 vel = {0.0f, 0.0f}, float max = 1.0f)
        : velocity(vel), maxSpeed(max),
          acceleration(6.0f), deceleration(4.0f) {}
};

struct SpawnComponent {
    float time_left;

    SpawnComponent(float time = 0.0f) : time_left(time) {}
};

struct FixedVelocityComponent {
    Vector2 velocity;

    FixedVelocityComponent(Vector2 vel = {0, 0}) : velocity(vel) {}
};

struct DestroyOutOfBoundsComponent {};
struct BlockOutOfBoundsComponent {};

struct CollisionComponent {
    Rectangle collider;
    float rotation;

    CollisionComponent(Rectangle col = {0, 0, 0, 0}, float rot = 0.0f)
        : collider(col), rotation(rot) {}
};

struct ShipComponent {};

struct BulletComponent {
    float size;
    Color color;

    BulletComponent(float sz = 2.0f, Color clr = RED)
        : size(sz), color(clr) {}
};

struct NetworkPositionComponent {
    Vector2 lastPosition;
    Vector2 targetPosition;
    float lerpFactor;

    NetworkPositionComponent(Vector2 pos = {0.0f, 0.0f})
        : lastPosition(pos), targetPosition(pos), lerpFactor(0.0f) {}
};
