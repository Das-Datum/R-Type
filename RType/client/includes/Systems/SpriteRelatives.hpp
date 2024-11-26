#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../Components.hpp"

class SpriteFrameSystem : public System {
  public:
    void update() {
        for (auto const &entity : entities) {
            auto &spriteFrame = gCoordinator.getComponent<SpriteFrameComponent>(entity);

            if (IsKeyPressed(KEY_UP)) {
                spriteFrame.nextFrame();
            }

            if (IsKeyPressed(KEY_DOWN)) {
                spriteFrame.prevFrame();
            }

            if (IsKeyReleased(KEY_UP)) {
                spriteFrame.prevFrame();
            }

            if (IsKeyReleased(KEY_DOWN)) {
                spriteFrame.nextFrame();
            }
        }
    }
};
