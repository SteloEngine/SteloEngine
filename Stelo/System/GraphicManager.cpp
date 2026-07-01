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

#include "GraphicManager.hpp"

#include <iostream>

namespace Stelo {
namespace {
constexpr float kRenderTimeStep = 1.0f / 60.0f;
const char* ResultName(Result result) {
    switch (result) {
        case RESULT_SUCCESS: return "RESULT_SUCCESS";
        case RESULT_INVALID_ARGUMENT: return "RESULT_INVALID_ARGUMENT";
        case RESULT_UNSUPPORTED_API: return "RESULT_UNSUPPORTED_API";
        case RESULT_OUT_OF_HOST_MEMORY: return "RESULT_OUT_OF_HOST_MEMORY";
        case RESULT_OUT_OF_DEVICE_MEMORY: return "RESULT_OUT_OF_DEVICE_MEMORY";
        case RESULT_ALREADY_EXISTS: return "RESULT_ALREADY_EXISTS";
        case RESULT_NOT_FOUND: return "RESULT_NOT_FOUND";
        case RESULT_NOT_READY: return "RESULT_NOT_READY";
        case RESULT_INVALID_STATE: return "RESULT_INVALID_STATE";
        case RESULT_INVALID_ACCESS: return "RESULT_INVALID_ACCESS";
        case RESULT_UNKNOWN_ERROR: return "RESULT_UNKNOWN_ERROR";
    }
    return "RESULT_UNKNOWN";
}
bool Check(Result result, const char* operation, ALog& log) {
    if (result == RESULT_SUCCESS) {
        if (log.Size() != 0) {
            std::cout << log.ToString();
            log.Reset();
        }
        return true;
    }

    std::cerr << operation << " failed: " << ResultName(result) << '\n';
    if (log.Size() != 0) {
        std::cerr << log.ToString();
        log.Reset();
    }
    return false;
}
}

void GraphicManager::Initialize(Widget* target) {
    _target = target;
    RenderContext renderContext = target->GetRenderContext();
    if (renderContext.pNativeView == nullptr) return;

    DeviceDesc deviceDesc{.mPreferredApi = GRAPHICS_API_AUTO};
    if (!Check(CreateDevice(&deviceDesc, &_device, &_log), "CreateDevice", _log)) return;

    QueueDesc queueDesc{.mType = QUEUE_TYPE_GRAPHICS};
    if (!Check(_device->CreateQueue(&queueDesc, &_queue, &_log), "CreateQueue", _log)) return;

    CmdDesc cmdDesc = {};
    if (!Check(_queue->CreateCmd(&cmdDesc, &_cmd, &_log), "CreateCmd", _log)) return;

    SwapChainDesc swapChainDesc{
        .pContext = &renderContext,
        .mWidth = static_cast<uint32_t>(target->GetSize().width),
        .mHeight = static_cast<uint32_t>(target->GetSize().height),
        .mFormat = FORMAT_BGRA8_UNORM
    };
    Check(_device->CreateSwapChain(&swapChainDesc, &_swapChain, &_log), "CreateSwapChain", _log);
}
void GraphicManager::Shutdown() {
    if (_swapChain) {
        _swapChain->Release();
        _swapChain = nullptr;
    }
    if (_cmd) {
        _cmd->Release();
        _cmd = nullptr;
    }
    if (_queue) {
        _queue->Release();
        _queue = nullptr;
    }
    if (_device) {
        _device->Release();
        _device = nullptr;
    }
}
}