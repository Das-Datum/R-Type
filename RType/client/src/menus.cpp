#include "client.hpp"

void initMenus(std::shared_ptr<MenuManager> manager, std::shared_ptr<Settings> settings, int windowWidth, int windowHeight) {
    std::cout << "Initializing menus\n";
    settings->Load();
    auto &shadersManager = ShadersManager::getInstance();
    shadersManager.initializeShaders();

    auto style = std::make_shared<UIStyle>();
    style->font = GetFontDefault();
    style->fontSize = settings->getFontSize();
    style->baseColor = LIGHTGRAY;
    style->hoverColor = GRAY;
    style->textColor = WHITE;

    auto pageMain = std::make_shared<MenuPage>("MainMenu", style);

    pageMain->AddWidget(std::make_shared<LabelWidget>(
        "R-TYPE",
        2, 2,
        style,
        3.0f
    ));

    pageMain->AddWidget(std::make_shared<ButtonWidget>(
        "Play Game",
        5, 2,
        []() {
            //! Temporary until we have lobbies and single player mode.
            std::cout << "Connecting to server...\n";
            gCoordinator.getSystem<ClientSystem>()->init("Player", "127.0.0.1", 5000);
            gCoordinator.getSystem<PlayerNetworkSystem>()->init(*gCoordinator.getSystem<ClientSystem>());
        },
        style));

    pageMain->AddWidget(std::make_shared<ButtonWidget>(
        "Settings",
        6, 2,
        [manager, windowWidth, windowHeight]() {
            manager->SetActivePage("Settings", windowWidth, windowHeight);
        },
        style));

    pageMain->AddWidget(std::make_shared<ButtonWidget>(
        "Exit",
        7, 2,
        []() {
            auto &TexturesManager = TexturesManager::getInstance();
            TexturesManager.unloadAllTextures();
            CloseWindow();
            exit(0);
        },
        style));

    auto pageSettings = std::make_shared<MenuPage>("Settings", style);

    //? SLIDER
    pageSettings->AddWidget(std::make_shared<SliderWidget>(
        "Master Volume",
        2, 1,
        0.0f, 1.0f,
        [&settings](float value) { settings->setMasterVolume(value); },
        style,
        [&settings]() -> float { return settings->getMasterVolume(); }));

    pageSettings->AddWidget(std::make_shared<SliderWidget>(
        "Music Volume",
        3, 1,
        0.0f, 1.0f,
        [&settings](float value) { settings->setMusicVolume(value); },
        style,
        [&settings]() -> float { return settings->getMusicVolume(); }));

    pageSettings->AddWidget(std::make_shared<SliderWidget>(
        "SFX Volume",
        4, 1,
        0.0f, 1.0f,
        [&settings](float value) { settings->setSfxVolume(value); },
        style,
        [&settings]() -> float { return settings->getSfxVolume(); }));

    pageSettings->AddWidget(std::make_shared<SliderWidget>(
        "Font Size",
        5, 1,
        10.0f, 40.0f,
        [settings, style](float value) {
            settings->setFontSize(value);
            style->fontSize = value;
        },
        style,
        [settings]() -> float { return settings->getFontSize(); }
    ));

    pageSettings->AddWidget(std::make_shared<LabelWidget>(
        "Color Blind Mode",
        6, 1,
        style));

    pageSettings->AddWidget(std::make_shared<ButtonWidget>(
        "Color Blind Mode",
        7, 1,
        [settings]() {
            settings->switchColorBlindMode();
            std::cout << "Switching color blind mode to " << settings->getColorBlindMode() << std::endl;
        },
        style,
        [settings]() -> std::string {
            switch (settings->getColorBlindMode()) {
                case NORMAL: return "Normal";
                case DEUTERANOPIA: return "Deuteranopia";
                case PROTANOPIA: return "Protanopia";
                case TRITANOPIA: return "Tritanopia";
                case ACHROMATOPSIA: return "Achromatopsia";
            }
            return "Unknown";
        }));

    std::vector<std::pair<std::string, std::pair<std::function<int()>, std::function<void(int)>>>> keyBindings = {
        {"Shoot", {
            [&settings]() { return settings->getShootKey(); },
            [&settings](int key) { settings->setShootKey(key); }
        }},
        {"Move Up", {
            [&settings]() { return settings->getMoveUpKey(); },
            [&settings](int key) { settings->setMoveUpKey(key); }
        }},
        {"Move Down", {
            [&settings]() { return settings->getMoveDownKey(); },
            [&settings](int key) { settings->setMoveDownKey(key); }
        }},
        {"Move Left", {
            [&settings]() { return settings->getMoveLeftKey(); },
            [&settings](int key) { settings->setMoveLeftKey(key); }
        }},
        {"Move Right", {
            [&settings]() { return settings->getMoveRightKey(); },
            [&settings](int key) { settings->setMoveRightKey(key); }
        }}
    };

    for (size_t i = 0; i < keyBindings.size(); ++i) {
        pageSettings->AddWidget(std::make_shared<KeyBindingWidget>(
            keyBindings[i].first,
            i + 2, 3,
            keyBindings[i].second.first,
            keyBindings[i].second.second,
            style));
    }

    pageSettings->AddWidget(std::make_shared<ButtonWidget>(
        "Apply & Back",
        8, 2,
        [manager, settings, windowWidth, windowHeight]() {
            settings->Save();
            manager->SetActivePage("MainMenu", windowWidth, windowHeight);
        },
        style));

    manager->AddPage(pageMain);
    manager->AddPage(pageSettings);
    manager->SetActivePage("MainMenu", windowWidth, windowHeight);
}
