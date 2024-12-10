#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../Components.hpp"
#include <cmath>

class BackgroundScrollSystem : public System {
  public:
    void update(float deltaTime) {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        for (auto const &entity : entities) {
            auto &transform = gCoordinator.getComponent<TransformComponent>(entity);
            auto &sprite = gCoordinator.getComponent<SpriteComponent>(entity);
            auto &backgroundScroll = gCoordinator.getComponent<BackgroundScrollComponent>(entity);

            backgroundScroll.offset += backgroundScroll.scrollSpeed * deltaTime;

            float realWidth = backgroundScroll.textureWidth * baseWidth;

            if (backgroundScroll.offset >= realWidth) {
                backgroundScroll.offset = 0.0f;
            }
        }
    }

    private:
        float baseWidth = 1920.0f;
        float baseHeight = 1080.0f;
};
