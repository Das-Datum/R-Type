#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../../../shared/includes/Components/GameComponents.hpp"
#include "ClientEntitiesManager.hpp"
#include "../Components.hpp"
#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>
#include <vector>

class RenderSystem : public System {
  public:
    float baseWidth = 1920.0f;
    float baseHeight = 1080.0f;
    float viewportX = 0.0f;
    float viewportY = 0.0f;
    float viewportWidth = 0.0f;
    float viewportHeight = 0.0f;

    void setViewport(float x, float y, float width, float height) {
        viewportX = x;
        viewportY = y;
        viewportWidth = width;
        viewportHeight = height;
    }

    void update() {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        float aspectRatio = 16.0f / 9.0f;
        float scale = fmin(screenWidth / baseWidth, screenHeight / baseHeight);
        viewportWidth = baseWidth * scale;
        viewportHeight = baseHeight * scale;
        viewportX = (screenWidth - viewportWidth) * 0.5f;
        viewportY = (screenHeight - viewportHeight) * 0.5f;

        std::vector<std::pair<int, Entity>> sortedEntities;
        for (auto const &entity : entities) {
            auto &sprite = gCoordinator.getComponent<SpriteComponent>(entity);
            sortedEntities.emplace_back(sprite.zIndex, entity);
        }

        std::sort(sortedEntities.begin(), sortedEntities.end(), [](const std::pair<int, Entity> &a, const std::pair<int, Entity> &b) {
            return a.first < b.first;
        });

        for (auto const &pair : sortedEntities) {
            Entity entity = pair.second;

            if (gCoordinator.hasComponent<BackgroundScrollComponent>(entity)) {
                //! Background rendering
                auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
                auto &sprite = gCoordinator.getComponent<SpriteComponent>(entity);
                auto &backgroundScroll = gCoordinator.getComponent<BackgroundScrollComponent>(entity);

                float bgScaleX = viewportWidth / (float)sprite.texture.width;
                float bgScaleY = viewportHeight / (float)sprite.texture.height;
                float bgScale = fmin(bgScaleX, bgScaleY);
                float scaledWidth = viewportWidth;
                float scaledHeight = viewportHeight;
                float effectiveOffset = fmod(backgroundScroll.offset, transform.size.x);
                float ratio = scaledWidth / transform.size.x;
                float scaledOffset = effectiveOffset * ratio;

                for (int i = -1; i <= 1; i++) {
                    Rectangle sourceRect = sprite.sourceRect;
                    Rectangle destRect = {
                        viewportX + (scaledWidth * i) - scaledOffset,
                        viewportY,
                        scaledWidth,
                        scaledHeight};

                    DrawTexturePro(sprite.texture, sourceRect, destRect, Vector2{0, 0}, transform.rotation, WHITE);
                }

                DrawRectangle(0, 0, screenWidth, (int)viewportY, BLACK);
                DrawRectangle(0, (int)(viewportY + viewportHeight), screenWidth, (int)(screenHeight - (viewportY + viewportHeight)), BLACK);
                DrawRectangle(0, (int)viewportY, (int)viewportX, (int)viewportHeight, BLACK);
                DrawRectangle((int)(viewportX + viewportWidth), (int)viewportY, (int)(screenWidth - (viewportX + viewportWidth)), (int)viewportHeight, BLACK);

            } else {
                //! Entities rendering
                auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
                auto &collider = gCoordinator.getComponent<CollisionComponent>(entity);

                if (gCoordinator.hasComponent<SpriteComponent>(entity)) {
                    auto &sprite = gCoordinator.getComponent<SpriteComponent>(entity);

                    if (gCoordinator.hasComponent<SpriteAnimationComponent>(entity)) {
                        auto &animation = gCoordinator.getComponent<SpriteAnimationComponent>(entity);
                        animation.elapsedTime += GetFrameTime();
                        if (animation.elapsedTime >= animation.frameDuration) {
                            animation.currentFrame = (animation.currentFrame + 1) % animation.frameCount;
                            animation.elapsedTime = 0.0f;
                            sprite.sourceRect.x = animation.currentFrame * sprite.sourceRect.width;
                        }
                    }

                    if (gCoordinator.hasComponent<SpriteFrameComponent>(entity)) {
                        auto &spriteFrame = gCoordinator.getComponent<SpriteFrameComponent>(entity);
                        sprite.sourceRect.x = spriteFrame.frameIndex * sprite.sourceRect.width;
                    }

                    float desiredHeight = transform.size.y * viewportHeight;
                    float spriteAspectRatio = (float)sprite.sourceRect.width / sprite.sourceRect.height;
                    float scaledWidth = desiredHeight * spriteAspectRatio;
                    float scaledHeight = desiredHeight;
                    float scaledX = viewportX + (transform.position.x * viewportWidth);
                    float scaledY = viewportY + (transform.position.y * viewportHeight);

                    Vector2 origin = {scaledWidth * 0.5f, scaledHeight * 0.5f};

                    Rectangle destRect = {
                        scaledX,
                        scaledY,
                        scaledWidth,
                        scaledHeight};

                    DrawTexturePro(sprite.texture, sprite.sourceRect, destRect, origin, collider.rotation, WHITE);
                }
            }
        }

        //! Developer Tool (Draw Colliders)
        if (IsKeyDown(KEY_C)) {

            DrawRectangleLines((int)viewportX, (int)viewportY, (int)viewportWidth, (int)viewportHeight, RED);

            for (auto const &pair : sortedEntities) {
                Entity entity = pair.second;
                if (gCoordinator.hasComponent<CollisionComponent>(entity)) {
                    auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
                    auto &collider = gCoordinator.getComponent<CollisionComponent>(entity);
                    auto &sprite = gCoordinator.getComponent<SpriteComponent>(entity);

                    float angle = collider.rotation * (PI / 180.0f);

                    float desiredHeight = transform.size.y * viewportHeight;
                    float spriteAspectRatio = (float)sprite.sourceRect.width / sprite.sourceRect.height;
                    float scaledWidth = desiredHeight * spriteAspectRatio;
                    float scaledHeight = desiredHeight;
                    float scaledX = viewportX + (transform.position.x * viewportWidth);
                    float scaledY = viewportY + (transform.position.y * viewportHeight);

                    Vector2 origin = {scaledWidth * 0.5f, scaledHeight * 0.5f};

                    Vector2 corners[4] = {
                        {scaledX - origin.x, scaledY - origin.y},
                        {scaledX - origin.x + scaledWidth, scaledY - origin.y},
                        {scaledX - origin.x + scaledWidth, scaledY - origin.y + scaledHeight},
                        {scaledX - origin.x, scaledY - origin.y + scaledHeight}};

                    if (angle != 0) {
                        Vector2 center = {
                            scaledX,
                            scaledY};

                        float cosA = cosf(angle);
                        float sinA = sinf(angle);

                        for (int i = 0; i < 4; i++) {
                            float dx = corners[i].x - center.x;
                            float dy = corners[i].y - center.y;
                            corners[i].x = center.x + (dx * cosA - dy * sinA);
                            corners[i].y = center.y + (dx * sinA + dy * cosA);
                        }
                    }

                    DrawLineEx(corners[0], corners[1], 2.0f, RED);
                    DrawLineEx(corners[1], corners[2], 2.0f, RED);
                    DrawLineEx(corners[2], corners[3], 2.0f, RED);
                    DrawLineEx(corners[3], corners[0], 2.0f, RED);
                }
            }
        }
    }
};

class InputSystem : public System {
  public:
    float baseWidth = 1920.0f;

    void update() {
        for (auto const &entity : entities) {
            auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
            auto &playerNetwork = gCoordinator.getComponent<PlayerNetworkComponent>(entity);
            float speed = (1000.0f / 1920.0f) * GetFrameTime();

            if (IsKeyDown(KEY_RIGHT)) {
                transform.position.x += speed;
                playerNetwork.lastMessagesReceived.push_back("MRT" + std::to_string(playerNetwork.id));
            }
            if (IsKeyDown(KEY_LEFT)) {
                transform.position.x -= speed;
                playerNetwork.lastMessagesReceived.push_back("MLF" + std::to_string(playerNetwork.id));
            }
            if (IsKeyDown(KEY_UP)) {
                transform.position.y -= speed * (16.0 / 9.0);
                playerNetwork.lastMessagesReceived.push_back("MUP" + std::to_string(playerNetwork.id));
            }
            if (IsKeyDown(KEY_DOWN)) {
                transform.position.y += speed * (16.0 / 9.0);
                playerNetwork.lastMessagesReceived.push_back("MDW" + std::to_string(playerNetwork.id));
            }

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
                auto &entitiesManager = EntitiesManager::getInstance();

                Vector2 bulletPosition = transform.position;
                bulletPosition.x += 0.02f;
                bulletPosition.y += 0.01f;

                if (timer.elapsedTime >= timer.duration) {
                    entitiesManager.createBullet(bulletPosition, {0.9f, 0.0f});
                    playerNetwork.lastMessagesReceived.push_back("DEM" + std::to_string(playerNetwork.id));
                } else {
                    playerNetwork.lastMessagesReceived.push_back("SHT" + std::to_string(playerNetwork.id));
                    entitiesManager.createBullet(bulletPosition, {0.5f, 0.0f});
                }
                timer.active = false;
                timer.elapsedTime = 0.0f;
            }
        }
    }
};
