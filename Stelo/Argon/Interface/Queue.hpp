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

#define ARGON_INTERNAL

#include "../Config.hpp"
#include "ALog.hpp"
#include "DeclareFunction.hpp"
#include "Enums.hpp"
#include "IUnknown.hpp"

struct QueueDesc {
    const void* pNext = nullptr;
    QueueType mType = QUEUE_TYPE_GRAPHICS;
    uint32_t mIndex = 0;
};

struct Queue : public IUnknown {
  private:
    DEFINE_GRAPHICS_API_TABLE(CreateCmd);
    DEFINE_GRAPHICS_API_TABLE(CreateCmdPool);
    DEFINE_GRAPHICS_API_TABLE(QueueSubmitCmd);
    DEFINE_GRAPHICS_API_TABLE(QueueSubmitCmds);
    DEFINE_GRAPHICS_API_TABLE(QueueSubmitCmdPool);
    DEFINE_GRAPHICS_API_TABLE(QueuePresent);

    DEFINE_GRAPHICS_API_TABLE(ReleaseQueue);

    QueueType mType = QUEUE_TYPE_GRAPHICS;

  protected:
    Queue(GraphicsApi api, const QueueDesc& desc) : IUnknown(api), mType(desc.mType) {}

  public:
    QueueType GetType() const {
        return mType;
    }
    Result CreateCmd(const CmdDesc* pDesc, Cmd** ppCmd, ALog* pLog = nullptr) {
        return _CreateCmd[GetIndexAPI()](this, pDesc, ppCmd, pLog);
    }
    Result CreateCmdPool(const CmdPoolDesc* pDesc, CmdPool** ppCmdPool, ALog* pLog = nullptr) {
        return _CreateCmdPool[GetIndexAPI()](this, pDesc, ppCmdPool, pLog);
    }
    Result Submit(Cmd* pCmd, ALog* pLog = nullptr) {
        return _QueueSubmitCmd[GetIndexAPI()](this, pCmd, pLog);
    }
    Result Submit(Cmd* const* ppCmds, uint32_t count, ALog* pLog = nullptr) {
        return _QueueSubmitCmds[GetIndexAPI()](this, ppCmds, count, pLog);
    }
    Result Submit(CmdPool* pCmdPool, ALog* pLog = nullptr) {
        return _QueueSubmitCmdPool[GetIndexAPI()](this, pCmdPool, pLog);
    }
    Result Present(SwapChain* pSwapChain, ALog* pLog = nullptr) {
        return _QueuePresent[GetIndexAPI()](this, pSwapChain, pLog);
    }
    void Release() {
        _ReleaseQueue[GetIndexAPI()](this);
    }
};

#undef ARGON_INTERNAL
