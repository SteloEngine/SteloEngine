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

#include "Type.hpp"
#include <cstdint>

namespace Stelo {
    struct KeyCode {
        enum Code : uint16_t {
            //0
            Null = 0, StartOfHeader, StartOfText, EndOfText, EndOfTransmission, Enquiry,
            Acknowledge, Bell, Backspace, Tab, LineFeed, VerticalTab, FormFeed, CarriageReturn,
            ShiftOut, ShiftIn, DataLinkEscape, DeviceControl1, DeviceControl2, DeviceControl3,
            DeviceControl4, NegativeAcknowledge, SynchronousIdle, EndOfTransmissionBlock,
            Cancel, EndOfMedium, Substitute, Escape, FileSeparator, GroupSeparator, RecordSeparator,
            UnitSeparator,
            //32
            Space, ExclamationMark, DoubleQuote, Hash, Dollar, Percent, Ampersand, SingleQuote,
            OpenParenthesis, CloseParenthesis, Asterisk, Plus, Comma, Hyphen, Period, Slash,
            Digit0, Digit1, Digit2, Digit3, Digit4, Digit5, Digit6, Digit7, Digit8, Digit9,
            Colon, Semicolon, LessThan, Equal, GreaterThan, QuestionMark,
            //64
            At,
            A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
            OpenBracket, Backslash, CloseBracket, Caret, Underscore, GraveAccent,
            AlphaA, AlphaB, AlphaC, AlphaD, AlphaE, AlphaF, AlphaG, AlphaH, AlphaI, AlphaJ, 
            AlphaK, AlphaL, AlphaM, AlphaN, AlphaO, AlphaP, AlphaQ, AlphaR, AlphaS, AlphaT, 
            AlphaU, AlphaV, AlphaW, AlphaX, AlphaY, AlphaZ,

            OpenBrace, VerticalBar, CloseBrace, Tilde, Delete,
            //128

            Count
        };
    };
    
    enum class HorizontalAlignment : uint8_t {
        Left, Right, Center, Stretch
    };
    enum class VerticalAlignment : uint8_t {
        Top, Bottom, Center, Stretch
    };

    enum class Event : uint16_t {
        None, Close, Destroy, Resize, Move, Click, TextChange
    };

    enum class PollType : uint8_t {
        App, Sandbox
    };

    enum class HandleType : uint8_t {
        Unknown = 0,
        NSWindow = 1,
        NSView = 2,
        UIView = 3,
        CametalLayer = 4,
        Win32Hwnd = 5,
        XlibWindow = 6
    };

    enum class AppContextType : uint8_t {
        Unknown = 0,
        NSAppKit = 1,
        UIKit = 2,
        Win32 = 3,
        Xlib = 4
    };
}
