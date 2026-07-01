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

#include "Vector.hpp"

namespace Stelo {
    const Vector2 Vector2::Up(0.0f, 1.0f);
    const Vector2 Vector2::Down(0.0f, -1.0f);
    const Vector2 Vector2::Left(-1.0f, 0.0f);
    const Vector2 Vector2::Right(1.0f, 0.0f);

    const Vector3 Vector3::Up(0.0f, 1.0f, 0.0f);
    const Vector3 Vector3::Down(0.0f, -1.0f, 0.0f);
    const Vector3 Vector3::Left(-1.0f, 0.0f, 0.0f);
    const Vector3 Vector3::Right(1.0f, 0.0f, 0.0f);
    const Vector3 Vector3::Forward(0.0f, 0.0f, 1.0f);
    const Vector3 Vector3::Back(0.0f, 0.0f, -1.0f);

    const Vector4 Vector4::Up(0.0f, 1.0f, 0.0f, 0.0f);
    const Vector4 Vector4::Down(0.0f, -1.0f, 0.0f, 0.0f);
    const Vector4 Vector4::Right(1.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::Left(-1.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::Forward(0.0f, 0.0f, 1.0f, 0.0f);
    const Vector4 Vector4::Back(0.0f, 0.0f, -1.0f, 0.0f);
    const Vector4 Vector4::Inside(0.0f, 0.0f, 0.0f, 1.0f);
    const Vector4 Vector4::Outside(0.0f, 0.0f, 0.0f, -1.0f);
} // Stelo