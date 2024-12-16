#pragma once

#include <raylib.h>
#include <algorithm>
#include "Components/GameComponents.hpp"

struct TimerComponent {
    float elapsedTime = 0.0f;
    float duration = 0.0f;
    bool active = false;
    bool done = false;
};

struct SpriteComponent {
    Texture2D texture;
    Rectangle sourceRect;
    int zIndex;

    SpriteComponent(Texture2D tex, Rectangle srcRect, int z = 0)
        : texture(tex), sourceRect(srcRect), zIndex(z) {}
};

struct SpriteAnimationComponent {
    int frameCount;
    int currentFrame;
    float frameDuration;
    float elapsedTime;

    SpriteAnimationComponent(int frames = 1, float duration = 0.1f)
        : frameCount(frames), currentFrame(0), frameDuration(duration), elapsedTime(0.0f) {}
};

struct SpriteFrameComponent {
    int frameIndex;
    int frameCount;

    SpriteFrameComponent(int index = 0, int count = 1)
        : frameIndex(index), frameCount(count) {}

    void nextFrame() {
        frameIndex = (frameIndex + 1) % frameCount;
    }

    void prevFrame() {
        frameIndex = (frameIndex - 1) % frameCount;
    }
};

struct BackgroundScrollComponent {
    float scrollSpeed;
    float offset;
    float textureWidth;
    float windowWidth;

    BackgroundScrollComponent(float speed = 100.0f, float texWidth = 0.0f, float winWidth = 0.0f)
        : scrollSpeed(speed), offset(0.0f), textureWidth(texWidth), windowWidth(winWidth) {}
};

struct InputComponent {};
struct StaticComponent {};
