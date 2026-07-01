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

#if ENABLE_METAL

#include "../../Interface/SwapChain.hpp"
#include "Texture.hpp"

#import <QuartzCore/CAMetalLayer.h>

struct Metal_SwapChain : public SwapChain {
    Metal_SwapChain(const SwapChainDesc& desc) : SwapChain(GRAPHICS_API_METAL, desc) {}
    CAMetalLayer* pLayer = nil;
    id<CAMetalDrawable> pCurrentDrawable = nil;
    Metal_Texture** pTexture = nullptr;
    uint32_t mBackBufferCount = 0;
    uint32_t mCurrentIndex = 0;
    bool mOwnsLayer = false;

    void SetBackendSize(uint32_t width, uint32_t height) {
        SetSize(width, height);
    }
};

#define SWAPCHAIN(ptr) static_cast<Metal_SwapChain*>(ptr)

#endif
