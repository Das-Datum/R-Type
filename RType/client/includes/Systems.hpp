#pragma once
#include "../../../ECS/includes/ECS.hpp"
#include "Components.hpp"

class RenderSystem : public System {
public:
    void update() {
        for (auto const& entity : entities) {
            auto& transform = gCoordinator.getComponent<TransformComponent>(entity);

            // render ship as triangle
            if (gCoordinator.hasComponent<ShipComponent>(entity)) {
                Vector2 vertices[3] = {
                    {transform.position.x + 10, transform.position.y},
                    {transform.position.x - 10, transform.position.y - 10},
                    {transform.position.x - 10, transform.position.y + 10},
                };
                DrawTriangle(vertices[0], vertices[1], vertices[2], BLUE);
            }

            // render bullets (circles)
            if (gCoordinator.hasComponent<BulletComponent>(entity)) {
                DrawCircleV(transform.position, 2, RED);
            }
        }
    }
};

class InputSystem : public System {
public:
    void update() {
        for (auto const& entity : entities) {
            auto& transform = gCoordinator.getComponent<TransformComponent>(entity);

            float speed = 200.0f * GetFrameTime();
            if (IsKeyDown(KEY_RIGHT)) transform.position.x += speed;
            if (IsKeyDown(KEY_LEFT)) transform.position.x -= speed;
            if (IsKeyDown(KEY_UP)) transform.position.y -= speed;
            if (IsKeyDown(KEY_DOWN)) transform.position.y += speed;

            // shoot bullet
            if (IsKeyPressed(KEY_SPACE)) {
                Entity bullet = gCoordinator.createEntity();
                gCoordinator.addComponent(bullet, TransformComponent(transform.position));
                gCoordinator.addComponent(bullet, BulletComponent({300.0f, 0.0f}));
            }
        }
    }
};

class PhysicsSystem : public System {
public:
    void update() {
        for (auto const& entity : entities) {
            if (gCoordinator.hasComponent<BulletComponent>(entity)) {
                auto& transform = gCoordinator.getComponent<TransformComponent>(entity);
                auto& bullet = gCoordinator.getComponent<BulletComponent>(entity);

                transform.position.x += bullet.velocity.x * GetFrameTime();
                transform.position.y += bullet.velocity.y * GetFrameTime();

                if (transform.position.y < 0) {
                    gCoordinator.destroyEntity(entity);
                }
            }
        }
    }
};

class NetworkSystem : public System {
public:
    void update() {}
};
