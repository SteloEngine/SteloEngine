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

#include "Widget.hpp"

namespace Stelo {
    void Widget::LockAspectRatio(bool width, bool height) {
        _lock.height = height;
        _lock.width = width;
    }
    void Widget::LockWidthRatio(bool lock) {
        _lock.width = lock;
    }
    void Widget::LockHeightRatio(bool lock) {
        _lock.height = lock;
    }
    
    void Widget::SetPoint(Point value) {
        _point = value;
    }
    void Widget::SetSize(Size value) {
        _size = value;
    }
    void Widget::SetBackground(Color value) {
        _background = value;
    }
    void Widget::SetBorderColor(Color value) {
        _borderColor = value;
    }
    void Widget::SetMargin(Margin value) {
        _margin = value;
    }
    void Widget::SetBorder(Border value) {
        _border = value;
    }
    void Widget::SetHandle(Handle handle) {
        _handle = HasHandle() ? _handle : handle;
    }
    void Widget::SetParent(Widget* parent) {
        // logic not enough, add later
        _parent = parent;
    }

    void Widget::AddChildren(Widget* children) {
        if(!children) return;
        children->_parent = this;
        _children.push_back(children);
    }
    void Widget::RemoveChildren(Widget* children) {
        if(!children) return;
        if(!children->_parent) return;

        auto it = std::find(_children.begin(), _children.end(), children);
        if(it != _children.end()) _children.erase(it);
        children->_parent = nullptr;
    }
    
    void Widget::Bind(Event _event, uint64_t _id, Callback<void(EventPtr)> _func) {
        _eventRegistered[_event].push_back(std::make_pair(_id, _func));
    }
    void Widget::Unbind(Event _event, uint64_t _id) {
        auto it_e = _eventRegistered.find(_event);

        if(it_e != _eventRegistered.end()) {
            auto it = it_e->second.begin();
            while(it != it_e->second.end()) {
                if(it->first == _id) {
                    it_e->second.erase(it);
                    break;
                }
                ++it;
            }
        }
    }

    void Widget::CallEvent(Event _event, EventPtr _ptr) {
        auto it = _eventRegistered.find(_event);
        if(it != _eventRegistered.end()) for (auto &e : it->second) e.second(_ptr);
    }

    void Widget::Destroy() {
        while(!_children.empty()) _children.back()->Destroy();
        if(_parent) {
            auto rit = std::find(_parent->_children.begin(), _parent->_children.end(), this);
            if (rit != _parent->_children.end()) _parent->_children.erase(rit);
        }
        auto it = _eventRegistered.find(Event::Destroy);
        if (it != _eventRegistered.end()) for (auto &e : it->second) e.second(0);

        delete this;
    }
}