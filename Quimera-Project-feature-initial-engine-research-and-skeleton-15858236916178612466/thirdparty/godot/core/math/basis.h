#pragma once

#include "quaternion.h"
#include "vector3.h"

struct [[nodiscard]] Basis {
        static const Basis FLIP_X;
        static const Basis FLIP_Y;
        static const Basis FLIP_Z;

        Vector3 rows[3];

        constexpr const Vector3 &operator[](int p_row) const {
                return rows[p_row];
        }
        constexpr Vector3 &operator[](int p_row) {
                return rows[p_row];
        }

        _FORCE_INLINE_ real_t determinant() const {
                return rows[0][0] * (rows[1][1] * rows[2][2] - rows[2][1] * rows[1][2]) -
                       rows[1][0] * (rows[0][1] * rows[2][2] - rows[2][1] * rows[0][2]) +
                       rows[2][0] * (rows[0][1] * rows[1][2] - rows[1][1] * rows[0][2]);
        }

        _FORCE_INLINE_ real_t tdotx(const Vector3 &p_v) const {
                return rows[0][0] * p_v[0] + rows[1][0] * p_v[1] + rows[2][0] * p_v[2];
        }
        _FORCE_INLINE_ real_t tdoty(const Vector3 &p_v) const {
                return rows[0][1] * p_v[0] + rows[1][1] * p_v[1] + rows[2][1] * p_v[2];
        }
        _FORCE_INLINE_ real_t tdotz(const Vector3 &p_v) const {
                return rows[0][2] * p_v[0] + rows[1][2] * p_v[1] + rows[2][2] * p_v[2];
        }

        constexpr bool operator==(const Basis &p_matrix) const {
                for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                                if (rows[i][j] != p_matrix.rows[i][j]) return false;
                        }
                }
                return true;
        }

        constexpr bool operator!=(const Basis &p_matrix) const {
                return (!(*this == p_matrix));
        }

        _FORCE_INLINE_ Vector3 xform(const Vector3 &p_vector) const {
                return Vector3(
                                rows[0].dot(p_vector),
                                rows[1].dot(p_vector),
                                rows[2].dot(p_vector));
        }

        _FORCE_INLINE_ Vector3 xform_inv(const Vector3 &p_vector) const {
                return Vector3(
                                (rows[0][0] * p_vector.x) + (rows[1][0] * p_vector.y) + (rows[2][0] * p_vector.z),
                                (rows[0][1] * p_vector.x) + (rows[1][1] * p_vector.y) + (rows[2][1] * p_vector.z),
                                (rows[0][2] * p_vector.x) + (rows[1][2] * p_vector.y) + (rows[2][2] * p_vector.z));
        }

        _FORCE_INLINE_ void operator*=(const Basis &p_matrix) {
                set(
                                p_matrix.tdotx(rows[0]), p_matrix.tdoty(rows[0]), p_matrix.tdotz(rows[0]),
                                p_matrix.tdotx(rows[1]), p_matrix.tdoty(rows[1]), p_matrix.tdotz(rows[1]),
                                p_matrix.tdotx(rows[2]), p_matrix.tdoty(rows[2]), p_matrix.tdotz(rows[2]));
        }

        _FORCE_INLINE_ Basis operator*(const Basis &p_matrix) const {
                return Basis(
                                p_matrix.tdotx(rows[0]), p_matrix.tdoty(rows[0]), p_matrix.tdotz(rows[0]),
                                p_matrix.tdotx(rows[1]), p_matrix.tdoty(rows[1]), p_matrix.tdotz(rows[1]),
                                p_matrix.tdotx(rows[2]), p_matrix.tdoty(rows[2]), p_matrix.tdotz(rows[2]));
        }

        _FORCE_INLINE_ void set(real_t p_xx, real_t p_xy, real_t p_xz, real_t p_yx, real_t p_yy, real_t p_yz, real_t p_zx, real_t p_zy, real_t p_zz) {
                rows[0][0] = p_xx; rows[0][1] = p_xy; rows[0][2] = p_xz;
                rows[1][0] = p_yx; rows[1][1] = p_yy; rows[1][2] = p_yz;
                rows[2][0] = p_zx; rows[2][1] = p_zy; rows[2][2] = p_zz;
        }

        constexpr Basis() : rows{ {1, 0, 0}, {0, 1, 0}, {0, 0, 1} } {}

        constexpr Basis(real_t p_xx, real_t p_xy, real_t p_xz, real_t p_yx, real_t p_yy, real_t p_yz, real_t p_zx, real_t p_zy, real_t p_zz) :
                        rows{
                                { p_xx, p_xy, p_xz },
                                { p_yx, p_yy, p_yz },
                                { p_zx, p_zy, p_zz },
                        } {}

        constexpr Basis(const Vector3 &p_x_axis, const Vector3 &p_y_axis, const Vector3 &p_z_axis) :
                        rows{
                                { p_x_axis.x, p_y_axis.x, p_z_axis.x },
                                { p_x_axis.y, p_y_axis.y, p_z_axis.y },
                                { p_x_axis.z, p_y_axis.z, p_z_axis.z },
                        } {}
};

inline constexpr Basis Basis::FLIP_X = { { -1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };
inline constexpr Basis Basis::FLIP_Y = { { 1, 0, 0 }, { 0, -1, 0 }, { 0, 0, 1 } };
inline constexpr Basis Basis::FLIP_Z = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 } };
