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

const char* GetKeyName(int key) {
    switch (key) {
        case KEY_NULL: return "NULL";
        case KEY_APOSTROPHE: return "APOSTROPHE";
        case KEY_COMMA: return "COMMA";
        case KEY_MINUS: return "MINUS";
        case KEY_PERIOD: return "PERIOD";
        case KEY_SLASH: return "SLASH";
        case KEY_ZERO: return "ZERO";
        case KEY_ONE: return "ONE";
        case KEY_TWO: return "TWO";
        case KEY_THREE: return "THREE";
        case KEY_FOUR: return "FOUR";
        case KEY_FIVE: return "FIVE";
        case KEY_SIX: return "SIX";
        case KEY_SEVEN: return "SEVEN";
        case KEY_EIGHT: return "EIGHT";
        case KEY_NINE: return "NINE";
        case KEY_SEMICOLON: return "SEMICOLON";
        case KEY_EQUAL: return "EQUAL";
        case KEY_A: return "A";
        case KEY_B: return "B";
        case KEY_C: return "C";
        case KEY_D: return "D";
        case KEY_E: return "E";
        case KEY_F: return "F";
        case KEY_G: return "G";
        case KEY_H: return "H";
        case KEY_I: return "I";
        case KEY_J: return "J";
        case KEY_K: return "K";
        case KEY_L: return "L";
        case KEY_M: return "M";
        case KEY_N: return "N";
        case KEY_O: return "O";
        case KEY_P: return "P";
        case KEY_Q: return "Q";
        case KEY_R: return "R";
        case KEY_S: return "S";
        case KEY_T: return "T";
        case KEY_U: return "U";
        case KEY_V: return "V";
        case KEY_W: return "W";
        case KEY_X: return "X";
        case KEY_Y: return "Y";
        case KEY_Z: return "Z";
        case KEY_LEFT_BRACKET: return "LEFT_BRACKET";
        case KEY_BACKSLASH: return "BACKSLASH";
        case KEY_RIGHT_BRACKET: return "RIGHT_BRACKET";
        case KEY_GRAVE: return "GRAVE";
        case KEY_SPACE: return "SPACE";
        case KEY_ESCAPE: return "ESCAPE";
        case KEY_ENTER: return "ENTER";
        case KEY_TAB: return "TAB";
        case KEY_BACKSPACE: return "BACKSPACE";
        case KEY_INSERT: return "INSERT";
        case KEY_DELETE: return "DELETE";
        case KEY_RIGHT: return "RIGHT";
        case KEY_LEFT: return "LEFT";
        case KEY_DOWN: return "DOWN";
        case KEY_UP: return "UP";
        case KEY_PAGE_UP: return "PAGE_UP";
        case KEY_PAGE_DOWN: return "PAGE_DOWN";
        case KEY_HOME: return "HOME";
        case KEY_END: return "END";
        case KEY_CAPS_LOCK: return "CAPS_LOCK";
        case KEY_SCROLL_LOCK: return "SCROLL_LOCK";
        case KEY_NUM_LOCK: return "NUM_LOCK";
        case KEY_PRINT_SCREEN: return "PRINT_SCREEN";
        case KEY_PAUSE: return "PAUSE";
        case KEY_F1: return "F1";
        case KEY_F2: return "F2";
        case KEY_F3: return "F3";
        case KEY_F4: return "F4";
        case KEY_F5: return "F5";
        case KEY_F6: return "F6";
        case KEY_F7: return "F7";
        case KEY_F8: return "F8";
        case KEY_F9: return "F9";
        case KEY_F10: return "F10";
        case KEY_F11: return "F11";
        case KEY_F12: return "F12";
        case KEY_LEFT_SHIFT: return "LEFT_SHIFT";
        case KEY_LEFT_CONTROL: return "LEFT_CONTROL";
        case KEY_LEFT_ALT: return "LEFT_ALT";
        case KEY_LEFT_SUPER: return "LEFT_SUPER";
        case KEY_RIGHT_SHIFT: return "RIGHT_SHIFT";
        case KEY_RIGHT_CONTROL: return "RIGHT_CONTROL";
        case KEY_RIGHT_ALT: return "RIGHT_ALT";
        case KEY_RIGHT_SUPER: return "RIGHT_SUPER";
        case KEY_KB_MENU: return "KB_MENU";
        default: return "UNKNOWN_KEY";
    }
}
