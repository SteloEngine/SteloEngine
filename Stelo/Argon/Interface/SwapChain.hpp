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

#include "ALog.hpp"
#include "DeclareFunction.hpp"
#include "Enums.hpp"
#include "IUnknown.hpp"

struct SwapChainDesc {
    const void* pNext = nullptr;
    RenderContext* pContext = nullptr;
    uint32_t mWidth = 0;
    uint32_t mHeight = 0;
    uint32_t mBackBufferCount = 2;
    Format mFormat = FORMAT_BGRA8_UNORM;
    PresentMode mPresentMode = PRESENT_MODE_FIFO;
    bool mFramebufferOnly = true;
};

struct SwapChain : public IUnknown {
  private:
    DEFINE_GRAPHICS_API_TABLE(SwapChainAcquireTexture);
    DEFINE_GRAPHICS_API_TABLE(SwapChainResize);
    DEFINE_GRAPHICS_API_TABLE(ReleaseSwapChain);
    uint32_t mWidth = 0;
    uint32_t mHeight = 0;
    Format mFormat = FORMAT_BGRA8_UNORM;

  protected:
    SwapChain(GraphicsApi api, const SwapChainDesc& desc)
        : IUnknown(api), mWidth(desc.mWidth), mHeight(desc.mHeight), mFormat(desc.mFormat) {}
    void SetSize(uint32_t width, uint32_t height) {
        mWidth = width;
        mHeight = height;
    }

  public:
    uint32_t GetWidth() const {
        return mWidth;
    }
    uint32_t GetHeight() const {
        return mHeight;
    }
    Format GetFormat() const {
        return mFormat;
    }
    Result AcquireTexture(Texture** ppTexture, ALog* pLog = nullptr) {
        return _SwapChainAcquireTexture[GetIndexAPI()](this, ppTexture, pLog);
    }
    Result Resize(uint32_t width, uint32_t height, ALog* pLog = nullptr) {
        return _SwapChainResize[GetIndexAPI()](this, width, height, pLog);
    }
    void Release() {
        _ReleaseSwapChain[GetIndexAPI()](this);
    }
};

#undef ARGON_INTERNAL
