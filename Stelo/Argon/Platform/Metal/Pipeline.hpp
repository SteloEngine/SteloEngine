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

#include "../../Interface/Pipeline.hpp"

#import <Metal/Metal.h>

struct Metal_Pipeline : public Pipeline {
    Metal_Pipeline(const PipelineDesc& desc) : Pipeline(GRAPHICS_API_METAL, desc) {}
    id<MTLRenderPipelineState> pRenderPipeline;
    id<MTLComputePipelineState> pComputePipeline;
    id<MTLDepthStencilState> pDepthStencil;
    union {
        struct {
            MTLCullMode mCullMode;
            MTLTriangleFillMode mFillMode;
            MTLWinding mWinding;
            bool mDepthClipMode;
            uint32_t mPatchControlPointCount;
            bool mTessellation;

            float mDepthBias;
            float mSlopeScale;
        };
        struct {
            MTLSize mNumThreadsPerGroup;
        };
    };
    PipelineType mType;
    const char* name;
};

#define PIPELINE(ptr) static_cast<Metal_Pipeline*>(ptr)

#endif
