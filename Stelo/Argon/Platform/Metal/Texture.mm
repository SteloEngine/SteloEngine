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

#if ENABLE_METAL

#include "Texture.hpp"
#include "Device.hpp"
#include "MetalUtils.hpp"

#include <new>

namespace {
MTLTextureType TextureTypeFromDesc(const TextureDesc& desc) {
    if (desc.mSampleCount != SAMPLE_COUNT_1) {
        return desc.mArrayLayerCount > 1 ? MTLTextureType2DMultisampleArray : MTLTextureType2DMultisample;
    }

    switch (desc.mDimension) {
    case TEXTURE_DIMENSION_1D:
        return desc.mArrayLayerCount > 1 ? MTLTextureType1DArray : MTLTextureType1D;
    case TEXTURE_DIMENSION_3D:
        return MTLTextureType3D;
    case TEXTURE_DIMENSION_2D:
    default:
        return desc.mArrayLayerCount > 1 ? MTLTextureType2DArray : MTLTextureType2D;
    }
}

NSUInteger BytesPerPixel(Format format) {
    switch (format) {
    case FORMAT_R8_UNORM:
        return 1;
    case FORMAT_RG8_UNORM:
        return 2;
    case FORMAT_RGBA8_UNORM:
    case FORMAT_BGRA8_UNORM:
    case FORMAT_R32_FLOAT:
    case FORMAT_D32_FLOAT:
    case FORMAT_D24_UNORM_S8_UINT:
        return 4;
    case FORMAT_RGBA16_FLOAT:
        return 8;
    case FORMAT_RGBA32_FLOAT:
        return 16;
    case FORMAT_UNDEFINED:
    default:
        return 0;
    }
}
} // namespace

Result Metal_CreateTexture(Device* device, const TextureDesc* desc, Texture** out, ALog* log) {
    if (out) *out = nullptr;
    if (device == nullptr || desc == nullptr || out == nullptr || desc->mWidth == 0 || desc->mHeight == 0 ||
        desc->mDepth == 0 || desc->mMipCount == 0 || desc->mArrayLayerCount == 0 || desc->mFormat == FORMAT_UNDEFINED ||
        desc->mUsage == TEXTURE_USAGE_NONE) {
        WriteMetalLog(log, ALog::Level::Error, "CreateTexture received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }

    MTLPixelFormat pixelFormat = MetalPixelFormat(desc->mFormat);
    if (pixelFormat == MTLPixelFormatInvalid) {
        WriteMetalLog(log, ALog::Level::Error, "CreateTexture received unsupported texture format");
        return RESULT_UNSUPPORTED_API;
    }

    try {
        MTLTextureDescriptor* nativeDesc = [[MTLTextureDescriptor alloc] init];
        nativeDesc.textureType = TextureTypeFromDesc(*desc);
        nativeDesc.pixelFormat = pixelFormat;
        nativeDesc.width = desc->mWidth;
        nativeDesc.height = desc->mHeight;
        nativeDesc.depth = desc->mDepth;
        nativeDesc.mipmapLevelCount = desc->mMipCount;
        nativeDesc.arrayLength = desc->mArrayLayerCount;
        nativeDesc.sampleCount = desc->mSampleCount;
        nativeDesc.usage = MetalTextureUsage(desc->mUsage);
        nativeDesc.storageMode = MetalStorageMode(desc->mMemoryUsage);

        id<MTLTexture> nativeTexture = [DEVICE(device)->pDevice newTextureWithDescriptor:nativeDesc];
        [nativeDesc release];

        if (nativeTexture == nil) {
            WriteMetalLog(log, ALog::Level::Error, "Failed to create Metal texture");
            return RESULT_OUT_OF_DEVICE_MEMORY;
        }

        if (desc->pInitialData != nullptr) {
            if (desc->mMemoryUsage == MEMORY_USAGE_GPU_ONLY) {
                [nativeTexture release];
                WriteMetalLog(log, ALog::Level::Warning,
                              "Initial data upload for private textures is not implemented yet");
                return RESULT_UNSUPPORTED_API;
            }

            const NSUInteger bytesPerPixel = BytesPerPixel(desc->mFormat);
            if (bytesPerPixel == 0) {
                [nativeTexture release];
                WriteMetalLog(log, ALog::Level::Error, "Cannot upload initial texture data for this format");
                return RESULT_UNSUPPORTED_API;
            }
            MTLRegion region = MTLRegionMake2D(0, 0, desc->mWidth, desc->mHeight);
            [nativeTexture replaceRegion:region
                             mipmapLevel:0
                               withBytes:desc->pInitialData
                             bytesPerRow:desc->mWidth * bytesPerPixel];
        }

        Metal_Texture* texture = new Metal_Texture(*desc);
        texture->pTexture = nativeTexture;
        *out = texture;
        return RESULT_SUCCESS;
    } catch (const std::bad_alloc&) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to allocate Metal texture wrapper");
        return RESULT_OUT_OF_HOST_MEMORY;
    }
}

void Metal_ReleaseTexture(Texture* texture) {
    if (texture == nullptr) return;

    Metal_Texture* metalTexture = TEXTURE(texture);
    if (metalTexture->pTexture != nil) {
        [metalTexture->pTexture release];
        metalTexture->pTexture = nil;
    }
    delete metalTexture;
}

#endif
