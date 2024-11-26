#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../../../shared/includes/Components/GameComponents.hpp"
#include "../Components.hpp"

class RenderSystem : public System {
  public:
    void update() {
        for (auto const &entity : entities) {
            auto &transform = gCoordinator.getComponent<TransformComponent>(entity);

            if (gCoordinator.hasComponent<SpriteComponent>(entity)) {
                auto &sprite = gCoordinator.getComponent<SpriteComponent>(entity);

                if (gCoordinator.hasComponent<SpriteAnimationComponent>(entity)) {
                    auto &animation = gCoordinator.getComponent<SpriteAnimationComponent>(entity);
                    animation.elapsedTime += GetFrameTime();
                    if (animation.elapsedTime >= animation.frameDuration) {
                        animation.currentFrame =
                            (animation.currentFrame + 1) % animation.frameCount;
                        animation.elapsedTime = 0.0f;

                        sprite.sourceRect.x = animation.currentFrame * sprite.sourceRect.width;
                    }
                }

                if (gCoordinator.hasComponent<SpriteFrameComponent>(entity)) {
                    auto &spriteFrame = gCoordinator.getComponent<SpriteFrameComponent>(entity);
                    sprite.sourceRect.x = spriteFrame.frameIndex * sprite.sourceRect.width;
                }

                DrawTexturePro(sprite.texture,
                    sprite.sourceRect,
                    Rectangle{transform.position.x,
                        transform.position.y,
                        transform.size.x,
                        transform.size.y},
                    Vector2{0, 0},
                    transform.rotation,
                    WHITE);
            }

            //! Bullet (temporary)
            if (gCoordinator.hasComponent<BulletComponent>(entity)) {
                auto &bullet = gCoordinator.getComponent<BulletComponent>(entity);
                DrawCircleV(transform.position, bullet.size, bullet.color);
            }
        }
    }
};

class InputSystem : public System {
  public:
    void update() {
        for (auto const &entity : entities) {
            auto &transform = gCoordinator.getComponent<TransformComponent>(entity);

            float speed = 200.0f * GetFrameTime();
            if (IsKeyDown(KEY_RIGHT))
                transform.position.x += speed;
            if (IsKeyDown(KEY_LEFT))
                transform.position.x -= speed;
            if (IsKeyDown(KEY_UP))
                transform.position.y -= speed;
            if (IsKeyDown(KEY_DOWN))
                transform.position.y += speed;

            auto &timer = gCoordinator.getComponent<TimerComponent>(entity);

            if (IsKeyPressed(KEY_SPACE)) {
                timer.active = true;
                timer.elapsedTime = 0.0f;
                timer.duration = 3.0f;
            }

            if (IsKeyDown(KEY_SPACE) && timer.active) {
                timer.elapsedTime += GetFrameTime();
                if (timer.elapsedTime >= timer.duration) {
                    timer.elapsedTime = timer.duration;
                }
            }

            if (IsKeyReleased(KEY_SPACE) && timer.active) {
                Vector2 bulletPosition = {transform.position.x + transform.size.x / 2,
                    transform.position.y};
                if (timer.elapsedTime >= timer.duration) {
                    Entity powerShotBullet = gCoordinator.createEntity();
                    gCoordinator.addComponent(powerShotBullet, TransformComponent(bulletPosition));
                    gCoordinator.addComponent(powerShotBullet, BulletComponent(5.0f, ORANGE));
                    gCoordinator.addComponent(powerShotBullet, PhysicsComponent({200.0f, 0.0f}));
                } else {
                    Entity bullet = gCoordinator.createEntity();
                    gCoordinator.addComponent(bullet, TransformComponent(bulletPosition));
                    gCoordinator.addComponent(bullet, BulletComponent(2.0f, RED));
                    gCoordinator.addComponent(bullet, PhysicsComponent({300.0f, 0.0f}));
                }
                timer.active = false;
                timer.elapsedTime = 0.0f;
            }
        }
    }
};
