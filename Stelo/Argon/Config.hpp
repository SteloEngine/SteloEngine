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

/// @brief GRAPHICS_API_LIST
/// @note CRITICAL: Do NOT reorder, remove, or modify existing entries in this list. 
///       The index of these entries is used for serialization and array indexing.
///       Changing the order will break binary compatibility and runtime logic.
///       If you need to add a new API, append it to the end of the list.
#define GRAPHICS_API_LIST(X, api) \
    X(METAL, api)                 \
    X(D3D12, api)                 \
    X(VULKAN, api)                \
    X(D3D11, api)                 \
    X(OPENGL, api)

#if ENABLE_METAL
#define DECLARE_METAL_API(ret, api) ret Metal_##api;
#define MAP_API_METAL(api) &Metal_##api,
#else
#define DECLARE_METAL_API(ret, api)
#define MAP_API_METAL(api)
#endif

#if ENABLE_VULKAN
#define DECLARE_VULKAN_API(ret, api) ret Vulkan_##api;
#define MAP_API_VULKAN(api) &Vulkan_##api,

#else
#define DECLARE_VULKAN_API(ret, api)
#define MAP_API_VULKAN(api)
#endif

#if ENABLE_D3D11
#define DECLARE_D3D11_API(ret, api) ret D3D11_##api;
#define MAP_API_D3D11(api) &D3D11_##api,
#else
#define DECLARE_D3D11_API(ret, api)
#define MAP_API_D3D11(api)
#endif

#if ENABLE_D3D12
#define DECLARE_D3D12_API(ret, api) ret D3D12_##api;
#define MAP_API_D3D12(api) &D3D12_##api,
#else
#define DECLARE_D3D12_API(ret, api)
#define MAP_API_D3D12(api)
#endif

#if ENABLE_OPENGL
#define DECLARE_OPENGL_API(ret, api) ret OpenGL_##api;
#define MAP_API_OPENGL(api) &OpenGL_##api,
#else
#define DECLARE_OPENGL_API(ret, api)
#define MAP_API_OPENGL(api)
#endif
#define DECLARE_INVALID_API(ret, api) ret Invalid_##api;

#define DECLARE_GRAPHICS_API(ret, api)                                                                                 \
    DECLARE_METAL_API(ret, api)                                                                                        \
    DECLARE_VULKAN_API(ret, api)                                                                                       \
    DECLARE_D3D11_API(ret, api)                                                                                        \
    DECLARE_D3D12_API(ret, api)                                                                                        \
    DECLARE_OPENGL_API(ret, api)                                                                                       \
    DECLARE_INVALID_API(ret, api)

#define DISPATCH_MAP_GRAPHICS_API(api_name, api) MAP_API_##api_name(api)
#define DEFINE_GRAPHICS_API_TABLE(api)                                                                                 \
    using api##Func = decltype(&Invalid_##api);                                                                        \
    static constexpr api##Func _##api[] = {&Invalid_##api, GRAPHICS_API_LIST(DISPATCH_MAP_GRAPHICS_API, api)}
