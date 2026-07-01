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

#include "../../Interface/Enums.hpp"
#include "Interface/ALog.hpp"

#include <Metal/MTLTexture.h>
#import <Metal/Metal.h>

inline MTLPixelFormat MetalPixelFormat(Format format) {
    switch (format) {
    case FORMAT_R8_UNORM:
        return MTLPixelFormatR8Unorm;
    case FORMAT_RG8_UNORM:
        return MTLPixelFormatRG8Unorm;
    case FORMAT_RGBA8_UNORM:
        return MTLPixelFormatRGBA8Unorm;
    case FORMAT_BGRA8_UNORM:
        return MTLPixelFormatBGRA8Unorm;
    case FORMAT_RGBA16_FLOAT:
        return MTLPixelFormatRGBA16Float;
    case FORMAT_RGBA32_FLOAT:
        return MTLPixelFormatRGBA32Float;
    case FORMAT_R32_FLOAT:
        return MTLPixelFormatR32Float;
    case FORMAT_D32_FLOAT:
        return MTLPixelFormatDepth32Float;
    case FORMAT_D24_UNORM_S8_UINT:
        return MTLPixelFormatDepth24Unorm_Stencil8;
    case FORMAT_UNDEFINED:
    default:
        return MTLPixelFormatInvalid;
    }
}

inline MTLVertexFormat MetalVertexFormat(Format format) {
    switch (format) {
    case FORMAT_R8_UNORM:
        return MTLVertexFormatUCharNormalized;
    case FORMAT_RG8_UNORM:
        return MTLVertexFormatUChar2Normalized;
    case FORMAT_RGBA8_UNORM:
    case FORMAT_BGRA8_UNORM:
        return MTLVertexFormatUChar4Normalized;
    case FORMAT_R32_FLOAT:
        return MTLVertexFormatFloat;
    case FORMAT_RGBA16_FLOAT:
        return MTLVertexFormatHalf4;
    case FORMAT_RGBA32_FLOAT:
        return MTLVertexFormatFloat4;
    case FORMAT_UNDEFINED:
    case FORMAT_D32_FLOAT:
    case FORMAT_D24_UNORM_S8_UINT:
    default:
        return MTLVertexFormatInvalid;
    }
}

inline MTLResourceOptions MetalResourceOptions(MemoryUsage usage) {
    switch (usage) {
    case MEMORY_USAGE_CPU_TO_GPU:
    case MEMORY_USAGE_GPU_TO_CPU:
        return MTLResourceStorageModeShared;
    case MEMORY_USAGE_GPU_ONLY:
    default:
        return MTLResourceStorageModePrivate;
    }
}

inline MTLStorageMode MetalStorageMode(MemoryUsage usage) {
    switch (usage) {
    case MEMORY_USAGE_CPU_TO_GPU:
    case MEMORY_USAGE_GPU_TO_CPU:
        return MTLStorageModeShared;
    case MEMORY_USAGE_GPU_ONLY:
    default:
        return MTLStorageModePrivate;
    }
}

inline MTLVertexStepFunction MetalStepFunction(VertexStepRate rate) {
    switch (rate) {
    case VERTEX_STEP_RATE_INSTANCE:
        return MTLVertexStepFunctionPerInstance;
    case VERTEX_STEP_RATE_VERTEX:
    default:
        return MTLVertexStepFunctionPerVertex;
    }
}

inline MTLTextureUsage MetalTextureUsage(TextureUsageFlags usage) {
    MTLTextureUsage result = MTLTextureUsageUnknown;
    if (HasFlag(usage, TEXTURE_USAGE_SAMPLED)) result |= MTLTextureUsageShaderRead;
    if (HasFlag(usage, TEXTURE_USAGE_STORAGE)) result |= MTLTextureUsageShaderWrite;
    if (HasFlag(usage, TEXTURE_USAGE_COLOR_ATTACHMENT) || HasFlag(usage, TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT)) {
        result |= MTLTextureUsageRenderTarget;
    }
    return result;
}

inline MTLLoadAction MetalLoadAction(LoadOp op) {
    switch (op) {
    case LOAD_OP_LOAD:
        return MTLLoadActionLoad;
    case LOAD_OP_CLEAR:
        return MTLLoadActionClear;
    case LOAD_OP_DONT_CARE:
    default:
        return MTLLoadActionDontCare;
    }
}

inline MTLStoreAction MetalStoreAction(StoreOp op) {
    switch (op) {
    case STORE_OP_STORE:
        return MTLStoreActionStore;
    case STORE_OP_DONT_CARE:
    default:
        return MTLStoreActionDontCare;
    }
}

inline MTLPrimitiveType MetalPrimitiveType(PrimitiveTopology topology) {
    switch (topology) {
    case PRIMITIVE_TOPOLOGY_POINT_LIST:
        return MTLPrimitiveTypePoint;
    case PRIMITIVE_TOPOLOGY_LINE_LIST:
        return MTLPrimitiveTypeLine;
    case PRIMITIVE_TOPOLOGY_LINE_STRIP:
        return MTLPrimitiveTypeLineStrip;
    case PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
        return MTLPrimitiveTypeTriangleStrip;
    case PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
    default:
        return MTLPrimitiveTypeTriangle;
    }
}

inline uint32_t GetFormatByteSize(Format format) {
    switch (format) {
    case FORMAT_BGRA8_UNORM:
        return 4;
    case FORMAT_RGBA16_FLOAT:
        return 8;
    case FORMAT_RGBA32_FLOAT:
        return 16;
    default:
        return 0;
    }
}

inline MTLWinding MetalWinding(FrontFace frontFace) {
    switch (frontFace) {
    case FRONT_FACE_CW:
        return MTLWindingClockwise;
    case FRONT_FACE_CCW:
    default:
        return MTLWindingCounterClockwise;
    }
}

inline MTLCullMode MetalCullMode(CullMode cullMode) {
    switch (cullMode) {
    case CULL_MODE_NONE:
        return MTLCullModeNone;
    case CULL_MODE_FRONT:
        return MTLCullModeFront;
    case CULL_MODE_BACK:
    default:
        return MTLCullModeBack;
    }
}

inline MTLTriangleFillMode MetalFillMode(FillMode fillMode) {
    switch (fillMode) {
    case FILL_MODE_WIREFRAME:
        return MTLTriangleFillModeLines;
    case FILL_MODE_SOLID:
    default:
        return MTLTriangleFillModeFill;
    }
}

inline void WriteMetalLog(ALog* log, ALog::Level level, const char* message) {
    if (log) log->Write(level, "Argon.Metal", message);
}

inline void WriteNSError(ALog* log, const char* prefix, NSError* error) {
    if (!log) return;
    std::string message = prefix;
    if (error != nil && [error localizedDescription] != nil) {
        message += ": ";
        message += [[error localizedDescription] UTF8String];
    }
    log->Write(ALog::Level::Error, "Argon.Metal", message);
}