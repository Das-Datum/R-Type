#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../Components.hpp"

class TimerSystem : public System {
  public:
    void update() {
        for (auto const &entity : entities) {
            auto &timer = gCoordinator.getComponent<TimerComponent>(entity);
            if (timer.active) {
                if (timer.elapsedTime >= timer.duration) {
                    timer.done = true;
                } else {
                    timer.elapsedTime += GetFrameTime();
                }
            }
        }
    }
};
