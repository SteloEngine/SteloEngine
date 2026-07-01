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
 
#if ENABLE_METAL

#include "Buffer.hpp"
#include "Device.hpp"
#include "MetalUtils.hpp"

#include <cstring>
#include <new>

Result Metal_CreateBuffer(Device* device, const BufferDesc* desc, Buffer** out, ALog* log) {
    if (out) *out = nullptr;
    if (desc == nullptr || out == nullptr || desc->mSize == 0) {
        WriteMetalLog(log, ALog::Level::Error, "CreateBuffer received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }

    try {
        MTLResourceOptions options = MetalResourceOptions(desc->mMemoryUsage);
        id<MTLBuffer> nativeBuffer = [DEVICE(device)->pDevice newBufferWithLength:desc->mSize options:options];
        if (nativeBuffer == nil) {
            WriteMetalLog(log, ALog::Level::Error, "Failed to create Metal buffer");
            return RESULT_OUT_OF_DEVICE_MEMORY;
        }
        Metal_Buffer* buffer = new Metal_Buffer(*desc);
        buffer->pBuffer = nativeBuffer;
        *out = buffer;
        return RESULT_SUCCESS;
    } catch (const std::bad_alloc&) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to allocate Metal buffer wrapper");
        return RESULT_OUT_OF_HOST_MEMORY;
    }
}

Result Metal_MapBuffer(Device* device, MapBufferDesc* desc, ALog* log) {
    if (device == nullptr || desc == nullptr || desc->pBuffer == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "MapBuffer received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }

    Metal_Buffer* mbuffer = BUFFER(desc->pBuffer);
    if (mbuffer->pBuffer == nil) {
        WriteMetalLog(log, ALog::Level::Error, "MapBuffer called on null buffer");
        return RESULT_INVALID_STATE;
    }

    if (mbuffer->GetMemoryUsage() == MEMORY_USAGE_GPU_ONLY) {
        WriteMetalLog(log, ALog::Level::Error, "Cannot map a GPU_ONLY (Private) buffer!");
        return RESULT_INVALID_ACCESS; 
    }

    void* basePointer = [mbuffer->pBuffer contents];
    if (basePointer == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to get buffer contents (Pointer is null)");
        return RESULT_UNKNOWN_ERROR;
    }

    desc->pMapped = static_cast<uint8_t*>(basePointer) + desc->mOffset;

    return RESULT_SUCCESS;
}

Result Metal_UnmapBuffer(Device* device, MapBufferDesc* desc, ALog* log) {
    if (device == nullptr || desc == nullptr || desc->pBuffer == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "UnmapBuffer received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }

    Metal_Buffer* mbuffer = BUFFER(desc->pBuffer);
    if (mbuffer->pBuffer == nil) {
        WriteMetalLog(log, ALog::Level::Error, "UnmapBuffer called on null buffer");
        return RESULT_INVALID_STATE;
    }

    desc->pMapped = nullptr;
    if (mbuffer->pBuffer.storageMode == MTLStorageModeManaged) {
        uint64_t flushSize = desc->mSize;
        if (flushSize == 0) flushSize = mbuffer->GetSize() - desc->mOffset;
        [mbuffer->pBuffer didModifyRange:NSMakeRange(desc->mOffset, flushSize)];
    }

    return RESULT_SUCCESS;
}

void Metal_ReleaseBuffer(Buffer* pBuffer) {
    Metal_Buffer* buffer = BUFFER(pBuffer);
    if (buffer->pBuffer != nil) {
        [buffer->pBuffer release];
        buffer->pBuffer = nil;
    }
    delete buffer;
}

#endif
