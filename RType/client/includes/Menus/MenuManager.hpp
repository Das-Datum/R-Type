#pragma once
#include <map>
#include <string>
#include <memory>
#include "MenuPage.hpp"

class MenuManager {
public:
    void addPage(const std::shared_ptr<MenuPage>& page) {
        pages[page->GetName()] = page;
    }

    void setActivePage(const std::string& name, int windowWidth, int windowHeight) {
        if (pages.find(name) != pages.end()) {
            lastPageName = activePage ? activePage->GetName() : "";
            activePage = pages[name];
        }
    }

    void update(float dt) {
        if (activePage) activePage->update(dt);
    }

    void draw() {
        if (activePage) activePage->draw();
    }

    void handleEvent() {
        if (activePage) activePage->handleEvent();
    }

    void closeCurrentPage() {
        lastPageName = activePage ? activePage->GetName() : "";
        activePage = nullptr;
    }

    bool isPageActive() {
        return activePage != nullptr;
    }

    std::string getActivePageName() {
        if (activePage) return activePage->GetName();
        return "";
    }

    std::string getLastPageName() const {
        return lastPageName;
    }

private:
    std::map<std::string, std::shared_ptr<MenuPage>> pages;
    std::shared_ptr<MenuPage> activePage = nullptr;
    std::string lastPageName;
};
