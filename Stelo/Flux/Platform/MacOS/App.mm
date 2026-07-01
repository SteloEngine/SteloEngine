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

#include <Carbon/Carbon.h>
#include <cstdint>
#include "../../App.hpp"
#include "../InputData.hpp"
#import <Cocoa/Cocoa.h>

namespace Stelo {

Size App::GetScreenSize() {
    NSScreen* screen = [NSScreen mainScreen];
    NSRect frame = [screen frame];
    return {(float)frame.size.width, (float)frame.size.height};
}
void App::PollEvent(PollType type) {
    ResetAllKeyState();
    UpdateMousePosition();
    
    @autoreleasepool {
        NSEvent* event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                        untilDate:nil
                                            inMode:NSDefaultRunLoopMode
                                            dequeue:YES])) {
            [NSApp sendEvent:event];
        }
    }
}

static inline uint16_t ConvertToMacKey(KeyCode::Code code) {
    switch (code) {
        case KeyCode::Digit1: case KeyCode::ExclamationMark: return kVK_ANSI_1;
        case KeyCode::Digit2: case KeyCode::At:              return kVK_ANSI_2;
        case KeyCode::Digit3: case KeyCode::Hash:            return kVK_ANSI_3;
        case KeyCode::Digit4: case KeyCode::Dollar:          return kVK_ANSI_4;
        case KeyCode::Digit5: case KeyCode::Percent:         return kVK_ANSI_5;
        case KeyCode::Digit6: case KeyCode::Caret:           return kVK_ANSI_6;
        case KeyCode::Digit7: case KeyCode::Ampersand:       return kVK_ANSI_7;
        case KeyCode::Digit8: case KeyCode::Asterisk:        return kVK_ANSI_8;
        case KeyCode::Digit9: case KeyCode::OpenParenthesis: return kVK_ANSI_9;
        case KeyCode::Digit0: case KeyCode::CloseParenthesis:return kVK_ANSI_0;

        case KeyCode::A: case KeyCode::AlphaA: return kVK_ANSI_A;
        case KeyCode::B: case KeyCode::AlphaB: return kVK_ANSI_B;
        case KeyCode::C: case KeyCode::AlphaC: return kVK_ANSI_C;
        case KeyCode::D: case KeyCode::AlphaD: return kVK_ANSI_D;
        case KeyCode::E: case KeyCode::AlphaE: return kVK_ANSI_E;
        case KeyCode::F: case KeyCode::AlphaF: return kVK_ANSI_F;
        case KeyCode::G: case KeyCode::AlphaG: return kVK_ANSI_G;
        case KeyCode::H: case KeyCode::AlphaH: return kVK_ANSI_H;
        case KeyCode::I: case KeyCode::AlphaI: return kVK_ANSI_I;
        case KeyCode::J: case KeyCode::AlphaJ: return kVK_ANSI_J;
        case KeyCode::K: case KeyCode::AlphaK: return kVK_ANSI_K;
        case KeyCode::L: case KeyCode::AlphaL: return kVK_ANSI_L;
        case KeyCode::M: case KeyCode::AlphaM: return kVK_ANSI_M;
        case KeyCode::N: case KeyCode::AlphaN: return kVK_ANSI_N;
        case KeyCode::O: case KeyCode::AlphaO: return kVK_ANSI_O;
        case KeyCode::P: case KeyCode::AlphaP: return kVK_ANSI_P;
        case KeyCode::Q: case KeyCode::AlphaQ: return kVK_ANSI_Q;
        case KeyCode::R: case KeyCode::AlphaR: return kVK_ANSI_R;
        case KeyCode::S: case KeyCode::AlphaS: return kVK_ANSI_S;
        case KeyCode::T: case KeyCode::AlphaT: return kVK_ANSI_T;
        case KeyCode::U: case KeyCode::AlphaU: return kVK_ANSI_U;
        case KeyCode::V: case KeyCode::AlphaV: return kVK_ANSI_V;
        case KeyCode::W: case KeyCode::AlphaW: return kVK_ANSI_W;
        case KeyCode::X: case KeyCode::AlphaX: return kVK_ANSI_X;
        case KeyCode::Y: case KeyCode::AlphaY: return kVK_ANSI_Y;
        case KeyCode::Z: case KeyCode::AlphaZ: return kVK_ANSI_Z;

        case KeyCode::Hyphen:       case KeyCode::Underscore:  return kVK_ANSI_Minus;
        case KeyCode::Equal:        case KeyCode::Plus:        return kVK_ANSI_Equal;
        case KeyCode::OpenBracket:  case KeyCode::OpenBrace:   return kVK_ANSI_LeftBracket;
        case KeyCode::CloseBracket: case KeyCode::CloseBrace:  return kVK_ANSI_RightBracket;
        case KeyCode::Backslash:    case KeyCode::VerticalBar: return kVK_ANSI_Backslash;
        case KeyCode::Semicolon:    case KeyCode::Colon:       return kVK_ANSI_Semicolon;
        case KeyCode::SingleQuote:  case KeyCode::DoubleQuote: return kVK_ANSI_Quote;
        case KeyCode::Comma:        case KeyCode::LessThan:    return kVK_ANSI_Comma;
        case KeyCode::Period:       case KeyCode::GreaterThan: return kVK_ANSI_Period;
        case KeyCode::Slash:        case KeyCode::QuestionMark:return kVK_ANSI_Slash;
        case KeyCode::GraveAccent:  case KeyCode::Tilde:       return kVK_ANSI_Grave;

        case KeyCode::Space:          return kVK_Space;
        case KeyCode::Escape:         return kVK_Escape;
        case KeyCode::Tab:            return kVK_Tab;
        case KeyCode::CarriageReturn:
        case KeyCode::LineFeed:       return kVK_Return;

        case KeyCode::Backspace:      return kVK_Delete;
        case KeyCode::Delete:         return kVK_ForwardDelete;

        default: return 0xFFFF;
    }
}

enum class ShiftState { Required, Forbidden, Any };
ShiftState shiftRequir[KeyCode::Count];
uint16_t macKeyCode[KeyCode::Count];

static bool InitKeyMappings() {
    for (int i = 0; i < KeyCode::Count; ++i) macKeyCode[i] = 0xFFFF;
    for (int i = 0; i < KeyCode::Count; ++i) macKeyCode[i] = ConvertToMacKey(static_cast<KeyCode::Code>(i));

    for (int i = 0; i < KeyCode::Count; ++i) { shiftRequir[i] = ShiftState::Any; }
    for (int i = KeyCode::A; i <= KeyCode::Z; ++i) { shiftRequir[i] = ShiftState::Required; }
    for (int i = KeyCode::AlphaA; i <= KeyCode::AlphaZ; ++i) { shiftRequir[i] = ShiftState::Forbidden; }
    for (int i = KeyCode::Digit0; i <= KeyCode::Digit9; ++i) { shiftRequir[i] = ShiftState::Forbidden; }
    
    shiftRequir[KeyCode::ExclamationMark] = ShiftState::Required;
    shiftRequir[KeyCode::At] = ShiftState::Required;
    shiftRequir[KeyCode::Hash] = ShiftState::Required;
    shiftRequir[KeyCode::Dollar] = ShiftState::Required;
    shiftRequir[KeyCode::Percent] = ShiftState::Required;
    shiftRequir[KeyCode::Caret] = ShiftState::Required;
    shiftRequir[KeyCode::Ampersand] = ShiftState::Required;
    shiftRequir[KeyCode::Asterisk] = ShiftState::Required;
    shiftRequir[KeyCode::OpenParenthesis] = ShiftState::Required;
    shiftRequir[KeyCode::CloseParenthesis] = ShiftState::Required;
    shiftRequir[KeyCode::Underscore] = ShiftState::Required;
    shiftRequir[KeyCode::Plus] = ShiftState::Required;
    shiftRequir[KeyCode::OpenBrace] = ShiftState::Required;
    shiftRequir[KeyCode::CloseBrace] = ShiftState::Required;
    shiftRequir[KeyCode::VerticalBar] = ShiftState::Required;
    shiftRequir[KeyCode::Colon] = ShiftState::Required;
    shiftRequir[KeyCode::DoubleQuote] = ShiftState::Required;
    shiftRequir[KeyCode::LessThan] = ShiftState::Required;
    shiftRequir[KeyCode::GreaterThan] = ShiftState::Required;
    shiftRequir[KeyCode::QuestionMark] = ShiftState::Required;
    shiftRequir[KeyCode::Tilde] = ShiftState::Required;

    shiftRequir[KeyCode::Hyphen] = ShiftState::Forbidden;
    shiftRequir[KeyCode::Equal] = ShiftState::Forbidden;
    shiftRequir[KeyCode::OpenBracket] = ShiftState::Forbidden;
    shiftRequir[KeyCode::CloseBracket] = ShiftState::Forbidden;
    shiftRequir[KeyCode::Backslash] = ShiftState::Forbidden;
    shiftRequir[KeyCode::Semicolon] = ShiftState::Forbidden;
    shiftRequir[KeyCode::SingleQuote] = ShiftState::Forbidden;
    shiftRequir[KeyCode::Comma] = ShiftState::Forbidden;
    shiftRequir[KeyCode::Period] = ShiftState::Forbidden;
    shiftRequir[KeyCode::Slash] = ShiftState::Forbidden;
    shiftRequir[KeyCode::GraveAccent] = ShiftState::Forbidden;

    return true;
}

static inline bool IsMacShiftPressed() {
    return CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, kVK_Shift) ||
           CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, kVK_RightShift);
}

void UpdateKeyState(KeyCode::Code code) {
    static const bool init = InitKeyMappings();
    const uint16_t macKey = macKeyCode[code];
    if (macKey == 0xFFFF) return;

    bool isLogicalDown = false;

    if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, macKey)) {
        ShiftState shiftReq = shiftRequir[code];
        const bool shiftPressed = IsMacShiftPressed();
        if (shiftReq == ShiftState::Required) isLogicalDown = shiftPressed;
        else if (shiftReq == ShiftState::Forbidden) isLogicalDown = !shiftPressed;
        else isLogicalDown = true;
    }

    if (isLogicalDown) InputData::inputState[code] = InputData::stateDown[InputData::inputState[code]];
    else InputData::inputState[code] = InputData::stateUp[InputData::inputState[code]];
    InputData::isKeyUpdate[code] = true;
}

void UpdateMousePosition() {
    NSPoint p = [NSEvent mouseLocation];
    Point mousePos = Point(p.x, p.y);
    InputData::vecMouse = mousePos - InputData::posMouse;
    InputData::posMouse = mousePos;
}

}
