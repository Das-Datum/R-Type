#pragma once
#include "Menus/IWidget.hpp"
#include "Menus/UIStyle.hpp"
#include <functional>
#include <memory>
#include <string>

class ButtonWidget : public IWidget {
  public:
    ButtonWidget(const std::string &text, int row, int col, 
                std::function<void()> onClick, 
                const std::shared_ptr<UIStyle> &style,
                std::function<std::string()> textUpdateFn = nullptr)
        : text(text), gridRow(row), gridCol(col), 
          onClick(onClick), style(style), 
          textUpdateFn(textUpdateFn) {}

    void update(float dt) override {
        (void)dt;
        if (textUpdateFn) {
            text = textUpdateFn();
        }
        Vector2 mousePos = GetMousePosition();
        Rectangle bounds = CalculateBounds();
        hovered = CheckCollisionPointRec(mousePos, bounds);
        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (onClick)
                onClick();
        }
    }

    void draw() override {
        Rectangle bounds = CalculateBounds();
        Color bg = hovered ? style->hoverColor : style->baseColor;
        DrawRectangleRec(bounds, bg);
        DrawRectangleLinesEx(bounds, 2, BLACK);

        Vector2 textSize = MeasureTextEx(style->font, text.c_str(), style->fontSize, 1);
        DrawTextEx(
            style->font,
            text.c_str(),
            {bounds.x + (bounds.width - textSize.x) * 0.5f,
                bounds.y + (bounds.height - textSize.y) * 0.5f},
            style->fontSize,
            1,
            style->textButtonColor);
    }

    void handleEvent() override {}

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

    void setText(const std::string &text) { this->text = text; }

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

        float btnWidth = cellWidth * 0.7f;
        float btnHeight = cellHeight * 0.8f;

        return Rectangle{
            x + (cellWidth - btnWidth) / 2,
            y + (cellHeight - btnHeight) / 2,
            btnWidth,
            btnHeight};
    }

    Vector2 size;
    std::string text;
    int gridRow;
    int gridCol;
    std::function<void()> onClick;
    std::shared_ptr<UIStyle> style;
    bool hovered = false;
    std::function<std::string()> textUpdateFn;
};
