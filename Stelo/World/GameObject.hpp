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

#include "../ECS/Component.hpp"

#include <vector>

namespace Stelo {
class Transform;
class Scene;

struct GameObject {
    std::vector<Comp<CompObject>> _components;

    char* _name = nullptr;
    char* _tag = nullptr;
    
    Comp<Transform> _transform = {};
    Scene* _scene = {};

    uint32_t _layer = 0;
    uint32_t _sizeName;
    uint32_t _sizeTag;

    bool _isStatic = false, _isActive = true;
public:
    bool IsActive() const { return _isActive; }
    bool IsStatic() const { return _isStatic; }

    template<typename T>
    Comp<T> AddComponent();
    template<>
    Comp<Transform> AddComponent<Transform>();
    template<typename T>
    Comp<T> GetComponent() const;

    Comp<Transform> GetTransform() const { return _transform; }

    void EraseComponent(Comp<CompObject> component);
    void Destroy();
};

} // Stelo