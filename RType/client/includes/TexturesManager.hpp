#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>

/**
 * @brief Singleton class to manage textures
 */
class TexturesManager {
public:
    /**
     * @brief Get the Instance object
     * @return TexturesManager&
     */
    static TexturesManager &getInstance() {
        static TexturesManager instance;
        return instance;
    }

    /**
     * @brief Load a texture from a file
     * @param filename
     * @return Texture2D
     */
    Texture2D loadTexture(const std::string &filename) {
        if (textureCache.find(filename) == textureCache.end()) {
            textureCache[filename] = LoadTexture(filename.c_str());
        }
        return textureCache[filename];
    }

    /**
     * @brief Get the Texture object
     *
     * @param filename
     * @return Texture2D
     */
    Texture2D getTexture(const std::string &filename) {
        auto it = textureCache.find(filename);
        if (it != textureCache.end()) {
            return it->second;
        }
        return loadTexture(filename);
    }

    /**
     * @brief Unload all textures
     */
    void unloadAllTextures() {
        for (auto &[key, texture] : textureCache) {
            std::cout << "Unloading texture: " << key << std::endl;
            std::cout << "Texture id: " << texture.id << std::endl;
            UnloadTexture(texture);
        }
        std::cout << "Clearing texture cache" << std::endl;
        textureCache.clear();
    }

    /**
     * @brief Get the Frame object
     * @param filename
     * @param frameIndex
     * @param frameCount
     * @return Rectangle
     */
    Rectangle getFrame(const std::string &filename, int frameIndex,
                       int frameCount) {
        Texture2D texture = getTexture(filename);
        float frameWidth = (float)texture.width / frameCount;
        return Rectangle{frameIndex * frameWidth, 0, frameWidth,
                         (float)texture.height};
    }

private:
    TexturesManager() = default;
    ~TexturesManager() {}

    std::unordered_map<std::string, Texture2D> textureCache;
};
