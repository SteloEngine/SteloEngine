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

#include "../../ThirdParty/RenderContext.hpp"
#include "../../ThirdParty/flat_hash_map.hpp"
#include "../Callback.hpp"

#include "DataStruct.hpp"
#include "Enums.hpp"

#include <cstdint>

namespace Stelo {
    class Widget {
    public:
        virtual ~Widget() = default;

        RenderContext GetRenderContext();
        Point GetPoint() const { return _point; }
        Point GetPointWithAlignment() const;
        Size GetSize() const { return _size; }
        Color GetBackground() const { return _background; }
        Color GetBorderColor() const { return _borderColor; }
        Margin GetMargin() const { return _margin; }
        Border GetBorder() const { return _border; }
        Widget* GetParent() const { return _parent; }

        void SetPoint(Point value);
        void SetSize(Size value);
        void SetBackground(Color value);
        void SetBorderColor(Color value);
        void SetMargin(Margin value);
        void SetBorder(Border value);
        void SetHandle(Handle handle);
        void SetParent(Widget* parent);

        HorizontalAlignment horizontalAlignment() const { return _horizontalAlignment; }
        VerticalAlignment verticalAlignment() const { return _verticalAlignment; }

        void AddChildren(Widget* children);
        void RemoveChildren(Widget* children);

        void Show(bool show);
        void Bind(Event _event, uint64_t _id, Callback<void(EventPtr)> _func);
        void Unbind(Event _event, uint64_t _id);
        void CallEvent(Event _event, EventPtr _ptr = 0);
        void Destroy();

        void LockAspectRatio(bool width, bool height);
        void LockWidthRatio(bool lock);
        void LockHeightRatio(bool lock);

        bool HasHandle() const { return _handle.handle != nullptr; }
    private:
        ska::flat_hash_map<Event, std::vector<std::pair<uint64_t, Callback<void(EventPtr)>>>> _eventRegistered;
        std::vector<Widget*> _children{};
        Widget* _parent = nullptr;
        Handle _handle;

        struct Texture* _textureBackground = nullptr;
        struct Texture* _textureBorder = nullptr;
        Color _background = {1, 1, 1, 1};
        Color _borderColor = {0, 0, 0, 0};
        Margin _margin = {0, 0, 0, 0};
        Border _border = {0, 0, 0, 0};
        /// Relative position
        /// or absolute position if parent is null
        Point _point = {0, 0};
        Size _size = {0, 0};
        AspectRatio _ratio = {1, 1};
        AspectLock _lock = {false, false};    
        HorizontalAlignment _horizontalAlignment = HorizontalAlignment::Center;
        VerticalAlignment _verticalAlignment = VerticalAlignment::Center;

        bool _isDirty = true;

        void DestroyHandle(Handle handle);
    };
}
