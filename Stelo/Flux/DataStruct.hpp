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

#include "Type.hpp"
#include "Enums.hpp"

namespace Stelo {
    struct Handle {
        void* handle = nullptr;
        void* appContext = nullptr;
        HandleType handleType = HandleType::Unknown;
        AppContextType appContextType = AppContextType::Unknown;
    };

    struct AspectRatio {
        float width;
        float height;
    };

    struct AspectLock {
        bool width;
        bool height;
    };

    struct Point {
        float x;
        float y;

        Point() : x(0), y(0) {}
        Point(float x, float y) : x(x), y(y) {}
        Point(float v) : x(v), y(v) {}

		inline Point& operator+=(const Point& v) {
			x += v.x; y += v.y;
			return *this;
		}
		inline Point& operator-=(const Point& v) {
			x -= v.x; y -= v.y;
			return *this;
		}
		inline Point& operator*=(const Point& v) {
			x *= v.x; y *= v.y;
			return *this;
		}
		inline Point& operator/=(const Point& v) {
			x = (v.x != 0) ? x / v.x : 0;
			y = (v.y != 0) ? y / v.y : 0;
			return *this;
		}
		inline Point& operator*=(float k) {
			x *= k; y *= k;
			return *this;
		}
		inline Point& operator/=(float k) {
			if (k == 0.0f) return *this;
			x /= k; y /= k;
			return *this;
		}

        inline Point operator-() const {
            return Point(-x, -y);
        }
        inline Point operator*(const Point& other) const {
            return Point(x * other.x, y * other.y);
        }
		inline Point operator*(float k) const {
			return Point(x * k, y * k);
		}
		inline Point operator/(const Point& other) const {
			return Point(
				other.x != 0 ? x / other.x : 0,
				other.y != 0 ? y / other.y : 0
			);
		}
		inline Point operator/(float k) const {
			if (k == 0.0f) return Point(0, 0);
			return Point(x / k, y / k);
		}
		inline Point operator +(const Point& other) const {
			return Point(x + other.x, y + other.y);
		}
		inline Point operator -(const Point& other) const {
			return Point(x - other.x, y - other.y);
		}   
    };

    struct Size {
        float width;
        float height;

        Size() : width(0), height(0) {}
        Size(float w, float h) : width(w), height(h) {}
        Size(float v) : width(v), height(v) {}

        inline Size& operator+=(const Size& v) {
			width += v.width; height += v.height;
			return *this;
		}
		inline Size& operator-=(const Size& v) {
			width -= v.width; height -= v.height;
			return *this;
		}
		inline Size& operator*=(const Size& v) {
			width *= v.width; height *= v.height;
			return *this;
		}
		inline Size& operator/=(const Size& v) {
			width = (v.width != 0.0f) ? width / v.width : 0;
			height = (v.height != 0.0f) ? height / v.height : 0;
			return *this;
		}
		inline Size& operator*=(float k) {
			width *= k; height *= k;
			return *this;
		}
		inline Size& operator/=(float k) {
			if (k == 0.0f) return *this;
			width /= k; height /= k;
			return *this;
		}

        inline Size operator-() const {
            return Size(-width, -height);
        }
        inline Size operator*(const Size& other) const {
            return Size(width * other.width, height * other.height);
        }
		inline Size operator*(float k) const {
			return Size(width * k, height * k);
		}
		inline Size operator/(const Size& other) const {
			return Size(
				other.width != 0 ? width / other.width : 0,
				other.height != 0 ? height / other.height : 0
			);
		}
		inline Size operator/(float k) const {
			if (k == 0.0f) return Size(0.0f);
			return Size(width / k, height / k);
		}
		inline Size operator +(const Size& other) const {
			return Size(width + other.width, height + other.height);
		}
		inline Size operator -(const Size& other) const {
			return Size(width - other.width, height - other.height);
		}
    };

    struct Margin {
        float top, left, bottom, right;
    };
    using Border = Margin;
    
    struct Color {
		union {
			struct {
				float r, g, b, a;
			};
			float m[4];
		};

		Color() noexcept : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
		Color(float v) noexcept : r(v), g(v), b(v), a(v) {}
		Color(float R, float G, float B, float A = 1.0f) noexcept : r(R), g(G), b(B), a(A) {}

		static const Color Red, Green, Blue, Yellow, White, Black,
			Cyan, Magenta, Gray, Grey, Clear;
	};

	inline bool operator ==(const Color& v1, const Color& v2) {
		return (v1.r == v2.r && v1.g == v2.g && v1.b == v2.b && v1.a == v2.a);
	}
	inline bool operator !=(const Color& v1, const Color& v2) {
		return (v1.r != v2.r || v1.g != v2.g || v1.b != v2.b || v1.a != v2.a);
	}
}
