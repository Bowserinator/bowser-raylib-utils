#ifndef BOWSER_UTIL_VECTOR_VECTOR2_H
#define BOWSER_UTIL_VECTOR_VECTOR2_H

#include "raylib.h"
#include <iostream>
#include <functional>
#include <cmath>

#ifndef BOWSER_UTIL_ARITHMETIC_CONCEPT
#define BOWSER_UTIL_ARITHMETIC_CONCEPT
template <class T>
concept arithmetic = std::integral<T> or std::floating_point<T>;
#endif

namespace bowser_util {
    template <class T> requires arithmetic<T>
    struct _baseVec2 {
        T x, y;

        _baseVec2(): x(0), y(0) {}
        _baseVec2(T x, T y): x(x), y(y) {}
        _baseVec2(T val): x(val), y(val) {}
        _baseVec2(const Vector2 &other): x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}
        _baseVec2(const _baseVec2<T> &other): x(other.x), y(other.y) {}
        _baseVec2 &operator=(const _baseVec2<T> &other) {
            x = other.x;
            y = other.y;
            return *this;
        }
        _baseVec2 &operator=(const Vector2 &other) {
            x = static_cast<T>(other.x);
            y = static_cast<T>(other.y);
            return *this;
        }

        operator Vector2() const {
            return Vector2{
                static_cast<float>(x),
                static_cast<float>(y)
            };
        }

        template <class U> requires arithmetic<U>
        operator _baseVec2<U>() const {
            return _baseVec2<U>(
                static_cast<U>(x),
                static_cast<U>(y)
            );
        }

        bool almostEquals(const _baseVec2<T> &other) const {
            return
                ((fabsf(x - other.x)) <= (0.000001f * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(other.x))))) &&
                ((fabsf(y - other.y)) <= (0.000001f * fmaxf(1.0f, fmaxf(fabsf(y), fabsf(other.y)))));
        }

        // Magnitude and magnitude squared of vector, will overflow
        // with large values but raymath also has that issue
        float length() const { return std::sqrt(lengthSqr()); }
        float lengthSqr() const { return static_cast<float>(x * x + y * y); }

        // Distance to another vector
        float distance(const _baseVec2<T> &other) const {
            return std::sqrt(distanceSqr(other));
        }
        float distanceSqr(const _baseVec2<T> &other) const {
            T dx = other.x - x;
            T dy = other.y - y;
            return static_cast<float>(dx * dx + dy * dy);
        }

        // Dot product
        T dot(const _baseVec2<T> &other) const {
            return other.x * x + other.y * y;
        }

        // Angle with other vector (from origin 0,0)
        float angle(const _baseVec2<T> &other) const {
            const auto dot = static_cast<float>(this->dot(other));
            const auto det = static_cast<float>(x * other.y - y * other.x);
            return std::atan2(det, dot);
        }

        // Normalize self to unit length
        template <class U=T> requires std::floating_point<U>
        _baseVec2<U> normalize() const {
            float len = length() + 0.000001f; // Avoid division by zero
            return _baseVec2<U>(x / len, y / len);
        }

        _baseVec2<T> applyOp(std::function<T(T)> op) const {
            return _baseVec2<T>(op(x), op(y));
        }

        // Transform by matrix (4x4 transformation, assumes z = 0)
        template <class U=T> requires std::floating_point<U>
        _baseVec2<U> transform(const Matrix &mat) const {
            return _baseVec2<U>(
                mat.m0 * x + mat.m4 * y + mat.m12,
                mat.m1 * x + mat.m5 * y + mat.m13
            );
        }

        // Lerp amount (0 - 1) way to the target
        template <class U=T> requires std::floating_point<U>
        _baseVec2<U> lerp(const _baseVec2<T> &target, float amount) const {
            return _baseVec2<T>(
                x + amount * (target.x - x),
                y + amount * (target.y - y)
            );
        }
    
        // Return this vector reflected across a surface given its normal
        _baseVec2<T> reflect(const _baseVec2<T> &normal) const {
            auto dotProduct = dot(normal);
            return _baseVec2<T>(
                x - 2 * normal.x * dotProduct,
                y - 2 * normal.y * dotProduct
            );
        }

        // Get this vector rotated around the origin by angle (radians)
        _baseVec2<T> rotate(const float angle) const {
            const float cosres = std::cos(angle);
            const float sinres = std::sin(angle);
            
            return _baseVec2<T>(
                x * cosres - y * sinres,
                x * sinres + y * cosres
            );
        }

        // Get this vector rotated around given origin by angle (radians)
        _baseVec2<T> rotate(const float angle, const _baseVec2<T> &origin) const {
            const _baseVec2<T> delta = *this - origin;
            const float cosres = std::cos(angle);
            const float sinres = std::sin(angle);
            
            return _baseVec2<T>(
                origin.x + delta.x * cosres - delta.y * sinres,
                origin.y + delta.x * sinres + delta.y * cosres
            );
        }
    
        // Move distance towards target (will not overshoot)
        template <class U=T> requires std::floating_point<U>
        _baseVec2<U> moveTowards(const _baseVec2<U> &target, const float distance) const {
            float disToOtherSqr = distanceSqr(target);
            if (disToOtherSqr == 0 || disToOtherSqr < distance * distance)
                return target;
            return lerp(target, distance / std::sqrt(disToOtherSqr));
        }
    
        // Clamp all values between vec1 and vec2
        _baseVec2<T> clamp(const _baseVec2<T> &vec1, const _baseVec2<T> &vec2) const {
            return _baseVec2(
                std::max(vec1.x, std::min(vec2.x, x)),
                std::max(vec1.y, std::min(vec2.y, y))
            );
        }

        // Clamp all components between a and b
        _baseVec2<T> clamp(const T a, const T b) const {
            return _baseVec2(
                std::max(a, std::min(b, x)),
                std::max(a, std::min(b, y))
            );
        }

        // Clamp magnitude
        _baseVec2<T> clampMagnitude(const T a, const T b) const {
            float len = length();
            if (len >= a && len <= b) return *this;
            float lenClamped = std::max(a, std::min(b, len));
            return (*this) / len * lenClamped;
        }


        friend inline bool operator==(const _baseVec2<T> &lhs, const _baseVec2<T> &rhs) {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }
        friend inline bool operator!=(const _baseVec2<T> &lhs, const _baseVec2<T> &rhs) {
            return lhs.x != rhs.x || lhs.y != rhs.y;
        }

        friend inline _baseVec2<T> &operator+=(_baseVec2<T> &lhs, const _baseVec2<T> &rhs) {
            lhs.x += rhs.x;
            lhs.y += rhs.y;
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> &operator+=(_baseVec2<T> &lhs, const S rhs) {
            lhs.x += rhs;
            lhs.y += rhs;
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> operator+(_baseVec2<T> lhs, const S rhs) {
            lhs += rhs;
            return lhs;
        }

        friend inline _baseVec2<T> operator+(_baseVec2<T> lhs, const _baseVec2<T> &rhs) {
            lhs += rhs;
            return lhs;
        }

        friend inline _baseVec2<T> &operator-=(_baseVec2<T> &lhs, const _baseVec2<T> &rhs) {
            lhs.x -= rhs.x;
            lhs.y -= rhs.y;
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> &operator-=(_baseVec2<T> &lhs, const S rhs) {
            lhs.x -= rhs;
            lhs.y -= rhs;
            return lhs;
        }

        friend inline _baseVec2<T> operator-(_baseVec2<T> lhs, const _baseVec2<T> &rhs) {
            lhs -= rhs;
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> operator-(_baseVec2<T> lhs, const S rhs) {
            lhs -= rhs;
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> &operator*=(_baseVec2<T> &lhs, const S rhs) {
            lhs.x = static_cast<T>(lhs.x * rhs);
            lhs.y = static_cast<T>(lhs.y * rhs);
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> &operator*=(_baseVec2<T> &lhs, const _baseVec2<S> &rhs) {
            lhs.x = static_cast<T>(lhs.x * rhs.x);
            lhs.y = static_cast<T>(lhs.y * rhs.y);
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> operator*(_baseVec2<T> lhs, const S rhs) {
            lhs *= rhs;
            return lhs;
        }
        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> operator*(_baseVec2<T> lhs, const _baseVec2<S> &rhs) {
            lhs *= rhs;
            return lhs;
        }
        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> operator*(const S lhs, _baseVec2<T> rhs) {
            rhs *= lhs;
            return rhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> &operator/=(_baseVec2<T> &lhs, const S rhs) {
            lhs.x = static_cast<T>(lhs.x / rhs);
            lhs.y = static_cast<T>(lhs.y / rhs);
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> operator/(_baseVec2<T> lhs, const S rhs) {
            lhs /= rhs;
            return lhs;
        }
        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> operator/(const S lhs, const _baseVec2<T> &rhs) {
            return _baseVec2(static_cast<T>(lhs / rhs.x), static_cast<T>(lhs / rhs.y));
        }

        friend inline _baseVec2<T> operator-(_baseVec2<T> in) {
            return -1 * in;
        }

        template <class S> requires std::integral<S>
        friend inline _baseVec2<T> operator%=(_baseVec2<T> lhs, const S rhs) {
            lhs.x = static_cast<T>(lhs.x % rhs);
            lhs.y = static_cast<T>(lhs.y % rhs);
            return lhs;
        }
        template <class S> requires std::floating_point<S>
        friend inline _baseVec2<T> operator%=(_baseVec2<T> lhs, const S rhs) {
            lhs.x = static_cast<T>(lhs.x - rhs * std::floor(lhs.x / rhs));
            lhs.y = static_cast<T>(lhs.y - rhs * std::floor(lhs.x / rhs));
            return lhs;
        }
        template <class S> requires arithmetic<S>
        friend inline _baseVec2<T> operator%(_baseVec2<T> lhs, const S rhs) {
            lhs %= rhs;
            return lhs;
        }

        template <class U=T> requires std::integral<U>
        inline _baseVec2<U> operator~() {
            return _baseVec2<U>(
                static_cast<U>(~x),
                static_cast<U>(~y)
            );
        }

        template <class U=T> requires std::integral<U>
        inline _baseVec2<U> operator&=(const _baseVec2<U> &rhs) {
            x = static_cast<U>(x & rhs.x);
            y = static_cast<U>(y & rhs.y);
            return *this;
        }

        template <class U=T> requires std::integral<U>
        inline _baseVec2<U> operator|=(const _baseVec2<U> &rhs) {
            x = static_cast<U>(x | rhs.x);
            y = static_cast<U>(y | rhs.y);
            return *this;
        }

        template <class U=T> requires std::integral<U>
        inline _baseVec2<U> operator^=(const _baseVec2<U> &rhs) {
            x = static_cast<U>(x ^ rhs.x);
            y = static_cast<U>(y ^ rhs.y);
            return *this;
        }

        template <class S, class U=T> requires (std::integral<S> and std::integral<U>)
        inline _baseVec2<U> operator<<=(const S &rhs) {
            x = static_cast<U>(x << rhs);
            y = static_cast<U>(y << rhs);
            return *this;
        }

        template <class S, class U=T> requires (std::integral<S> and std::integral<U>)
        inline _baseVec2<U> operator>>=(const S &rhs) {
            x = static_cast<U>(x >> rhs);
            y = static_cast<U>(y >> rhs);
            return *this;
        }
    };
    
    template <class T> requires arithmetic<T>
    inline std::ostream& operator<<(std::ostream& os, const _baseVec2<T>& vec) {
        os << '<' << vec.x << ", " << vec.y << '>';
        return os;
    }

    template <class T> requires arithmetic<T>
    inline std::istream& operator>>(std::istream& is, _baseVec2<T>& vec) {
        T x, y;
        if ((is >> x) && (is >> y)) {
            vec.x = x;
            vec.y = y;
        }
        return is;
    }

    template <class U> requires std::integral<U>
    inline _baseVec2<U> operator&(_baseVec2<U> lhs, const _baseVec2<U> &rhs) {
        lhs &= rhs;
        return lhs;
    }

    template <class U> requires std::integral<U>
    inline _baseVec2<U> operator|(_baseVec2<U> lhs, const _baseVec2<U> &rhs) {
        lhs |= rhs;
        return lhs;
    }

    template <class U> requires std::integral<U>
    inline _baseVec2<U> operator^(_baseVec2<U> lhs, const _baseVec2<U> &rhs) {
        lhs ^= rhs;
        return lhs;
    }

    template <class S, class U> requires (std::integral<S> and std::integral<U>)
    inline _baseVec2<U> operator<<(_baseVec2<U> lhs, const S &rhs) {
        lhs <<= rhs;
        return lhs;
    }

    template <class S, class U> requires (std::integral<S> and std::integral<U>)
    inline _baseVec2<U> operator>>(_baseVec2<U> lhs, const S &rhs) {
        lhs >>= rhs;
        return lhs;
    }
}

#endif