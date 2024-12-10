#pragma once
#include "raylib.h"
#include "UIStyle.hpp"
#include <memory>

class IWidget {
public:
    virtual ~IWidget() = default;
    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;
    virtual void HandleEvent() = 0;
    virtual Rectangle GetBounds() const = 0;
    virtual void SetSize(Vector2 size) = 0;
    virtual bool IsFocused() const = 0;
    virtual void SetFocus(bool focus) = 0;

    // New methods for grid positions
    virtual void SetGridPosition(int row, int col) = 0;
    virtual std::pair<int, int> GetGridPosition() const = 0;
};
