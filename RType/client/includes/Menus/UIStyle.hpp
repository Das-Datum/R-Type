#pragma once
#include "raylib.h"

struct UIStyle {
    Color baseColor = LIGHTGRAY;
    Color hoverColor = GRAY;
    Color clickedColor = DARKGRAY;
    Color textColor = WHITE;
    Color textButtonColor = BLACK;

    Color secondaryColor = RED;
    Color secondaryHoverColor = MAROON;

    Font font = GetFontDefault();
    float fontSize = 20.0f;

    Color panelColor = { 0, 0, 0, 150 };

    struct GridConfig {
        int totalRows = 8;
        int totalCols = 3;
        float padding = 0.02f;
    } grid;
};

std::string GetKeyName(int key);
