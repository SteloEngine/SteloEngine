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
 * See the License for the specific language governing permissions and limitations
 * under the License.
*/

#pragma once

#include "IUnknown.hpp"
#include "DeclareFunction.hpp"
#include "Enums.hpp"

struct DescriptorBinding {
    uint32_t mBinding;
    uint32_t mCount;
    ShaderStage mShaderStages;
    DescriptorType mType;
};

struct DescriptorSetLayoutDesc {
    DescriptorBinding* pBindings;
    uint32_t mBindingCount;
};

struct DescriptorSetLayout : public IUnknown {
private:
    DEFINE_GRAPHICS_API_TABLE(ReleaseDescriptorSetLayout);
protected:
    DescriptorSetLayout(GraphicsApi api) : IUnknown(api) {}
public:
    void Release() { _ReleaseDescriptorSetLayout[GetIndexAPI()](this); }
};

struct PipelineLayoutDesc {
    DescriptorSetLayout** ppSetLayouts;
    uint32_t mSetLayoutCount;
};

struct PipelineLayout : public IUnknown {
private:
    DEFINE_GRAPHICS_API_TABLE(ReleasePipelineLayout);
protected:
    PipelineLayout(GraphicsApi api) : IUnknown(api) {}
public:
    void Release() { _ReleasePipelineLayout[GetIndexAPI()](this); }
};

struct DescriptorUpdateInfo {
    union {
        struct {
            Buffer* pBuffer;
            uint64_t mBufferOffset;
            uint64_t mBufferRange;
        };
        struct {
            Texture* pTexture;
        };
    };
    uint32_t mBinding;
    DescriptorType mType;
};

struct DescriptorSet : public IUnknown {
private:
    DEFINE_GRAPHICS_API_TABLE(DescriptorSetUpdate);
    DEFINE_GRAPHICS_API_TABLE(ReleaseDescriptorSet);
protected:
    DescriptorSet(GraphicsApi api) : IUnknown(api) {}
public:
    void Update(uint32_t infoCount, const DescriptorUpdateInfo* pInfos) { _DescriptorSetUpdate[GetIndexAPI()](this, infoCount, pInfos); }
    void Release() { _ReleaseDescriptorSet[GetIndexAPI()](this); }
};