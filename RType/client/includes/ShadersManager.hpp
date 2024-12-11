#pragma once

#include "raylib.h"
#include <vector>
#include <string>
#include "Menus.hpp"

class ShadersManager {
public:
    static ShadersManager& getInstance() {
        static ShadersManager instance;
        return instance;
    }

    void initializeShaders() {
        shaders.resize(5);
        shaders[NORMAL] = LoadShader(NULL, NULL);
        shaders[DEUTERANOPIA] = LoadShader(NULL, "assets/shaders/deuteranopia.frag");
        shaders[PROTANOPIA] = LoadShader(NULL, "assets/shaders/protanopia.frag");
        shaders[TRITANOPIA] = LoadShader(NULL, "assets/shaders/tritanopia.frag");
        shaders[ACHROMATOPSIA] = LoadShader(NULL, "assets/shaders/achromatopsia.frag");
    }

    void unloadAllShaders() {
        for (auto& shader : shaders) {
            UnloadShader(shader);
        }
    }

    Shader getShaderForMode(ColorBlindMode mode) const {
        return shaders[mode];
    }

private:
    ShadersManager() {
        initializeShaders();
    }
    ~ShadersManager() {}

    ShadersManager(const ShadersManager&) = delete;
    ShadersManager& operator=(const ShadersManager&) = delete;

    std::vector<Shader> shaders;
};
