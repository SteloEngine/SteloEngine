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

#pragma once

#include "../../Interface/Buffer.hpp"

#import <Metal/Metal.h>

struct Metal_Buffer : public Buffer {
    Metal_Buffer(const BufferDesc& desc) : Buffer(GRAPHICS_API_METAL, desc) {}
    id<MTLBuffer> pBuffer = nil;

    void SetState(ResourceStateFlags state) {
        mState = state;
    }
};

#define BUFFER(ptr) static_cast<Metal_Buffer*>(ptr)
