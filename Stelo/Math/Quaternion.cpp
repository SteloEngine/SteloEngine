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

#include "Quaternion.hpp"

namespace Stelo {
    constexpr float radToDeg = 180.0f / 3.141592f;
    constexpr float degToRad = 3.141592f / 180.0f;
    constexpr float halfPI = 3.141592f / 2;

    Vector3 Quaternion::EulerAngles() const {
        Vector3 v3;
        v3.z = std::atan2f(2.0f * (x * y + z * w), 1.0f - 2.0f * (y * y + z * z));

        float sinp = 2.0f * (x * z - w * y);
        if (std::abs(sinp) >= 1.0f)
            v3.y = std::copysignf(3.141592f / 2, sinp);
        else
            v3.y = std::asinf(sinp);
        v3.x = std::atan2f(2.0f * (y * z + x * w), 1.0f - 2.0f * (x * x + z * z));

        return v3 * radToDeg;
    }

    Quaternion Quaternion::FromRadian(float x, float y, float z) noexcept {
        const float cx = cos(x * 0.5f);
        const float sx = sin(x * 0.5f);
        const float cy = cos(y * 0.5f);
        const float sy = sin(y * 0.5f);
        const float cz = cos(z * 0.5f);
        const float sz = sin(z * 0.5f);

        return {
            sx * cy * cz - cx * sy * sz,
            cx * sy * cz + sx * cy * sz,
            cx * cy * sz - sx * sy * cz,
            cx * cy * cz + sx * sy * sz
        };
    }

    float Quaternion::GetPitch() const {
        float sinp = 2.0f * (w * x - y * z);
        if (std::abs(sinp) >= 1)
            return std::copysign(halfPI, sinp);
        return std::asin(sinp);
    }

    float Quaternion::GetYaw() const {
        float siny = 2.0f * (w * y - z * x);
        if (std::abs(siny) >= 1)
            return std::copysign(halfPI, siny);
        return std::asin(siny);
    }

    float Quaternion::GetRoll() const {
        return std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));
    }

    Quaternion Quaternion::FromEulerAngles(float x, float y, float z) noexcept {
        return FromRadian(x * degToRad, y * degToRad, z * degToRad);
    }
    Quaternion Quaternion::FromEulerAngles(const Vector3& euler_angle) noexcept {
        return FromRadian(euler_angle.x * degToRad, euler_angle.y * degToRad, euler_angle.z * degToRad);
    }
    Quaternion Quaternion::FromRadian(const Vector3& euler_angle) noexcept {
        return FromRadian(euler_angle.x, euler_angle.y, euler_angle.z);
    }
    Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle) noexcept {
        const float halfAngle = angle * 0.5f;
        const float s = std::sin(halfAngle);
        return Quaternion(axis.x * s, axis.y * s, axis.z * s, std::cos(halfAngle));
    }

    Matrix4x4 Quaternion::ToMatrix4x4() const noexcept {
        const float x2 = x + x;
        const float y2 = y + y;
        const float z2 = z + z;

        const float xx2 = x * x2;
        const float yy2 = y * y2;
        const float zz2 = z * z2;
        const float xy2 = x * y2;
        const float xz2 = x * z2;
        const float yz2 = y * z2;
        const float wx2 = w * x2;
        const float wy2 = w * y2;
        const float wz2 = w * z2;

        Matrix4x4 r;
        r._11 = 1.0f - (yy2 + zz2);
        r._12 = xy2 - wz2;
        r._13 = xz2 + wy2;

        r._21 = xy2 + wz2;
        r._22 = 1.0f - (xx2 + zz2);
        r._23 = yz2 - wx2;

        r._31 = xz2 - wy2;
        r._32 = yz2 + wx2;
        r._33 = 1.0f - (xx2 + yy2);

        return r;
    }

    Quaternion Quaternion::Inverse() const noexcept {
        const float lenSq = w*w + x*x + y*y + z*z;
        if (lenSq == 0.0f) return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
        const float invLenSq = 1.0f / lenSq;
        return Quaternion(-x * invLenSq, -y * invLenSq, -z * invLenSq, w * invLenSq);
    }
} // Stelo