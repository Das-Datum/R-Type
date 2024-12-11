#pragma once
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <cstdlib>
#include <filesystem>
#include "raylib.h"
#include <iostream>
#include <exception>

typedef enum {
    NORMAL,
    DEUTERANOPIA,
    PROTANOPIA,
    TRITANOPIA,
    ACHROMATOPSIA
} ColorBlindMode;

class Settings {
public:
    Settings() = default;

    float getMasterVolume() const { return masterVolume; }
    void setMasterVolume(float volume) { masterVolume = volume; }

    float getMusicVolume() const { return musicVolume; }
    void setMusicVolume(float volume) { musicVolume = volume; }

    float getSfxVolume() const { return sfxVolume; }
    void setSfxVolume(float volume) { sfxVolume = volume; }

    int getMoveUpKey() const { return moveUpKey; }
    void setMoveUpKey(int key) { moveUpKey = key; }

    int getMoveDownKey() const { return moveDownKey; }
    void setMoveDownKey(int key) { moveDownKey = key; }

    int getMoveLeftKey() const { return moveLeftKey; }
    void setMoveLeftKey(int key) { moveLeftKey = key; }

    int getMoveRightKey() const { return moveRightKey; }
    void setMoveRightKey(int key) { moveRightKey = key; }

    int getShootKey() const { return shootKey; }
    void setShootKey(int key) { shootKey = key; }

    float getFontSize() const { return fontSize; }
    void setFontSize(float size) { fontSize = size; }

    ColorBlindMode getColorBlindMode() const { return currentMode; }
    void setColorBlindMode(ColorBlindMode mode) { currentMode = mode; }
    void switchColorBlindMode() {
        currentMode = static_cast<ColorBlindMode>((currentMode + 1) % 5);
    }

    void Load() {
        std::string path = GetConfigPath();
        std::ifstream file(path);
        if (!file.is_open()) return;

        nlohmann::json j;
        file >> j;
        if (j.find("masterVolume") != j.end()) masterVolume = j["masterVolume"];
        if (j.find("musicVolume") != j.end()) musicVolume = j["musicVolume"];
        if (j.find("sfxVolume") != j.end()) sfxVolume = j["sfxVolume"];
        if (j.find("moveUpKey") != j.end()) moveUpKey = j["moveUpKey"];
        if (j.find("moveDownKey") != j.end()) moveDownKey = j["moveDownKey"];
        if (j.find("moveLeftKey") != j.end()) moveLeftKey = j["moveLeftKey"];
        if (j.find("moveRightKey") != j.end()) moveRightKey = j["moveRightKey"];
        if (j.find("shootKey") != j.end()) shootKey = j["shootKey"];
        if (j.find("fontSize") != j.end()) fontSize = j["fontSize"];
        if (j.find("colorBlindMode") != j.end()) currentMode = static_cast<ColorBlindMode>(j["colorBlindMode"]);
    }

    void Save() const {
        try {
            std::string path = GetConfigPath();
            std::cout << "Saving settings to: " << path << std::endl;

            nlohmann::json j;
            j["masterVolume"] = masterVolume;
            j["musicVolume"] = musicVolume;
            j["sfxVolume"] = sfxVolume;
            j["moveUpKey"] = moveUpKey;
            j["moveDownKey"] = moveDownKey;
            j["moveLeftKey"] = moveLeftKey;
            j["moveRightKey"] = moveRightKey;
            j["shootKey"] = shootKey;
            j["fontSize"] = fontSize;
            j["colorBlindMode"] = currentMode;

            std::ofstream file(path);
            if (!file.is_open()) {
                std::cerr << "Failed to open file for writing: " << path << std::endl;
                return;
            }
            std::cout << "File opened successfully for writing.\n";

            file << j.dump(4);
            std::cout << "Settings saved successfully.\n";
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error in Save(): " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Exception in Save(): " << e.what() << std::endl;
        }
    }

private:
    float masterVolume = 1.0f;
    float musicVolume = 0.5f;
    float sfxVolume = 0.5f;
    int moveUpKey = KEY_W;
    int moveDownKey = KEY_S;
    int moveLeftKey = KEY_A;
    int moveRightKey = KEY_D;
    int shootKey = KEY_SPACE;
    float fontSize = 20.0f;

    ColorBlindMode currentMode = NORMAL;

    std::string GetConfigPath() const {
        return "config.json";
    }
};
