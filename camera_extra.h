#ifndef BOWSER_UTIL_CAMERA_EXTRA_H
#define BOWSER_UTIL_CAMERA_EXTRA_H

#include "raylib.h"
#include "raymath.h"
#include "external/stb_perlin.h" // Raylib perlin noise
#include <cmath>
#include <functional>
#include <iostream>

namespace bowser_util {
    class Camera2DExtended {
    public:
        operator Camera2D() {
            return Camera2D {
                .offset = Vector2Add(offset, offsetTrauma),
                .target = target,
                .rotation = rotation + rotationTrauma,
                .zoom = zoom
            };
        }

        void tick() {
            if (trauma > 0.0f) {
                trauma = std::max(0.0f, trauma - 0.02f);
                const float scale = trauma * trauma;
                constexpr float tScale = 10.0f;

                rotationTrauma = scale * stb_perlin_fbm_noise3(0.0f + tScale * GetTime(), 1.0f * tScale, 1.0f, 2.0f, 0.5f, 6);
                offsetTrauma = Vector2 {
                    scale * 30.0f * stb_perlin_fbm_noise3(0.0f + tScale * GetTime(), 20.0f * tScale, 1.0f, 2.0f, 0.5f, 6),
                    scale * 30.0f * stb_perlin_fbm_noise3(0.0f + tScale * GetTime(), 30.0f * tScale, 1.0f, 2.0f, 0.5f, 6)
                };
            }
        }

        Vector2 screenToWorld(Vector2 screenPos) { return GetScreenToWorld2D(screenPos, *this); }
        Vector2 worldToScreen(Vector2 worldPos)  { return GetWorldToScreen2D(worldPos, *this); }
        Matrix getCameraMatrix() { return GetCameraMatrix2D(*this); }

        Vector2 offset{0, 0};     // Camera offset (displacement from target)
        Vector2 target{0, 0};     // Camera target (rotation and zoom origin)
        float rotation = 0.0;     // Camera rotation in degrees
        float zoom = 1.0f;        // Camera zoom (scaling), should be 1.0f by default
        float trauma = 0.0f;      // Shake, decays to 0
    private:
        float rotationTrauma = 0.0f;
        Vector2 offsetTrauma{0.0f, 0.0f};
    };
};

#endif