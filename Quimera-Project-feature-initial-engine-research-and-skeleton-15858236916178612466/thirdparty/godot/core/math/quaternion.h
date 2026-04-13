#pragma once

#include "vector3.h"

struct [[nodiscard]] Quaternion {
        union {
                struct {
                        real_t x;
                        real_t y;
                        real_t z;
                        real_t w;
                };
                real_t components[4];
        };

        _FORCE_INLINE_ real_t &operator[](int p_idx) {
                return components[p_idx];
        }
        _FORCE_INLINE_ const real_t &operator[](int p_idx) const {
                return components[p_idx];
        }

        _FORCE_INLINE_ real_t length_squared() const { return dot(*this); }
        _FORCE_INLINE_ real_t length() const { return Math::sqrt(length_squared()); }

        void normalize() {
            real_t l = length_squared();
            if (l == 0) {
                x = y = z = 0;
                w = 1;
            } else {
                l = Math::sqrt(l);
                x /= l;
                y /= l;
                z /= l;
                w /= l;
            }
        }

        Quaternion normalized() const {
            Quaternion q = *this;
            q.normalize();
            return q;
        }

        Quaternion inverse() const {
            return Quaternion(-x, -y, -z, w);
        }

        _FORCE_INLINE_ real_t dot(const Quaternion &p_q) const {
            return x * p_q.x + y * p_q.y + z * p_q.z + w * p_q.w;
        }

        _FORCE_INLINE_ Vector3 xform(const Vector3 &p_v) const {
                Vector3 u(x, y, z);
                Vector3 uv = u.cross(p_v);
                return p_v + ((uv * w) + u.cross(uv)) * ((real_t)2);
        }

        _FORCE_INLINE_ Vector3 xform_inv(const Vector3 &p_v) const {
                return inverse().xform(p_v);
        }

        constexpr void operator+=(const Quaternion &p_q) {
            x += p_q.x; y += p_q.y; z += p_q.z; w += p_q.w;
        }

        constexpr void operator-=(const Quaternion &p_q) {
            x -= p_q.x; y -= p_q.y; z -= p_q.z; w -= p_q.w;
        }

        constexpr void operator*=(real_t p_s) {
            x *= p_s; y *= p_s; z *= p_s; w *= p_s;
        }

        constexpr void operator/=(real_t p_s) {
            *this *= (1.0f / p_s);
        }

        constexpr Quaternion operator+(const Quaternion &p_q2) const {
            return Quaternion(x + p_q2.x, y + p_q2.y, z + p_q2.z, w + p_q2.w);
        }

        constexpr Quaternion operator-(const Quaternion &p_q2) const {
            return Quaternion(x - p_q2.x, y - p_q2.y, z - p_q2.z, w - p_q2.w);
        }

        constexpr Quaternion operator-() const {
            return Quaternion(-x, -y, -z, -w);
        }

        constexpr Quaternion operator*(real_t p_s) const {
            return Quaternion(x * p_s, y * p_s, z * p_s, w * p_s);
        }

        constexpr Quaternion operator/(real_t p_s) const {
            return *this * (1.0f / p_s);
        }

        constexpr bool operator==(const Quaternion &p_quaternion) const {
            return x == p_quaternion.x && y == p_quaternion.y && z == p_quaternion.z && w == p_quaternion.w;
        }

        constexpr bool operator!=(const Quaternion &p_quaternion) const {
            return !(*this == p_quaternion);
        }

        constexpr Quaternion() : x(0), y(0), z(0), w(1) {}

        constexpr Quaternion(real_t p_x, real_t p_y, real_t p_z, real_t p_w) :
                        x(p_x), y(p_y), z(p_z), w(p_w) {}

        constexpr Quaternion(const Quaternion &p_q) :
                        x(p_q.x), y(p_q.y), z(p_q.z), w(p_q.w) {}

        constexpr void operator=(const Quaternion &p_q) {
                x = p_q.x;
                y = p_q.y;
                z = p_q.z;
                w = p_q.w;
        }

        constexpr void operator*=(const Quaternion &p_q) {
            real_t xx = w * p_q.x + x * p_q.w + y * p_q.z - z * p_q.y;
            real_t yy = w * p_q.y + y * p_q.w + z * p_q.x - x * p_q.z;
            real_t zz = w * p_q.z + z * p_q.w + x * p_q.y - y * p_q.x;
            w = w * p_q.w - x * p_q.x - y * p_q.y - z * p_q.z;
            x = xx;
            y = yy;
            z = zz;
        }

        constexpr Quaternion operator*(const Quaternion &p_q) const {
            Quaternion r = *this;
            r *= p_q;
            return r;
        }
};

inline constexpr Quaternion operator*(real_t p_real, const Quaternion &p_quaternion) {
        return p_quaternion * p_real;
}
