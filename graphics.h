#ifndef BOWSER_UTIL_GRAPHICS_H
#define BOWSER_UTIL_GRAPHICS_H

#include "rlgl.h"
#include "raylib.h"
#include "raymath.h"
#include "types/vector.h"

namespace bowser_util {
    // Quick assignment for shader vector values
    inline void setShaderValue(const Shader &shaderId, int propertyLoc, Vector2 vec) {
        SetShaderValue(shaderId, propertyLoc, &vec, SHADER_UNIFORM_VEC2);
    }
    inline void setShaderValue(const Shader &shaderId, int propertyLoc, Vector3 vec) {
        SetShaderValue(shaderId, propertyLoc, &vec, SHADER_UNIFORM_VEC3);
    }
    inline void setShaderValue(const Shader &shaderId, int propertyLoc, Vector4 vec) {
        SetShaderValue(shaderId, propertyLoc, &vec, SHADER_UNIFORM_VEC4);
    }
    inline void setShaderValue(const Shader &shaderId, int propertyLoc, ivec2 vec) {
        SetShaderValue(shaderId, propertyLoc, &vec, SHADER_UNIFORM_IVEC2);
    }
    inline void setShaderValue(const Shader &shaderId, int propertyLoc, ivec3 vec) {
        SetShaderValue(shaderId, propertyLoc, &vec, SHADER_UNIFORM_IVEC3);
    }
    inline void setShaderValue(const Shader &shaderId, int propertyLoc, ivec4 vec) {
        SetShaderValue(shaderId, propertyLoc, &vec, SHADER_UNIFORM_IVEC4);
    }
    inline void setShaderValue(const Shader &shaderId, int propertyLoc, float val) {
        SetShaderValue(shaderId, propertyLoc, &val, SHADER_UNIFORM_FLOAT);
    }
    inline void setShaderValue(const Shader &shaderId, int propertyLoc, int val) {
        SetShaderValue(shaderId, propertyLoc, &val, SHADER_UNIFORM_INT);
    }

    // Draw render texture
    inline void drawRenderTexture(const RenderTexture2D &tex, const Vector2 origin, const Vector2 size) {
        const Rectangle source{ 0.0f, 0.0f, (float)tex.texture.width, -(float)tex.texture.height };
        const Rectangle dest{ origin.x, origin.y, size.x, size.y };
        DrawTexturePro(tex.texture, source, dest, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    inline void drawRenderTexture(const RenderTexture2D &tex) {
        drawRenderTexture(tex, Vector2{0.0f, 0.0f}, Vector2{ (float)tex.texture.width, (float)tex.texture.height });
    }
}

#endif