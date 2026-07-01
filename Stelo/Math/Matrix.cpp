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

#include "Matrix.hpp"
#include <cmath>

namespace Stelo {
    Matrix4x4::Matrix4x4() noexcept : _11(0.0f), _12(0.0f), _13(0.0f), _14(0.0f),
        _21(0.0f), _22(0.0f), _23(0.0f), _24(0.0f),
        _31(0.0f), _32(0.0f), _33(0.0f), _34(0.0f),
        _41(0.0f), _42(0.0f), _43(0.0f), _44(0.0f) {
    }
    Matrix4x4::Matrix4x4(float value) noexcept : _11(value), _12(value), _13(value), _14(value),
        _21(value), _22(value), _23(value), _24(value),
        _31(value), _32(value), _33(value), _34(value),
        _41(value), _42(value), _43(value), _44(value) {
    }
    Matrix4x4::Matrix4x4(float m11, float m12, float m13, float m14,
        float m21, float m22, float m23, float m24,
        float m31, float m32, float m33, float m34,
        float m41, float m42, float m43, float m44) noexcept : _11(m11), _12(m12), _13(m13), _14(m14),
        _21(m21), _22(m22), _23(m23), _24(m24),
        _31(m31), _32(m32), _33(m33), _34(m34),
        _41(m41), _42(m42), _43(m43), _44(m44) {
    }

    Matrix4x4 Matrix4x4::Identity = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    Matrix4x4 Matrix4x4::LookAtLH(const Vector3& Position, const Vector3& Target, const Vector3& VectorUp) noexcept {
        return LookToLH(Position, Target - Position, VectorUp);
    }
    Matrix4x4 Matrix4x4::LookAtRH(const Vector3& Position, const Vector3& Target, const Vector3& VectorUp) noexcept {
        return LookToLH(Position, Position - Target, VectorUp);
    }
    Matrix4x4 Matrix4x4::LookToLH(const Vector3& Position, const Vector3& Dir, const Vector3& VectorUp) noexcept {
        const Vector3 L = Dir.Normalize();
        const Vector3 R = VectorUp.Cross(L);
        const Vector3 U = L.Cross(R);

        const Vector3 Neg = Position.Negate();

        return Matrix4x4(R.x, U.x, L.x, 0.0f,
            R.y, U.y, L.y, 0.0f,
            R.z, U.z, L.z, 0.0f,
            R.Dot(Neg), U.Dot(Neg), L.Dot(Neg), 1.0f
        );
    }
    Matrix4x4 Matrix4x4::LookToRH(const Vector3& Position, const Vector3& Dir, const Vector3& VectorUp) noexcept {
        return LookToLH(Position, Dir.Negate(), VectorUp);
    }

    Matrix4x4 Matrix4x4::PerFovLH(float AngleView, float aspectRatio, float NearZ, float FarZ) noexcept {
        if(NearZ < 0.f || FarZ < 0.f) return Matrix4x4::Identity;

        const float Y = 1.0f / tanf(AngleView);
        const float fRange = FarZ / (FarZ - NearZ);

        return Matrix4x4(Y / aspectRatio, 0.0f, 0.0f, 0.0f,
            0.0f, Y, 0.0f, 0.0f,
            0.0f, 0.0f, fRange, 1.0f,
            0.0f, 0.0f, -NearZ * fRange, 0.0f
        );
    }
    Matrix4x4 Matrix4x4::PerFovRH(float AngleView, float aspectRatio, float NearZ, float FarZ)noexcept {
        if(NearZ < 0.f || FarZ < 0.f) return Matrix4x4::Identity;

        const float poav = AngleView * 0.5f;

        const float height = std::cosf(poav) / std::sinf(poav);
        const float fRange = FarZ / (NearZ - FarZ);

        return Matrix4x4(height / aspectRatio, 0.0f, 0.0f, 0.0f,
            0.0f, height, 0.0f, 0.0f,
            0.0f, 0.0f, fRange, 1.0f,
            0.0f, 0.0f, -fRange * NearZ, 0.0f
        );
    }
    Matrix4x4 Matrix4x4::OrthoRH(float Width, float Height, float NearZ, float FarZ) noexcept {
        return Matrix4x4(
            2.0f / Width, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / Height, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f / (NearZ - FarZ), 0.0f,
            0, 0, NearZ / (NearZ - FarZ), 1.0f
        );
    }
    Matrix4x4 Matrix4x4::OrthoLH(float Width, float Height, float NearZ, float FarZ) noexcept {
        return Matrix4x4(
            2.0f / Width, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / Height, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f / (FarZ - NearZ), 0.0f,
            0.0, 0.0, -NearZ / (FarZ - NearZ), 1.0f
        );
    }
    Matrix4x4 Matrix4x4::Transpose() const noexcept {
        return Matrix4x4(
            _11, _21, _31, _41,
            _12, _22, _32, _42,
            _13, _23, _33, _43,
            _14, _24, _34, _44
        );
    }
} // Stelo