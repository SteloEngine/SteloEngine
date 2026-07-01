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

#pragma once

#include "../Argon/Argon.hpp"
#include "../Flux/Flux.hpp"

namespace Stelo {

struct GraphicManager {
private:
    static inline Device* _device = nullptr;
    static inline Queue* _queue = nullptr;
    static inline Cmd* _cmd  = nullptr;
    static inline SwapChain* _swapChain = nullptr;
    static inline ALog _log;
    static inline Widget* _target = nullptr;
public:
    static void Initialize(Widget* target);
    static void Shutdown();
    static Device* GetDevice() { return _device; }
    static Queue* Queue() { return _queue; }
    static Cmd* Cmd() { return _cmd; }
    static Widget* GetTarget() { return _target; }
    static SwapChain* GetSwapChain() { return _swapChain; }
};

}
