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

#include "DataStruct.hpp"
#include "Widget.hpp"
#include "App.hpp"

namespace Stelo {
    class Window : public Widget {
    public:
        Window(const char* title, Point position, Size size, Widget* parent = nullptr) {
            _title = title;
            SetPoint(position);
            SetSize(size);
            Handle handle = CreateHandle();
            SetHandle(handle);
            App::RegisterWidget(this);
        }
        ~Window() = default;
        
        void SetTitle(const std::string& title) { _title = title; }
        std::string GetTitle() const { return _title; }
        
    private:
        std::string _title;
        Handle CreateHandle();
    };
}
