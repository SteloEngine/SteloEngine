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

#include <cstddef>
#include <cstring>

namespace Stelo {
    struct SmallString {
        char* _data;
        size_t _size;
        
        SmallString() : _data(nullptr), _size(0) {}
        SmallString(const char* str) : _size(strlen(str)) {
            _data = new char[_size + 1];
            strcpy(_data, str);
        }
        SmallString(const SmallString& other) : _data(other._data), _size(other._size) {}
        SmallString(SmallString&& other) : _data(other._data), _size(other._size) {
            other._data = nullptr;
            other._size = 0;
        }
        ~SmallString() {
            delete[] _data;
        }
    };
}
