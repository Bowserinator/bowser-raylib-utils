#ifndef BOWSER_UTIL_EASING_H
#define BOWSER_UTIL_EASING_H

#include <cmath>
#include <numbers>

namespace bowser_util {
    // @see https://easings.net/
    inline float easeInSine(float t) { return 1 - std::cos(t * std::numbers::pi / 2); }
    inline float easeOutSine(float t) { return std::sin(t * std::numbers::pi / 2); }
    inline float easeInOutSine(float t) { return -(std::cos(std::numbers::pi * t) - 1) / 2; }

    inline float easeInCubic(float t) { return t * t * t; }
    inline float easeOutCubic(float t) { return 1 - (1-t)*(1-t)*(1-t); }
    inline float easeInOutCubic(float t) { return t < 0.5 ? 4*t*t*t : 1 - std::pow(-2*t + 2, 3) / 2; }

    inline float easeInExp(float t, float lambda = 10) { t == 0 ? 0 : std::pow(2, lambda * (t - 1)); }
    inline float easeOutExp(float t, float lambda = 10) { t == 1 ? 1 : 1 - std::pow(2, -lambda * t); }
    inline float easeInOutExp(float t, float lambda = 10) {
        if (t == 0) return 0;
        if (t == 1) return 1;
        return t < 0.5 ?
            std::pow(2, 2 * lambda * t - lambda) / 2 :
            2 - std::pow(2, -2 * lambda * t + lambda) / 2;
    }

    inline float easeInBounce(float t) { return 1- easeOutBounce(1 - t); }
    inline float easeOutBounce(float t) {
        constexpr float n1 = 7.5625;
        constexpr float d1 = 2.75;

        if (t < 1 / d1)
            return n1 * t * t;
        else if (t < 2 / d1)
            return n1 * (t -= 1.5 / d1) * t + 0.75;
        else if (t < 2.5 / d1)
            return n1 * (t -= 2.25 / d1) * t + 0.9375;
        else 
            return n1 * (t -= 2.625 / d1) * t + 0.984375;
    }
    inline float easeInOutBounce(float t) {
        return t < 0.5
            ? (1 - easeOutBounce(1 - 2 * t)) / 2
            : (1 + easeOutBounce(2 * t - 1)) / 2;
    }

    inline float easeInBack(float t) {
        constexpr float c1 = 1.70158;
        constexpr float c3 = c1 + 1;
        return c3 * t * t* t - c1 * t * t;
    }
    inline float easeOutBack(float t) {
        constexpr float c1 = 1.70158;
        constexpr float c3 = c1 + 1;
        return 1 + c3 * std::pow(t - 1, 3) + c1 * std::pow(t - 1, 2);
    }
    inline float easeInOutBack(float t) {
        constexpr float c1 = 1.70158;
        constexpr float c2 = c1 * 1.525;
        return t < 0.5
            ? (std::pow(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2
            : (std::pow(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2;
    }
};

#endif