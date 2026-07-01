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

#include "../../Interface/Cmd.hpp"
#include "Queue.hpp"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

struct Metal_Cmd : public Cmd {
    Metal_Cmd() : Cmd(GRAPHICS_API_METAL) {}

    Metal_Queue* pQueue = nullptr;
    id<MTLCommandBuffer> pCommandBuffer = nil;
    id<MTLRenderCommandEncoder> pRenderEncoder = nil;
    id<MTLBlitCommandEncoder> pBlitEncoder = nil;
    id<MTLComputeCommandEncoder> pComputeEncoder = nil;
    MTLPrimitiveType mPrimitiveType = MTLPrimitiveTypeTriangle;

    id<MTLBuffer> pIndexBuffer = nil;
    uint64_t      mIndexOffset = 0;
    MTLIndexType  mIndexType   = MTLIndexTypeUInt32;

    void SetBackendState(CmdState state) {
        SetState(state);
    }
};

#define CMD(ptr) static_cast<Metal_Cmd*>(ptr)

#endif
