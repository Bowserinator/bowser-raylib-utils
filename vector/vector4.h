#ifndef BOWSER_UTIL_VECTOR_VECTOR4_H
#define BOWSER_UTIL_VECTOR_VECTOR4_H

#include "raymath.h"
#include <iostream>
#include <cmath>
#include <functional>

#ifndef BOWSER_UTIL_ARITHMETIC_CONCEPT
#define BOWSER_UTIL_ARITHMETIC_CONCEPT
template <class T>
concept arithmetic = std::integral<T> or std::floating_point<T>;
#endif

namespace bowser_util {
    template <class T> requires arithmetic<T>
    struct _baseVec4 {
        T x, y, z, w;

        _baseVec4(T x, T y, T z, T w): x(x), y(y), z(z), w(w) {}
        _baseVec4(T val): x(val), y(val), z(val), w(val) {}
        _baseVec4(const Vector4 &other):
            x(static_cast<T>(other.x)),
            y(static_cast<T>(other.y)),
            z(static_cast<T>(other.z)),
            w(static_cast<T>(other.w)) {}
        _baseVec4(const _baseVec4<T> &other): x(other.x), y(other.y), z(other.z), w(other.w) {}
        _baseVec4 &operator=(const _baseVec4<T> &other) {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
            return *this;
        }
        _baseVec4 &operator=(const Vector4 &other) {
            x = static_cast<T>(other.x);
            y = static_cast<T>(other.y);
            z = static_cast<T>(other.z);
            w = static_cast<T>(other.w);
            return *this;
        }

        operator Vector4() const {
            return Vector4{
                static_cast<float>(x),
                static_cast<float>(y),
                static_cast<float>(z),
                static_cast<float>(w)
            };
        }

        template <class U> requires arithmetic<U>
        operator _baseVec4<U>() const {
            return _baseVec4<U>(
                static_cast<U>(x),
                static_cast<U>(y),
                static_cast<U>(z),
                static_cast<U>(w)
            );
        }

        bool almostEquals(const _baseVec4<T> &other) const {
            return
                ((fabsf(x - other.x)) <= (0.000001f * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(other.x))))) &&
                ((fabsf(y - other.y)) <= (0.000001f * fmaxf(1.0f, fmaxf(fabsf(y), fabsf(other.y))))) &&
                ((fabsf(z - other.z)) <= (0.000001f * fmaxf(1.0f, fmaxf(fabsf(z), fabsf(other.z))))) &&
                ((fabsf(w - other.w)) <= (0.000001f * fmaxf(1.0f, fmaxf(fabsf(w), fabsf(other.w)))));
        }

        // Magnitude and magnitude squared of vector, will overflow
        // with large values but raymath also has that issue
        float length() const { return std::sqrt(lengthSqr()); }
        float lengthSqr() const { return static_cast<float>(x * x + y * y + z * z + w * w); }

        // Distance to another vector
        float distance(const _baseVec4<T> &other) const {
            return std::sqrt(distanceSqr(other));
        }
        float distanceSqr(const _baseVec4<T> &other) const {
            T dx = other.x - x;
            T dy = other.y - y;
            T dz = other.z - z;
            T dw = other.w - w;
            return static_cast<float>(dx * dx + dy * dy + dz * dz + dw * dw);
        }

        // Dot product
        T dot(const _baseVec4<T> &other) const {
            return other.x * x + other.y * y + other.z * z + other.w * w;
        }

        // Normalize self to unit length
        template <class U=T> requires std::floating_point<U>
        _baseVec4<U> normalize() const {
            float len = length() + 0.000001f; // Avoid division by zero
            return _baseVec4<U>(x / len, y / len, z / len, w / len);
        }

        _baseVec4<T> applyOp(std::function<T(T)> op) const {
            return _baseVec4<T>(op(x), op(y), op(z), op(w));
        }

        // Transform by matrix (4x4 transformation, assumes z = 0)
        template <class U=T> requires std::floating_point<U>
        _baseVec4<U> transform(const Matrix &mat) const {
            return _baseVec4<U>(
                mat.m0 * x + mat.m4 * y + mat.m8 * z + mat.m12 * w,
                mat.m1 * x + mat.m5 * y + mat.m9 * z + mat.m13 * w,
                mat.m2 * x + mat.m6 * y + mat.m10 * z + mat.m14 * w,
                mat.m3 * x + mat.m7 * y + mat.m11 * z + mat.m15 * w
            );
        }

        // Lerp amount (0 - 1) way to the target
        template <class U=T> requires std::floating_point<U>
        _baseVec4<U> lerp(const _baseVec4<T> &target, float amount) const {
            return _baseVec4<T>(
                x + amount * (target.x - x),
                y + amount * (target.y - y),
                z + amount * (target.z - z),
                w + amount * (target.w - w)
            );
        }
    
        // Return this vector reflected across a surface given its normal
        _baseVec4<T> reflect(const _baseVec4<T> &normal) const {
            auto dotProduct = dot(normal);
            return _baseVec4<T>(
                x - 2 * normal.x * dotProduct,
                y - 2 * normal.y * dotProduct,
                z - 2 * normal.z * dotProduct,
                w - 2 * normal.w * dotProduct
            );
        }

        // Move distance towards target (will not overshoot)
        template <class U=T> requires std::floating_point<U>
        _baseVec4<U> moveTowards(const _baseVec4<U> &target, const float distance) const {
            float disToOtherSqr = distanceSqr(target);
            if (disToOtherSqr == 0 || disToOtherSqr < distance * distance)
                return target;
            return lerp(target, distance / std::sqrt(disToOtherSqr));
        }
    
        // Clamp all values between vec1 and vec2
        _baseVec4<T> clamp(const _baseVec4<T> &vec1, const _baseVec4<T> &vec2) const {
            return _baseVec4(
                std::max(vec1.x, std::min(vec2.x, x)),
                std::max(vec1.y, std::min(vec2.y, y)),
                std::max(vec1.z, std::min(vec2.z, z)),
                std::max(vec1.w, std::min(vec2.w, w))
            );
        }

        // Clamp all components between a and b
        _baseVec4<T> clamp(const T a, const T b) const {
            return _baseVec4(
                std::max(a, std::min(b, x)),
                std::max(a, std::min(b, y)),
                std::max(a, std::min(b, z)),
                std::max(a, std::min(b, w))
            );
        }


        friend inline bool operator==(const _baseVec4<T> &lhs, const _baseVec4<T> &rhs) {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
        }
        friend inline bool operator!=(const _baseVec4<T> &lhs, const _baseVec4<T> &rhs) {
            return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w;
        }

        friend inline _baseVec4<T> &operator+=(_baseVec4<T> &lhs, const _baseVec4<T> &rhs) {
            lhs.x += rhs.x;
            lhs.y += rhs.y;
            lhs.z += rhs.z;
            lhs.w += rhs.w;
            return lhs;
        }

        friend inline _baseVec4<T> operator+(_baseVec4<T> lhs, const _baseVec4<T> &rhs) {
            lhs += rhs;
            return lhs;
        }

        friend inline _baseVec4<T> &operator-=(_baseVec4<T> &lhs, const _baseVec4<T> &rhs) {
            lhs.x -= rhs.x;
            lhs.y -= rhs.y;
            lhs.z -= rhs.z;
            lhs.w -= rhs.w;
            return lhs;
        }

        friend inline _baseVec4<T> operator-(_baseVec4<T> lhs, const _baseVec4<T> &rhs) {
            lhs -= rhs;
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec4<T> &operator*=(_baseVec4<T> &lhs, const S rhs) {
            lhs.x = static_cast<T>(lhs.x * rhs);
            lhs.y = static_cast<T>(lhs.y * rhs);
            lhs.z = static_cast<T>(lhs.z * rhs);
            lhs.w = static_cast<T>(lhs.w * rhs);
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec4<T> operator*(_baseVec4<T> lhs, const S rhs) {
            lhs *= rhs;
            return lhs;
        }
        template <class S> requires arithmetic<S>
        friend inline _baseVec4<T> operator*(const S lhs, _baseVec4<T> rhs) {
            rhs *= lhs;
            return rhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec4<T> &operator/=(_baseVec4<T> &lhs, const S rhs) {
            lhs.x = static_cast<T>(lhs.x / rhs);
            lhs.y = static_cast<T>(lhs.y / rhs);
            lhs.z = static_cast<T>(lhs.z / rhs);
            lhs.w = static_cast<T>(lhs.w / rhs);
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec4<T> operator/(_baseVec4<T> lhs, const S rhs) {
            lhs /= rhs;
            return lhs;
        }
        template <class S> requires arithmetic<S>
        friend inline _baseVec4<T> operator/(const S lhs, const _baseVec4<T> &rhs) {
            return _baseVec4(
                static_cast<T>(lhs / rhs.x),
                static_cast<T>(lhs / rhs.y),
                static_cast<T>(lhs / rhs.z),
                static_cast<T>(lhs / rhs.w)
            );
        }

        friend inline _baseVec4<T> operator-(_baseVec4<T> in) {
            return -1 * in;
        }

        template <class S> requires std::integral<S>
        friend inline _baseVec4<T> operator%=(_baseVec4<T> lhs, const S rhs) {
            lhs.x = static_cast<T>(lhs.x % rhs);
            lhs.y = static_cast<T>(lhs.y % rhs);
            lhs.z = static_cast<T>(lhs.z % rhs);
            lhs.w = static_cast<T>(lhs.w % rhs);
            return lhs;
        }
        template <class S> requires std::floating_point<S>
        friend inline _baseVec4<T> operator%=(_baseVec4<T> lhs, const S rhs) {
            lhs.x = static_cast<T>(lhs.x - rhs * std::floor(lhs.x / rhs));
            lhs.y = static_cast<T>(lhs.y - rhs * std::floor(lhs.x / rhs));
            lhs.z = static_cast<T>(lhs.z - rhs * std::floor(lhs.z / rhs));
            lhs.w = static_cast<T>(lhs.w - rhs * std::floor(lhs.w / rhs));
            return lhs;
        }
        template <class S> requires arithmetic<S>
        friend inline _baseVec4<T> operator%(_baseVec4<T> lhs, const S rhs) {
            lhs %= rhs;
            return lhs;
        }

        template <class U=T> requires std::integral<U>
        inline _baseVec4<U> operator~() {
            return _baseVec4<U>(
                static_cast<U>(~x),
                static_cast<U>(~y),
                static_cast<U>(~z),
                static_cast<U>(~w)
            );
        }

        template <class U=T> requires std::integral<U>
        inline _baseVec4<U> operator&=(const _baseVec4<U> &rhs) {
            x = static_cast<U>(x & rhs.x);
            y = static_cast<U>(y & rhs.y);
            z = static_cast<U>(z & rhs.z);
            w = static_cast<U>(w & rhs.w);
            return *this;
        }

        template <class U=T> requires std::integral<U>
        inline _baseVec4<U> operator|=(const _baseVec4<U> &rhs) {
            x = static_cast<U>(x | rhs.x);
            y = static_cast<U>(y | rhs.y);
            z = static_cast<U>(z | rhs.z);
            w = static_cast<U>(w | rhs.w);
            return *this;
        }

        template <class U=T> requires std::integral<U>
        inline _baseVec4<U> operator^=(const _baseVec4<U> &rhs) {
            x = static_cast<U>(x ^ rhs.x);
            y = static_cast<U>(y ^ rhs.y);
            z = static_cast<U>(z ^ rhs.z);
            w = static_cast<U>(w ^ rhs.w);
            return *this;
        }

        template <class S, class U=T> requires (std::integral<S> and std::integral<U>)
        inline _baseVec4<U> operator<<=(const S &rhs) {
            x = static_cast<U>(x << rhs);
            y = static_cast<U>(y << rhs);
            z = static_cast<U>(z << rhs);
            w = static_cast<U>(w << rhs);
            return *this;
        }

        template <class S, class U=T> requires (std::integral<S> and std::integral<U>)
        inline _baseVec4<U> operator>>=(const S &rhs) {
            x = static_cast<U>(x >> rhs);
            y = static_cast<U>(y >> rhs);
            z = static_cast<U>(z >> rhs);
            w = static_cast<U>(w >> rhs);
            return *this;
        }
    };
    
    template <class T> requires arithmetic<T>
    inline std::ostream& operator<<(std::ostream& os, const _baseVec4<T>& vec) {
        os << '<' << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << '>';
        return os;
    }

    template <class T> requires arithmetic<T>
    inline std::istream& operator>>(std::istream& is, _baseVec4<T>& vec) {
        T x, y, z, w;
        if ((is >> x) && (is >> y) && (is >> z) && (is >> w)) {
            vec.x = x;
            vec.y = y;
            vec.z = z;
            vec.w = w;
        }
        return is;
    }

    template <class U> requires std::integral<U>
    inline _baseVec4<U> operator&(_baseVec4<U> lhs, const _baseVec4<U> &rhs) {
        lhs &= rhs;
        return lhs;
    }

    template <class U> requires std::integral<U>
    inline _baseVec4<U> operator|(_baseVec4<U> lhs, const _baseVec4<U> &rhs) {
        lhs |= rhs;
        return lhs;
    }

    template <class U> requires std::integral<U>
    inline _baseVec4<U> operator^(_baseVec4<U> lhs, const _baseVec4<U> &rhs) {
        lhs ^= rhs;
        return lhs;
    }

    template <class S, class U> requires (std::integral<S> and std::integral<U>)
    inline _baseVec4<U> operator<<(_baseVec4<U> lhs, const S &rhs) {
        lhs <<= rhs;
        return lhs;
    }

    template <class S, class U> requires (std::integral<S> and std::integral<U>)
    inline _baseVec4<U> operator>>(_baseVec4<U> lhs, const S &rhs) {
        lhs >>= rhs;
        return lhs;
    }
}

#endif