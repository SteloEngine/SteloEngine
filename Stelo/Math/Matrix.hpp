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

#pragma once

#include "Vector.hpp"

namespace Stelo {
    struct Matrix4x4 {
		union {
			Vector4 r[4];
			struct {
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			float m[4][4];
		};

		Matrix4x4() noexcept;
		Matrix4x4(float value) noexcept;
		Matrix4x4(
			float m11, float m12, float m13, float m14,
			float m21, float m22, float m23, float m24,
			float m31, float m32, float m33, float m34,
			float m41, float m42, float m43, float m44) noexcept;

		static Matrix4x4 Identity;

		Matrix4x4 operator*(const Matrix4x4& rhs) const noexcept {
			Matrix4x4 result;

			result._11 = _11 * rhs._11 + _12 * rhs._21 + _13 * rhs._31 + _14 * rhs._41;
			result._12 = _11 * rhs._12 + _12 * rhs._22 + _13 * rhs._32 + _14 * rhs._42;
			result._13 = _11 * rhs._13 + _12 * rhs._23 + _13 * rhs._33 + _14 * rhs._43;
			result._14 = _11 * rhs._14 + _12 * rhs._24 + _13 * rhs._34 + _14 * rhs._44;

			result._21 = _21 * rhs._11 + _22 * rhs._21 + _23 * rhs._31 + _24 * rhs._41;
			result._22 = _21 * rhs._12 + _22 * rhs._22 + _23 * rhs._32 + _24 * rhs._42;
			result._23 = _21 * rhs._13 + _22 * rhs._23 + _23 * rhs._33 + _24 * rhs._43;
			result._24 = _21 * rhs._14 + _22 * rhs._24 + _23 * rhs._34 + _24 * rhs._44;

			result._31 = _31 * rhs._11 + _32 * rhs._21 + _33 * rhs._31 + _34 * rhs._41;
			result._32 = _31 * rhs._12 + _32 * rhs._22 + _33 * rhs._32 + _34 * rhs._42;
			result._33 = _31 * rhs._13 + _32 * rhs._23 + _33 * rhs._33 + _34 * rhs._43;
			result._34 = _31 * rhs._14 + _32 * rhs._24 + _33 * rhs._34 + _34 * rhs._44;

			result._41 = _41 * rhs._11 + _42 * rhs._21 + _43 * rhs._31 + _44 * rhs._41;
			result._42 = _41 * rhs._12 + _42 * rhs._22 + _43 * rhs._32 + _44 * rhs._42;
			result._43 = _41 * rhs._13 + _42 * rhs._23 + _43 * rhs._33 + _44 * rhs._43;
			result._44 = _41 * rhs._14 + _42 * rhs._24 + _43 * rhs._34 + _44 * rhs._44;

			return result;
		}
    	Matrix4x4& operator*=(const Matrix4x4& rhs) noexcept {
			Matrix4x4 result;

			result._11 = _11 * rhs._11 + _12 * rhs._21 + _13 * rhs._31 + _14 * rhs._41;
			result._12 = _11 * rhs._12 + _12 * rhs._22 + _13 * rhs._32 + _14 * rhs._42;
			result._13 = _11 * rhs._13 + _12 * rhs._23 + _13 * rhs._33 + _14 * rhs._43;
			result._14 = _11 * rhs._14 + _12 * rhs._24 + _13 * rhs._34 + _14 * rhs._44;

			result._21 = _21 * rhs._11 + _22 * rhs._21 + _23 * rhs._31 + _24 * rhs._41;
			result._22 = _21 * rhs._12 + _22 * rhs._22 + _23 * rhs._32 + _24 * rhs._42;
			result._23 = _21 * rhs._13 + _22 * rhs._23 + _23 * rhs._33 + _24 * rhs._43;
			result._24 = _21 * rhs._14 + _22 * rhs._24 + _23 * rhs._34 + _24 * rhs._44;

			result._31 = _31 * rhs._11 + _32 * rhs._21 + _33 * rhs._31 + _34 * rhs._41;
			result._32 = _31 * rhs._12 + _32 * rhs._22 + _33 * rhs._32 + _34 * rhs._42;
			result._33 = _31 * rhs._13 + _32 * rhs._23 + _33 * rhs._33 + _34 * rhs._43;
			result._34 = _31 * rhs._14 + _32 * rhs._24 + _33 * rhs._34 + _34 * rhs._44;

			result._41 = _41 * rhs._11 + _42 * rhs._21 + _43 * rhs._31 + _44 * rhs._41;
			result._42 = _41 * rhs._12 + _42 * rhs._22 + _43 * rhs._32 + _44 * rhs._42;
			result._43 = _41 * rhs._13 + _42 * rhs._23 + _43 * rhs._33 + _44 * rhs._43;
			result._44 = _41 * rhs._14 + _42 * rhs._24 + _43 * rhs._34 + _44 * rhs._44;

			return *this = result;
		}
    	/// @brief Returns a left-handed look-at matrix
    	static Matrix4x4 LookAtLH(const Vector3& Position, const Vector3& Target, const Vector3& VectorUp) noexcept;
    	/// @brief Returns a right-handed look-at matrix
    	static Matrix4x4 LookAtRH(const Vector3& Position, const Vector3& Target, const Vector3& VectorUp) noexcept;
    	/// @brief Returns a left-handed look-to matrix
    	static Matrix4x4 LookToLH(const Vector3& Position, const Vector3& Dir, const Vector3& VectorUp) noexcept;
    	/// @brief Returns a right-handed look-to matrix
    	static Matrix4x4 LookToRH(const Vector3& Position, const Vector3& Dir, const Vector3& VectorUp) noexcept;

    	/// @brief Returns a left-handed perspective projection matrix
    	static Matrix4x4 PerFovLH(float AngleView, float aspectRatio, float NearZ, float FarZ) noexcept;
    	/// @brief Returns a right-handed perspective projection matrix
    	static Matrix4x4 PerFovRH(float AngleView, float aspectRatio, float NearZ, float FarZ) noexcept;
    	/// @brief Returns a right-handed orthographic projection matrix
    	static Matrix4x4 OrthoRH(float Width, float Height, float NearZ, float FarZ) noexcept;
    	/// @brief Returns a left-handed orthographic projection matrix
    	static Matrix4x4 OrthoLH(float Width, float Height, float NearZ, float FarZ) noexcept;

    	/// @brief Transposes matrix
    	Matrix4x4 Transpose() const noexcept;
	};
} // Stelo
