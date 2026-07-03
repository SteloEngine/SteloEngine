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
#include "../Math/Box.hpp"

#include "../Argon/Argon.hpp"

#include "Asset.hpp"

namespace Stelo {

struct VertexBase {
    Vector3 position;
    Vector3 normal;
    Vector4 tangent;
    Vector2 uv0;
};

struct VertexSkin {
    float    boneWeights[4];
    uint32_t boneIndices[4];
};

struct SubMesh {
    uint32_t indexStart;
    uint32_t indexCount;
    uint32_t materialSlot;
};

struct MeshData : public AssetObject {
    std::vector<VertexBase> vertices;
    std::vector<VertexSkin> skinData;
    std::vector<uint32_t> indices;

    std::vector<SubMesh> subMeshes;

    Box bounds;

    Buffer* vertexBuffer;
    Buffer* skinBuffer;
    Buffer* indexBuffer;

    bool isSkinned = false;
    bool keepCPUData = false;
};


}