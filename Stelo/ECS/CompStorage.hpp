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

#include "../World/GameObject.hpp"
#include "../Time.hpp"

#include "Comp.hpp"
#include "Component.hpp"
#include "CompTraits.hpp"
#include <cstddef>

namespace Stelo {

#ifdef DEBUG
#define VALID_PARTItION(T) assert((_activeEnd <= _staticEnd && _staticEnd <= _components.size()) && "Invalid component storage state");
#else
#define VALID_PARTItION(T)
#endif

template<typename C>
struct CompStorageConfig {
    static constexpr uint32_t InitialPackCount = 4;
    /// size pack = 2^PackSizeShift;
    static constexpr uint8_t PackSizeShift = 6;
    static constexpr bool IsInstance = false;

    using HookCallback = void(*)(uint32_t begin, uint32_t end, C* data);

    static constexpr HookCallback Start = nullptr;

    static constexpr HookCallback PreFixedUpdate = nullptr;
    static constexpr HookCallback FixedUpdate = nullptr;
    
    static constexpr HookCallback PreUpdate = nullptr;
    static constexpr HookCallback Update = nullptr;
    static constexpr HookCallback PostUpdate = nullptr;

    static constexpr HookCallback PreRender = nullptr;
    static constexpr HookCallback Render = nullptr;
    static constexpr HookCallback PostRender = nullptr;
};

template<typename C, bool isInstance = CompStorageConfig<C>::IsInstance>
struct CompStorage;

template<typename C>
struct CompStorage<C, false> {
    static_assert(std::is_base_of_v<Component, C>, "C must be derived from Component");

    using Pack = std::array<C, 1 << CompStorageConfig<C>::PackSizeShift>;

    static constexpr uint32_t PackSize = 1 << CompStorageConfig<C>::PackSizeShift;
    static constexpr uint32_t PackSizeMask = PackSize - 1;
    static constexpr uint32_t PackSizeShift = CompStorageConfig<C>::PackSizeShift;
    static constexpr uint32_t InitialPackCount = CompStorageConfig<C>::InitialPackCount;

    static inline std::vector<Pack*> _components = {};
    static inline std::vector<CompIndex> _sparse = {};
    static inline std::vector<uint32_t> _stateMoveCode = {};
    static inline std::vector<std::pair<uint32_t, CompMoveCode>> _moveCode = {};
    static inline std::vector<uint32_t> _freeId = {};

    /// @brief _activeEnd to _staticEnd is static components
    /// @note before _activeEnd is active components
    static inline uint32_t _activeEnd = 0;
    /// @brief _staticEnd to end is inactive components
    static inline uint32_t _staticEnd = 0;
    /// @brief _inactiveEnd to end is pending adding components
    static inline uint32_t _inactiveEnd = 0;
    /// @brief _size to capacity is destroy components
    static inline uint32_t _size = 0;
    /// @brief Type ID for this component type
    static inline uint16_t _typeId = 0;
    static inline uint32_t _nextId = 0;

    static inline bool _executeMoveActionRegistered = false;
    static inline bool _startRegistered = false;

    static inline C& GetCompRef(uint32_t physicalIdx) {
        return (*_components[physicalIdx >> PackSizeShift])[physicalIdx & PackSizeMask];
    }
    static void UpdateView() {
        CompStorageView view;
        view.memPack = reinterpret_cast<void**>(_components.data());
        view.sparse = _sparse.data();
        view.sizeSparse = static_cast<uint32_t>(_sparse.size());
        view.sizeComponent = static_cast<uint32_t>(sizeof(C));
        view.sizePackMask = PackSizeMask;
        view.PackSizeShift = PackSizeShift;
        
        CompManager::UpdateView(view, _typeId);
    }
    
    static inline void Swap(uint32_t physicalA, uint32_t physicalB) {
        if (physicalA == physicalB) return;

        C& compA = GetCompRef(physicalA);
        C& compB = GetCompRef(physicalB);

        const uint32_t idA = compA.GetID();
        const uint32_t idB = compB.GetID();

        std::swap(compA, compB);

        _sparse[idA].index = physicalB;
        _sparse[idB].index = physicalA;
    }

    static bool Initial() {
        if constexpr (HasPreFixedUpdate<C>) CompManager::RegisterPreFixedUpdate(PreFixedUpdate);
        if constexpr (HasPreUpdate<C>) CompManager::RegisterPreUpdate(PreUpdate);
        if constexpr (HasUpdate<C>) CompManager::RegisterUpdate(Update);
        if constexpr (HasPostUpdate<C>) CompManager::RegisterPostUpdate(PostUpdate);

        if constexpr (HasPreRender<C>) CompManager::RegisterPreRender(PreRender);
        if constexpr (HasRender<C>) CompManager::RegisterRender(Render);
        if constexpr (HasPostRender<C>) CompManager::RegisterPostRender(PostRender);

        for (uint32_t i = 0; i < InitialPackCount; ++i) _components.push_back(new Pack());
        _sparse.resize(InitialPackCount * PackSize);
        _stateMoveCode.resize(InitialPackCount * PackSize);
        UpdateView();
        return true;
    }
    static bool Register() {
        _typeId = CompTypeManager::Register<C>();
        CompManager::RegisterCallByID(ShutDown, SetComp, Create, _typeId);
        return true;
    }
    static bool Unregister() {
        if constexpr (HasStart<C>) CompManager::UnregisterStart(Start);
        if constexpr (HasFixedUpdate<C>) CompManager::UnregisterFixedUpdate(FixedUpdate);
        if constexpr (HasPreUpdate<C>) CompManager::UnregisterPreUpdate(PreUpdate);
        if constexpr (HasUpdate<C>) CompManager::UnregisterUpdate(Update);
        if constexpr (HasPostUpdate<C>) CompManager::UnregisterPostUpdate(PostUpdate);

        if constexpr (HasPreRender<C>) CompManager::UnregisterPreRender(PreRender);
        if constexpr (HasRender<C>) CompManager::UnregisterRender(Render);
        if constexpr (HasPostRender<C>) CompManager::UnregisterPostRender(PostRender);
        
        CompManager::UnregisterCallByID(_typeId);

        return true;
    }

    static Comp<C> CreateRaw(GameObjectState ctx) {
        static bool initial = Initial();
        if(!_executeMoveActionRegistered) {
            CompManager::QueueExecuteMoveAction(ExecuteMoveAction);
            _executeMoveActionRegistered = true;
        }
        if constexpr (HasStart<C>) {
            if(!_startRegistered) {
                CompManager::QueueStart(Start);
                _startRegistered = true;
            }
        }

        bool updateView = false;

        uint32_t id;
        if (!_freeId.empty()) {
            id = _freeId.back();
            _freeId.pop_back();
        } else {
            id = _nextId++;
            if (id >= _sparse.size()) {
                size_t newSize = _sparse.size() == 0 ? id + 1 : _sparse.size() * 2;
                _sparse.resize(newSize);
                _stateMoveCode.resize(_sparse.size());
                updateView = true;
            }
        }
        
        const uint32_t physicIndex = _size++;
        const uint32_t packIndex = physicIndex >> PackSizeShift;
        const uint32_t packOffset = physicIndex & PackSizeMask;
        const uint32_t lastSize = _components.size();
        if(packIndex >= lastSize) {
            _components.resize(packIndex + 1);
            const uint32_t newSize = _components.size();
            for(uint32_t i = lastSize; i < newSize; ++i)  _components[i] = new Pack();
            updateView = true;
        }
        
        CompInfo info(id, 0, _typeId);
        CompIndex& sparseIndex = _sparse[id];
        sparseIndex.index = physicIndex;
        sparseIndex.typeID = _typeId;

        info.SetSelfPending(true);
        info.SetSelfActive(true);
        info.SetSelfStatic(false);
        info.SetSelfDestroyed(false);

        info.SetContextState(ctx);
        info.SetGameObjectState(ctx);

        C& ptr = (*_components[packIndex])[packOffset];
        ptr.InternalSetCompInfo(info);

        _stateMoveCode[id] = _moveCode.size();
        _moveCode.push_back({id, ContextStateToMoveCode(ctx)});

        if(updateView) UpdateView();
        return Comp<C>(id, _sparse[id].generation, _typeId);
    }
    static Comp<CompObject> Create(GameObjectState ctx) {
        const Comp<C> comp = CreateRaw(ctx);
        return Comp<CompObject>(comp.GetID(), comp.GetGeneration(), comp.GetTypeID());
    }
    
    static void SetComp(CompInfo& info, CompSetCode code, bool value) {
        if(info.GetID() >= _nextId) return;

        if (!_executeMoveActionRegistered) {
            _executeMoveActionRegistered = true;
            CompManager::QueueExecuteMoveAction(ExecuteMoveAction);
        }

        CompMoveCode mvCode;

        switch (code) {
            case CompSetCode::SetActive: {
                info.SetSelfActive(value);
                if (value) {
                    if (info.IsSelfStatic()) {
                        info.SetContextState(ContextState::Static);
                        mvCode = CompMoveCode::MoveToStatic;
                    } else {
                        info.SetContextState(ContextState::Active);
                        mvCode = CompMoveCode::MoveToActive;
                    }
                    if constexpr (HasOnEnable<C>) GetCompRef(info.GetID()).OnEnable();
                } else {
                    info.SetContextState(ContextState::Inactive);
                    mvCode = CompMoveCode::MoveToInactive;
                    if constexpr (HasOnDisable<C>) GetCompRef(info.GetID()).OnDisable();
                }
                break;
            }
            
            case CompSetCode::SetStatic: {
                info.SetSelfStatic(value);
                if (value) {
                    info.SetContextState(ContextState::Static);
                    mvCode = CompMoveCode::MoveToStatic;
                } else {
                    info.SetContextState(ContextState::Active);
                    mvCode = CompMoveCode::MoveToActive;
                }
                break;
            }
            
            case CompSetCode::SetDestroy: {
                info.SetContextState(ContextState::Destroy);
                if constexpr (HasOnDestroy<C>) GetCompRef(info.GetID()).OnDestroy();
                mvCode = CompMoveCode::MoveToDestroy;
                break;
            }
        }

        if(_stateMoveCode[info.GetID()] != InvalidIndex) {
            uint32_t index = _stateMoveCode[info.GetID()];
            _moveCode[index] = {info.GetID(), mvCode};
        } else {
            _stateMoveCode[info.GetID()] = static_cast<uint32_t>(_moveCode.size());
            _moveCode.push_back({info.GetID(), mvCode});
        }
    }

    static void Start() {
        _startRegistered = false;
        if (_inactiveEnd == _size) return;

        if constexpr (CompStorageConfig<C>::Start != nullptr) {
            uint32_t current = _inactiveEnd;
            const uint32_t startOffset = current & PackSizeMask;
            if (startOffset != 0) {
                const uint32_t countInPack = std::min(PackSize - startOffset, _size - current);
                CompStorageConfig<C>::Start(startOffset, countInPack, _components[current >> PackSizeShift]->data());
                current += countInPack;
            }
            const uint32_t endPack = _size >> PackSizeShift;            
            for (uint32_t i = current >> PackSizeShift; i < endPack; ++i) {
                CompStorageConfig<C>::Start(0, PackSize, _components[i]->data());
                current += PackSize;
            }

            if (current < _size) CompStorageConfig<C>::Start(0, _size - current, _components[current >> PackSizeShift]->data());
        }
        else {
            for (uint32_t i = _inactiveEnd; i < _size; ++i) {
                Pack& packRef = *_components[i >> PackSizeShift];            
                C& comp = packRef[i & PackSizeMask];
                if (comp.IsValidForStart()) comp.Start(); 
            }
        }
    }
    static void PreFixedUpdate() {
        if (_activeEnd == 0) return;
        if constexpr (CompStorageConfig<C>::PreFixedUpdate != nullptr) {
            const uint32_t endPack = _activeEnd >> CompStorageConfig<C>::PackSizeShift;
            for(uint32_t i = 0; i < endPack; ++i) {
                CompStorageConfig<C>::PreFixedUpdate(0, PackSize, _components[i]->data());
            }
            const uint32_t sizeLastPack = _activeEnd & PackSizeMask;
            if (sizeLastPack != 0) CompStorageConfig<C>::PreFixedUpdate(0, sizeLastPack, _components[endPack]->data());
        }
        else {
            for (uint32_t i = 0; i < _activeEnd; ++i) {
                Pack& packRef = *_components[i >> PackSizeShift];            
                C& comp = packRef[i & PackSizeMask];
                if (comp.IsValidForUpdate()) comp.PreFixedUpdate(Time::fixedTime);
            }
        }
    }
    static void FixedUpdate() {
        if (_activeEnd == 0) return;
        if constexpr (CompStorageConfig<C>::FixedUpdate != nullptr) {
            const uint32_t endPack = _activeEnd >> CompStorageConfig<C>::PackSizeShift;
            for(uint32_t i = 0; i < endPack; ++i) {
                CompStorageConfig<C>::FixedUpdate(0, PackSize, _components[i]->data());
            }
            const uint32_t sizeLastPack = _activeEnd & PackSizeMask;
            if (sizeLastPack != 0) CompStorageConfig<C>::FixedUpdate(0, sizeLastPack, _components[endPack]->data());
        }
        else {
            for (uint32_t i = 0; i < _activeEnd; ++i) {
                Pack& packRef = *_components[i >> PackSizeShift];            
                C& comp = packRef[i & PackSizeMask];
                if (comp.IsValidForUpdate()) comp.FixedUpdate(Time::fixedTime);
            }
        }
    }
    static void PreUpdate() {
        if (_activeEnd == 0) return;
        if constexpr (CompStorageConfig<C>::PreUpdate != nullptr) {
            const uint32_t endPack = _activeEnd >> CompStorageConfig<C>::PackSizeShift;
            for(uint32_t i = 0; i < endPack; ++i) {
                CompStorageConfig<C>::PreUpdate(0, PackSize, _components[i]->data());
            }
            const uint32_t sizeLastPack = _activeEnd & PackSizeMask;
            if (sizeLastPack != 0) CompStorageConfig<C>::PreUpdate(0, sizeLastPack, _components[endPack]->data());
        }
        else {
            for (uint32_t i = 0; i < _activeEnd; ++i) {
                Pack& packRef = *_components[i >> PackSizeShift];            
                C& comp = packRef[i & PackSizeMask];
                if (comp.IsValidForUpdate()) comp.PreUpdate(Time::logicTime);
            }
        }
    }
    static void Update() {
        if (_activeEnd == 0) return;
        if constexpr (CompStorageConfig<C>::Update != nullptr) {
            const uint32_t endPack = _activeEnd >> CompStorageConfig<C>::PackSizeShift;
            for(uint32_t i = 0; i < endPack; ++i) {
                CompStorageConfig<C>::Update(0, PackSize, _components[i]->data());
            }
            const uint32_t sizeLastPack = _activeEnd & PackSizeMask;
            if (sizeLastPack != 0) CompStorageConfig<C>::Update(0, sizeLastPack, _components[endPack]->data());
        }
        else {
            for (uint32_t i = 0; i < _activeEnd; ++i) {
                C& comp = (*_components[i >> PackSizeShift])[i & PackSizeMask];
                if (comp.IsValidForUpdate()) comp.Update(Time::logicTime);
            }
        }
    }
    static void PostUpdate() {
        if (_activeEnd == 0) return;
        if constexpr (CompStorageConfig<C>::PostUpdate != nullptr) {
            const uint32_t endPack = _activeEnd >> CompStorageConfig<C>::PackSizeShift;
            for(uint32_t i = 0; i < endPack; ++i) {
                CompStorageConfig<C>::PostUpdate(0, PackSize, _components[i]->data());
            }
            const uint32_t sizeLastPack = _activeEnd & PackSizeMask;
            if (sizeLastPack != 0) CompStorageConfig<C>::PostUpdate(0, sizeLastPack, _components[endPack]->data());
        }
        else {
            for (uint32_t i = 0; i < _activeEnd; ++i) {
                Pack& packRef = *_components[i >> PackSizeShift];            
                C& comp = packRef[i & PackSizeMask];
                if (comp.IsValidForUpdate()) comp.PostUpdate(Time::logicTime);
            }
        }
    }
    static void PreRender() {
        if (_staticEnd == 0) return;
        if constexpr (CompStorageConfig<C>::PreRender != nullptr) {
            const uint32_t endPack = _staticEnd >> CompStorageConfig<C>::PackSizeShift;
            for(uint32_t i = 0; i < endPack; ++i) {
                CompStorageConfig<C>::PreRender(0, PackSize, _components[i]->data());
            }
            const uint32_t sizeLastPack = _staticEnd & PackSizeMask;
            if (sizeLastPack != 0) CompStorageConfig<C>::PreRender(0, sizeLastPack, _components[endPack]->data());
        }
        else {
            for (uint32_t i = 0; i < _staticEnd; ++i) {
                Pack& packRef = *_components[i >> PackSizeShift];            
                C& comp = packRef[i & PackSizeMask];
                if (comp.IsValidForRender()) comp.PreRender();
            }
        }
    }
    static void Render() {
        if (_staticEnd == 0) return;
        if constexpr (CompStorageConfig<C>::Render != nullptr) {
            const uint32_t endPack = _staticEnd >> CompStorageConfig<C>::PackSizeShift;
            for(uint32_t i = 0; i < endPack; ++i) {
                CompStorageConfig<C>::Render(0, PackSize, _components[i]->data());
            }
            const uint32_t sizeLastPack = _staticEnd & PackSizeMask;
            if (sizeLastPack != 0) CompStorageConfig<C>::Render(0, sizeLastPack, _components[endPack]->data());
        }
        else {
            for (uint32_t i = 0; i < _staticEnd; ++i) {
                Pack& packRef = *_components[i >> PackSizeShift];            
                C& comp = packRef[i & PackSizeMask];
                if (comp.IsValidForRender()) comp.Render();
            }
        }
    }
    static void PostRender() {
        if (_staticEnd == 0) return;
        if constexpr (CompStorageConfig<C>::PostRender != nullptr) {
            const uint32_t endPack = _staticEnd >> CompStorageConfig<C>::PackSizeShift;
            for(uint32_t i = 0; i < endPack; ++i) {
                CompStorageConfig<C>::PostRender(0, PackSize, _components[i]->data());
            }
            const uint32_t sizeLastPack = _staticEnd & PackSizeMask;
            if (sizeLastPack != 0) CompStorageConfig<C>::PostRender(0, sizeLastPack, _components[endPack]->data());
        }
        else {
            for (uint32_t i = 0; i < _staticEnd; ++i) {
                Pack& packRef = *_components[i >> PackSizeShift];            
                C& comp = packRef[i & PackSizeMask];
                if (comp.IsValidForRender()) comp.PostRender();
            }
        }
    } 
    
    static void ExecuteMoveAction() {
        _executeMoveActionRegistered = false;
        for (const auto& [id, moveCode] : _moveCode) {
            uint32_t idx = _sparse[id].index;
            _stateMoveCode[id] = InvalidIndex;

            switch (moveCode) {
                case CompMoveCode::MoveToActive: {
                    if (idx >= _inactiveEnd) {
                        Swap(idx, _inactiveEnd);
                        idx = _inactiveEnd; 
                        ++_inactiveEnd;
                    }
                    if (idx >= _staticEnd) {
                        Swap(idx, _staticEnd);
                        idx = _staticEnd;
                        ++_staticEnd;
                    }
                    if (idx >= _activeEnd) {
                        Swap(idx, _activeEnd);
                        idx = _activeEnd;
                        ++_activeEnd;
                    }
                    break;
                }
                case CompMoveCode::MoveToStatic: {
                    if (idx < _activeEnd) {
                        Swap(idx, _activeEnd - 1);
                        idx = _activeEnd - 1;
                        --_activeEnd;
                    } 
                    else {
                        if (idx >= _inactiveEnd) {
                            Swap(idx, _inactiveEnd);
                            idx = _inactiveEnd;
                            ++_inactiveEnd;
                        }
                        if (idx >= _staticEnd) {
                            Swap(idx, _staticEnd);
                            idx = _staticEnd;
                            ++_staticEnd;
                        }
                    }
                    break;
                }
                case CompMoveCode::MoveToInactive: {
                    if (idx < _activeEnd) {
                        Swap(idx, _activeEnd - 1);
                        idx = _activeEnd - 1;
                        --_activeEnd;
                    }
                    if (idx < _staticEnd) {
                        Swap(idx, _staticEnd - 1);
                        idx = _staticEnd - 1;
                        --_staticEnd;
                    } 
                    else if (idx >= _inactiveEnd) {
                        Swap(idx, _inactiveEnd);
                        idx = _inactiveEnd;
                        ++_inactiveEnd;
                    }
                    break;
                }
                case CompMoveCode::MoveToDestroy: {
                    if (idx < _activeEnd) {
                        Swap(idx, _activeEnd - 1);
                        idx = _activeEnd - 1;
                        --_activeEnd;
                    }
                    if (idx < _staticEnd) {
                        Swap(idx, _staticEnd - 1);
                        idx = _staticEnd - 1;
                        --_staticEnd;
                    }
                    if (idx < _inactiveEnd) {
                        Swap(idx, _inactiveEnd - 1);
                        idx = _inactiveEnd - 1;
                        --_inactiveEnd;
                    }
                    
                    Swap(idx, _size - 1);
                    --_size;
                    _sparse[id].index = InvalidIndex;
                    _freeId.push_back(id);
                    break;
                }
            }
        }
        _moveCode.clear();

        const uint32_t targetPacks = std::max((uint32_t)InitialPackCount, (_size + PackSizeMask) >> PackSizeShift);
        while (_components.size() > targetPacks) {
            delete _components.back();
            _components.pop_back();
        }
    }

    static void ShutDown() {
        if constexpr (HasOnDestroy<C>) {
            const uint32_t endPack = _size >> CompStorageConfig<C>::PackSizeShift;
            for(uint32_t i = 0; i < endPack; ++i) {
                Pack& pack = *_components[i];
                for(uint32_t j = 0; j < PackSize; ++j) {
                    pack[j].OnDestroy();
                }
            }
            const uint32_t sizeLastPack = _size & PackSizeMask;
            if (sizeLastPack != 0) {
                Pack& pack = *_components[endPack];
                for(uint32_t j = 0; j < sizeLastPack; ++j) {
                    pack[j].OnDestroy();
                }
            }
        }

        for (auto& comp : _components) delete comp;

        _components.clear();
        _components.shrink_to_fit();

        _moveCode.clear();
        _moveCode.shrink_to_fit();

        _sparse.clear();
        _sparse.shrink_to_fit();

        _freeId.clear();
        _freeId.shrink_to_fit();
    }
};

template<typename C>
struct CompStorage<C, true> {
    static inline C _instance;
    static inline CompIndex _index;
    static inline uint16_t _typeId = 0;

    bool FistCreate() {
        new (&_instance) C();
        _index.index = 0;
        _index.generation = 0;
        _index.typeID = _typeId;
        return true;
    }
    Comp<C> Create() {
        
        return Comp<C>(&_instance);
    }
};

template<typename T>
Comp<T> GameObject::AddComponent() {
    GameObjectState ctx = _isActive ? (_isStatic ? GameObjectState::Static : GameObjectState::Active) : GameObjectState::Inactive;
    Comp<T> comp = CompStorage<T>::CreateRaw(ctx);
    comp->InternalSetGameObject(this);
    comp->InternalSetTransform(_transform);
    _components.push_back({comp.GetID(), comp.GetGeneration(), comp.GetTypeID()});

    if constexpr (HasOnAwake<T>) comp->OnAwake();
    if (ctx != ContextState::Inactive) if constexpr (HasOnEnable<T>) comp->OnEnable();
    return comp;
}

template<typename T>
Comp<T> GameObject::GetComponent() const {
    for (const auto& comp : _components) {
        Comp<T> casted = CompDynamicCast<T>(comp);
        if (casted.IsValid()) return casted;
    }
    return Comp<T>(nullptr);
}

template<typename T>
bool RegisterComponent() {
    static bool reg = CompStorage<T>::Register();
    return reg;
}

}