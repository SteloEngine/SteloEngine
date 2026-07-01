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

#define ARGON_INTERNAL

#include "ALog.hpp"
#include "DeclareFunction.hpp"
#include "IUnknown.hpp"

struct CmdPoolDesc {
    const void* pNext = nullptr;
    const char* pDebugName = nullptr;
    uint32_t mInitialCmdCount = 0;
    bool mResetCmdOnAcquire = true;
};

struct CmdPool : public IUnknown {
  private:
    DEFINE_GRAPHICS_API_TABLE(CmdPoolAcquireCmd);
    DEFINE_GRAPHICS_API_TABLE(ReleaseCmdPool);

  protected:
    CmdPool(GraphicsApi api) : IUnknown(api) {}

  public:
    Result AcquireCmd(Cmd** ppCmd, ALog* pLog = nullptr) {
        return _CmdPoolAcquireCmd[GetIndexAPI()](this, ppCmd, pLog);
    }
    void Release() {
        _ReleaseCmdPool[GetIndexAPI()](this);
    }
};

#undef ARGON_INTERNAL
