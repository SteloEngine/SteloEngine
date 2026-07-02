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

#include "GameObject.hpp"
#include "ECS/Comp.hpp"
#include "Transform.hpp"

namespace Stelo {
    template<>
    Comp<Transform> GameObject::AddComponent<Transform>() {
        if(_transform.GetID() != InvalidIndex) return _transform;
        GameObjectState ctx = _isActive ? (_isStatic ? GameObjectState::Static : GameObjectState::Active) : GameObjectState::Inactive;
        _transform = CompStorage<Transform>::CreateRaw(ctx);
        _components.push_back({_transform.GetID(), _transform.GetGeneration(), _transform.GetTypeID()});
        return _transform;
    }

    void GameObject::EraseComponent(Comp<CompObject> component) {
        if(!component.IsValid()) return;

        uint i = 0;
        for(; i < _components.size(); i++) {
            if(_components[i] == component) {
                component.GetUnsafePtr()->Destroy();
                std::erase(_components, component);
                return;
            }
        }
        
    }
    void GameObject::Destroy() {
        for(uint i = 0; i < _components.size(); i++) {
            _components[i]->Destroy();
        }
    }
}