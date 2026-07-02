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

#include "DeclareTypes.hpp"
#include "Comp.hpp"
#include <cstdint>

namespace Stelo {

struct alignas(8) CompInfo {
    constexpr static uint16_t STATE_CONTEXT_STATE_MASK = 0b11;
    constexpr static uint16_t STATE_GAME_OBJECT_STATE_MASK = 0b1100;

    constexpr static uint16_t STATE_SELF_ACTIVE_MASK = 1 << 4;
    constexpr static uint16_t STATE_SELF_DESTROY_MASK = 1 << 5;
    constexpr static uint16_t STATE_SELF_STATIC_MASK = 1 << 6;
    constexpr static uint16_t STATE_SELF_PENDING_MASK = 1 << 7;

    uint32_t id = InvalidIndex;
    uint16_t state = 0;
    uint16_t typeID = 0xffff;

    CompInfo() = default;
    CompInfo(uint32_t id, uint16_t state, uint16_t typeID) : id(id), state(state), typeID(typeID) {}
    
    bool IsSelfActive() const { return (state & STATE_SELF_ACTIVE_MASK) != 0; }
    bool IsSelfDestroyed() const { return (state & STATE_SELF_DESTROY_MASK) != 0; }
    bool IsSelfStatic() const { return (state & STATE_SELF_STATIC_MASK) != 0; }
    bool IsSelfPending() const { return (state & STATE_SELF_PENDING_MASK) != 0; }

    ContextState GetContextState() const { 
        return static_cast<ContextState>(state & STATE_CONTEXT_STATE_MASK); 
    }
    void SetContextState(ContextState stateValue) { 
        state = (state & ~STATE_CONTEXT_STATE_MASK) | static_cast<uint8_t>(stateValue); 
    }

    GameObjectState GetGameObjectState() const {
        return static_cast<GameObjectState>((state & STATE_GAME_OBJECT_STATE_MASK) >> 2);
    }
    void SetGameObjectState(GameObjectState stateValue) { 
        state = (state & ~STATE_GAME_OBJECT_STATE_MASK) | (static_cast<uint8_t>(stateValue) << 2);
    }
    
    void SetSelfActive(bool value) { if (value) state |= STATE_SELF_ACTIVE_MASK; else state &= ~STATE_SELF_ACTIVE_MASK; }
    void SetSelfDestroyed(bool value) { if (value) state |= STATE_SELF_DESTROY_MASK; else state &= ~STATE_SELF_DESTROY_MASK; }
    void SetSelfStatic(bool value) { if (value) state |= STATE_SELF_STATIC_MASK; else state &= ~STATE_SELF_STATIC_MASK; }
    void SetSelfPending(bool value) { if (value) state |= STATE_SELF_PENDING_MASK; else state &= ~STATE_SELF_PENDING_MASK; }

    uint16_t GetTypeID() const { return typeID; }
    uint32_t GetID() const { return id; }
};

template<typename T>
bool RegisterComponent();
#define REGISTER_COMPONENT(T) namespace { const bool _register##T = RegisterComponent<T>(); }

struct CompObject {
private:
    CompInfo _info = CompInfo();
public:
    virtual ~CompObject() = default;

    void InternalSetCompInfo(CompInfo info) { _info = info; }

    CompInfo GetCompInfo() const { return _info; }
    uint16_t GetTypeID() const { return _info.GetTypeID(); }
    uint32_t GetID() const { return _info.GetID(); }

    bool IsActive() const { return _info.IsSelfActive(); }
    bool IsDestroyed() const { return _info.IsSelfDestroyed(); }
    bool IsStatic() const { return _info.IsSelfStatic(); }
    
    GameObjectState GetGameObjectState() const { return _info.GetContextState(); }
    void SetGameObjectState(GameObjectState value) { _info.SetContextState(value); }

    bool IsValidForStart() const { return _info.GetContextState() != ContextState::Destroy; }
    bool IsValidForUpdate() const { return _info.GetContextState() == ContextState::Active; }
    bool IsValidForRender() const { return _info.GetContextState() <= ContextState::Static; }

    void SetActive(bool value);
    void SetStatic(bool value);

    Comp<CompObject> GetHandle();

    void Destroy();
};

struct Component : public CompObject{
private:
    GameObject* _gameObject = nullptr;
    Comp<Transform> _transform = nullptr;
public:
    void InternalSetGameObject(GameObject* gameObject) { _gameObject = gameObject; }
    void InternalSetTransform(Comp<Transform> transform) { _transform = transform; }

    Comp<Transform> GetTransform() const { return _transform; }
    GameObject* GetGameObject() const { return _gameObject; }

    void ChangeGameObject(GameObject* gameObject);
};

}