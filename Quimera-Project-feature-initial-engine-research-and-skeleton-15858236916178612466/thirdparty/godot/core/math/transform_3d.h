// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once

#include "basis.h"

struct [[nodiscard]] Transform3D {
        Basis basis;
        Vector3 origin;

        const Basis &get_basis() const { return basis; }
        void set_basis(const Basis &p_basis) { basis = p_basis; }

        const Vector3 &get_origin() const { return origin; }
        void set_origin(const Vector3 &p_origin) { origin = p_origin; }

        constexpr bool operator==(const Transform3D &p_transform) const {
                return (basis == p_transform.basis && origin == p_transform.origin);
        }

        constexpr bool operator!=(const Transform3D &p_transform) const {
                return (basis != p_transform.basis || origin != p_transform.origin);
        }

        _FORCE_INLINE_ Vector3 xform(const Vector3 &p_vector) const {
                return Vector3(
                                basis[0].dot(p_vector) + origin.x,
                                basis[1].dot(p_vector) + origin.y,
                                basis[2].dot(p_vector) + origin.z);
        }

        _FORCE_INLINE_ Vector3 xform_inv(const Vector3 &p_vector) const {
                Vector3 v = p_vector - origin;
                return Vector3(
                                (basis[0][0] * v.x) + (basis[1][0] * v.y) + (basis[2][0] * v.z),
                                (basis[0][1] * v.x) + (basis[1][1] * v.y) + (basis[2][1] * v.z),
                                (basis[0][2] * v.x) + (basis[1][2] * v.y) + (basis[2][2] * v.z));
        }

        void operator*=(const Transform3D &p_transform) {
                origin = xform(p_transform.origin);
                basis *= p_transform.basis;
        }

        Transform3D operator*(const Transform3D &p_transform) const {
                Transform3D t = *this;
                t *= p_transform;
                return t;
        }

        Transform3D() = default;
        constexpr Transform3D(const Basis &p_basis, const Vector3 &p_origin = Vector3()) :
                        basis(p_basis),
                        origin(p_origin) {}

        constexpr Transform3D(const Vector3 &p_x, const Vector3 &p_y, const Vector3 &p_z, const Vector3 &p_origin) :
                        basis(p_x, p_y, p_z),
                        origin(p_origin) {}
};
