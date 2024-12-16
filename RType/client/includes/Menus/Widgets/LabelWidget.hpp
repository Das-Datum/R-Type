#pragma once
#include "Menus/IWidget.hpp"
#include "Menus/UIStyle.hpp"
#include <memory>
#include <string>

class LabelWidget : public IWidget {
  public:
    LabelWidget(const std::string &text, int row, int col, const std::shared_ptr<UIStyle> &style, float scale = 1.0f)
        : text(text), gridRow(row), gridCol(col), style(style), scale(scale) {}

    void draw() override {
        Rectangle bounds = CalculateBounds();
        Vector2 textSize = MeasureTextEx(style->font, text.c_str(), style->fontSize * scale, 1);

        DrawTextEx(
            style->font,
            text.c_str(),
            {bounds.x + (bounds.width - textSize.x) * 0.5f,
                bounds.y + (bounds.height - textSize.y) * 0.5f},
            style->fontSize * scale,
            1,
            style->textColor);
    }

    void update(float dt) override { (void)dt; }
    void handleEvent() override {}
    Rectangle GetBounds() const override { return CalculateBounds(); }
    void SetSize(Vector2 size) override { this->size = size; }
    void SetGridPosition(int row, int col) override {
        gridRow = row;
        gridCol = col;
    }
    std::pair<int, int> GetGridPosition() const override {
        return {gridRow, gridCol};
    }
    bool IsFocused() const override { return false; }
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
        float cellWidth = (baseWidth - (padding * (style->grid.totalCols + 1))) /
                          static_cast<float>(style->grid.totalCols);
        float cellHeight = (baseHeight - (padding * (style->grid.totalRows + 1))) /
                           static_cast<float>(style->grid.totalRows);

        float startX = (screenWidth - baseWidth) / 2.0f;
        float startY = (screenHeight - baseHeight) / 2.0f;

        float x = startX + padding + (gridCol - 1) * (cellWidth + padding);
        float y = startY + padding + (gridRow - 1) * (cellHeight + padding);

        return Rectangle{x, y, cellWidth, cellHeight};
    }

    std::string text;
    Vector2 size;
    int gridRow;
    int gridCol;
    std::shared_ptr<UIStyle> style;
    float scale;
};
