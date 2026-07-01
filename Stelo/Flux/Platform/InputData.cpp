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

#include "InputData.hpp"
#include "../Input.hpp"
#include <algorithm>
#include <cstring>
#include <iterator>

namespace Stelo {
    KeyState::State InputData::inputState[KeyCode::Count] = { KeyState::None };
    bool InputData::isKeyUpdate[KeyCode::Count] = { false };

    KeyState::State InputData::stateUp[KeyState::Count] {
        KeyState::None,
        KeyState::None,
        KeyState::Up,
        KeyState::Up
    };

    KeyState::State InputData::stateDown[KeyState::Count] {
        KeyState::Down,
        KeyState::Down,
        KeyState::Hold,
        KeyState::Hold
    };

    void ResetAllKeyState() {
        std::fill(std::begin(InputData::isKeyUpdate), std::end(InputData::isKeyUpdate), false);
    }

    bool Input::GetKey(KeyCode::Code code) {
        if(!InputData::isKeyUpdate[code]) UpdateKeyState(code);
        return InputData::inputState[code] >= KeyState::Down;
    }
    bool Input::GetKeyDown(KeyCode::Code code) {
        if(!InputData::isKeyUpdate[code]) UpdateKeyState(code);
        return InputData::inputState[code] == KeyState::Down;
    }
    bool Input::GetKeyHold(KeyCode::Code code) {
        if(!InputData::isKeyUpdate[code]) UpdateKeyState(code);
        return InputData::inputState[code] == KeyState::Hold;
    }
    bool Input::GetKeyUp(KeyCode::Code code) {
        if(!InputData::isKeyUpdate[code]) UpdateKeyState(code);
        return InputData::inputState[code] == KeyState::Up;
    }

    Point Input::GetMouseDeltaPosition() { return InputData::vecMouse; }
    Point Input::GetMousePosition() { return InputData::posMouse; }
}