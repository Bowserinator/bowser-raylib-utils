#ifndef BOWSER_UTIL_VECTOR_VECTOR3_H
#define BOWSER_UTIL_VECTOR_VECTOR3_H

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
    struct _baseVec3 {
        T x, y, z;

        _baseVec3(T x, T y, T z): x(x), y(y), z(z) {}
        _baseVec3(T val): x(val), y(val), z(val) {}
        _baseVec3(const Vector3 &other):
            x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)) {}
        _baseVec3(const _baseVec3<T> &other): x(other.x), y(other.y), z(other.z) {}
        _baseVec3 &operator=(const _baseVec3<T> &other) {
            x = other.x;
            y = other.y;
            z = other.z;
            return *this;
        }
        _baseVec3 &operator=(const Vector3 &other) {
            x = static_cast<T>(other.x);
            y = static_cast<T>(other.y);
            z = static_cast<T>(other.z);
            return *this;
        }

        operator Vector3() const {
            return Vector3{
                static_cast<float>(x),
                static_cast<float>(y),
                static_cast<float>(z)
            };
        }

        template <class U> requires arithmetic<U>
        operator _baseVec3<U>() const {
            return _baseVec3<U>(
                static_cast<U>(x),
                static_cast<U>(y),
                static_cast<U>(z)
            );
        }

        bool almostEquals(const _baseVec3<T> &other) const {
            return
                ((fabsf(x - other.x)) <= (0.000001f * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(other.x))))) &&
                ((fabsf(y - other.y)) <= (0.000001f * fmaxf(1.0f, fmaxf(fabsf(y), fabsf(other.y))))) &&
                ((fabsf(z - other.z)) <= (0.000001f * fmaxf(1.0f, fmaxf(fabsf(z), fabsf(other.z)))));
        }

        // Magnitude and magnitude squared of vector, will overflow
        // with large values but raymath also has that issue
        float length() const { return std::sqrt(lengthSqr()); }
        float lengthSqr() const { return static_cast<float>(x * x + y * y + z * z); }

        // Distance to another vector
        float distance(const _baseVec3<T> &other) const {
            return std::sqrt(distanceSqr(other));
        }
        float distanceSqr(const _baseVec3<T> &other) const {
            T dx = other.x - x;
            T dy = other.y - y;
            T dz = other.z - z;
            return static_cast<float>(dx * dx + dy * dy + dz * dz);
        }

        // Dot product
        T dot(const _baseVec3<T> &other) const {
            return other.x * x + other.y * y + other.z * z;
        }

        // Angle with other vector (from origin 0,0)
        float angle(const _baseVec3<T> &other) const {
            return Vector3Angle(*this, other);
        }

        // Normalize self to unit length
        template <class U=T> requires std::floating_point<U>
        _baseVec3<U> normalize() const {
            float len = length() + 0.000001f; // Avoid division by zero
            return _baseVec3<U>(x / len, y / len, z / len);
        }

        _baseVec3<T> applyOp(std::function<T(T)> op) const {
            return _baseVec3<T>(op(x), op(y), op(z));
        }

        // Transform by matrix (4x4 transformation, assumes z = 0)
        template <class U=T> requires std::floating_point<U>
        _baseVec3<U> transform(const Matrix &mat) const {
            return _baseVec3<U>(
                mat.m0 * x + mat.m4 * y + mat.m8 * z + mat.m12,
                mat.m1 * x + mat.m5 * y + mat.m9 * z + mat.m13,
                mat.m2 * x + mat.m6 * y + mat.m10 * z + mat.m14
            );
        }

        // Lerp amount (0 - 1) way to the target
        template <class U=T> requires std::floating_point<U>
        _baseVec3<U> lerp(const _baseVec3<T> &target, float amount) const {
            return _baseVec3<T>(
                x + amount * (target.x - x),
                y + amount * (target.y - y),
                z + amount * (target.z - z)
            );
        }
    
        // Return this vector reflected across a surface given its normal
        _baseVec3<T> reflect(const _baseVec3<T> &normal) const {
            auto dotProduct = dot(normal);
            return _baseVec3<T>(
                x - 2 * normal.x * dotProduct,
                y - 2 * normal.y * dotProduct,
                z - 2 * normal.z * dotProduct
            );
        }

        // Cross product
        _baseVec3<T> cross(const _baseVec3<T> &other) const {
            return _baseVec3<T>(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        _baseVec3<T> rotateByQuaternion(const Quaternion &q) const {
            return Vector3RotateByQuaternion(*this, q);
        }

        _baseVec3<T> rotateByAxisAngle(const _baseVec3<T> &axis, float angle) const {
            return Vector3RotateByAxisAngle(*this, axis, angle);
        }

        _baseVec3<T> unproject(const Matrix &projection, const Matrix &view) const {
            return Vector3Unproject(*this, projection, view);
        }

        // Refract with given surface normal and index of refraction
        // Note: this must be normalized
        // Returns 0 if total internal reflection would occur
        _baseVec3<T> refract(const _baseVec3<T> &normal, const float r) const {
            _baseVec3<T> out(0);
            auto dot = dot(normal);
            float d = 1.0f - r * r * (1.0f - dot * dot);

            if (d >= 0.0f) {
                d = std::sqrt(d);
                out.x = r * x - (r * dot + d) * normal.x;
                out.y = r * y - (r * dot + d) * normal.y;
                out.z = r * z - (r * dot + d) * normal.z;
            }
            return out;
        }

        // Move distance towards target (will not overshoot)
        template <class U=T> requires std::floating_point<U>
        _baseVec3<U> moveTowards(const _baseVec3<U> &target, const float distance) const {
            float disToOtherSqr = distanceSqr(target);
            if (disToOtherSqr == 0 || disToOtherSqr < distance * distance)
                return target;
            return lerp(target, distance / std::sqrt(disToOtherSqr));
        }
    
        // Clamp all values between vec1 and vec2
        _baseVec3<T> clamp(const _baseVec3<T> &vec1, const _baseVec3<T> &vec2) const {
            return _baseVec3(
                std::max(vec1.x, std::min(vec2.x, x)),
                std::max(vec1.y, std::min(vec2.y, y)),
                std::max(vec1.z, std::min(vec2.z, z))
            );
        }

        // Clamp all components between a and b
        _baseVec3<T> clamp(const T a, const T b) const {
            return _baseVec3(
                std::max(a, std::min(b, x)),
                std::max(a, std::min(b, y)),
                std::max(a, std::min(b, z))
            );
        }


        friend inline bool operator==(const _baseVec3<T> &lhs, const _baseVec3<T> &rhs) {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
        }
        friend inline bool operator!=(const _baseVec3<T> &lhs, const _baseVec3<T> &rhs) {
            return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
        }

        friend inline _baseVec3<T> &operator+=(_baseVec3<T> &lhs, const _baseVec3<T> &rhs) {
            lhs.x += rhs.x;
            lhs.y += rhs.y;
            lhs.z += rhs.z;
            return lhs;
        }

        friend inline _baseVec3<T> operator+(_baseVec3<T> lhs, const _baseVec3<T> &rhs) {
            lhs += rhs;
            return lhs;
        }

        friend inline _baseVec3<T> &operator-=(_baseVec3<T> &lhs, const _baseVec3<T> &rhs) {
            lhs.x -= rhs.x;
            lhs.y -= rhs.y;
            lhs.z -= rhs.z;
            return lhs;
        }

        friend inline _baseVec3<T> operator-(_baseVec3<T> lhs, const _baseVec3<T> &rhs) {
            lhs -= rhs;
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec3<T> &operator*=(_baseVec3<T> &lhs, const S rhs) {
            lhs.x = static_cast<T>(lhs.x * rhs);
            lhs.y = static_cast<T>(lhs.y * rhs);
            lhs.z = static_cast<T>(lhs.z * rhs);
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec3<T> operator*(_baseVec3<T> lhs, const S rhs) {
            lhs *= rhs;
            return lhs;
        }
        template <class S> requires arithmetic<S>
        friend inline _baseVec3<T> operator*(const S lhs, _baseVec3<T> rhs) {
            rhs *= lhs;
            return rhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec3<T> &operator/=(_baseVec3<T> &lhs, const S rhs) {
            lhs.x = static_cast<T>(lhs.x / rhs);
            lhs.y = static_cast<T>(lhs.y / rhs);
            lhs.z = static_cast<T>(lhs.z / rhs);
            return lhs;
        }

        template <class S> requires arithmetic<S>
        friend inline _baseVec3<T> operator/(_baseVec3<T> lhs, const S rhs) {
            lhs /= rhs;
            return lhs;
        }
        template <class S> requires arithmetic<S>
        friend inline _baseVec3<T> operator/(const S lhs, const _baseVec3<T> &rhs) {
            return _baseVec3(
                static_cast<T>(lhs / rhs.x),
                static_cast<T>(lhs / rhs.y),
                static_cast<T>(lhs / rhs.z)
            );
        }

        friend inline _baseVec3<T> operator-(_baseVec3<T> in) {
            return -1 * in;
        }

        template <class S> requires std::integral<S>
        friend inline _baseVec3<T> operator%=(_baseVec3<T> lhs, const S rhs) {
            lhs.x = static_cast<T>(lhs.x % rhs);
            lhs.y = static_cast<T>(lhs.y % rhs);
            lhs.z = static_cast<T>(lhs.z % rhs);
            return lhs;
        }
        template <class S> requires std::floating_point<S>
        friend inline _baseVec3<T> operator%=(_baseVec3<T> lhs, const S rhs) {
            lhs.x = static_cast<T>(lhs.x - rhs * std::floor(lhs.x / rhs));
            lhs.y = static_cast<T>(lhs.y - rhs * std::floor(lhs.x / rhs));
            lhs.z = static_cast<T>(lhs.z - rhs * std::floor(lhs.z / rhs));
            return lhs;
        }
        template <class S> requires arithmetic<S>
        friend inline _baseVec3<T> operator%(_baseVec3<T> lhs, const S rhs) {
            lhs %= rhs;
            return lhs;
        }

        template <class U=T> requires std::integral<U>
        inline _baseVec3<U> operator~() {
            return _baseVec3<U>(
                static_cast<U>(~x),
                static_cast<U>(~y),
                static_cast<U>(~z)
            );
        }

        template <class U=T> requires std::integral<U>
        inline _baseVec3<U> operator&=(const _baseVec3<U> &rhs) {
            x = static_cast<U>(x & rhs.x);
            y = static_cast<U>(y & rhs.y);
            z = static_cast<U>(z & rhs.z);
            return *this;
        }

        template <class U=T> requires std::integral<U>
        inline _baseVec3<U> operator|=(const _baseVec3<U> &rhs) {
            x = static_cast<U>(x | rhs.x);
            y = static_cast<U>(y | rhs.y);
            z = static_cast<U>(z | rhs.z);
            return *this;
        }

        template <class U=T> requires std::integral<U>
        inline _baseVec3<U> operator^=(const _baseVec3<U> &rhs) {
            x = static_cast<U>(x ^ rhs.x);
            y = static_cast<U>(y ^ rhs.y);
            z = static_cast<U>(z ^ rhs.z);
            return *this;
        }

        template <class S, class U=T> requires (std::integral<S> and std::integral<U>)
        inline _baseVec3<U> operator<<=(const S &rhs) {
            x = static_cast<U>(x << rhs);
            y = static_cast<U>(y << rhs);
            z = static_cast<U>(z << rhs);
            return *this;
        }

        template <class S, class U=T> requires (std::integral<S> and std::integral<U>)
        inline _baseVec3<U> operator>>=(const S &rhs) {
            x = static_cast<U>(x >> rhs);
            y = static_cast<U>(y >> rhs);
            z = static_cast<U>(z >> rhs);
            return *this;
        }
    };
    
    template <class T> requires arithmetic<T>
    inline std::ostream& operator<<(std::ostream& os, const _baseVec3<T>& vec) {
        os << '<' << vec.x << ", " << vec.y << ", " << vec.z << '>';
        return os;
    }

    template <class T> requires arithmetic<T>
    inline std::istream& operator>>(std::istream& is, _baseVec3<T>& vec) {
        T x, y, z;
        if ((is >> x) && (is >> y) && (is >> z)) {
            vec.x = x;
            vec.y = y;
            vec.z = z;
        }
        return is;
    }

    template <class U> requires std::integral<U>
    inline _baseVec3<U> operator&(_baseVec3<U> lhs, const _baseVec3<U> &rhs) {
        lhs &= rhs;
        return lhs;
    }

    template <class U> requires std::integral<U>
    inline _baseVec3<U> operator|(_baseVec3<U> lhs, const _baseVec3<U> &rhs) {
        lhs |= rhs;
        return lhs;
    }

    template <class U> requires std::integral<U>
    inline _baseVec3<U> operator^(_baseVec3<U> lhs, const _baseVec3<U> &rhs) {
        lhs ^= rhs;
        return lhs;
    }

    template <class S, class U> requires (std::integral<S> and std::integral<U>)
    inline _baseVec3<U> operator<<(_baseVec3<U> lhs, const S &rhs) {
        lhs <<= rhs;
        return lhs;
    }

    template <class S, class U> requires (std::integral<S> and std::integral<U>)
    inline _baseVec3<U> operator>>(_baseVec3<U> lhs, const S &rhs) {
        lhs >>= rhs;
        return lhs;
    }
}

#endif