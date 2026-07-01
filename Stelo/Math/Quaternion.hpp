/*
 * Copyright (c) 2026 Stelo Engine
 *
 * This file is part of Stelo Engine.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations
 * under the License.
*/

#ifndef STELO_QUATERNION_HPP
#define STELO_QUATERNION_HPP

#include "Matrix.hpp"

namespace Stelo {
    struct Quaternion {
        union {
            struct {
                float x, y, z, w;
            };
            Vector3 xyz;
            float m[4];
        };

        Quaternion() noexcept : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
        Quaternion(float v) noexcept : x(v), y(v), z(v), w(v) {}
        Quaternion(float X, float Y, float Z, float W) noexcept : x(X), y(Y), z(Z), w(W) {}
        Quaternion(const Vector3& v, float _w) noexcept : w(_w) { xyz = v; }

        Vector3 EulerAngles() const;
        float GetPitch() const;
        float GetYaw() const;
        float GetRoll() const;

        static Quaternion FromEulerAngles(float x, float y, float z) noexcept;
        static Quaternion FromEulerAngles(const Vector3& euler_angle) noexcept;
        static Quaternion FromRadian(float x, float y, float z) noexcept;
        static Quaternion FromRadian(const Vector3& euler_angle) noexcept;
        static Quaternion FromAxisAngle(const Vector3& axis, float angle) noexcept;

        Matrix4x4 ToMatrix4x4() const noexcept;
        Quaternion Inverse() const noexcept;

        friend inline Vector3 operator*(const Vector3& v, const Quaternion& q) noexcept{
            const float tx = (q.y*v.z - q.z*v.y) * 2.0f;
            const float ty = (q.z*v.x - q.x*v.z) * 2.0f;
            const float tz = (q.x*v.y - q.y*v.x) * 2.0f;

            return {
                v.x + (tx * q.w) + (q.y*tz - q.z*ty),
                v.y + (ty * q.w) + (q.z*tx - q.x*tz),
                v.z + (tz * q.w) + (q.x*ty - q.y*tx)
            };
        }

        friend inline Vector3 operator*(const Quaternion& q, const Vector3& v) noexcept{
            const float tx = (q.y*v.z - q.z*v.y) * 2.0f;
            const float ty = (q.z*v.x - q.x*v.z) * 2.0f;
            const float tz = (q.x*v.y - q.y*v.x) * 2.0f;

            return {
                v.x + (tx * q.w) + (q.y*tz - q.z*ty),
                v.y + (ty * q.w) + (q.z*tx - q.x*tz),
                v.z + (tz * q.w) + (q.x*ty - q.y*tx)
            };
        }

        friend inline Quaternion operator*(const Quaternion& a, const Quaternion& b) noexcept{
            return {
                a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
                a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
                a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
                a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w
            };
        }
        friend inline Quaternion operator/(const Quaternion& a, const Quaternion& b) noexcept {
            float lenSq = b.w*b.w + b.x*b.x + b.y*b.y + b.z*b.z;

            if (lenSq == 0.0f) return a;
            const float invLenSq = 1.0f / lenSq;
            return a * Quaternion(b.w * invLenSq, -b.x * invLenSq, -b.y * invLenSq, -b.z * invLenSq);
        }

        inline Quaternion& operator*=(const Quaternion& q) noexcept{
            const float new_w = w*q.w - x*q.x - y*q.y - z*q.z;
            const float new_x = w*q.x + x*q.w + y*q.z - z*q.y;
            const float new_y = w*q.y - x*q.z + y*q.w + z*q.x;
            const float new_z = w*q.z + x*q.y - y*q.x + z*q.w;

            w = new_w;
            x = new_x;
            y = new_y;
            z = new_z;
            return *this;
        }

        friend inline bool operator==(const Quaternion& q1, const Quaternion& q2) noexcept{
            return (q1.w == q2.w && q1.x == q2.x && q1.y == q2.y && q1.z == q2.z);
        }
        friend inline bool operator!=(const Quaternion& q1, const Quaternion& q2) noexcept{
            return (q1.w != q2.w || q1.x != q2.x || q1.y != q2.y || q1.z != q2.z);
        }

        template <typename OStream>
        friend OStream& operator<<(OStream& os, const Quaternion& v) {
            os << v.w << ", " << v.x << ", " << v.y << ", " << v.z;
            return os;
        }
    };
} // Stelo

#endif