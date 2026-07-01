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

#include "../Enums.hpp"
#include "../DataStruct.hpp"
#include <cstdint>

namespace Stelo {
    struct KeyState {
        enum State : uint8_t {
            None, Up, Down, Hold, Count
        };
    };

    struct InputData {
        static KeyState::State inputState[KeyCode::Count];
        static bool isKeyUpdate[KeyCode::Count];
        
        static KeyState::State stateUp[KeyState::Count];
        static KeyState::State stateDown[KeyState::Count];

        static inline Point posMouse = Point{0, 0};
        static inline Point vecMouse = Point{0, 0};
    };
    
    void ResetAllKeyState();
    void UpdateKeyState(KeyCode::Code code);
    void UpdateMousePosition();
}