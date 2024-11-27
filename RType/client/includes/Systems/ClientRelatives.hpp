#pragma once
#include <cmath>
#include "../../../../ECS/includes/ECS.hpp"
#include "../../../shared/includes/Components/GameComponents.hpp"
#include "../Components.hpp"
#include "../../includes/EntitiesManager.hpp"

class RenderSystem : public System {
  public:
    void update() {
        std::vector<std::pair<int, Entity>> sortedEntities;
        for (auto const &entity : entities) {
            auto &sprite = gCoordinator.getComponent<SpriteComponent>(entity);
            sortedEntities.emplace_back(sprite.zIndex, entity);
        }

        std::sort(sortedEntities.begin(), sortedEntities.end(),
            [](const std::pair<int, Entity> &a, const std::pair<int, Entity> &b) {
                return a.first < b.first;
            });

        for (auto const &pair : sortedEntities) {
            Entity entity = pair.second;

            //? HANDLE SPRITE TYPE
            if (gCoordinator.hasComponent<BackgroundScrollComponent>(entity)) {

                //! IS A BACKGROUND (SCROLLING)
                auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
                auto &sprite = gCoordinator.getComponent<SpriteComponent>(entity);
                auto &backgroundScroll = gCoordinator.getComponent<BackgroundScrollComponent>(entity);

                float scaleX = transform.size.x / sprite.sourceRect.width;
                float effectiveOffset = fmod(backgroundScroll.offset, transform.size.x);

                for (int i = -1; i <= 1; i++) {
                    Rectangle sourceRect = sprite.sourceRect;
                    Rectangle destRect = {
                        transform.position.x + (transform.size.x * i) - effectiveOffset,
                        transform.position.y,
                        transform.size.x,
                        transform.size.y
                    };

                    DrawTexturePro(sprite.texture, sourceRect, destRect, Vector2{0, 0}, transform.rotation, WHITE);
                }

            } else {

                //! IS A REGULAR SPRITE
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
            }
        }

        //! Developer Tool (Draw Colliders)
        if (IsKeyDown(KEY_C)) {
            for (auto const &pair : sortedEntities) {
                Entity entity = pair.second;
                if (gCoordinator.hasComponent<CollisionComponent>(entity)) {
                    auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
                    auto &collider = gCoordinator.getComponent<CollisionComponent>(entity);
                    Rectangle rect = {
                        transform.position.x + collider.collider.x,
                        transform.position.y + collider.collider.y,
                        collider.collider.width,
                        collider.collider.height
                    };
                    DrawRectangleLinesEx(rect, 1.0f, RED);
                }
            }
        }
    }
};

class InputSystem : public System {
  public:
    void update() {
        for (auto const &entity : entities) {
            auto &transform = gCoordinator.getComponent<TransformComponent>(entity);

            float speed = 400.0f * GetFrameTime();
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
                auto& entitiesManager = EntitiesManager::getInstance();

                Vector2 bulletPosition = {transform.position.x + transform.size.x / 2,
                    transform.position.y};
                if (timer.elapsedTime >= timer.duration) {
                    entitiesManager.createBullet(bulletPosition, {200.0f, 0.0f});
                } else {
                    entitiesManager.createBullet(bulletPosition, {400.0f, 0.0f});
                }
                timer.active = false;
                timer.elapsedTime = 0.0f;
            }
        }
    }
};
