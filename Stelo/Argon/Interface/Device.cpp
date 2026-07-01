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

#include "Device.hpp"
#include "Enums.hpp"

#include <string>
#include <string_view>

namespace {
const char* GraphicsApiName(GraphicsApi api) {
    switch (api) {
    case GRAPHICS_API_UNKNOWN:
        return "Unknown";
    case GRAPHICS_API_METAL:
        return "Metal";
    case GRAPHICS_API_D3D12:
        return "D3D12";
    case GRAPHICS_API_VULKAN:
        return "Vulkan";
    case GRAPHICS_API_D3D11:
        return "D3D11";
    case GRAPHICS_API_OPENGL:
        return "OpenGL";
    case GRAPHICS_API_AUTO:
        return "Auto";
    default:
        return "Unknown";
    }
}

void WriteCoreLog(ALog* debug, ALog::Level level, std::string_view message) {
    if (debug) debug->Write(level, "Argon.Core", message);
}

void WriteCoreApiLog(ALog* debug, ALog::Level level, std::string_view prefix, GraphicsApi api) {
    if (!debug) return;

    std::string message(prefix);
    message += GraphicsApiName(api);
    debug->Write(level, "Argon.Core", message);
}
} // namespace

namespace {
Result TryCreateDevice(GraphicsApi api, const DeviceDesc* desc, Device** out, ALog* debug) {
    switch (api) {
#if ENABLE_METAL
    case GRAPHICS_API_METAL:
        return Metal_CreateDevice(desc, out, debug);
#endif
#if ENABLE_D3D12
    case GRAPHICS_API_D3D12:
        return D3D12_CreateDevice(desc, out, debug);
#endif
#if ENABLE_VULKAN
    case GRAPHICS_API_VULKAN:
        return Vulkan_CreateDevice(desc, out, debug);
#endif
#if ENABLE_D3D11
    case GRAPHICS_API_D3D11:
        return D3D11_CreateDevice(desc, out, debug);
#endif
#if ENABLE_OPENGL
    case GRAPHICS_API_OPENGL:
        return OpenGL_CreateDevice(desc, out, debug);
#endif
    default:
        return Invalid_CreateDevice(desc, out, debug);
    }
}
} // namespace

Result CreateDevice(const DeviceDesc* desc, Device** out, ALog* debug) {
    if (!desc || !out) {
        WriteCoreLog(debug, ALog::Level::Error, "CreateDevice received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }

    *out = nullptr;

    if (desc->mPreferredApi != GRAPHICS_API_AUTO &&
        (desc->mPreferredApi == GRAPHICS_API_UNKNOWN || desc->mPreferredApi >= GRAPHICS_API_COUNT)) {
        WriteCoreLog(debug, ALog::Level::Error, "CreateDevice received an out-of-range preferred API");
        return RESULT_INVALID_ARGUMENT;
    }

    if (desc->mPreferredApi != GRAPHICS_API_AUTO) {
        WriteCoreApiLog(debug, ALog::Level::Debug, "Trying preferred backend ", desc->mPreferredApi);
        Result r = TryCreateDevice(desc->mPreferredApi, desc, out, debug);
        if (r == RESULT_SUCCESS) {
            WriteCoreApiLog(debug, ALog::Level::Info, "Created device with preferred backend ", desc->mPreferredApi);
            return RESULT_SUCCESS;
        }
        if (!desc->mAllowFallback) {
            WriteCoreApiLog(debug, ALog::Level::Error, "Preferred backend failed without fallback ",
                            desc->mPreferredApi);
            return r;
        }
        WriteCoreApiLog(debug, ALog::Level::Warning, "Preferred backend failed, falling back from ",
                        desc->mPreferredApi);
    }

    static constexpr GraphicsApi kApis[] = {
#if ENABLE_METAL
        GRAPHICS_API_METAL,
#endif
#if ENABLE_D3D12
        GRAPHICS_API_D3D12,
#endif
#if ENABLE_VULKAN
        GRAPHICS_API_VULKAN,
#endif
#if ENABLE_D3D11
        GRAPHICS_API_D3D11,
#endif
#if ENABLE_OPENGL
        GRAPHICS_API_OPENGL,
#endif
        GRAPHICS_API_UNKNOWN};

    for (GraphicsApi api : kApis) {
        if (api == GRAPHICS_API_UNKNOWN) {
            continue;
        }
        if (desc->mPreferredApi != GRAPHICS_API_AUTO && api == desc->mPreferredApi) {
            continue;
        }

        WriteCoreApiLog(debug, ALog::Level::Debug, "Trying fallback backend ", api);
        Result r = TryCreateDevice(api, desc, out, debug);
        if (r == RESULT_SUCCESS) {
            WriteCoreApiLog(debug, ALog::Level::Info, "Created device with backend ", api);
            return RESULT_SUCCESS;
        }
    }

    WriteCoreLog(debug, ALog::Level::Error, "CreateDevice could not create a device on any compiled backend");

    return RESULT_UNSUPPORTED_API;
}
