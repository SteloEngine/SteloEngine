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
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "DeclareTypes.hpp"
#include "Enums.hpp"

struct ClearColorValue {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
};

struct ClearDepthStencilValue {
    float mDepth = 1.0f;
    uint32_t mStencil = 0;
};

struct ColorAttachment {
    Texture* pTexture = nullptr;
    ClearColorValue mClearColor = {};
    LoadOp mLoadOp = LOAD_OP_CLEAR;
    StoreOp mStoreOp = STORE_OP_STORE;
    uint32_t mMipLevel = 0;
    uint32_t mArrayLayer = 0;
};

struct DepthStencilAttachment {
    Texture* pTexture = nullptr;
    ClearDepthStencilValue mClearValue = {};
    LoadOp mDepthLoadOp = LOAD_OP_CLEAR;
    StoreOp mDepthStoreOp = STORE_OP_STORE;
    LoadOp mStencilLoadOp = LOAD_OP_DONT_CARE;
    StoreOp mStencilStoreOp = STORE_OP_DONT_CARE;
    uint32_t mMipLevel = 0;
    uint32_t mArrayLayer = 0;
};

struct RenderPass {
    const void* pNext = nullptr;
    const ColorAttachment* pColorAttachments = nullptr;
    const DepthStencilAttachment* pDepthStencilAttachment = nullptr;
    uint32_t mColorAttachmentCount = 0;
    uint32_t mWidth = 0;
    uint32_t mHeight = 0;
};