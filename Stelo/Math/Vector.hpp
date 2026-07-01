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

#include <cmath>
#include <iostream>

namespace Stelo {
	struct Vector2 {
		union {
			float m[2];
			struct {
				float x, y;
			};
		};

		Vector2() noexcept : x(0.0f), y(0.0f) {}
		Vector2(float v) noexcept : x(v), y(v) {}
		Vector2(float X, float Y) noexcept : x(X), y(Y) {}

		inline Vector2 Add(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
		inline Vector2 Sub(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
		inline Vector2 Negate() const { return Vector2(-x, -y); }
		inline float Dot(const Vector2& v) const { return x * v.x + y * v.y; }
		inline Vector2 Reflect(const Vector2& n) const { return *this - n * (2.0f * Dot(n)); }
		inline Vector2 Lerp(const Vector2& v, float t) const { return *this * (1.0f - t) + v * t; }
		inline float Magnitude() const { return std::sqrt(x * x + y * y); }
		inline Vector2 Normalize() const { float len = Magnitude(); return len > 0 ? *this / len : Vector2(); }
		inline Vector2 Abs() const { return Vector2(std::fabs(x), std::fabs(y)); }

		inline Vector2& operator+=(const Vector2& v) {
			x += v.x; y += v.y;
			return *this;
		}
		inline Vector2& operator-=(const Vector2& v) {
			x -= v.x; y -= v.y;
			return *this;
		}
		inline Vector2& operator*=(const Vector2& v) {
			x *= v.x; y *= v.y;
			return *this;
		}
		inline Vector2& operator/=(const Vector2& v) {
			x = (v.x != 0.0f) ? x / v.x : 0;
			y = (v.y != 0.0f) ? y / v.y : 0;
			return *this;
		}
		inline Vector2& operator*=(float k) {
			x *= k; y *= k;
			return *this;
		}
		inline Vector2& operator/=(float k) {
			if (k == 0.0f) return *this;
			x /= k; y /= k;
			return *this;
		}

		inline Vector2 operator*(float k) const {
			return Vector2(x * k, y * k);
		}
		inline Vector2 operator/(const Vector2& other) const {
			return Vector2(
				other.x != 0 ? x / other.x : 0,
				other.y != 0 ? y / other.y : 0
			);
		}
		inline Vector2 operator/(float k) const {
			if (k == 0.0f) return Vector2(0.0f);
			return Vector2(x / k, y / k);
		}
		inline Vector2 operator +(const Vector2& other) const {
			return Vector2(x + other.x, y + other.y);
		}
		inline Vector2 operator -(const Vector2& other) const {
			return Vector2(x - other.x, y - other.y);
		}
		inline Vector2 operator *(const Vector2& other) const {
			return Vector2(x * other.x, y * other.y);
		}
		inline bool operator ==(const Vector2& other) const {
			return (x == other.x && y == other.y);
		}
		inline bool operator !=(const Vector2& other) const {
			return (x != other.x && y != other.y);
		}
		inline bool operator <(const Vector2& other) const {
			return (x*x + y*y < other.x*other.x + other.y*other.y);
		}
		inline bool operator >(const Vector2& other) const {
			return (x*x + y*y > other.x*other.x + other.y*other.y);
		}
		inline std::ostream& operator<<(std::ostream& os) const {
			return os << "(" << x << ", " << y << ")";
		}

		static const Vector2 Down, Left, Up, Right;
	};

	struct Vector3 {
		union {
			float m[3];
			struct {
				float x, y, z;
			};
            Vector2 xy;
            struct {
                float _x;
                Vector2 yz;
            };
		};

		Vector3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
		Vector3(float v) noexcept : x(v), y(v), z(v) {}
		Vector3(float X, float Y, float Z) noexcept : x(X), y(Y), z(Z) {}

		inline Vector3 Add(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
		inline Vector3 Sub(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
		inline Vector3 Negate() const { return Vector3(-x, -y, -z); }
		inline float Dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }
		inline Vector3 Cross(const Vector3& v) const { return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
		inline Vector3 Reflect(const Vector3& n) const { return (*this) - n * (2.0f * Dot(n)); }
		inline Vector3 Lerp(const Vector3& v, float t) const { return (*this) * (1.0f - t) + v * t; }
		inline float Magnitude() const { return std::sqrt(x * x + y * y + z * z); }
		inline Vector3 Normalize() const { float len = Magnitude(); return len > 0 ? *this / len : Vector3(); }
		inline Vector3 Abs() const { return Vector3(std::fabs(x), std::fabs(y), std::fabs(z)); }

		static Vector3 Min(const Vector3& a, const Vector3& b) {
			return Vector3{
				std::min(a.x, b.x),
				std::min(a.y, b.y),
				std::min(a.z, b.z)
			};
		}
		static Vector3 Max(const Vector3& a, const Vector3& b) {
			return Vector3{
				std::max(a.x, b.x),
				std::max(a.y, b.y),
				std::max(a.z, b.z)
			};
		}

		inline Vector3& operator+=(const Vector3& v) {
			x += v.x; y += v.y; z += v.z;
			return *this;
		}
		inline Vector3& operator-=(const Vector3& v) {
			x -= v.x; y -= v.y; z -= v.z;
			return *this;
		}
		inline Vector3& operator*=(const Vector3& v) {
			x *= v.x; y *= v.y; z *= v.z;
			return *this;
		}
		inline Vector3& operator/=(const Vector3& v) {
			x = (v.x != 0.0f) ? x / v.x : 0;
			y = (v.y != 0.0f) ? y / v.y : 0;
			z = (v.z != 0.0f) ? z / v.z : 0;
			return *this;
		}
		inline Vector3& operator*=(float k) {
			x *= k; y *= k; z *= k;
			return *this;
		}
		inline Vector3& operator/=(float k) {
			if (k == 0.0f) return *this;
			x /= k; y /= k; z /= k;
			return *this;
		}

		inline Vector3 operator*(float k) const {
			return Vector3(x * k, y * k, z * k);
		}
		inline Vector3 operator/(const Vector3& other) const {
			return Vector3(
				other.x != 0 ? x / other.x : 0,
				other.y != 0 ? y / other.y : 0,
				other.z != 0 ? z / other.z : 0
			);
		}
		inline Vector3 operator/(float k) const {
			if (k == 0.0f) return Vector3(0.0f);
			return Vector3(x / k, y / k, z / k);
		}
		inline Vector3 operator +(const Vector3& other) const {
			return Vector3(x + other.x, y + other.y, z + other.z);
		}
		inline Vector3 operator -(const Vector3& other) const {
			return Vector3(x - other.x, y - other.y, z - other.z);
		}
		inline Vector3 operator *(const Vector3& other) const {
			return Vector3(x * other.x, y * other.y, z * other.z);
		}
		inline bool operator ==(const Vector3& other) const {
			return (x == other.x && y == other.y && z == other.z);
		}
		inline bool operator !=(const Vector3& other) const {
			return (x != other.x && y != other.y && z != other.z);
		}
		inline bool operator <(const Vector3& other) const {
			return (x < other.x && y < other.y && z < other.z);
		}
		inline bool operator >(const Vector3& other) const {
			return (x > other.x && y > other.y && z > other.z);
		}
		inline std::ostream& operator<<(std::ostream& os) const {
			return os << "(" << x << ", " << y << ", " << z << ")";
		}

		static const Vector3 Down, Left, Up, Right, Forward, Back;
	};

	struct Vector4 {
		union {
			float m[4];
			struct {
				float x, y, z, w;
			};
            Vector2 xy;
			Vector3 xyz;
		};

		Vector4() noexcept : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		Vector4(float v) noexcept : x(v), y(v), z(v), w(v) {}
		Vector4(float X, float Y, float Z, float W) noexcept : x(X), y(Y), z(Z), w(W) {}

		inline Vector4 Add(const Vector4& v) const { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
		inline Vector4 Sub(const Vector4& v) const { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
		inline Vector4 Negate() const { return Vector4(-x, -y, -z, -w); }
		inline float Dot(const Vector4& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }
		inline Vector4 Reflect(const Vector4& n) const { return *this - n * (2.0f * Dot(n)); }
		inline Vector4 Lerp(const Vector4& v, float t) const { return *this * (1.0f - t) + v * t; }
		inline float Magnitude() const { return std::sqrt(x * x + y * y + z * z + w * w); }
		inline Vector4 Normalize() const { float len = Magnitude(); return len > 0 ? *this / len : Vector4(); }
		inline Vector4 Abs() const { return Vector4(std::fabs(x), std::fabs(y), std::fabs(z), std::fabs(w)); }

		inline Vector4& operator+=(const Vector4& v) {
			x += v.x; y += v.y; z += v.z; w += v.w;
			return *this;
		}
		inline Vector4& operator-=(const Vector4& v) {
			x -= v.x; y -= v.y; z -= v.z; w -= v.w;
			return *this;
		}
		inline Vector4& operator*=(const Vector4& v) {
			x *= v.x; y *= v.y; z *= v.z; w *= v.w;
			return *this;
		}
		inline Vector4& operator/=(const Vector4& v) {
			x = (v.x != 0.0f) ? x / v.x : 0;
			y = (v.y != 0.0f) ? y / v.y : 0;
			z = (v.z != 0.0f) ? z / v.z : 0;
			w = (v.w != 0.0f) ? w / v.w : 0;
			return *this;
		}
		inline Vector4& operator*=(float k) {
			x *= k; y *= k; z *= k; w *= k;
			return *this;
		}
		inline Vector4& operator/=(float k) {
			if (k == 0.0f) return *this;
			x /= k; y /= k; z /= k; w /= k;
			return *this;
		}

		inline Vector4 operator*(float k) const {
			return Vector4(x * k, y * k, z * k, w * k);
		}
		inline Vector4 operator/(const Vector4& other) const {
			return Vector4(
				other.x != 0 ? x / other.x : 0,
				other.y != 0 ? y / other.y : 0,
				other.z != 0 ? z / other.z : 0,
				other.w != 0 ? w / other.w : 0
			);
		}
		inline Vector4 operator/(float k) const {
			if (k == 0.0f) return Vector4(0.0f);
			return Vector4(x / k, y / k, z / k, w / k);
		}
		inline Vector4 operator +(const Vector4& v2) const {
			return Vector4(x + v2.x, y + v2.y, z + v2.z, w + v2.w);
		}
		inline Vector4 operator -(const Vector4& v2) const {
			return Vector4(x - v2.x, y - v2.y, z - v2.z, w - v2.w);
		}
		inline Vector4 operator *(const Vector4& v2) const {
			return Vector4(x * v2.x, y * v2.y, z * v2.z, w * v2.w);
		}
		inline bool operator ==(const Vector4& v2) const {
			return (x == v2.x && y == v2.y && z == v2.z && w == v2.w);
		}
		inline bool operator !=(const Vector4& v2) const {
			return (x != v2.x && y != v2.y && z != v2.z && w != v2.w);
		}
		inline bool operator <(const Vector4& v2) const {
			return (x < v2.x && y < v2.y && z < v2.z && w < v2.w);
		}
		inline bool operator >(const Vector4& v2) const {
			return (x > v2.x && y > v2.y && z > v2.z && w > v2.w);
		}
		inline std::ostream& operator<<(std::ostream& os) const {
			return os << "(" << x << ", " << y << ", " << z << ", " << w << ")";
		}

		static const Vector4 Down, Left, Up, Right, Forward, Back, Inside, Outside;
	};
} // Stelo
