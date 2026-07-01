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
#include "DeclareTypes.hpp"
#include "GpuInfo.hpp"
#include "IUnknown.hpp"

struct DeviceDesc {
    const char* pApplicationName = nullptr;
    const char* pEngineName = "Argon";
    RenderContext* pContext = nullptr;
    GraphicsApi mPreferredApi = GRAPHICS_API_AUTO;
    uint32_t mPreferredGpuIndex = 0;
    bool mEnableValidation = false;
    bool mAllowFallback = true;
};

struct Device : public IUnknown {
  private:
    DEFINE_GRAPHICS_API_TABLE(CreateBuffer);
    DEFINE_GRAPHICS_API_TABLE(CreateTexture);
    DEFINE_GRAPHICS_API_TABLE(CreateQueue);
    DEFINE_GRAPHICS_API_TABLE(CreatePipeline);
    DEFINE_GRAPHICS_API_TABLE(CreateSwapChain);
    DEFINE_GRAPHICS_API_TABLE(CreateDescriptorSetLayout);
    DEFINE_GRAPHICS_API_TABLE(CreatePipelineLayout);

    DEFINE_GRAPHICS_API_TABLE(AllocateDescriptorSet);
    DEFINE_GRAPHICS_API_TABLE(MapBuffer);
    DEFINE_GRAPHICS_API_TABLE(UnmapBuffer);

    DEFINE_GRAPHICS_API_TABLE(ReleaseDevice);
  protected:
    DeviceFeatures mFeatures = {};
    DeviceLimits mLimits = {};

    Device(GraphicsApi api) : IUnknown(api) {}
    void SetFeatures(const DeviceFeatures& features) {
        mFeatures = features;
    }
    void SetLimits(const DeviceLimits& limits) {
        mLimits = limits;
    }

  public:
    inline Result CreateBuffer(const BufferDesc* pDesc, Buffer** ppOut, ALog* pDebug = nullptr) {
        return _CreateBuffer[GetIndexAPI()](this, pDesc, ppOut, pDebug);
    }
    inline Result CreateTexture(const TextureDesc* pDesc, Texture** ppOut, ALog* pDebug = nullptr) {
        return _CreateTexture[GetIndexAPI()](this, pDesc, ppOut, pDebug);
    }
    inline Result CreateQueue(const QueueDesc* pDesc, Queue** ppOut, ALog* pDebug = nullptr) {
        return _CreateQueue[GetIndexAPI()](this, pDesc, ppOut, pDebug);
    }
    inline Result CreatePipeline(const PipelineDesc* pDesc, Pipeline** ppOut, ALog* pDebug = nullptr) {
        return _CreatePipeline[GetIndexAPI()](this, pDesc, ppOut, pDebug);
    }
    inline Result CreateSwapChain(const SwapChainDesc* pDesc, SwapChain** ppOut, ALog* pDebug = nullptr) {
        return _CreateSwapChain[GetIndexAPI()](this, pDesc, ppOut, pDebug);
    }
    inline Result CreateDescriptorSetLayout(const DescriptorSetLayoutDesc* pDesc, DescriptorSetLayout** ppOut, ALog* pDebug = nullptr) {
        return _CreateDescriptorSetLayout[GetIndexAPI()](this, pDesc, ppOut, pDebug);
    }
    inline Result CreatePipelineLayout(const PipelineLayoutDesc* pDesc, PipelineLayout** ppOut, ALog* pDebug = nullptr) {
        return _CreatePipelineLayout[GetIndexAPI()](this, pDesc, ppOut, pDebug);
    }
    inline Result AllocateDescriptorSet(DescriptorSetLayout* pLayout, DescriptorSet** ppOut, ALog* pDebug = nullptr) {
        return _AllocateDescriptorSet[GetIndexAPI()](this, pLayout, ppOut, pDebug);
    }
    inline Result MapBuffer(MapBufferDesc* pDesc, ALog* pDebug = nullptr) {
        return _MapBuffer[GetIndexAPI()](this, pDesc, pDebug);
    }
    inline Result UnmapBuffer(MapBufferDesc* pDesc, ALog* pDebug = nullptr) {
        return _UnmapBuffer[GetIndexAPI()](this, pDesc, pDebug);
    }
    
    inline const DeviceFeatures& GetFeatures() const {
        return mFeatures;
    }
    inline const DeviceLimits& GetLimits() const {
        return mLimits;
    }

    inline void Release() {
        _ReleaseDevice[GetIndexAPI()](this);
    }
};

Result CreateDevice(const DeviceDesc* desc, Device** out, ALog* debug = nullptr);

#undef ARGON_INTERNAL
