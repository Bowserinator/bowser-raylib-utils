#ifndef BOWSER_UTIL_MATH_H
#define BOWSER_UTIL_MATH_H

#include <cmath>
#include "raymath.h"

#ifndef BOWSER_UTIL_ARITHMETIC_CONCEPT
#define BOWSER_UTIL_ARITHMETIC_CONCEPT
template <class T>
concept arithmetic = std::integral<T> or std::floating_point<T>;
#endif

namespace bowser_util {
    template <class T> requires arithmetic<T>
    constexpr T clamp(const T val, const T min, const T max) {
        const T tmp = val < min ? min : val;
        return tmp > max ? max : tmp;
    }

    template <class T> requires arithmetic<T>
    constexpr T lerp(const T start, const T end, const float amount) {
        return start + (end - start) * amount;
    }

    template <class T> requires arithmetic<T>
    constexpr T normalizeInRange(const T value, const T start, const T end) {
        return (value - start) / (end - start);
    }

    template <class T> requires arithmetic<T>
    constexpr T remap(const T value, const T start, const T end, const T targetStart, const T targetEnd) {
        return (value - start) / (end - start) * (targetEnd - targetStart) + targetStart;
    }

    template <class T> requires arithmetic<T>
    constexpr T wrap(const T value, const T min, const T max) {
        return value - (max - min) * std::floor((value - min) / (max - min));
    }

    /**
     * @brief Return sign of value
     * @param a 
     * @return constexpr int -1 if negative, 0 if 0, 1 if pos
     */
    template <class T> requires arithmetic<T>
    constexpr int sign(T a) {
        if (a == 0) return 0;
        return a < 0 ? -1 : 1;
    }

    /**
     * @brief Radians to degrees
     * @param rad Radians
     * @return constexpr float 
     */
    constexpr float rad2deg(const float rad) {
        return rad * 180.0f / 3.1415926535897f;
    }

    /**
     * @brief Degrees to radians
     * @param deg Degrees
     * @return constexpr float 
     */
    constexpr float deg2rad(const float deg) {
        return deg * 3.1415926535897f / 180.0f;
    }

    /**
     * @brief Take the given transform matrix, remove the translation
     *        and scaling in-place. (Note: assumes there is no skew,
     *        otherwise it will leave behind the skew and rotation)
     * 
     *        Assume matrix is multiplied with points so translation is last
     *        column and not the other way around (being, translation is last row)
     * @param mat Matrix to reduce in place
     */
    inline void reduce_to_rotation(Matrix &mat) {
        mat.m12 = mat.m13 = mat.m14 = mat.m15 = 0.0f; // Remove translation column

        // Normalize first 3 columns
        auto hypot = [](float a, float b, float c, float d) {
            return std::sqrt(a * a + b * b + c * c + d * d);
        };
        auto sx = hypot(mat.m0, mat.m1, mat.m2, mat.m3);
        mat.m0 /= sx;
        mat.m1 /= sx;
        mat.m2 /= sx;
        mat.m3 /= sx;
        auto sy = hypot(mat.m4, mat.m5, mat.m6, mat.m7);
        mat.m4 /= sy;
        mat.m5 /= sy;
        mat.m6 /= sy;
        mat.m7 /= sy;
        auto sz = hypot(mat.m8, mat.m9, mat.m10, mat.m11);
        mat.m8 /= sz;
        mat.m9 /= sz;
        mat.m10 /= sz;
        mat.m11 /= sz;
    }
}

#endif