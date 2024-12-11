#pragma once
#include <map>
#include <string>
#include <memory>
#include "MenuPage.hpp"

class MenuManager {
public:
    void AddPage(const std::shared_ptr<MenuPage>& page) {
        pages[page->GetName()] = page;
    }

    void SetActivePage(const std::string& name, int windowWidth, int windowHeight) {
        if (pages.find(name) != pages.end()) {
            activePage = pages[name];
        }
    }

    void Update(float dt) {
        if (activePage) activePage->Update(dt);
    }

    void Draw() {
        if (activePage) activePage->Draw();
    }

    void HandleEvent() {
        if (activePage) activePage->HandleEvent();
    }

private:
    std::map<std::string, std::shared_ptr<MenuPage>> pages;
    std::shared_ptr<MenuPage> activePage = nullptr;
};
