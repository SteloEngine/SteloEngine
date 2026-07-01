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

#include "../../Interface/Device.hpp"

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

struct Metal_Device : public Device {
  public:
    Metal_Device() : Device(GRAPHICS_API_METAL) {
        DeviceFeatures features = {};
        features.mCompute = true;
        features.mTimestampQuery = true;
        features.mIndirectDraw = true;
        SetFeatures(features);

        DeviceLimits limits = {};
        limits.mMaxTexture2DSize = 16384;
        limits.mMaxVertexAttributes = 31;
        limits.mUniformBufferAlignment = 256;
        SetLimits(limits);
    }
    id<MTLDevice> pDevice = nil;
};

#define DEVICE(ptr) static_cast<Metal_Device*>(ptr)

#endif
