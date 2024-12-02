#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../Components.hpp"
#include <cmath>

class BackgroundScrollSystem : public System {
  public:
    void update(float deltaTime) {
        for (auto const &entity : entities) {
            auto &backgroundScroll = gCoordinator.getComponent<BackgroundScrollComponent>(entity);
            backgroundScroll.offset += backgroundScroll.scrollSpeed * deltaTime;

            float effectiveWidth = (backgroundScroll.textureWidth * backgroundScroll.windowWidth) / backgroundScroll.textureWidth;

            if (backgroundScroll.offset >= effectiveWidth) {
                backgroundScroll.offset = fmod(backgroundScroll.offset, effectiveWidth);
            }
        }
    }

  private:
    float backgroundWidth;
};
