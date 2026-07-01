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

#include <vector>
#include "DataStruct.hpp"

namespace Stelo {
    enum class Easing {
        Linear,
        EaseIn,
        EaseOut,
        EaseInOut,

        CubicIn,
        CubicOut,

        Elastic,
        Bounce,
        Back,

        SmoothStep,
        SmootherStep
    };
    using EasingFunc = float(*)(float);
    EasingFunc GetEasingFunc(Easing easing);

    enum class Property {
        Position,
        Rotation,
        Scale,
        Color,
        Opacity
    };
    struct KeyFrame {
        union {
            Point position;
            Point rotation;
            Size  scale;
            Color color;
            float opacity;
        } value;
        float time;

        Property property;
        EasingFunc easing;
    };
    enum LoopMode {
        Once,
        Loop,
        PingPong,
        Reverse,
        LoopReverse,
        /// LoopPerKey là các key khi hoàn thành việc của mình 
        /// sẽ lặp lại ngay lặp tức thay vì đợi các key khác trong cùng 1 animation rồi mới lặp lại
        LoopPerKey,
    };
    struct Animation {
        std::vector<KeyFrame> keyFrames;
        std::vector<KeyFrame> keyFramesActive;
        LoopMode loopMode;
        
        void Start();
        void Update(float deltaTime);
        void Stop();
    };
}