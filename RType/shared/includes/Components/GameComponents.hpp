#pragma once
#include <raylib.h>

struct TransformComponent {
    Vector2 position;
    float rotation;
    Vector2 scale;
    Vector2 size;

    TransformComponent(Vector2 pos = {0, 0}, float rot = 0.0f, Vector2 scl = {1, 1}, Vector2 sz = {1, 1})
        : position(pos), rotation(rot), scale(scl), size(sz) {}
};

struct PhysicsComponent {
    Vector2 velocity;
};
