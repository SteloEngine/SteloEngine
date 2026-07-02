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

#include "../Math/Vector.hpp"
#include "../Math/Quaternion.hpp"

#include "../ECS/Component.hpp"
#include "../ECS/CompStorage.hpp"
#include "ECS/Comp.hpp"

namespace Stelo {
struct Transform : public CompObject {
private:
    Quaternion _rotation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
    Vector3 _position = Vector3(0);
    Vector3 _scale = Vector3(1, 1, 1);

    uint _version = 0;
    uint _lastVersion = 0;
public:
    void SetPosition(const Vector3& value) { _position = value; ++_version; }
    void SetScale(const Vector3& value) { _scale = value; ++_version; }
    void SetRotation(const Quaternion& value) { _rotation = value; ++_version; }

    inline void Translate(const Vector3& delta) { _position += delta; ++_version; }
    inline void AddScale(const Vector3& delta) { _scale += delta; ++_version; }
    inline void Rotate(const Quaternion& delta) { _rotation *= delta; ++_version; }

    inline void SetTransform(const Vector3& pos, const Quaternion& rot, const Vector3& scale) {
        _position = pos;
        _rotation = rot;
        _scale = scale;
        ++_version;
    }

    const Quaternion& GetRotation() const { return _rotation; }
    const Vector3& GetPosition() const { return _position; }
    const Vector3& GetScale() const { return _scale; }

    inline Vector3 Forward() const { return _rotation * Vector3::Forward;}
    inline Vector3 Up() const { return _rotation * Vector3::Up; }

    inline uint GetVersion() const { return _version; }
};

REGISTER_COMPONENT(Transform)

template<>
struct CompStorage<Transform, false> {
    using C = Transform;

    struct TreeInfo {
        std::vector<Comp<Transform>> children;
        Comp<Transform> parent;
    };

    using Pack = std::array<C, 1 << CompStorageConfig<C>::PackSizeShift>;
    using PackTree = std::array<TreeInfo, 1 << CompStorageConfig<C>::PackSizeShift>;
    static const uint16_t InitDefaulteCreateState() {
        CompInfo info(0, 0, 0);
        info.SetSelfPending(true);
        info.SetSelfActive(true);
        info.SetSelfStatic(false);
        info.SetSelfDestroyed(false);
        return info.state;
    }

    static constexpr uint32_t PackSize = 1 << CompStorageConfig<C>::PackSizeShift;
    static constexpr uint32_t PackSizeMask = PackSize - 1;
    static constexpr uint32_t PackSizeShift = CompStorageConfig<C>::PackSizeShift;
    static constexpr uint32_t InitialPackCount = CompStorageConfig<C>::InitialPackCount;

    static const inline uint16_t DefaultCreateState = InitDefaulteCreateState();

    static inline std::vector<Pack*> _components = {};
    static inline std::vector<PackTree*> _componentsTree = {};

    static inline std::vector<CompIndex> _sparse = {};
    static inline std::vector<uint32_t> _stateMoveCode = {};
    static inline std::vector<std::pair<uint32_t, CompMoveCode>> _moveCode = {};
    static inline std::vector<uint32_t> _freeId = {};

    static inline uint32_t _activeEnd = 0;
    static inline uint32_t _staticEnd = 0;
    static inline uint32_t _inactiveEnd = 0;
    static inline uint32_t _size = 0;
    static inline uint16_t _typeId = 0;
    static inline uint32_t _nextId = 0;

    static inline std::atomic<bool> _isRegistered = false;
    static inline std::atomic<bool> _executeMoveActionRegistered = false;

    static inline C& GetCompRef(uint32_t physicalIdx) {
        return (*_components[physicalIdx >> PackSizeShift])[physicalIdx & PackSizeMask];
    }
    static void UpdateView() {
        void** mem_blocks = reinterpret_cast<void**>(_components.data());
        CompStorageView view;
        view.memPack = mem_blocks;
        view.sparse = _sparse.data();
        view.sizeSparse = static_cast<uint32_t>(_sparse.size());
        view.sizeComponent = static_cast<uint32_t>(sizeof(C));
        view.sizePackMask = PackSizeMask;
        view.PackSizeShift = PackSizeShift;
        CompManager::UpdateView(view, _typeId);
    }
    
    static inline void Swap(uint32_t physicalA, uint32_t physicalB) {
        if (physicalA == physicalB) return;

        const uint32_t packIndexA = physicalA >> PackSizeShift;
        const uint32_t localIndexA = physicalA & PackSizeMask;

        const uint32_t packIndexB = physicalB >> PackSizeShift;
        const uint32_t localIndexB = physicalB & PackSizeMask;

        C& compA = (*_components[packIndexA])[localIndexA];
        TreeInfo& treeA = (*_componentsTree[packIndexA])[localIndexA];

        C& compB = (*_components[packIndexB])[localIndexB];
        TreeInfo& treeB = (*_componentsTree[packIndexB])[localIndexB];

        const uint32_t idA = compA.GetID();
        const uint32_t idB = compB.GetID();

        std::swap(compA, compB);
        std::swap(treeA, treeB);

        _sparse[idA].index = physicalB;
        _sparse[idB].index = physicalA;
    }

    static bool Initial() {
        _components.resize(InitialPackCount);
        for (uint32_t i = 0; i < InitialPackCount; i++) _components[i] = new Pack();
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
        _isRegistered.store(false);
        return true;
    }

    static Comp<C> CreateRaw(GameObjectState ctx) {
        static bool initial = Initial();
        if(!_executeMoveActionRegistered) {
            CompManager::QueueExecuteMoveAction(ExecuteMoveAction);
            _executeMoveActionRegistered = true;
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
                _stateMoveCode.resize(newSize);
                updateView = true;
            }
        }
        
        const uint32_t physicIndex = _size++;
        const uint32_t packIndex = physicIndex >> PackSizeShift;
        const uint32_t packOffset = physicIndex & PackSizeMask;
        const uint32_t lastSize = _components.size();
        if(packIndex >= lastSize) {
            _components.resize(packIndex + 1);
            _componentsTree.resize(packIndex + 1);

            const uint32_t newSize = _components.size();
            for(uint32_t i = lastSize; i < newSize; ++i) {
                _components[i] = new Pack();
                _componentsTree[i] = new PackTree();
            }
            updateView = true;
        }
        
        CompInfo info(id, DefaultCreateState, _typeId);
        CompIndex& sparseIndex = _sparse[id];
        sparseIndex.index = physicIndex;
        sparseIndex.typeID = _typeId;

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
                if (value) {
                    if (info.IsSelfStatic()) {
                        info.SetContextState(ContextState::Static);
                        mvCode = CompMoveCode::MoveToStatic;
                    } else {
                        info.SetContextState(ContextState::Active);
                        mvCode = CompMoveCode::MoveToActive;
                    }
                } else {
                    info.SetContextState(ContextState::Inactive);
                    mvCode = CompMoveCode::MoveToInactive;
                }
                break;
            }
            
            case CompSetCode::SetStatic: {
                if (info.GetGameObjectState() == GameObjectState::Active && info.IsSelfActive()) {
                    if (value) {
                        info.SetContextState(ContextState::Static);
                        mvCode = CompMoveCode::MoveToStatic;
                    } else {
                        info.SetContextState(ContextState::Active);
                        mvCode = CompMoveCode::MoveToActive;
                    }
                } else {
                    mvCode = CompMoveCode::MoveToInactive;
                }
                break;
            }
            
            case CompSetCode::SetDestroy: {
                info.SetContextState(ContextState::Destroy);
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
    static void ExecuteMoveAction() {
        _executeMoveActionRegistered = false;
        for (const auto& [id, moveCode] : _moveCode) {
            if (id >= _sparse.size() || _sparse[id].index == InvalidIndex) continue;

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
        for (auto& pack : _components) delete pack;
        for (auto& pack : _componentsTree) delete pack;

        _components.clear();
        _components.shrink_to_fit();

        _componentsTree.clear();
        _componentsTree.shrink_to_fit();

        _stateMoveCode.clear();
        _stateMoveCode.shrink_to_fit();

        _moveCode.clear();
        _moveCode.shrink_to_fit();

        _sparse.clear();
        _sparse.shrink_to_fit();

        _freeId.clear();
        _freeId.shrink_to_fit();
    }
};

}