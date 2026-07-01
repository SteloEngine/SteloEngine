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

#include "Queue.hpp"
#include "Cmd.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "MetalUtils.hpp"

Result Metal_CreateQueue(Device* device, const QueueDesc* desc, Queue** out, ALog* log) {
    if (out) *out = nullptr;
    if (device == nullptr || desc == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "CreateQueue received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (desc->mType != QUEUE_TYPE_GRAPHICS || desc->mIndex != 0) {
        WriteMetalLog(log, ALog::Level::Error, "Metal backend only supports one graphics pQueue in v0");
        return RESULT_UNSUPPORTED_API;
    }

    try {
        id<MTLCommandQueue> nativeQueue = [DEVICE(device)->pDevice newCommandQueue];
        if (nativeQueue == nil) {
            WriteMetalLog(log, ALog::Level::Error, "Failed to create Metal command pQueue");
            return RESULT_OUT_OF_DEVICE_MEMORY;
        }

        Metal_Queue* pQueue = new Metal_Queue(*desc);
        pQueue->pCommandQueue = nativeQueue;
        *out = pQueue;
        return RESULT_SUCCESS;
    } catch (const std::bad_alloc&) {
        WriteMetalLog(log, ALog::Level::Error, "Failed to allocate Metal pQueue wrapper");
        return RESULT_OUT_OF_HOST_MEMORY;
    }
}

void Metal_ReleaseQueue(Queue* pQueue) {
    if (pQueue == nullptr) return;

    Metal_Queue* metalQueue = QUEUE(pQueue);
    if (metalQueue->pCommandQueue != nil) {
        [metalQueue->pCommandQueue release];
        metalQueue->pCommandQueue = nil;
    }
    delete metalQueue;
}

Result Metal_QueueSubmitCmd(Queue* queue, Cmd* cmd, ALog* log) {
    if (queue == nullptr || cmd == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "QueueSubmitCmd received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (cmd->GetAPI() != GRAPHICS_API_METAL) {
        WriteMetalLog(log, ALog::Level::Error, "QueueSubmitCmd received command from a different backend");
        return RESULT_INVALID_ARGUMENT;
    }
    if (CMD(cmd)->pQueue != QUEUE(queue)) {
        WriteMetalLog(log, ALog::Level::Error, "QueueSubmitCmd received command from a different pQueue");
        return RESULT_INVALID_ARGUMENT;
    }

    [CMD(cmd)->pCommandBuffer commit];
    CMD(cmd)->SetBackendState(CMD_STATE_SUBMITTED);
    return RESULT_SUCCESS;
}

Result Metal_QueueSubmitCmds(Queue* queue, Cmd* const* cmds, uint32_t count, ALog* log) {
    if (queue == nullptr || cmds == nullptr || count == 0) {
        WriteMetalLog(log, ALog::Level::Error, "QueueSubmitCmds received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }

    for (uint32_t i = 0; i < count; ++i) {
        [CMD(cmds[i])->pCommandBuffer commit];
        CMD(cmds[i])->SetBackendState(CMD_STATE_SUBMITTED);
    }
    return RESULT_SUCCESS;
}

Result Metal_QueueSubmitCmdPool(Queue*, CmdPool*, ALog* log) {
    WriteMetalLog(log, ALog::Level::Warning, "CmdPool submission is not implemented for Metal v0");
    return RESULT_UNSUPPORTED_API;
}

Result Metal_QueuePresent(Queue* queue, SwapChain* swapChain, ALog* log) {
    if (queue == nullptr || swapChain == nullptr) {
        WriteMetalLog(log, ALog::Level::Error, "QueuePresent received invalid arguments");
        return RESULT_INVALID_ARGUMENT;
    }
    if (swapChain->GetAPI() != GRAPHICS_API_METAL) {
        WriteMetalLog(log, ALog::Level::Error, "QueuePresent received swap chain from a different backend");
        return RESULT_INVALID_ARGUMENT;
    }

    Metal_SwapChain* metalSwapChain = SWAPCHAIN(swapChain);
    Metal_Queue* metalQueue = QUEUE(queue);
    id<MTLCommandBuffer> commandBuffer = [metalQueue->pCommandQueue commandBuffer]; 
    [commandBuffer presentDrawable:metalSwapChain->pCurrentDrawable];
    [commandBuffer commit];
    [commandBuffer release];

    return RESULT_SUCCESS;
}

#endif
