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

#include "../Math/Vector.hpp"
#include "../Argon/Argon.hpp"

#include "Asset.hpp"

namespace Stelo {

struct Vertex {
    Vector3 position;
    Vector2 uv;
    float boneWeight;
    uint32_t boneIndex;
};

struct Mesh : public AssetObject {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    Buffer* vertexBuffer;
    Buffer* indexBuffer;


};
    
}