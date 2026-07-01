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

#include "Device.hpp"
#include "MetalUtils.hpp"

#import <stdexcept>

Result Metal_CreateDevice(const DeviceDesc* desc, Device** out, ALog* debug) {
    if (out == nullptr || desc == nullptr) {
        WriteMetalLog(debug, ALog::Level::Error, "CreateDevice received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (desc->mPreferredGpuIndex != 0) {
        WriteMetalLog(debug, ALog::Level::Warning, "Preferred GPU index selection is not implemented on Metal yet");
        return RESULT_NOT_FOUND;
    }

    *out = nullptr;

    try {
        Metal_Device* pDevice = new Metal_Device();
        pDevice->pDevice = MTLCreateSystemDefaultDevice();
        if (pDevice->pDevice == nullptr) {
            WriteMetalLog(debug, ALog::Level::Error, "MTLCreateSystemDefaultDevice returned null");
            delete pDevice;
            return RESULT_UNSUPPORTED_API;
        }

        *out = pDevice;

        NSString* deviceName = [pDevice->pDevice name];
        const char* name = deviceName != nil ? [deviceName UTF8String] : nullptr;

        if (name != nullptr) {
            std::string message = "Created device ";
            message += name;
            WriteMetalLog(debug, ALog::Level::Info, message.c_str());
        } else {
            WriteMetalLog(debug, ALog::Level::Info, "Created device");
        }
    } catch (const std::bad_alloc&) {
        WriteMetalLog(debug, ALog::Level::Error, "Failed to allocate the Metal device wrapper");
        return RESULT_OUT_OF_HOST_MEMORY;
    }
    return RESULT_SUCCESS;
}

void Metal_ReleaseDevice(Device* device) {
    if (device == nullptr) return;

    Metal_Device* metalDevice = DEVICE(device);
    if (metalDevice->pDevice != nil) {
        [metalDevice->pDevice release];
        metalDevice->pDevice = nil;
    }
    delete metalDevice;
}
#endif
