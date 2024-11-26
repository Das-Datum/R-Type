#pragma once
#include <raylib.h>

struct ShipComponent {};

struct BulletComponent {
    float size;
    Color color;

    BulletComponent(float sz = 2.0f, Color clr = RED)
        : size(sz), color(clr) {}
};

struct TimerComponent {
    float elapsedTime = 0.0f;
    float duration = 0.0f;
    bool active = false;
    bool done = false;
};

struct SpriteComponent {
    Texture2D texture;
    Rectangle sourceRect;

    SpriteComponent(Texture2D tex, Rectangle srcRect)
        : texture(tex), sourceRect(srcRect) {}
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

struct InputComponent {};
