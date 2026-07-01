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

#if ENABLE_METAL

#include "../../Interface/PipelineLayout.hpp"

#import <Metal/Metal.h>

struct Metal_DescriptorSetLayout : public DescriptorSetLayout {
    Metal_DescriptorSetLayout() : DescriptorSetLayout(GRAPHICS_API_METAL) {}
    id<MTLArgumentEncoder> pArgumentEncoder;
    NSUInteger mBufferLength;
    ShaderStage mStageFlags;
};

struct Metal_PipelineLayout : public PipelineLayout {
    Metal_PipelineLayout() : PipelineLayout(GRAPHICS_API_METAL) {}
    struct StageSlots {
        uint32_t vertexSlot = 0xFFFFFFFF;
        uint32_t fragmentSlot = 0xFFFFFFFF;
    };
    std::vector<StageSlots> mSetToSlotMappings;
};

struct Metal_DescriptorSet : public DescriptorSet {
    Metal_DescriptorSet() : DescriptorSet(GRAPHICS_API_METAL) {}
    id<MTLBuffer> pArgumentBuffer;
    Metal_DescriptorSetLayout* pLayout;
    std::vector<id<MTLResource>> mTrackedResources;
};

#define DESCRIPTOR_SET_LAYOUT(ptr) static_cast<Metal_DescriptorSetLayout*>(ptr)
#define PIPELINE_LAYOUT(ptr) static_cast<Metal_PipelineLayout*>(ptr)
#define DESCRIPTOR_SET(ptr) static_cast<Metal_DescriptorSet*>(ptr)

#endif // ENABLE_METAL

