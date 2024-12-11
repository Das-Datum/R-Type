#pragma once
#include "Menus/IWidget.hpp"
#include "Menus/UIStyle.hpp"
#include "raylib.h"
#include <functional>
#include <memory>
#include <string>

class KeyBindingWidget : public IWidget {
  public:
    KeyBindingWidget(const std::string &actionName, int row, int col, std::function<int()> getKeyFunc, std::function<void(int)> setKeyFunc, const std::shared_ptr<UIStyle> &style)
        : actionName(actionName), gridRow(row), gridCol(col), getKeyFunc(getKeyFunc), setKeyFunc(setKeyFunc), style(style) {}

    void Update(float dt) override {
        (void)dt;
        Vector2 mousePos = GetMousePosition();
        Rectangle bounds = CalculateBounds();
        hovered = CheckCollisionPointRec(mousePos, bounds);

        if (waitingForKey) {
            for (int k = KEY_SPACE; k < 350; ++k) {
                if (IsKeyPressed(k)) {
                    setKeyFunc(k);
                    waitingForKey = false;
                    break;
                }
            }
        } else {
            if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                waitingForKey = true;
            }
        }
    }

    void Draw() override {
        Rectangle bounds = CalculateBounds();
        Color bg = hovered ? style->hoverColor : style->baseColor;
        DrawRectangleRec(bounds, bg);
        DrawRectangleLinesEx(bounds, 2, BLACK);

        std::string displayText = actionName + ": " + (waitingForKey ? "Press a key..." : GetKeyNameStr());
        Vector2 textSize = MeasureTextEx(style->font, displayText.c_str(), style->fontSize, 1);
        DrawTextEx(
            style->font,
            displayText.c_str(),
            {bounds.x + 10.0f,
                bounds.y + (bounds.height - textSize.y) * 0.5f},
            style->fontSize,
            1,
            style->textButtonColor);
    }

    void HandleEvent() override {}

    Rectangle GetBounds() const override { return CalculateBounds(); }

    void SetSize(Vector2 size) override {
        this->size = size;
    }

    void SetGridPosition(int row, int col) override {
        gridRow = row;
        gridCol = col;
    }

    std::pair<int, int> GetGridPosition() const override {
        return {gridRow, gridCol};
    }

    bool IsFocused() const override { return hovered; }
    void SetFocus(bool focus) override { (void)focus; }

  private:
    Rectangle CalculateBounds() const {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        float aspectRatio = 16.0f / 9.0f;

        float baseWidth = screenWidth;
        float baseHeight = baseWidth / aspectRatio;
        if (baseHeight > screenHeight) {
            baseHeight = screenHeight;
            baseWidth = baseHeight * aspectRatio;
        }

        int padding = static_cast<int>(baseWidth * style->grid.padding);
        float cellWidth = (baseWidth - (padding * (style->grid.totalCols + 1))) / static_cast<float>(style->grid.totalCols);
        float cellHeight = (baseHeight - (padding * (style->grid.totalRows + 1))) / static_cast<float>(style->grid.totalRows);
        float startX = (screenWidth - baseWidth) / 2.0f;
        float startY = (screenHeight - baseHeight) / 2.0f;
        float x = startX + padding + (gridCol - 1) * (cellWidth + padding);
        float y = startY + padding + (gridRow - 1) * (cellHeight + padding);

        float btnWidth = cellWidth * 0.9f;
        float btnHeight = cellHeight * 0.6f;

        return Rectangle{
            x + (cellWidth - btnWidth) / 2,
            y + (cellHeight - btnHeight) / 2,
            btnWidth,
            btnHeight};
    }

    std::string GetKeyNameStr() const {
        if (waitingForKey)
            return "Press a key...";
        int key = getKeyFunc();
        return GetKeyName(key);
    }

    Vector2 size;
    std::string actionName;
    int gridRow;
    int gridCol;
    std::function<int()> getKeyFunc;
    std::function<void(int)> setKeyFunc;
    std::shared_ptr<UIStyle> style;
    bool hovered = false;
    bool waitingForKey = false;
};
