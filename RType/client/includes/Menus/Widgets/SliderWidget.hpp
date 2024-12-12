#pragma once
#include "Menus/IWidget.hpp"
#include "Menus/UIStyle.hpp"
#include <functional>
#include <memory>

class SliderWidget : public IWidget {
  public:
    SliderWidget(const std::string &label, int row, int col, float minVal, float maxVal, std::function<void(float)> onValueChanged, const std::shared_ptr<UIStyle> &style, std::function<float()> getDefaultValue)
        : label(label), gridRow(row), gridCol(col), minVal(minVal), maxVal(maxVal), onValueChanged(onValueChanged), style(style), currentValue(getDefaultValue()) {}

    void update(float dt) override {
        (void)dt;
        Vector2 mousePos = GetMousePosition();
        Rectangle bounds = CalculateBounds();
        hovered = CheckCollisionPointRec(mousePos, bounds);
        if (hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            float t = (mousePos.x - bounds.x) / bounds.width;
            if (t < 0.0f)
                t = 0.0f;
            if (t > 1.0f)
                t = 1.0f;
            currentValue = minVal + t * (maxVal - minVal);
            onValueChanged(currentValue);
        }
    }

    void draw() override {
        Rectangle bounds = CalculateBounds();
        DrawRectangleRec(bounds, style->baseColor);
        DrawRectangleLinesEx(bounds, 2, BLACK);

        float t = (currentValue - minVal) / (maxVal - minVal);
        float handleX = bounds.x + t * bounds.width;
        DrawCircleV({handleX, bounds.y + bounds.height / 2.0f}, bounds.height / 2.0f, hovered ? style->secondaryHoverColor : style->secondaryColor);

        DrawLabel();
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

        float sliderWidth = cellWidth * 0.9f;
        float sliderHeight = cellHeight * 0.25f;

        return Rectangle{
            x + (cellWidth - sliderWidth) / 2,
            y + (cellHeight - sliderHeight) / 2,
            sliderWidth,
            sliderHeight};
    }

    void DrawLabel() const {
        Rectangle bounds = CalculateBounds();
        Vector2 textSize = MeasureTextEx(style->font, label.c_str(), style->fontSize * 0.8f, 1);

        DrawTextEx(
            style->font,
            label.c_str(),
            {bounds.x,
                bounds.y - textSize.y - 5},
            style->fontSize * 0.8f,
            1,
            style->textColor);
    }

    const std::string label = "";
    Vector2 size;
    int gridRow;
    int gridCol;
    float minVal;
    float maxVal;
    float currentValue;
    std::function<void(float)> onValueChanged;
    std::shared_ptr<UIStyle> style;
    bool hovered = false;
};
