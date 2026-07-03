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
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <vector>

namespace Stelo {

constexpr uint32_t InvalidID = std::numeric_limits<uint32_t>::max();
constexpr uint32_t InvalidIndex = std::numeric_limits<uint32_t>::max();
constexpr uint16_t InvalidTypeID = std::numeric_limits<uint16_t>::max();

/// DO NOT change the enum order.
///
/// The current order is relied upon by comparison operators
/// (==, !=, <=, etc.). Changing the order may break existing logic.
enum class ContextState : uint8_t {
    Active, Static, Inactive, Destroy,
};
using GameObjectState = ContextState;

enum class CompMoveCode : uint8_t {
    MoveToActive,
    MoveToStatic,
    MoveToInactive,
    MoveToDestroy
};

enum class CompSetCode : uint8_t {
    SetActive,
    SetStatic,
    SetDestroy
};

inline CompMoveCode ContextStateToMoveCode(ContextState state) {
    switch (state) {
        case ContextState::Active: return CompMoveCode::MoveToActive;
        case ContextState::Static: return CompMoveCode::MoveToStatic;
        case ContextState::Inactive: return CompMoveCode::MoveToInactive;
        case ContextState::Destroy: return CompMoveCode::MoveToDestroy;
        default: return CompMoveCode::MoveToInactive;
    }
}

struct CompTypeManager {
private:
    static inline uint16_t nextTypeID = 0;
    static inline std::vector<uint16_t> freeID = {};

    static uint16_t GetNextTypeID() {
        if (freeID.empty()) {
            return nextTypeID++;
        }
        uint16_t id = freeID.back();
        freeID.pop_back();
        return id;
    }
public:
    template<typename T>
    static uint16_t Register() {
        static uint16_t typeID = GetNextTypeID();
        return typeID;
    }
};

struct CompIndex {
    uint32_t index = InvalidIndex;
    uint16_t generation = 0;
    uint16_t typeID = InvalidTypeID;
};

struct CompStorageView {
    void** memPack;
    const CompIndex* sparse;

    uint32_t sizeSparse;
    uint32_t sizeComponent;
    uint32_t sizePackMask;
    uint8_t sizePackShift;
    uint8_t padding[3];
    
    CompStorageView() : memPack(nullptr), sparse(nullptr), sizeSparse(0), sizeComponent(0), sizePackMask(0), sizePackShift(0) {}
    CompStorageView(void** memPacks, const CompIndex* sp, uint32_t szSp, uint32_t szComp, uint32_t szPackMark, uint8_t gf2) 
        : memPack(memPacks), sparse(sp), sizeSparse(szSp), sizeComponent(szComp), sizePackMask(szPackMark), sizePackShift(gf2) {}
};

struct CompManager {
    using FCall = void(*)();
    using FCompSet = void(*)(CompInfo&, CompSetCode, bool value);
    using FCompCreate = Comp<CompObject>(*)(GameObjectState);

    static inline CompStorageView* storages = nullptr;
    static inline FCall* ShutdownCalls = nullptr;
    static inline FCompSet* CompSetCalls = nullptr;
    static inline FCompCreate* CompCreateCalls = nullptr;
    static inline uint16_t capacity = 0;

    static inline std::vector<FCall> ExecuteMoveActionCall;
    static inline std::vector<FCall> StartCall;

    static inline std::vector<FCall> PreFixedUpdateCall;
    static inline std::vector<FCall> FixedUpdateCall;
    static inline std::vector<FCall> PreUpdateCall;
    static inline std::vector<FCall> UpdateCall;
    static inline std::vector<FCall> PostUpdateCall;
    static inline std::vector<FCall> PreRenderCall;
    static inline std::vector<FCall> RenderCall;
    static inline std::vector<FCall> PostRenderCall;

    static void EnsureCapacity(uint16_t typeID) {
        if (typeID >= capacity) {
            uint16_t newCap = typeID + 1;
            
            CompStorageView* newStorages = new CompStorageView[newCap]{};
            FCall* newShutDownCalls = new FCall[newCap]{};
            FCompSet* newCompSetCalls = new FCompSet[newCap]{};
            FCompCreate* newCompCreateCalls = new FCompCreate[newCap]{};
            if (capacity > 0) {
                std::copy(storages, storages + capacity, newStorages);
                std::copy(ShutdownCalls, ShutdownCalls + capacity, newShutDownCalls);
                std::copy(CompSetCalls, CompSetCalls + capacity, newCompSetCalls);
                std::copy(CompCreateCalls, CompCreateCalls + capacity, newCompCreateCalls);

                delete[] storages;
                delete[] ShutdownCalls;
                delete[] CompSetCalls;
                delete[] CompCreateCalls;
            }

            storages = newStorages;
            ShutdownCalls = newShutDownCalls;
            CompSetCalls = newCompSetCalls;
            CompCreateCalls = newCompCreateCalls;
            capacity = newCap;
        }
    }

    static void RegisterCallByID(FCall sd, FCompSet cs, FCompCreate cc, uint16_t typeID) {
        EnsureCapacity(typeID);
        ShutdownCalls[typeID] = sd;
        CompSetCalls[typeID] = cs;
        CompCreateCalls[typeID] = cc;
    }
    static void UnregisterCallByID(uint16_t typeID) {
        assert(typeID < capacity);
        ShutdownCalls[typeID] = nullptr;
        CompSetCalls[typeID] = nullptr;
        CompCreateCalls[typeID] = nullptr;
    }
    
    static void UpdateView(const CompStorageView& storage, uint16_t typeID) {
        assert(typeID < capacity);
        storages[typeID] = storage;
    }

    static void Shutdown() {
        for (uint16_t i = 0; i < capacity; ++i) {
            if (ShutdownCalls[i]) ShutdownCalls[i]();
        }

        delete[] storages;
        delete[] ShutdownCalls;
        delete[] CompSetCalls;
        delete[] CompCreateCalls;

        storages = nullptr;
        ShutdownCalls = nullptr;
        CompSetCalls = nullptr;
        CompCreateCalls = nullptr;
        capacity = 0;
    }

    static void QueueExecuteMoveAction(FCall func) {
        ExecuteMoveActionCall.push_back(func);
    }

    static void QueueStart(FCall func) {
        StartCall.push_back(func);
    }

    static void ExecuteMoveAction() {
        for (const auto& call : ExecuteMoveActionCall) call();
        ExecuteMoveActionCall.clear();
    }

    static void Start() {
        const std::size_t size = StartCall.size();
        if(size == 0) return;
        for (std::size_t i = 0; i < size; ++i) {
            StartCall[i]();
        }
        
        const std::size_t residual = StartCall.size() - size;
        if (residual > 0) {
            std::vector<FCall> newCall(residual);
            std::copy(StartCall.begin() + size, StartCall.end(), newCall.begin());
            StartCall = newCall;
        } else {
            StartCall.clear();
        }
    }
    
    static void RegisterPreFixedUpdate(FCall func) {
        PreFixedUpdateCall.push_back(func);
    }
    static void RegisterFixedUpdate(FCall func) {
        FixedUpdateCall.push_back(func);
    }
    static void RegisterPreUpdate(FCall func) {
        PreUpdateCall.push_back(func);
    }
    static void RegisterUpdate(FCall func) {
        UpdateCall.push_back(func);
    }
    static void RegisterPostUpdate(FCall func) {
        PostUpdateCall.push_back(func);
    }
    static void RegisterPreRender(FCall func) {
        PreRenderCall.push_back(func);
    }
    static void RegisterRender(FCall func) {
        RenderCall.push_back(func);
    }
    static void RegisterPostRender(FCall func) {
        PostRenderCall.push_back(func);
    }

    static void FixedUpdate() {
        for (auto& call : FixedUpdateCall) call();
    }
    static void PreUpdate() {
        for (auto& call : PreUpdateCall) call();
    }
    static void Update() {
        for (auto& call : UpdateCall) call();
    }
    static void PostUpdate() {
        for (auto& call : PostUpdateCall) call();
    }
    static void PreRender() {
        for (auto& call : PreRenderCall) call();
    }
    static void Render() {
        for (auto& call : RenderCall) call();
    }
    static void PostRender() {
        for (auto& call : PostRenderCall) call();
    }
    
    static void Unregister(std::vector<FCall>& callList, FCall func) {
        callList.erase(std::remove(callList.begin(), callList.end(), func), callList.end());
    }
    static void UnregisterExecuteMoveAction(FCall func) {
        Unregister(ExecuteMoveActionCall, func);
    }
    static void UnregisterStart(FCall func) {
        Unregister(StartCall, func);
    }
    static void UnregisterPreFixedUpdate(FCall func) {
        Unregister(PreFixedUpdateCall, func);
    }
    static void UnregisterFixedUpdate(FCall func) {
        Unregister(FixedUpdateCall, func);
    }
    static void UnregisterPreUpdate(FCall func) {
        Unregister(PreUpdateCall, func);
    }
    static void UnregisterUpdate(FCall func) {
        Unregister(UpdateCall, func);
    }
    static void UnregisterPostUpdate(FCall func) {
        Unregister(PostUpdateCall, func);
    }
    static void UnregisterPreRender(FCall func) {
        Unregister(PreRenderCall, func);
    }
    static void UnregisterRender(FCall func) {
        Unregister(RenderCall, func);
    }
    static void UnregisterPostRender(FCall func) {
        Unregister(PostRenderCall, func);
    }
};

template<typename T>
struct Comp {
    uint32_t _id = InvalidIndex;
    uint16_t _generation = 0;
    uint16_t _typeID = 0xffff;
    
    Comp() : _id(InvalidIndex), _generation(0), _typeID(0xffff) {}
    Comp(uint32_t id, uint16_t generation, uint16_t typeID) : _id(id), _generation(generation), _typeID(typeID) {}
    Comp(const T* ptr) { 
        if (ptr == nullptr) {
            _id = InvalidIndex;
            _generation = 0;
            _typeID = 0xffff;
            return;
        }

        const uint32_t t_id = ptr->GetID();
        const uint16_t t_typeID = ptr->GetTypeID();

        if (t_typeID >= CompManager::capacity) [[unlikely]] return;
        const CompStorageView& stor = CompManager::storages[t_typeID];
        if (t_id >= stor.sizeSparse) [[unlikely]] return;
        const auto& element = stor.sparse[t_id];
        if ((element.index == InvalidIndex) & (ptr != reinterpret_cast<T*>(
            reinterpret_cast<uint8_t*>(stor.memPack[element.index >> stor.sizePackShift]) + 
            (element.index & stor.sizePackMask) * stor.sizeComponent))
        ) [[unlikely]] return;

        _id = t_id;
        _generation = stor.sparse[t_id].generation;
        _typeID = t_typeID;
    }

    inline uint32_t GetID() const {
        return _id;
    }
    inline uint16_t GetGeneration() const {
        return _generation;
    }
    inline uint16_t GetTypeID() const {
        return _typeID;
    }
    
    bool IsValid() const {
        if (_typeID >= CompManager::capacity) [[unlikely]] return false;
        const auto& stor = CompManager::storages[_typeID];
        if (_id >= stor.sizeSparse) [[unlikely]] return false;
        const auto& element = stor.sparse[_id];
        return (element.index != InvalidIndex) & (element.generation == _generation);
    }
    inline uint32_t GetUnsafeIndex() const {
        return CompManager::storages[_typeID].sparse[_id].index;
    }
    inline T* GetUnsafePtr() const {
        const CompStorageView& stor = CompManager::storages[_typeID];
        return reinterpret_cast<T*>(
            reinterpret_cast<uint8_t*>(stor.memPack[stor.sparse[_id].index >> stor.sizePackShift]) + 
            (stor.sparse[_id].index & stor.sizePackMask) * stor.sizeComponent
        );
    }
    inline T* GetPtr() const {
        return operator->();
    }

    inline T* operator->() const {
        if (IsValid()) [[likely]] {
            return GetUnsafePtr();
        } 
        else [[unlikely]] {
            throw std::runtime_error("SafeComp is accessing a dangling or invalid component!");
        }
    }
    inline T& operator*() const {
        return *(operator->());
    }

    bool operator==(const Comp<T>& other) const {
        return _id == other._id && _generation == other._generation && _typeID == other._typeID;
    }
};

template<typename N_T, typename O_T>
Comp<N_T> CompDynamicCast(Comp<O_T> comp) {
    if (!comp.IsValid()) return Comp<N_T>(nullptr);
    const N_T* ptr = dynamic_cast<N_T*>(comp.GetUnsafePtr());
    if (ptr == nullptr) return Comp<N_T>(nullptr);
    return Comp<N_T>(ptr);
};

}