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

    void Update(float dt) {
        for (auto& w : widgets) {
            w->Update(dt);
        }
    }

    void Draw() {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), style->panelColor);
        for (auto& w : widgets) {
            w->Draw();
        }
    }

    void HandleEvent() {
        for (auto& w : widgets) {
            w->HandleEvent();
        }
    }

    std::string GetName() const { return name; }

private:
    std::string name;
    std::vector<std::shared_ptr<IWidget>> widgets;
    std::shared_ptr<UIStyle> style;
};
