/*
 * Copyright (c) 2026 Stelo Engine
 *
 * This file is part of Stelo Engine.
 *
 * Licensed under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#pragma once

#define ARGON_INTERNAL
#include "../Config.hpp"
#include "DeclareFunction.hpp"
#include "Enums.hpp"
#include "Resource.hpp"

struct MapBufferDesc {
    Buffer*  pBuffer = nullptr;
    void*    pMapped = nullptr;
    uint64_t mOffset = 0;
    uint64_t mSize = 0;
};

struct BufferUpdateDesc {
    Buffer* pDstBuffer;
    uint64_t mDstOffset;

    Buffer* pSrcBuffer;
    uint64_t mSrcOffset;
    
    uint64_t mSize;
};

struct BufferDesc {
    const void* pNext = nullptr;
    const uint32_t* pQueueFamilyIndices = nullptr;
    uint64_t mSize = 0;
    uint32_t mQueueFamilyIndexCount = 0;
    ResourceStateFlags mState = RESOURCE_STATE_UNDEFINED;
    BufferUsageFlags mUsage = BUFFER_USAGE_NONE;
    BufferCreateFlags mFlags = BUFFER_CREATE_NONE;
    MemoryUsage mMemoryUsage = MEMORY_USAGE_GPU_ONLY;
    SharingMode mSharingMode = SHARING_EXCLUSIVE;
};

struct Buffer : public Resource {
    DEFINE_GRAPHICS_API_TABLE(ReleaseBuffer);
protected:
    uint64_t mSize = 0;
    BufferUsageFlags mUsage = BUFFER_USAGE_NONE;
    MemoryUsage mMemoryUsage = MEMORY_USAGE_GPU_ONLY;

    Buffer(GraphicsApi api, const BufferDesc& desc)
        : Resource(api, RESOURCE_BUFFER), mSize(desc.mSize), mUsage(desc.mUsage), mMemoryUsage(desc.mMemoryUsage) {}

public:
    uint64_t GetSize() const {
        return mSize;
    }
    BufferUsageFlags GetUsage() const {
        return mUsage;
    }
    MemoryUsage GetMemoryUsage() const {
        return mMemoryUsage;
    }
    void Release() {
        _ReleaseBuffer[GetIndexAPI()](this);
    }
};
#undef ARGON_INTERNAL
