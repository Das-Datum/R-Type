#pragma once
#include "ECS.hpp"
#include "Components.hpp"

class SpriteFrameSystem : public System {
  public:
    void update() {
        auto &settings = Settings::getInstance();

        for (auto const &entity : entities) {
            auto &spriteFrame = gCoordinator.getComponent<SpriteFrameComponent>(entity);

            //? Ship sprite animation on move
            if (gCoordinator.hasComponent<ShipComponent>(entity)) {
                if (IsKeyPressed(settings.getMoveUpKey())) {
                    spriteFrame.nextFrame();
                }

                if (IsKeyPressed(settings.getMoveDownKey())) {
                    spriteFrame.prevFrame();
                }

                if (IsKeyReleased(settings.getMoveUpKey())) {
                    spriteFrame.prevFrame();
                }

                if (IsKeyReleased(settings.getMoveDownKey())) {
                    spriteFrame.nextFrame();
                }
            }
        }
    }
};
