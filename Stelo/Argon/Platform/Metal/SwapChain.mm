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

#include "SwapChain.hpp"
#include "../../Tools/RenderContext.hpp"
#include "Device.hpp"
#include "MetalUtils.hpp"
#include "Texture.hpp"

#import <AppKit/AppKit.h>

#include <new>

namespace {
CAMetalLayer* ResolveLayer(const SwapChainDesc& desc, bool& ownsLayer, ALog* log) {
    ownsLayer = false;
    if (desc.pContext == nullptr || desc.pContext->pNativeView == nullptr) return nil;

    if (desc.pContext->mNativeViewType == NATIVE_VIEW_TYPE_CAMETAL_LAYER) {
        CAMetalLayer* layer = static_cast<CAMetalLayer*>(desc.pContext->pNativeView);
        return [layer retain];
    }

    if (desc.pContext->mNativeViewType == NATIVE_VIEW_TYPE_NS_VIEW) {
        NSView* view = static_cast<NSView*>(desc.pContext->pNativeView);
        CALayer* existingLayer = [view layer];
        if ([existingLayer isKindOfClass:[CAMetalLayer class]]) {
            return [(CAMetalLayer*) existingLayer retain];
        }

        CAMetalLayer* layer = [[CAMetalLayer alloc] init];
        [view setWantsLayer:YES];
        [view setLayer:layer];
        ownsLayer = true;
        return layer;
    }

    WriteMetalLog(log, ALog::Level::Error, "CreateSwapChain received unsupported native view type");
    return nil;
}
} // namespace

Result Metal_CreateSwapChain(Device* device, const SwapChainDesc* desc, SwapChain** out, ALog* log) {
    if (out) *out = nullptr;
    if (device == nullptr || desc == nullptr || out == nullptr || desc->pContext == nullptr || desc->mWidth == 0 ||
        desc->mHeight == 0) {
        WriteMetalLog(log, ALog::Level::Error, "CreateSwapChain received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }

    MTLPixelFormat pixelFormat = MetalPixelFormat(desc->mFormat);
    if (pixelFormat == MTLPixelFormatInvalid) {
        WriteMetalLog(log, ALog::Level::Error, "CreateSwapChain received unsupported format");
        return RESULT_UNSUPPORTED_API;
    }

    try {
        bool ownsLayer = false;
        CAMetalLayer* layer = ResolveLayer(*desc, ownsLayer, log);
        if (layer == nil) {
            return RESULT_INVALID_ARGUMENT;
        }

        layer.device = DEVICE(device)->pDevice;
        layer.pixelFormat = pixelFormat;
        layer.framebufferOnly = desc->mFramebufferOnly;
        layer.drawableSize = CGSizeMake(desc->mWidth, desc->mHeight);

        Metal_SwapChain* swapChain = new Metal_SwapChain(*desc);
        swapChain->pLayer = layer;
        swapChain->mOwnsLayer = ownsLayer;

        swapChain->pTexture = new Metal_Texture*[swapChain->pLayer.maximumDrawableCount];
        swapChain->mBackBufferCount = swapChain->pLayer.maximumDrawableCount;
        for (uint32_t i = 0; i < swapChain->pLayer.maximumDrawableCount; i++) {
            swapChain->pTexture[i] = new Metal_Texture({
                .mWidth = desc->mWidth,
                .mHeight = desc->mHeight,
                .mFormat = desc->mFormat,
                .mUsage = TEXTURE_USAGE_COLOR_ATTACHMENT | TEXTURE_USAGE_PRESENT,
                .mMemoryUsage = MEMORY_USAGE_GPU_ONLY,
                .mSampleCount = SAMPLE_COUNT_1,
            });
        }

        *out = swapChain;
        return RESULT_SUCCESS;
    } catch (const std::bad_alloc&) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to allocate Metal swapchain wrapper");
        return RESULT_OUT_OF_HOST_MEMORY;
    }
}

Result Metal_SwapChainAcquireTexture(SwapChain* swapChain, Texture** out, ALog* log) {
    if (out) *out = nullptr;
    if (swapChain == nullptr || out == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "SwapChainAcquireTexture received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    Metal_SwapChain* metalSwapChain = SWAPCHAIN(swapChain);
    metalSwapChain->pCurrentDrawable = [metalSwapChain->pLayer nextDrawable];
    if (metalSwapChain->pCurrentDrawable == nil) {
        WriteMetalLog(log, ALog::Level::Warning, "CAMetalLayer returned no drawable");
        return RESULT_NOT_READY;
    }
    
    Metal_Texture* texture = metalSwapChain->pTexture[metalSwapChain->mCurrentIndex];
    metalSwapChain->mCurrentIndex = (metalSwapChain->mCurrentIndex + 1) % metalSwapChain->mBackBufferCount;
    texture->pTexture = metalSwapChain->pCurrentDrawable.texture;
    *out = texture;
    return RESULT_SUCCESS;
}

Result Metal_SwapChainResize(SwapChain* swapChain, uint32_t width, uint32_t height, ALog* log) {
    if (swapChain == nullptr || width == 0 || height == 0) {
        WriteMetalLog(log, ALog::Level::Error, "SwapChainResize received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }

    Metal_SwapChain* metalSwapChain = SWAPCHAIN(swapChain);
    metalSwapChain->pLayer.drawableSize = CGSizeMake(width, height);
    metalSwapChain->SetBackendSize(width, height);
    return RESULT_SUCCESS;
}

void Metal_ReleaseSwapChain(SwapChain* swapChain) {
    if (swapChain == nullptr) return;

    Metal_SwapChain* metalSwapChain = SWAPCHAIN(swapChain);
    if (metalSwapChain->pLayer != nil) {
        [metalSwapChain->pLayer release];
        metalSwapChain->pLayer = nil;
    }
    for (uint32_t i = 0; i < metalSwapChain->mBackBufferCount; i++) {
        metalSwapChain->pTexture[i]->Release();
    }
    delete[] metalSwapChain->pTexture;
    delete metalSwapChain;
}

#endif
