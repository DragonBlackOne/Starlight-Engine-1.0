#pragma once

#include "math_stubs.h"

struct Basis;
struct Vector2;
struct Vector3i;

struct [[nodiscard]] Vector3 {
        static const Vector3 LEFT;
        static const Vector3 RIGHT;
        static const Vector3 UP;
        static const Vector3 DOWN;
        static const Vector3 FORWARD;
        static const Vector3 BACK;
        static const Vector3 MODEL_LEFT;
        static const Vector3 MODEL_RIGHT;
        static const Vector3 MODEL_TOP;
        static const Vector3 MODEL_BOTTOM;
        static const Vector3 MODEL_FRONT;
        static const Vector3 MODEL_REAR;

        static constexpr int AXIS_COUNT = 3;

        enum Axis {
                AXIS_X,
                AXIS_Y,
                AXIS_Z,
        };

        union {
                struct {
                        real_t x;
                        real_t y;
                        real_t z;
                };

                real_t coord[3];
        };

        _FORCE_INLINE_ const real_t &operator[](int p_axis) const {
                DEV_ASSERT((unsigned int)p_axis < 3);
                return coord[p_axis];
        }

        _FORCE_INLINE_ real_t &operator[](int p_axis) {
                DEV_ASSERT((unsigned int)p_axis < 3);
                return coord[p_axis];
        }

        _FORCE_INLINE_ Vector3::Axis min_axis_index() const {
                return x < y ? (x < z ? Vector3::AXIS_X : Vector3::AXIS_Z) : (y < z ? Vector3::AXIS_Y : Vector3::AXIS_Z);
        }

        _FORCE_INLINE_ Vector3::Axis max_axis_index() const {
                return x < y ? (y < z ? Vector3::AXIS_Z : Vector3::AXIS_Y) : (x < z ? Vector3::AXIS_Z : Vector3::AXIS_X);
        }

        Vector3 min(const Vector3 &p_vector3) const {
                return Vector3(MIN(x, p_vector3.x), MIN(y, p_vector3.y), MIN(z, p_vector3.z));
        }

        Vector3 minf(real_t p_scalar) const {
                return Vector3(MIN(x, p_scalar), MIN(y, p_scalar), MIN(z, p_scalar));
        }

        Vector3 max(const Vector3 &p_vector3) const {
                return Vector3(MAX(x, p_vector3.x), MAX(y, p_vector3.y), MAX(z, p_vector3.z));
        }

        Vector3 maxf(real_t p_scalar) const {
                return Vector3(MAX(x, p_scalar), MAX(y, p_scalar), MAX(z, p_scalar));
        }

        Vector3 clamp(const Vector3 &p_min, const Vector3 &p_max) const {
                return Vector3(
                                CLAMP(x, p_min.x, p_max.x),
                                CLAMP(y, p_min.y, p_max.y),
                                CLAMP(z, p_min.z, p_max.z));
        }

        Vector3 clampf(real_t p_min, real_t p_max) const {
                return Vector3(
                                CLAMP(x, p_min, p_max),
                                CLAMP(y, p_min, p_max),
                                CLAMP(z, p_min, p_max));
        }

        _FORCE_INLINE_ real_t length() const;
        _FORCE_INLINE_ real_t length_squared() const;

        _FORCE_INLINE_ void normalize();
        _FORCE_INLINE_ Vector3 normalized() const;
        _FORCE_INLINE_ bool is_normalized() const;
        _FORCE_INLINE_ Vector3 inverse() const;

        _FORCE_INLINE_ void zero();

        _FORCE_INLINE_ Vector3 cross(const Vector3 &p_with) const;
        _FORCE_INLINE_ real_t dot(const Vector3 &p_with) const;

        _FORCE_INLINE_ Vector3 abs() const;
        _FORCE_INLINE_ Vector3 floor() const;
        _FORCE_INLINE_ Vector3 sign() const;
        _FORCE_INLINE_ Vector3 ceil() const;
        _FORCE_INLINE_ Vector3 round() const;

        _FORCE_INLINE_ real_t distance_to(const Vector3 &p_to) const;
        _FORCE_INLINE_ real_t distance_squared_to(const Vector3 &p_to) const;

        _FORCE_INLINE_ Vector3 posmod(real_t p_mod) const;
        _FORCE_INLINE_ Vector3 posmodv(const Vector3 &p_modv) const;
        _FORCE_INLINE_ Vector3 project(const Vector3 &p_to) const;

        _FORCE_INLINE_ real_t angle_to(const Vector3 &p_to) const;
        _FORCE_INLINE_ Vector3 direction_to(const Vector3 &p_to) const;

        _FORCE_INLINE_ Vector3 slide(const Vector3 &p_normal) const;
        _FORCE_INLINE_ Vector3 bounce(const Vector3 &p_normal) const;
        _FORCE_INLINE_ Vector3 reflect(const Vector3 &p_normal) const;

        bool is_equal_approx(const Vector3 &p_v) const {
            return Math::is_equal_approx(x, p_v.x) && Math::is_equal_approx(y, p_v.y) && Math::is_equal_approx(z, p_v.z);
        }

        bool is_zero_approx() const {
            return Math::is_equal_approx(x, 0) && Math::is_equal_approx(y, 0) && Math::is_equal_approx(z, 0);
        }

        /* Operators */

        constexpr Vector3 &operator+=(const Vector3 &p_v);
        constexpr Vector3 operator+(const Vector3 &p_v) const;
        constexpr Vector3 &operator-=(const Vector3 &p_v);
        constexpr Vector3 operator-(const Vector3 &p_v) const;
        constexpr Vector3 &operator*=(const Vector3 &p_v);
        constexpr Vector3 operator*(const Vector3 &p_v) const;
        constexpr Vector3 &operator/=(const Vector3 &p_v);
        constexpr Vector3 operator/(const Vector3 &p_v) const;

        constexpr Vector3 &operator*=(real_t p_scalar);
        constexpr Vector3 operator*(real_t p_scalar) const;
        constexpr Vector3 &operator/=(real_t p_scalar);
        constexpr Vector3 operator/(real_t p_scalar) const;

        constexpr Vector3 operator-() const;

        constexpr bool operator==(const Vector3 &p_v) const;
        constexpr bool operator!=(const Vector3 &p_v) const;
        constexpr bool operator<(const Vector3 &p_v) const;
        constexpr bool operator<=(const Vector3 &p_v) const;
        constexpr bool operator>(const Vector3 &p_v) const;
        constexpr bool operator>=(const Vector3 &p_v) const;

        constexpr Vector3() : x(0), y(0), z(0) {}
        constexpr Vector3(real_t p_x, real_t p_y, real_t p_z) : x(p_x), y(p_y), z(p_z) {}
};

inline constexpr Vector3 Vector3::LEFT = { -1, 0, 0 };
inline constexpr Vector3 Vector3::RIGHT = { 1, 0, 0 };
inline constexpr Vector3 Vector3::UP = { 0, 1, 0 };
inline constexpr Vector3 Vector3::DOWN = { 0, -1, 0 };
inline constexpr Vector3 Vector3::FORWARD = { 0, 0, -1 };
inline constexpr Vector3 Vector3::BACK = { 0, 0, 1 };

inline Vector3 Vector3::cross(const Vector3 &p_with) const {
        return Vector3(
                        (y * p_with.z) - (z * p_with.y),
                        (z * p_with.x) - (x * p_with.z),
                        (x * p_with.y) - (y * p_with.x));
}

inline real_t Vector3::dot(const Vector3 &p_with) const {
        return x * p_with.x + y * p_with.y + z * p_with.z;
}

inline Vector3 Vector3::abs() const {
        return Vector3(Math::abs(x), Math::abs(y), Math::abs(z));
}

inline Vector3 Vector3::sign() const {
        return Vector3((real_t)SIGN(x), (real_t)SIGN(y), (real_t)SIGN(z));
}

inline Vector3 Vector3::floor() const {
        return Vector3(Math::floor(x), Math::floor(y), Math::floor(z));
}

inline Vector3 Vector3::ceil() const {
        return Vector3(Math::ceil(x), Math::ceil(y), Math::ceil(z));
}

inline Vector3 Vector3::round() const {
        return Vector3(Math::round(x), Math::round(y), Math::round(z));
}

inline real_t Vector3::distance_to(const Vector3 &p_to) const {
        return (p_to - *this).length();
}

inline real_t Vector3::distance_squared_to(const Vector3 &p_to) const {
        return (p_to - *this).length_squared();
}

inline Vector3 Vector3::posmod(real_t p_mod) const {
        return Vector3(Math::fposmod(x, p_mod), Math::fposmod(y, p_mod), Math::fposmod(z, p_mod));
}

inline Vector3 Vector3::posmodv(const Vector3 &p_modv) const {
        return Vector3(Math::fposmod(x, p_modv.x), Math::fposmod(y, p_modv.y), Math::fposmod(z, p_modv.z));
}

inline Vector3 Vector3::project(const Vector3 &p_to) const {
        return p_to * (dot(p_to) / p_to.length_squared());
}

inline real_t Vector3::angle_to(const Vector3 &p_to) const {
        return Math::atan2(cross(p_to).length(), dot(p_to));
}

inline Vector3 Vector3::direction_to(const Vector3 &p_to) const {
        Vector3 ret(p_to.x - x, p_to.y - y, p_to.z - z);
        ret.normalize();
        return ret;
}

/* Operators */

inline constexpr Vector3 &Vector3::operator+=(const Vector3 &p_v) {
        x += p_v.x;
        y += p_v.y;
        z += p_v.z;
        return *this;
}

inline constexpr Vector3 Vector3::operator+(const Vector3 &p_v) const {
        return Vector3(x + p_v.x, y + p_v.y, z + p_v.z);
}

inline constexpr Vector3 &Vector3::operator-=(const Vector3 &p_v) {
        x -= p_v.x;
        y -= p_v.y;
        z -= p_v.z;
        return *this;
}

inline constexpr Vector3 Vector3::operator-(const Vector3 &p_v) const {
        return Vector3(x - p_v.x, y - p_v.y, z - p_v.z);
}

inline constexpr Vector3 &Vector3::operator*=(const Vector3 &p_v) {
        x *= p_v.x;
        y *= p_v.y;
        z *= p_v.z;
        return *this;
}

inline constexpr Vector3 Vector3::operator*(const Vector3 &p_v) const {
        return Vector3(x * p_v.x, y * p_v.y, z * p_v.z);
}

inline constexpr Vector3 &Vector3::operator/=(const Vector3 &p_v) {
        x /= p_v.x;
        y /= p_v.y;
        z /= p_v.z;
        return *this;
}

inline constexpr Vector3 Vector3::operator/(const Vector3 &p_v) const {
        return Vector3(x / p_v.x, y / p_v.y, z / p_v.z);
}

inline constexpr Vector3 &Vector3::operator*=(real_t p_scalar) {
        x *= p_scalar;
        y *= p_scalar;
        z *= p_scalar;
        return *this;
}

inline constexpr Vector3 Vector3::operator*(real_t p_scalar) const {
        return Vector3(x * p_scalar, y * p_scalar, z * p_scalar);
}

inline constexpr Vector3 operator*(real_t p_scalar, const Vector3 &p_v) {
    return p_v * p_scalar;
}

inline constexpr Vector3 &Vector3::operator/=(real_t p_scalar) {
        x /= p_scalar;
        y /= p_scalar;
        z /= p_scalar;
        return *this;
}

inline constexpr Vector3 Vector3::operator/(real_t p_scalar) const {
        return Vector3(x / p_scalar, y / p_scalar, z / p_scalar);
}

inline constexpr Vector3 Vector3::operator-() const {
        return Vector3(-x, -y, -z);
}

inline constexpr bool Vector3::operator==(const Vector3 &p_v) const {
        return x == p_v.x && y == p_v.y && z == p_v.z;
}

inline constexpr bool Vector3::operator!=(const Vector3 &p_v) const {
        return x != p_v.x || y != p_v.y || z != p_v.z;
}

inline constexpr bool Vector3::operator<(const Vector3 &p_v) const {
        if (x == p_v.x) {
                if (y == p_v.y) {
                        return z < p_v.z;
                }
                return y < p_v.y;
        }
        return x < p_v.x;
}

inline constexpr bool Vector3::operator<=(const Vector3 &p_v) const {
        return *this == p_v || *this < p_v;
}

inline constexpr bool Vector3::operator>(const Vector3 &p_v) const {
        return !(*this <= p_v);
}

inline constexpr bool Vector3::operator>=(const Vector3 &p_v) const {
        return !(*this < p_v);
}

inline bool Vector3::is_normalized() const {
        return Math::is_equal_approx(length_squared(), (real_t)1, (real_t)0.00001);
}

inline Vector3 Vector3::inverse() const {
        return Vector3((real_t)1 / x, (real_t)1 / y, (real_t)1 / z);
}

inline real_t Vector3::length() const {
        return Math::sqrt(x * x + y * y + z * z);
}

inline real_t Vector3::length_squared() const {
        return x * x + y * y + z * z;
}

inline void Vector3::normalize() {
        real_t l = length_squared();
        if (l == 0) {
                zero();
        } else {
                l = Math::sqrt(l);
                x /= l;
                y /= l;
                z /= l;
        }
}

inline Vector3 Vector3::normalized() const {
        Vector3 v = *this;
        v.normalize();
        return v;
}

inline void Vector3::zero() {
        x = y = z = 0;
}

inline Vector3 Vector3::slide(const Vector3 &p_normal) const {
        return *this - p_normal * dot(p_normal);
}

inline Vector3 Vector3::reflect(const Vector3 &p_normal) const {
        return 2.0f * p_normal * dot(p_normal) - *this;
}

inline Vector3 Vector3::bounce(const Vector3 &p_normal) const {
        return -reflect(p_normal);
}
