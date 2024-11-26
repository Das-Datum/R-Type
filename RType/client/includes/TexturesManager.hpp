#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <raylib.h>

#include <string>
#include <unordered_map>

class TexturesManager {
public:
    static TexturesManager &getInstance() {
        static TexturesManager instance;
        return instance;
    }

    Texture2D loadTexture(const std::string &filename) {
        if (textureCache.find(filename) == textureCache.end()) {
            textureCache[filename] = LoadTexture(filename.c_str());
        }
        return textureCache[filename];
    }

    Texture2D getTexture(const std::string &filename) {
        auto it = textureCache.find(filename);
        if (it != textureCache.end()) {
            return it->second;
        }
        return loadTexture(filename);
    }

    void unloadAllTextures() {
        for (auto &[key, texture] : textureCache) {
            UnloadTexture(texture);
        }
        textureCache.clear();
    }

    Rectangle getFrame(const std::string &filename, int frameIndex,
                       int frameCount) {
        Texture2D texture = getTexture(filename);
        float frameWidth = (float)texture.width / frameCount;
        return Rectangle{frameIndex * frameWidth, 0, frameWidth,
                         (float)texture.height};
    }

private:
    TexturesManager() = default;
    ~TexturesManager() { unloadAllTextures(); }

    std::unordered_map<std::string, Texture2D> textureCache;
};

#endif // TEXTURE_MANAGER_HPP
