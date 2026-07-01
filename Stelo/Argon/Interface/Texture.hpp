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

#include "DeclareFunction.hpp"
#include "Enums.hpp"
#include "Resource.hpp"

struct TextureDesc {
    const void* pNext = nullptr;
    const void* pInitialData = nullptr;
    uint32_t mWidth = 1;
    uint32_t mHeight = 1;
    uint32_t mDepth = 1;
    uint32_t mMipCount = 1;
    uint32_t mArrayLayerCount = 1;
    Format mFormat = FORMAT_UNDEFINED;
    TextureDimension mDimension = TEXTURE_DIMENSION_2D;
    TextureUsageFlags mUsage = TEXTURE_USAGE_NONE;
    MemoryUsage mMemoryUsage = MEMORY_USAGE_GPU_ONLY;
    SampleCount mSampleCount = SAMPLE_COUNT_1;
    ResourceStateFlags mInitialState = RESOURCE_STATE_UNDEFINED;
};

struct Texture : public Resource {
  private:
    DEFINE_GRAPHICS_API_TABLE(ReleaseTexture);
    uint32_t mWidth = 0;
    uint32_t mHeight = 0;
    uint32_t mDepth = 1;
    Format mFormat = FORMAT_UNDEFINED;
    TextureUsageFlags mUsage = TEXTURE_USAGE_NONE;

  protected:
    Texture(GraphicsApi api, const TextureDesc& desc)
        : Resource(api, RESOURCE_TEXTURE), mWidth(desc.mWidth), mHeight(desc.mHeight), mDepth(desc.mDepth),
          mFormat(desc.mFormat), mUsage(desc.mUsage) {}

  public:
    uint32_t GetWidth() const {
        return mWidth;
    }
    uint32_t GetHeight() const {
        return mHeight;
    }
    uint32_t GetDepth() const {
        return mDepth;
    }
    Format GetFormat() const {
        return mFormat;
    }
    TextureUsageFlags GetUsage() const {
        return mUsage;
    }
    void Release() {
        _ReleaseTexture[GetIndexAPI()](this);
    }
};

#undef ARGON_INTERNAL
