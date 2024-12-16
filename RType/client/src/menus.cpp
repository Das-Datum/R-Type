#include "client.hpp"

void initMenus(int windowWidth, int windowHeight) {
    std::cout << "Initializing menus\n";
    Settings::getInstance().Load();
    auto &shadersManager = ShadersManager::getInstance();
    shadersManager.initializeShaders();

    auto style = std::make_shared<UIStyle>();
    style->font = GetFontDefault();
    style->fontSize = Settings::getInstance().getFontSize();
    style->baseColor = LIGHTGRAY;
    style->hoverColor = GRAY;
    style->textColor = WHITE;

    //? MAIN MENU
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
        [windowWidth, windowHeight]() {
            //! Temporary until we have lobbies and single player mode.
            MenuManager::getInstance().setActivePage("Play", windowWidth, windowHeight);
        },
        style));

    pageMain->AddWidget(std::make_shared<ButtonWidget>(
        "Settings",
        6, 2,
        [windowWidth, windowHeight]() {
            MenuManager::getInstance().setActivePage("Settings", windowWidth, windowHeight);
        },
        style));

    pageMain->AddWidget(std::make_shared<ButtonWidget>(
        "Exit",
        7, 2,
        []() {
            auto &TexturesManager = TexturesManager::getInstance();
            TexturesManager.unloadAllTextures();

            auto &ShadersManager = ShadersManager::getInstance();
            ShadersManager.unloadAllShaders();

            CloseWindow();
            exit(0);
        },
        style));


    //? SETTINGS
    auto pageSettings = std::make_shared<MenuPage>("Settings", style);

    pageSettings->AddWidget(std::make_shared<SliderWidget>(
        "Master Volume",
        2, 1,
        0.0f, 1.0f,
        [](float value) { Settings::getInstance().setMasterVolume(value); },
        style,
        []() -> float { return Settings::getInstance().getMasterVolume(); }));

    pageSettings->AddWidget(std::make_shared<SliderWidget>(
        "Music Volume",
        3, 1,
        0.0f, 1.0f,
        [](float value) { Settings::getInstance().setMusicVolume(value); },
        style,
        []() -> float { return Settings::getInstance().getMusicVolume(); }));

    pageSettings->AddWidget(std::make_shared<SliderWidget>(
        "SFX Volume",
        4, 1,
        0.0f, 1.0f,
        [](float value) { Settings::getInstance().setSfxVolume(value); },
        style,
        []() -> float { return Settings::getInstance().getSfxVolume(); }));

    pageSettings->AddWidget(std::make_shared<SliderWidget>(
        "Font Size",
        5, 1,
        10.0f, 40.0f,
        [style](float value) {
            Settings::getInstance().setFontSize(value);
            style->fontSize = value;
        },
        style,
        []() -> float { return Settings::getInstance().getFontSize(); }
    ));

    pageSettings->AddWidget(std::make_shared<LabelWidget>(
        "Color Blind Mode",
        6, 1,
        style));

    pageSettings->AddWidget(std::make_shared<ButtonWidget>(
        "Color Blind Mode",
        7, 1,
        []() {
            Settings::getInstance().switchColorBlindMode();
            std::cout << "Switching color blind mode to " << Settings::getInstance().getColorBlindMode() << std::endl;
        },
        style,
        []() -> std::string {
            switch (Settings::getInstance().getColorBlindMode()) {
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
            []() { return Settings::getInstance().getShootKey(); },
            [](int key) { Settings::getInstance().setShootKey(key); }
        }},
        {"Move Up", {
            []() { return Settings::getInstance().getMoveUpKey(); },
            [](int key) { Settings::getInstance().setMoveUpKey(key); }
        }},
        {"Move Down", {
            []() { return Settings::getInstance().getMoveDownKey(); },
            [](int key) { Settings::getInstance().setMoveDownKey(key); }
        }},
        {"Move Left", {
            []() { return Settings::getInstance().getMoveLeftKey(); },
            [](int key) { Settings::getInstance().setMoveLeftKey(key); }
        }},
        {"Move Right", {
            []() { return Settings::getInstance().getMoveRightKey(); },
            [](int key) { Settings::getInstance().setMoveRightKey(key); }
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
        [windowWidth, windowHeight]() {
            Settings::getInstance().Save();
            std::string pageToGo = MenuManager::getInstance().getLastPageName() != "" ? MenuManager::getInstance().getLastPageName() : "MainMenu";
            MenuManager::getInstance().setActivePage(pageToGo, windowWidth, windowHeight);
        },
        style));


    //? PAGE PLAY
    auto pagePlay = std::make_shared<MenuPage>("Play", style);

    pagePlay->AddWidget(std::make_shared<LabelWidget>(
        "Play",
        2, 2,
        style,
        3.0f
    ));

    pagePlay->AddWidget(std::make_shared<ButtonWidget>(
        "Single Player",
        5, 2,
        [windowWidth, windowHeight]() {
            std::cout << "Starting single player game...\n";
            auto &entitiesManager = EntitiesManager::getInstance();
            entitiesManager.createShip({0.2f, 0.5f}, 1, "Player", true);

            auto &settings = Settings::getInstance();
            settings.setMultiplayer(false);

            auto &stageLoader = StageLoader::getInstance();
            stageLoader.loadConfig("stages/stage1.json");
            stageLoader.genWaves();
            stageLoader.genMobsEntities(entitiesManager);

            MenuManager::getInstance().closeCurrentPage();
        },
        style));

    pagePlay->AddWidget(std::make_shared<ButtonWidget>(
        "Multiplayer",
        6, 2,
        [windowWidth, windowHeight]() {
            std::cout << "Connecting to server...\n";

            auto &settings = Settings::getInstance();
            settings.setMultiplayer(true);

            gCoordinator.getSystem<ClientManageNetworkSystem>()->init("Player", "127.0.0.1", 5000);
            MenuManager::getInstance().setActivePage("Lobby", windowWidth, windowHeight);
        },
        style));

    pagePlay->AddWidget(std::make_shared<ButtonWidget>(
        "Back",
        7, 2,
        [windowWidth, windowHeight]() {
            MenuManager::getInstance().setActivePage("MainMenu", windowWidth, windowHeight);
        },
        style));


    //? PAGE LOBBY
    auto pageLobby = std::make_shared<MenuPage>("Lobby", style);

    pageLobby->AddWidget(std::make_shared<LabelWidget>(
        "Lobby",
        2, 2,
        style,
        3.0f
    ));

    //! TODO: implement lobby list
    pageLobby->AddWidget(std::make_shared<ButtonWidget>(
        "Start Game",
        5, 2,
        []() {
            std::cout << "Starting game...\n";
            //! TODO: implement starting game
            gCoordinator.getSystem<ClientManageNetworkSystem>()->sendData("STA");

            MenuManager::getInstance().closeCurrentPage();
        },
        style));

    pageLobby->AddWidget(std::make_shared<ButtonWidget>(
        "Leave Lobby",
        6, 2,
        [windowWidth, windowHeight]() {
            std::cout << "Leaving lobby...\n";
            gCoordinator.getSystem<ClientManageNetworkSystem>()->disconnect();

            MenuManager::getInstance().setActivePage("MainMenu", windowWidth, windowHeight);
        },
        style));


    //? PAUSE MENU
    auto pagePause = std::make_shared<MenuPage>("PauseMenu", style);

    pagePause->AddWidget(std::make_shared<LabelWidget>(
        "Paused",
        2, 2,
        style,
        3.0f
    ));

    pagePause->AddWidget(std::make_shared<ButtonWidget>(
        "Resume",
        5, 2,
        [windowWidth, windowHeight]() {
            MenuManager::getInstance().closeCurrentPage();
        },
        style));

    pagePause->AddWidget(std::make_shared<ButtonWidget>(
        "Settings",
        6, 2,
        [windowWidth, windowHeight]() {
            MenuManager::getInstance().setActivePage("Settings", windowWidth, windowHeight);
        },
        style));

    pagePause->AddWidget(std::make_shared<ButtonWidget>(
        "Quit",
        7, 2,
        [windowWidth, windowHeight]() {
            MenuManager::getInstance().setActivePage("MainMenu", windowWidth, windowHeight);
            gCoordinator.getSystem<ClientManageNetworkSystem>()->disconnect();
            //! TODO: Implement game cleanup (graphical)
        },
        style));


    MenuManager::getInstance().addPage(pageMain);
    MenuManager::getInstance().addPage(pageSettings);
    MenuManager::getInstance().addPage(pageLobby);
    MenuManager::getInstance().addPage(pagePause);
    MenuManager::getInstance().addPage(pagePlay);

    MenuManager::getInstance().setActivePage("MainMenu", windowWidth, windowHeight);
}
