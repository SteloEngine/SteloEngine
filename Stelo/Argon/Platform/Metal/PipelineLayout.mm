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

#include "Interface/Enums.hpp"
#if ENABLE_METAL

#include "PipelineLayout.hpp"
#include "Device.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"

// SỬA ĐỔI: Không gán cứng Slot 8 nữa. Khởi động từ Slot 0 sạch sẽ như Vulkan!
// Việc phân định slot sẽ dựa hoàn toàn vào chỉ số Set Index của Pipeline Layout.
constexpr uint32_t METAL_DESCRIPTOR_SET_BASE_SLOT = 0;

Result Metal_CreatePipelineLayout(Device* device, const PipelineLayoutDesc* desc, PipelineLayout** out, ALog* log) {
    Metal_PipelineLayout* layout = new Metal_PipelineLayout();
    layout->mSetToSlotMappings.resize(desc->mSetLayoutCount);

    uint32_t currentVertexSlot = METAL_DESCRIPTOR_SET_BASE_SLOT;
    uint32_t currentFragmentSlot = METAL_DESCRIPTOR_SET_BASE_SLOT;

    for (uint32_t i = 0; i < desc->mSetLayoutCount; ++i) {
        auto* setLayout = static_cast<Metal_DescriptorSetLayout*>(desc->ppSetLayouts[i]);
        ShaderStage stages = setLayout->mStageFlags; 

        // Mỗi SetLayout sẽ chiếm 1 Slot duy nhất tương ứng trên Vertex và Fragment Stage
        if (stages & SHADER_STAGE_VERTEX) {
            layout->mSetToSlotMappings[i].vertexSlot = currentVertexSlot++;
        }
        if (stages & SHADER_STAGE_FRAGMENT) {
            layout->mSetToSlotMappings[i].fragmentSlot = currentFragmentSlot++;
        }
    }

    *out = layout;
    return RESULT_SUCCESS;
}

Result Metal_CreateDescriptorSetLayout(Device* device, const DescriptorSetLayoutDesc* desc, DescriptorSetLayout** out, ALog* log) {
    MTLArgumentDescriptor* argArray = (MTLArgumentDescriptor*)alloca(desc->mBindingCount * sizeof(MTLArgumentDescriptor*));
    NSMutableArray<MTLArgumentDescriptor *> *argumentArray = [[NSMutableArray alloc] initWithCapacity:desc->mBindingCount];

    for (uint32_t i = 0; i < desc->mBindingCount; ++i) {
        const auto& binding = desc->pBindings[i];
        MTLArgumentDescriptor *arg = [MTLArgumentDescriptor argumentDescriptor];
        arg.index = binding.mBinding;
        arg.arrayLength = binding.mCount;
        
        switch (binding.mType) {
            case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                arg.dataType = MTLDataTypePointer;
                arg.access = MTLBindingAccessReadOnly;
                break;
            case DESCRIPTOR_TYPE_STORAGE_BUFFER:
                arg.dataType = MTLDataTypePointer;
                arg.access = MTLBindingAccessReadWrite;
                break;
            case DESCRIPTOR_TYPE_TEXTURE:
                arg.dataType = MTLDataTypeTexture;
                arg.access = MTLBindingAccessReadOnly;
                break;
            default: break;
        }
        [argumentArray addObject:arg];
    }

    id<MTLArgumentEncoder> argumentEncoder = [DEVICE(device)->pDevice newArgumentEncoderWithArguments:argumentArray];
    NSUInteger bufferLength = argumentEncoder.encodedLength;

    Metal_DescriptorSetLayout* layout = new Metal_DescriptorSetLayout();
    layout->pArgumentEncoder = argumentEncoder;
    layout->mBufferLength = bufferLength;
    layout->mStageFlags = SHADER_STAGE_VERTEX | SHADER_STAGE_FRAGMENT;
    
    [argumentArray release];
    *out = layout;
    return RESULT_SUCCESS;
}

Result Metal_AllocateDescriptorSet(Device* device, DescriptorSetLayout* layout, DescriptorSet** out, ALog* log) {
    Metal_DescriptorSet* set = new Metal_DescriptorSet();
    set->pLayout = DESCRIPTOR_SET_LAYOUT(layout);
    
    set->pArgumentBuffer = [DEVICE(device)->pDevice newBufferWithLength:set->pLayout->mBufferLength 
                                                               options:MTLResourceStorageModeShared];
    
    *out = set;
    return RESULT_SUCCESS;
}

void Metal_DescriptorSetUpdate(DescriptorSet* set, uint32_t infoCount, const DescriptorUpdateInfo* pInfos) {
    Metal_DescriptorSet* metalSet = DESCRIPTOR_SET(set);
    [metalSet->pLayout->pArgumentEncoder setArgumentBuffer:metalSet->pArgumentBuffer offset:0];

    for (uint32_t i = 0; i < infoCount; ++i) {
        const auto& info = pInfos[i];
        switch (info.mType) {
            case DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            case DESCRIPTOR_TYPE_STORAGE_BUFFER: {
                [metalSet->pLayout->pArgumentEncoder setBuffer:BUFFER(info.pBuffer)->pBuffer 
                                                       offset:info.mBufferOffset 
                                                      atIndex:info.mBinding];
                break;
            }
            case DESCRIPTOR_TYPE_TEXTURE: {
                [metalSet->pLayout->pArgumentEncoder setTexture:TEXTURE(info.pTexture)->pTexture 
                                                       atIndex:info.mBinding];
                break;
            }
            default: break;
        }
    }
}

#endif // ENABLE_METAL

