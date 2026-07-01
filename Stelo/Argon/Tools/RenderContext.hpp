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

#include <cstdint>

enum NativeViewType : uint8_t {
    NATIVE_VIEW_TYPE_UNKNOWN = 0,
    NATIVE_VIEW_TYPE_NS_WINDOW = 1,
    NATIVE_VIEW_TYPE_NS_VIEW = 2,
    NATIVE_VIEW_TYPE_UI_VIEW = 3,
    NATIVE_VIEW_TYPE_CAMETAL_LAYER = 4,
    NATIVE_VIEW_TYPE_WIN32_HWND = 5,
    NATIVE_VIEW_TYPE_XLIB_WINDOW = 6
};

enum NativeDisplayType : uint8_t {
    NATIVE_DISPLAY_TYPE_UNKNOWN = 0,
    NATIVE_DISPLAY_TYPE_NS_APPKIT = 1,
    NATIVE_DISPLAY_TYPE_UIKIT = 2,
    NATIVE_DISPLAY_TYPE_WIN32 = 3,
    NATIVE_DISPLAY_TYPE_XLIB = 4
};

struct RenderContext {
    const void* pNext = nullptr;
    void* pNativeView = nullptr;
    void* pNativeDisplay = nullptr;
    NativeViewType mNativeViewType = NATIVE_VIEW_TYPE_UNKNOWN;
    NativeDisplayType mNativeDisplayType = NATIVE_DISPLAY_TYPE_UNKNOWN;
};
