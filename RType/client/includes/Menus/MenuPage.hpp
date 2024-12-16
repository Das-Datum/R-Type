#pragma once
#include <memory>
#include <string>
#include <vector>
#include "IWidget.hpp"
#include "UIStyle.hpp"

class MenuPage {
public:
    MenuPage(const std::string& name, const std::shared_ptr<UIStyle>& style)
        : name(name), style(style) {}

    void AddWidget(const std::shared_ptr<IWidget>& widget) {
        widgets.push_back(widget);
    }

    void update(float dt) {
        for (auto& w : widgets) {
            w->update(dt);
        }
    }

    void draw() {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), style->panelColor);
        for (auto& w : widgets) {
            w->draw();
        }
    }

    void handleEvent() {
        for (auto& w : widgets) {
            w->handleEvent();
        }
    }

    std::string GetName() const { return name; }

private:
    std::string name;
    std::vector<std::shared_ptr<IWidget>> widgets;
    std::shared_ptr<UIStyle> style;
};
