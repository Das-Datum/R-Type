#pragma once
#include <raylib.h>

struct TransformComponent {
    Vector2 position;
    float rotation;
    Vector2 scale;

    TransformComponent(Vector2 pos = {0, 0}, float rot = 0.0f, Vector2 scl = {1, 1})
        : position(pos), rotation(rot), scale(scl) {}
};

struct ShipComponent {};

struct BulletComponent {
    Vector2 velocity;
};

struct InputComponent {};
