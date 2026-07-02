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

#include "../World/GameObject.hpp"
#include "ECS/Comp.hpp"
#include "Component.hpp"

namespace Stelo {

Comp<CompObject> CompObject::GetHandle() {
    if(_info.IsSelfDestroyed()) return {};
    const CompStorageView& sto = CompManager::storages[_info.GetTypeID()];
    return Comp<CompObject>(_info.GetID(), sto.sparse[_info.GetID()].generation, _info.GetTypeID());
}

void CompObject::SetActive(bool value) {
    if (_info.IsSelfActive() == value || _info.IsSelfDestroyed()) return;
    if (_info.GetGameObjectState() == GameObjectState::Active) {
        CompManager::CompSetCalls[_info.GetTypeID()](_info, CompSetCode::SetActive, value);
    } 
}

void CompObject::SetStatic(bool value) {
    if (_info.IsSelfStatic() == value || _info.IsSelfDestroyed()) return;
    if (_info.GetGameObjectState() == GameObjectState::Active && _info.IsSelfActive()) {
        CompManager::CompSetCalls[_info.GetTypeID()](_info, CompSetCode::SetStatic, value);
    }
}

void CompObject::Destroy() {
    if (_info.IsSelfDestroyed()) return;
    _info.SetContextState(ContextState::Destroy);
    _info.SetSelfDestroyed(true);
    CompManager::CompSetCalls[_info.GetTypeID()](_info, CompSetCode::SetDestroy, true);
}


}
